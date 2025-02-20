/*
 * Copyright (c) 2018 The Fuchsia Authors
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lib/cdcconsole/cdcconsole.h>
#include <string.h>

#include <lk/compiler.h>
#include <lk/err.h>

#if !CONSOLE_HAS_INPUT_BUFFER
#error "CONSOLE_HAS_INPUT_BUFFER needs to be configured for cdcconsole."
#endif

#ifndef CDCCONSOLE_TX_BUFFER_SIZE
#define CDCCONSOLE_TX_BUFFER_SIZE 64
#endif  // CDCCONSOLE_TX_BUFFER_SIZE

/*
 * This implementation is pretty simple right now.  Possible future work:
 *  * Make cdcconsole_print() asynchronous.
 *  * Buffer output when offline so that you can see boot messages.
 *  * Buffer writes with a timeout allowing the driver to coalesce them into
 *    less packets.
 */

static status_t cdcconsole_rx_cb(ep_t endpoint, usbc_transfer_t *t);
static status_t cdcconsole_tx_cb(ep_t endpoint, usbc_transfer_t *t);

// Call with con->tx_lock
static void cdcconsole_handle_tx(cdcconsole_t *con) {
  iovec_t regions[2];
  cbuf_peek(&con->tx_buf, regions);
  if (regions[0].iov_len > 0) {
    cdcserial_write_async(&con->cdc_chan, &con->tx_transfer, cdcconsole_tx_cb, regions[0].iov_len,
                          regions[0].iov_base);
    con->transmitting = true;
  } else {
    con->transmitting = false;
  }
}

static void cdcconsole_print(print_callback_t *cb, const char *str, size_t len) {
  cdcconsole_t *con = cb->context;

  cbuf_write(&con->tx_buf, str, len, false);

  if (con->online) {
    spin_lock_saved_state_t state;
    spin_lock_irqsave(&con->tx_lock, state);
    if (!con->transmitting) {
      cdcconsole_handle_tx(con);
    }
    spin_unlock_irqrestore(&con->tx_lock, state);
  }
}

static void cdcconsole_queue_read(cdcconsole_t *con) {
  cdcserial_read_async(&con->cdc_chan, &con->rx_transfer, cdcconsole_rx_cb, sizeof(con->rx_buf),
                       con->rx_buf);
}

static status_t cdcconsole_tx_cb(ep_t endpoint, usbc_transfer_t *t) {
  cdcconsole_t *con = containerof(t, cdcconsole_t, tx_transfer);
  spin_lock_saved_state_t state;
  spin_lock_irqsave(&con->tx_lock, state);

  // Consume the peeked data.
  cbuf_read(&con->tx_buf, NULL, t->bufpos, false);
  cdcconsole_handle_tx(con);
  spin_unlock_irqrestore(&con->tx_lock, state);
  return NO_ERROR;
}

static status_t cdcconsole_rx_cb(ep_t endpoint, usbc_transfer_t *t) {
  cdcconsole_t *con = containerof(t, cdcconsole_t, rx_transfer);

  cbuf_write(&console_input_cbuf, t->buf, t->bufpos, false);
  cdcconsole_queue_read(con);
  return NO_ERROR;
}

static void cdcconsole_online(cdcserial_channel_t *chan, bool online) {
  cdcconsole_t *con = containerof(chan, cdcconsole_t, cdc_chan);
  con->online = online;
  if (online) {
    spin_lock_saved_state_t state;
    spin_lock_irqsave(&con->tx_lock, state);
    if (!con->transmitting) {
      // Throw away previous data.
      cbuf_reset(&con->tx_buf);
    }
    spin_unlock_irqrestore(&con->tx_lock, state);

    cdcconsole_queue_read(con);
  }
}

void cdcconsole_init(cdcconsole_t *con, int data_ep_addr, int ctrl_ep_addr) {
  memset(con, 0x0, sizeof(*con));

  cbuf_initialize(&con->tx_buf, CDCCONSOLE_TX_BUFFER_SIZE);
  spin_lock_init(&con->tx_lock);

  con->cdc_chan.online_cb = cdcconsole_online;
  cdcserial_create_channel(&con->cdc_chan, data_ep_addr, ctrl_ep_addr);

  con->print_cb.print = cdcconsole_print;
  con->print_cb.context = con;
  register_print_callback(&con->print_cb);
}
