/*
 * Copyright (c) 2009 Corey Tabaka
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <arch/x86/descriptor.h>
#include <lk/compiler.h>

/* not the best way to do this, but easy for now */
typedef union {
  struct {
    uint16_t limit_15_0;
    uint16_t base_15_0;
    uint8_t base_23_16;

    uint8_t type : 4;
    uint8_t s : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;

    uint8_t limit_19_16 : 4;
    uint8_t avl : 1;
    uint8_t reserved_0 : 1;
    uint8_t d_b : 1;
    uint8_t g : 1;

    uint8_t base_31_24;
  } __PACKED seg_desc_legacy;

  struct {
    uint32_t base_63_32;
    uint32_t reserved_1;
  } __PACKED seg_desc_64;
} __PACKED seg_desc_t;

extern seg_desc_t _gdt[];

void set_global_desc(seg_sel_t sel, void *base, uint32_t limit, uint8_t present, uint8_t ring,
                     uint8_t sys, uint8_t type, uint8_t gran, uint8_t bits) {
  // convert selector into index
  uint16_t index = sel >> 3;

  _gdt[index].seg_desc_legacy.limit_15_0 = limit & 0x0000ffff;
  _gdt[index].seg_desc_legacy.limit_19_16 = (limit & 0x000f0000) >> 16;

  _gdt[index].seg_desc_legacy.base_15_0 = ((uintptr_t)base) & 0x0000ffff;
  _gdt[index].seg_desc_legacy.base_23_16 = (((uintptr_t)base) & 0x00ff0000) >> 16;
  _gdt[index].seg_desc_legacy.base_31_24 = ((uintptr_t)base) >> 24;

  _gdt[index].seg_desc_legacy.type = type & 0x0f;  // segment type
  _gdt[index].seg_desc_legacy.p = present != 0;    // present
  _gdt[index].seg_desc_legacy.dpl = ring & 0x03;   // descriptor privilege level
  _gdt[index].seg_desc_legacy.g = gran != 0;       // granularity
  _gdt[index].seg_desc_legacy.s = sys != 0;        // system / non-system
  _gdt[index].seg_desc_legacy.d_b = bits != 0;     // 16 / 32 bit

#ifdef ARCH_X86_64
  if (TSS_SELECTOR == sel) {
    _gdt[index + 1].seg_desc_64.base_63_32 = (uint32_t)((uintptr_t)base >> 32);
    _gdt[index + 1].seg_desc_64.reserved_1 = 0;
  }
#endif
}
