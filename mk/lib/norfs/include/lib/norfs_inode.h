/*
 * Copyright (c) 2013 Heather Lee Wilson
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <stdint.h>

#include <lk/list.h>

struct norfs_inode {
  struct list_node lnode;
  uint32_t location;
  uint32_t reference_count;
};
