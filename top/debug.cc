// Copyright 2025 Mist Tecnologia Ltda
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008-2015 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include "debug.h"

#include <lk/debug.h>
#include <platform.h>

#include <__verbose_abort>

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

  fprintf(get_panic_fd(),
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
  fprintf(get_panic_fd(), "\n");

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
  vfprintf(get_panic_fd(), fmt, ap);
  va_end(ap);

  // Add a newline to the end of the panic message if it was missing.
  if (!EndsWith(fmt, '\n')) {
    fprintf(get_panic_fd(), "\n");
  }

  PanicFinish();
}

}  // namespace

// Inline functions in libc++ headers call this.
[[noreturn]] void std::__libcpp_verbose_abort(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  vpanic(__GET_CALLER(), __GET_FRAME(), format, ap);
}
