// Copyright 2025 Mist Tecnologia Ltda
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008-2015 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include "debug.h"

#include <align.h>
#include <ctype.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <zircon/types.h>

#include <__verbose_abort>

#include <arch/ops.h>
#include <kernel/spinlock.h>
#include <ktl/algorithm.h>
#include <platform/debug.h>

// Note <ktl/enforce.h> can't be used here because we need to define a function
// that was declared in the std:: namespace by <__verbose_abort>.

static ssize_t __panic_stdio_read(io_handle_t* io, char* s, size_t len) {
  if (len == 0)
    return 0;

  int err = platform_pgetc(s, false);
  if (err < 0)
    return err;

  return 1;
}

static ssize_t __panic_stdio_write(io_handle_t* io, const char* s, size_t len) {
  for (size_t i = 0; i < len; i++) {
    platform_pputc(s[i]);
  }
  return len;
}

static const io_handle_hooks_t panic_hooks = {
    .write = __panic_stdio_write,
    .read = __panic_stdio_read,
};

static io_handle_t panic_io = {
    .magic = IO_HANDLE_MAGIC,
    .hooks = &panic_hooks,
};

FILE stdout_panic_buffer = {
    .io = &panic_io,
};

namespace {

// Start a system panic, and print a header message.
//
// Calls should be followed by:
//
//   * Calling "printf" with the reason for the panic, followed by
//     a newline.
//
//   * A call to "PanicFinish".
__ALWAYS_INLINE inline void PanicStart(void* pc, void* frame) {
  // platform_panic_start();

  fprintf(&stdout_panic_buffer,
          "\n"
          "*** KERNEL PANIC (caller pc: %p, stack frame: %p):\n"
          "*** ",
          pc, frame);
}

// Finish a system panic.
//
// This function will not return, but will perform an action such as
// rebooting the system or dropping the system into a debug shell.
//
// Marked "__ALWAYS_INLINE" to avoid an additional stack frame from
// appearing in the backtrace.
__ALWAYS_INLINE __NO_RETURN inline void PanicFinish() {
  // Add a newline between the panic message and the stack trace.
  fprintf(&stdout_panic_buffer, "\n");

  // platform_halt(HALT_ACTION_HALT, ZirconCrashReason::Panic);
  platform_halt(HALT_ACTION_HALT, HALT_REASON_SW_PANIC);
}

// Determine if the given string ends with the given character.
bool EndsWith(const char* str, char x) {
  size_t len = strlen(str);
  return len > 0 && str[len - 1] == x;
}

[[noreturn]] void vpanic(void* pc, void* frame, const char* fmt, va_list ap) {
  PanicStart(pc, frame);

  // Print the user message.
  vfprintf(&stdout_panic_buffer, fmt, ap);
  va_end(ap);

  // Add a newline to the end of the panic message if it was missing.
  if (!EndsWith(fmt, '\n')) {
    fprintf(&stdout_panic_buffer, "\n");
  }

  PanicFinish();
}

}  // namespace

void spin(uint32_t usecs) {
  lk_bigtime_t start = current_time_hires();

  while ((current_time_hires() - start) < usecs)
    ;
}

void panic(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vpanic(__GET_CALLER(), __GET_FRAME(), fmt, ap);
}

// Inline functions in libc++ headers call this.
[[noreturn]] void std::__libcpp_verbose_abort(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  vpanic(__GET_CALLER(), __GET_FRAME(), format, ap);
}

void assert_fail_msg(const char* file, int line, const char* expression, const char* fmt, ...) {
  PanicStart(__GET_CALLER(), __GET_FRAME());

  // Print the user message.
  fprintf(&stdout_panic_buffer, "ASSERT FAILED at (%s:%d): %s\n", file, line, expression);
  va_list ap;
  va_start(ap, fmt);
  vfprintf(&stdout_panic_buffer, fmt, ap);
  va_end(ap);

  // Add a newline to the end of the panic message if it was missing.
  if (!EndsWith(fmt, '\n')) {
    fprintf(&stdout_panic_buffer, "\n");
  }

  PanicFinish();
}

void assert_fail(const char* file, int line, const char* expression) {
  PanicStart(__GET_CALLER(), __GET_FRAME());
  fprintf(&stdout_panic_buffer, "ASSERT FAILED at (%s:%d): %s\n", file, line, expression);
  PanicFinish();
}
