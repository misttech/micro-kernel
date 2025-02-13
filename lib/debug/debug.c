/*
 * Copyright (c) 2008-2015 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <assert.h>
#include <ctype.h>
#include <platform.h>
#include <printf.h>
#include <stdio.h>
#include <stdlib.h>

#include <arch/ops.h>
#include <kernel/spinlock.h>
#include <lk/debug.h>
#include <lk/list.h>
#include <platform/debug.h>

void spin(uint32_t usecs) {
  lk_bigtime_t start = current_time_hires();

  while ((current_time_hires() - start) < usecs)
    ;
}

void panic(const char *fmt, ...) {
  printf("panic (caller %p): ", __GET_CALLER());

  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  platform_halt(HALT_ACTION_HALT, HALT_REASON_SW_PANIC);
}

void assert_fail_msg(const char *file, int line, const char *expression, const char *fmt, ...) {
  // Print the user message.
  printf("ASSERT FAILED at (%s:%d): %s\n", file, line, expression);
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  platform_halt(HALT_ACTION_HALT, HALT_REASON_SW_PANIC);
}

void assert_fail(const char *file, int line, const char *expression) {
  printf("ASSERT FAILED at (%s:%d): %s\n", file, line, expression);
  platform_halt(HALT_ACTION_HALT, HALT_REASON_SW_PANIC);
}

#if !DISABLE_DEBUG_OUTPUT

static ssize_t __panic_stdio_read(io_handle_t *io, char *s, size_t len) {
  if (len == 0)
    return 0;

  int err = platform_pgetc(s, false);
  if (err < 0)
    return err;

  return 1;
}

static ssize_t __panic_stdio_write(io_handle_t *io, const char *s, size_t len) {
  for (size_t i = 0; i < len; i++) {
    platform_pputc(s[i]);
  }
  return len;
}

FILE *get_panic_fd(void) {
  static const io_handle_hooks_t panic_hooks = {
      .write = __panic_stdio_write,
      .read = __panic_stdio_read,
  };
  static io_handle_t panic_io = {.magic = IO_HANDLE_MAGIC, .hooks = &panic_hooks};
  static FILE panic_fd = {.io = &panic_io};

  return &panic_fd;
}

#endif  // !DISABLE_DEBUG_OUTPUT
