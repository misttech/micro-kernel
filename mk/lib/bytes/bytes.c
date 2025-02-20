/*
 * Copyright (c) 2013 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lib/bytes.h>
#include <stdint.h>

// Big endian interfaces
uint16_t bytes_read_u16_be(const uint8_t *buf) {
  uint16_t val;

  val = (buf[0] << 8) | buf[1];

  return (val);
}

uint32_t bytes_read_u24_be(const uint8_t *buf) {
  uint32_t val;

  val = (buf[0] << 16) | (buf[1] << 8) | buf[2];

  return (val);
}

uint32_t bytes_read_u32_be(const uint8_t *buf) {
  uint32_t val;

  val = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];

  return (val);
}

uint8_t *bytes_write_u16_be(uint8_t *buf, uint16_t val) {
  *buf++ = (val >> 8) & 0xFF;
  *buf++ = (val) & 0xFF;

  return (buf);
}

uint8_t *bytes_write_u24_be(uint8_t *buf, uint32_t val) {
  *buf++ = (val >> 16) & 0xFF;
  *buf++ = (val >> 8) & 0xFF;
  *buf++ = (val) & 0xFF;

  return (buf);
}

uint8_t *bytes_write_u32_be(uint8_t *buf, uint32_t val) {
  *buf++ = (val >> 24) & 0xFF;
  *buf++ = (val >> 16) & 0xFF;
  *buf++ = (val >> 8) & 0xFF;
  *buf++ = (val) & 0xFF;

  return (buf);
}

// Little endian interfaces
uint16_t bytes_read_u16_le(const uint8_t *buf) {
  uint16_t val;

  val = (buf[1] << 8) | buf[0];

  return (val);
}

uint32_t bytes_read_u24_le(const uint8_t *buf) {
  uint32_t val;

  val = (buf[2] << 16) | (buf[1] << 8) | buf[0];

  return (val);
}

uint32_t bytes_read_u32_le(const uint8_t *buf) {
  uint32_t val;

  val = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

  return (val);
}

uint8_t *bytes_write_u16_le(uint8_t *buf, uint16_t val) {
  *buf++ = (val) & 0xFF;
  *buf++ = (val >> 8) & 0xFF;

  return (buf);
}

uint8_t *bytes_write_u24_le(uint8_t *buf, uint32_t val) {
  *buf++ = (val) & 0xFF;
  *buf++ = (val >> 8) & 0xFF;
  *buf++ = (val >> 16) & 0xFF;

  return (buf);
}

uint8_t *bytes_write_u32_le(uint8_t *buf, uint32_t val) {
  *buf++ = (val) & 0xFF;
  *buf++ = (val >> 8) & 0xFF;
  *buf++ = (val >> 16) & 0xFF;
  *buf++ = (val >> 24) & 0xFF;

  return (buf);
}

uint8_t bytes_swap_bits_u8(uint8_t val) {
  val = ((val >> 1) & 0x55) | ((val & 0x55) << 1);
  val = ((val >> 2) & 0x33) | ((val & 0x33) << 2);
  val = ((val >> 4) & 0x0f) | ((val & 0x0f) << 4);

  return (val);
}

uint16_t bytes_swap_bits_u16(uint16_t val) {
  val = ((val >> 1) & 0x5555) | ((val & 0x5555) << 1);
  val = ((val >> 2) & 0x3333) | ((val & 0x3333) << 2);
  val = ((val >> 4) & 0x0f0f) | ((val & 0x0f0f) << 4);
  val = ((val >> 8) & 0x00ff) | ((val & 0x00ff) << 8);

  return (val);
}

uint32_t bytes_swap_bits_u24(uint32_t val) {
  val &= 0x00FFFFFF;
  val = ((val >> 1) & 0x555555) | ((val & 0x555555) << 1);
  val = ((val >> 2) & 0x333333) | ((val & 0x333333) << 2);
  val = ((val >> 4) & 0x0f0f0f) | ((val & 0x0f0f0f) << 4);
  val = ((val >> 16) & 0x0000ff) | ((val & 0x0000ff) << 16) | (val & 0x00FF00);

  return (val);
}

uint32_t bytes_swap_bits_u32(uint32_t val) {
  val = ((val >> 1) & 0x55555555) | ((val & 0x55555555) << 1);
  val = ((val >> 2) & 0x33333333) | ((val & 0x33333333) << 2);
  val = ((val >> 4) & 0x0f0f0f0f) | ((val & 0x0f0f0f0f) << 4);
  val = ((val >> 8) & 0x00ff00ff) | ((val & 0x00ff00ff) << 8);
  val = ((val >> 16) & 0x0000ffff) | ((val & 0x0000ffff) << 16);

  return (val);
}
