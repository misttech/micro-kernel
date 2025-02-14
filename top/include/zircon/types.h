// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOP_INCLUDE_MK_TYPES_H_
#define TOP_INCLUDE_MK_TYPES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <zircon/compiler.h>
// #include <zircon/errors.h>
// #include <zircon/limits.h>
// #include <zircon/rights.h>
// #include <zircon/time.h>

#ifndef __cplusplus
#ifndef _KERNEL
// We don't want to include <stdatomic.h> from the kernel code because the
// kernel definitions of atomic operations are incompatible with those defined
// in <stdatomic.h>.
//
// A better solution would be to use <stdatomic.h> and C11 atomic operation
// even in the kernel, but that would require modifying all the code that uses
// the existing homegrown atomics.
#include <stdatomic.h>
#endif
#endif

// `zx_status_t` is a simple error type used to communicate whether a particular
// action was successful or not. It is defined as being a signed 32-bit integer.
// The value `ZX_OK` (0) indicates an operation was successful. All other values
// indicate an error of some form.
//
// The range of valid `zx_status_t` values is `[-2^30, 0]` (that is,
// `-1073741824` to `0`). All values in this range will be system-defined error
// codes or the single success code `ZX_OK`.
//
// Application-defined error codes (previously defined as all positive
// `zx_status_t` values) are deprecated.
//
// * See errors.h for currently defined `zx_status_t` values.
// * See https://fuchsia.dev/fuchsia-src/contribute/governance/rfcs/0085_reducing_zx_status_t_space
//   for additional details about `zx_status_t`.
typedef int32_t zx_status_t;

#endif  // TOP_INCLUDE_MK_TYPES_H_
