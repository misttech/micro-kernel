/*
 * Copyright (c) 2025 Mist Tecnologia Ltda
 * Copyright (c) 2008-2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#ifndef __APP_TESTS_H
#define __APP_TESTS_H

#include <lk/console_cmd.h>

int cbuf_tests(int argc, const cmd_args *argv, uint32_t flags);
int fibo(int argc, const cmd_args *argv, uint32_t flags);
int port_tests(int argc, const cmd_args *argv, uint32_t flags);
int spinner(int argc, const cmd_args *argv, uint32_t flags);
int thread_tests(int argc, const cmd_args *argv, uint32_t flags);
int benchmarks(int argc, const cmd_args *argv, uint32_t flags);
int clock_tests(int argc, const cmd_args *argv, uint32_t flags);
int printf_tests(int argc, const cmd_args *argv, uint32_t flags);
int printf_tests_float(int argc, const cmd_args *argv, uint32_t flags);
int v9p_tests(int argc, const cmd_args *argv, uint32_t flags);
int v9fs_tests(int argc, const cmd_args *argv, uint32_t flags);

#endif
