/*
 * Copyright (c) 2013 Corey Tabaka
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <stdint.h>

#include <lk/compiler.h>

#define REG_APROM 0x00
#define REG_RDP 0x10
#define REG_RAP 0x14
#define REG_RESET 0x18
#define REG_BDP 0x1c

#define CSR0_INIT 0x0001
#define CSR0_STRT 0x0002
#define CSR0_STOP 0x0004
#define CSR0_TDMD 0x0008
#define CSR0_TXON 0x0010
#define CSR0_RXON 0x0020
#define CSR0_IENA 0x0040
#define CSR0_INTR 0x0080
#define CSR0_IDON 0x0100
#define CSR0_TINT 0x0200
#define CSR0_RINT 0x0400
#define CSR0_MERR 0x0800
#define CSR0_MISS 0x1000
#define CSR0_CERR 0x2000
#define CSR0_BABL 0x4000
#define CSR0_ERR 0x8000

#define CSR4_DMAPLUS 0x4000

#define DESC_SIZE (4 * sizeof(uint32_t))

struct init_block_32 {
  uint16_t mode;

  uint16_t reserved_0 : 4;
  uint16_t rlen : 4;
  uint16_t reserved_1 : 4;
  uint16_t tlen : 4;

  uint8_t padr[6];

  uint16_t reserved_2;

  uint64_t ladr;
  uint32_t rdra;
  uint32_t tdra;
} __PACKED;

struct td_style3 {
  uint32_t trc : 4;
  uint32_t reserved_1 : 8;
  uint32_t tdr : 14;
  uint32_t rtry : 1;
  uint32_t lcar : 1;
  uint32_t lcol : 1;
  uint32_t exdef : 1;
  uint32_t uflo : 1;
  uint32_t buff : 1;

  uint32_t bcnt : 12;
  uint32_t ones : 4;
  uint32_t reserved_0 : 7;
  uint32_t bpe : 1;
  uint32_t enp : 1;
  uint32_t stp : 1;
  uint32_t def : 1;
  uint32_t one : 1;
  uint32_t more_ltinit : 1;
  uint32_t add_no_fcs : 1;
  uint32_t err : 1;
  uint32_t own : 1;

  uint32_t tbadr;

  uint32_t reserved_2;
} __PACKED;

struct rd_style3 {
  uint16_t mcnt : 12;
  uint16_t zeros : 4;

  uint8_t rpc;
  uint8_t rcc;

  uint32_t bcnt : 12;
  uint32_t ones : 4;
  uint32_t reserved_0 : 4;
  uint32_t bam : 1;
  uint32_t lafm : 1;
  uint32_t pam : 1;
  uint32_t bpe : 1;
  uint32_t enp : 1;
  uint32_t stp : 1;
  uint32_t buff : 1;
  uint32_t crc : 1;
  uint32_t oflo : 1;
  uint32_t fram : 1;
  uint32_t err : 1;
  uint32_t own : 1;

  uint32_t rbadr;

  uint32_t reserved_1;
} __PACKED;
