/*
 * Copyright (c) 2012 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <target.h>

#include <dev/gpio.h>
#include <lk/compiler.h>
#include <lk/debug.h>
#include <lk/err.h>
#include <lk/trace.h>
#include <platform/gpio.h>
#include <platform/stm32.h>
#include <target/debugconfig.h>
#include <target/m4display.h>

void target_early_init(void) { stm32_debug_early_init(); }

void target_init(void) {
  TRACE_ENTRY;

  stm32_debug_init();

  init_display();

  TRACE_EXIT;
}