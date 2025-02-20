/*
 * Copyright (c) 2015 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <dev/usb.h>
#include <dev/usb/class/bulktest.h>
#include <dev/usbc.h>
#include <lk/debug.h>
#include <lk/err.h>
#include <lk/init.h>
#include <lk/list.h>
#include <lk/trace.h>

/* a simple demo usb class device that reflects data written to
 * one endpoint to the other.
 */

#define LOCAL_TRACE 0

#define W(w) (w & 0xff), (w >> 8)
#define W3(w) (w & 0xff), ((w >> 8) & 0xff), ((w >> 16) & 0xff)

static status_t ep_cb_rx(ep_t endpoint, usbc_transfer_t *t);
static status_t ep_cb_tx(ep_t endpoint, usbc_transfer_t *t);

#define TRANSFER_SIZE 512

static void queue_rx(void) {
  static usbc_transfer_t transfer;
  static uint8_t buf[TRANSFER_SIZE];

  transfer.callback = &ep_cb_rx;
  transfer.result = 0;
  transfer.buf = &buf;
  transfer.buflen = sizeof(buf);
  transfer.bufpos = 0;
  transfer.extra = 0;

  usbc_queue_rx(1, &transfer);
}

static void queue_tx(void) {
  static usbc_transfer_t transfer;
  static uint8_t buf[TRANSFER_SIZE];

  for (uint i = 0; i < sizeof(buf); i++) {
    buf[i] = ~i;
  }

  transfer.callback = &ep_cb_tx;
  transfer.result = 0;
  transfer.buf = &buf;
  transfer.buflen = sizeof(buf);
  transfer.bufpos = 0;
  transfer.extra = 0;

  usbc_queue_tx(1, &transfer);
}

static status_t ep_cb_rx(ep_t endpoint, usbc_transfer_t *t) {
#if LOCAL_TRACE
  LTRACEF("ep %u transfer %p\n", endpoint, t);
  usbc_dump_transfer(t);

  if (t->result >= 0) {
    hexdump8(t->buf, t->bufpos);
  }
#endif

  if (t->result >= 0)
    queue_rx();

  return NO_ERROR;
}

static status_t ep_cb_tx(ep_t endpoint, usbc_transfer_t *t) {
#if LOCAL_TRACE
  LTRACEF("ep %u transfer %p\n", endpoint, t);
  usbc_dump_transfer(t);
#endif

  if (t->result >= 0)
    queue_tx();

  return NO_ERROR;
}

static status_t bulktest_usb_cb(void *cookie, usb_callback_op_t op,
                                const union usb_callback_args *args) {
  LTRACEF("cookie %p, op %u, args %p\n", cookie, op, args);

  if (op == USB_CB_ONLINE) {
    usbc_setup_endpoint(1, USB_IN, 0x40, USB_BULK);
    usbc_setup_endpoint(1, USB_OUT, 0x40, USB_BULK);

    queue_rx();
    queue_tx();
  }
  return NO_ERROR;
}

status_t usb_class_bulktest_init(uint interface_num, ep_t epin, ep_t epout) {
  LTRACEF("epin %u, epout %u\n", epin, epout);

  /* build a descriptor for it */
  uint8_t if_descriptor[] = {
      0x09,          /* length */
      INTERFACE,     /* type */
      interface_num, /* interface num */
      0x00,          /* alternates */
      0x02,          /* endpoint count */
      0xff,          /* interface class */
      0xff,          /* interface subclass */
      0x00,          /* interface protocol */
      0x00,          /* string index */

      /* endpoint 1 IN */
      0x07,        /* length */
      ENDPOINT,    /* type */
      epin | 0x80, /* address: 1 IN */
      0x02,        /* type: bulk */
      W(64),       /* max packet size: 64 */
      00,          /* interval */

      /* endpoint 1 OUT */
      0x07,     /* length */
      ENDPOINT, /* type */
      epout,    /* address: 1 OUT */
      0x02,     /* type: bulk */
      W(64),    /* max packet size: 64 */
      00,       /* interval */
  };

  usb_append_interface_lowspeed(if_descriptor, sizeof(if_descriptor));
  usb_append_interface_highspeed(if_descriptor, sizeof(if_descriptor));

  usb_register_callback(&bulktest_usb_cb, NULL);

  return NO_ERROR;
}
