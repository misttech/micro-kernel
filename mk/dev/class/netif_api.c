/*
 * Copyright (c) 2013 Corey Tabaka
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <dev/class/netif.h>
#include <lk/err.h>

status_t class_netif_set_state(struct device *dev, struct netstack_state *state) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->set_state)
    return ops->set_state(dev, state);
  else
    return ERR_NOT_SUPPORTED;
}

ssize_t class_netif_get_hwaddr(struct device *dev, void *buf, size_t max_len) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->get_hwaddr)
    return ops->get_hwaddr(dev, buf, max_len);
  else
    return ERR_NOT_SUPPORTED;
}

ssize_t class_netif_get_mtu(struct device *dev) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->get_mtu)
    return ops->get_mtu(dev);
  else
    return ERR_NOT_SUPPORTED;
}

status_t class_netif_set_status(struct device *dev, bool up) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->set_status)
    return ops->set_status(dev, up);
  else
    return ERR_NOT_SUPPORTED;
}

status_t class_netif_output(struct device *dev, struct pbuf *p) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->output)
    return ops->output(dev, p);
  else
    return ERR_NOT_SUPPORTED;
}

status_t class_netif_mcast_filter(struct device *dev, const uint8_t *mac, int action) {
  struct netif_ops *ops = device_get_driver_ops(dev, struct netif_ops, std);
  if (!ops)
    return ERR_NOT_CONFIGURED;

  if (ops->mcast_filter)
    return ops->mcast_filter(dev, mac, action);
  else
    return ERR_NOT_SUPPORTED;
}
