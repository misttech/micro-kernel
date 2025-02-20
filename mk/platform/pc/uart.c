/*
 * Copyright (c) 2012 Corey Tabaka
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <lib/cbuf.h>
#include <malloc.h>

#include <arch/x86.h>
#include <dev/class/uart.h>
#include <dev/driver.h>
#include <kernel/thread.h>
#include <lk/debug.h>
#include <lk/err.h>
#include <platform/interrupts.h>
#include <platform/uart.h>

struct device_class uart_device_class = {
    .name = "uart",
};

struct uart_driver_state {
  struct cbuf rx_buf;
  struct cbuf tx_buf;
};

static status_t uart_init(struct device *dev);

static enum handler_return uart_irq_handler(void *arg);
static int uart_write_thread(void *arg);

static ssize_t uart_read(struct device *dev, void *buf, size_t len);
static ssize_t uart_write(struct device *dev, const void *buf, size_t len);

static struct uart_ops the_ops = {
    .std =
        {
            .device_class = &uart_device_class,
            .init = uart_init,
        },
    .read = uart_read,
    .write = uart_write,
};

DRIVER_EXPORT(uart, &the_ops.std);

static status_t uart_init(struct device *dev) {
  status_t res = NO_ERROR;

  if (!dev)
    return ERR_INVALID_ARGS;

  if (!dev->config)
    return ERR_NOT_CONFIGURED;

  const struct platform_uart_config *config = dev->config;

  struct uart_driver_state *state = malloc(sizeof(struct uart_driver_state));
  if (!state) {
    res = ERR_NO_MEMORY;
    goto done;
  }

  dev->state = state;

  /* set up the driver state */
  cbuf_initialize(&state->rx_buf, config->rx_buf_len);
  cbuf_initialize(&state->tx_buf, config->tx_buf_len);

  /* configure the uart */
  int divisor = 115200 / config->baud_rate;

  outp(config->io_port + 3, 0x80);            // set up to load divisor latch
  outp(config->io_port + 0, divisor & 0xff);  // lsb
  outp(config->io_port + 1, divisor >> 8);    // msb
  outp(config->io_port + 3, 3);               // 8N1
  outp(config->io_port + 2, 0x07);            // enable FIFO, clear, 14-byte threshold

  register_int_handler(config->irq, uart_irq_handler, dev);
  unmask_interrupt(config->irq);

  // outp(config->io_port + 1, 0x3); // enable rx data available and tx holding empty interrupts
  outp(config->io_port + 1, 0x1);  // enable rx data available interrupts

  thread_resume(
      thread_create("[uart writer]", uart_write_thread, dev, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));

done:
  return res;
}

static enum handler_return uart_irq_handler(void *arg) {
  bool resched = false;
  struct device *dev = arg;

  DEBUG_ASSERT(dev);
  DEBUG_ASSERT(dev->config);
  DEBUG_ASSERT(dev->state);

  const struct platform_uart_config *config = dev->config;
  struct uart_driver_state *state = dev->state;

  while (inp(config->io_port + 5) & (1 << 0)) {
    char c = inp(config->io_port + 0);
    cbuf_write(&state->rx_buf, &c, 1, false);
    resched = true;
  }

  return resched ? INT_RESCHEDULE : INT_NO_RESCHEDULE;
}

static int uart_write_thread(void *arg) {
  struct device *dev = arg;

  DEBUG_ASSERT(dev);
  DEBUG_ASSERT(dev->config);
  DEBUG_ASSERT(dev->state);

  const struct platform_uart_config *config = dev->config;
  struct uart_driver_state *state = dev->state;

  return 0;

  while (true) {
    char c = cbuf_read(&state->tx_buf, &c, 1, true);

    while ((inp(config->io_port + 5) & (1 << 6)) == 0)
      ;

    outp(config->io_port + 0, c);
  }

  return 0;
}

static ssize_t uart_read(struct device *dev, void *buf, size_t len) {
  if (!dev || !buf)
    return ERR_INVALID_ARGS;

  DEBUG_ASSERT(dev->state);
  struct uart_driver_state *state = dev->state;

  return cbuf_read(&state->rx_buf, buf, len, true);
}

static ssize_t uart_write(struct device *dev, const void *buf, size_t len) {
  if (!dev || !buf)
    return ERR_INVALID_ARGS;

  DEBUG_ASSERT(dev->state);
  struct uart_driver_state *state = dev->state;

  return cbuf_write(&state->tx_buf, buf, len, true);
}
