/*
 * Copyright (c) 2014 Chris Anderson
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <kernel/mutex.h>
#include <kernel/thread.h>
#include <lk/list.h>
#include <lk/trace.h>

#include "minip-internal.h"

#define LOCAL_TRACE 0
typedef struct {
  struct list_node node;
  uint32_t addr;
  uint8_t mac[6];
} arp_entry_t;

static struct list_node arp_list = LIST_INITIAL_VALUE(arp_list);
static mutex_t arp_mutex = MUTEX_INITIAL_VALUE(arp_mutex);

void arp_cache_init(void) {}

static void mru_update(struct list_node *entry) {
  if (arp_list.next == entry)
    return;

  list_delete(entry);
  list_add_head(&arp_list, entry);
}

void arp_cache_update(uint32_t addr, const uint8_t mac[6]) {
  arp_entry_t *arp;
  ipv4_t ip;
  bool found = false;

  ip.u = addr;

  // Ignore 0.0.0.0 or x.x.x.255
  if (ip.u == 0 || ip.b[3] == 0xFF) {
    return;
  }

  /* If the entry is in the cache update the address and move
   * it to head */
  mutex_acquire(&arp_mutex);
  list_for_every_entry (&arp_list, arp, arp_entry_t, node) {
    if (arp->addr == addr) {
      arp->addr = addr;
      mru_update(&arp->node);
      found = true;
      break;
    }
  }

  if (!found) {
    LTRACEF("Adding %u.%u.%u.%u -> %02x:%02x:%02x:%02x:%02x:%02x to cache\n", ip.b[0], ip.b[1],
            ip.b[2], ip.b[3], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    arp = malloc(sizeof(arp_entry_t));
    if (arp == NULL) {
      goto err;
    }

    arp->addr = addr;
    mac_addr_copy(arp->mac, mac);
    list_add_head(&arp_list, &arp->node);
  }

err:
  mutex_release(&arp_mutex);
  return;
}

/* Looks up and returns a MAC address based on the provided ip addr */
uint8_t *arp_cache_lookup(uint32_t addr) {
  arp_entry_t *arp = NULL;
  uint8_t *ret = NULL;

  /* If the entry is in the cache update the address and move
   * it to head */
  mutex_acquire(&arp_mutex);
  list_for_every_entry (&arp_list, arp, arp_entry_t, node) {
    if (arp->addr == addr) {
      mru_update(&arp->node);
      ret = arp->mac;
      break;
    }
  }
  mutex_release(&arp_mutex);

  return ret;
}

void arp_cache_dump(void) {
  int i = 0;
  arp_entry_t *arp;

  if (!list_is_empty(&arp_list)) {
    list_for_every_entry (&arp_list, arp, arp_entry_t, node) {
      ipv4_t ip;
      ip.u = arp->addr;
      printf("%2d: %u.%u.%u.%u -> %02x:%02x:%02x:%02x:%02x:%02x\n", i++, ip.b[0], ip.b[1], ip.b[2],
             ip.b[3], arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);
    }
  } else {
    printf("The arp table is empty\n");
  }
}

int arp_send_request(uint32_t addr) {
  pktbuf_t *p;
  struct eth_hdr *eth;
  struct arp_pkt *arp;

  if ((p = pktbuf_alloc()) == NULL) {
    return -1;
  }

  eth = pktbuf_prepend(p, sizeof(struct eth_hdr));
  arp = pktbuf_append(p, sizeof(struct arp_pkt));
  minip_build_mac_hdr(eth, bcast_mac, ETH_TYPE_ARP);

  arp->htype = htons(0x0001);
  arp->ptype = htons(0x0800);
  arp->hlen = 6;
  arp->plen = 4;
  arp->oper = htons(ARP_OPER_REQUEST);
  arp->spa = minip_get_ipaddr();
  arp->tpa = addr;
  minip_get_macaddr(arp->sha);
  mac_addr_copy(arp->tha, bcast_mac);

  minip_tx_handler(minip_tx_arg, p);
  return 0;
}

static void handle_arp_timeout_cb(void *arg) { *(bool *)arg = true; }

const uint8_t *arp_get_dest_mac(uint32_t host) {
  const uint8_t *dst_mac = NULL;
  bool arp_timeout = false;
  net_timer_t arp_timeout_timer;

  if (host == IPV4_BCAST) {
    return bcast_mac;
  }

  dst_mac = arp_cache_lookup(host);
  if (dst_mac == NULL) {
    arp_send_request(host);
    memset(&arp_timeout_timer, 0, sizeof(arp_timeout_timer));
    net_timer_set(&arp_timeout_timer, handle_arp_timeout_cb, &arp_timeout, 100);
    while (!arp_timeout) {
      dst_mac = arp_cache_lookup(host);
      if (dst_mac) {
        net_timer_cancel(&arp_timeout_timer);
        break;
      }
    }
  }

  return dst_mac;
}
