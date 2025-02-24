// Copyright 2022 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// DO NOT EDIT. Generated from FIDL library
//   zbi (//sdk/fidl/zbi/zbi.fidl)
// by zither, a Fuchsia platform tool.

#ifndef FIDL_ZBI_DATA_ASM_ZBI_H_
#define FIDL_ZBI_DATA_ASM_ZBI_H_

#define ZBI_ALIGNMENT (8)

// Numeric prefix for kernel types.
//
// 'KRN\0'
#define ZBI_TYPE_KERNEL_PREFIX (0x004e524b)

// Mask to compare against TYPE_KERNEL_PREFIX.
#define ZBI_TYPE_KERNEL_MASK (0x00ffffff)

// Numeric prefix for driver metadata types.
//
// 'm\0\0\0'
#define ZBI_TYPE_DRIVER_METADATA_PREFIX (0x0000006d)

// Mask to compare against TYPE_DRIVER_METADATA_PREFIX.
#define ZBI_TYPE_DRIVER_METADATA_MASK (0x000000ff)

// 'BOOT'
//
// Each ZBI starts with a container header.
//     length:          Total size of the image after this header.
//                      This includes all item headers, payloads, and padding.
//                      It does not include the container header itself.
//                      Must be a multiple of ZBI_ALIGNMENT.
//     extra:           Must be ZBI_CONTAINER_MAGIC.
//     flags:           Must be ZBI_FLAGS_VERSION and no other flags.
#define ZBI_TYPE_CONTAINER (0x544f4f42)

// x86-64 kernel. See zbi_kernel_t for a payload description.
//
// 'KRNL'
#define ZBI_TYPE_KERNEL_X64 (1280201291)  // 0x4c000000 | TYPE_KERNEL_PREFIX

// ARM64 kernel. See zbi_kernel_t for a payload description.
//
// KRN8
#define ZBI_TYPE_KERNEL_ARM64 (944656971)  // 0x38000000 | TYPE_KERNEL_PREFIX

// RISC-V kernel. See zbi_kernel_t for a payload description.
//
// 'KRNV'
#define ZBI_TYPE_KERNEL_RISCV64 (1447973451)  // 0x56000000 | TYPE_KERNEL_PREFIX

// A discarded item that should just be ignored.  This is used for an
// item that was already processed and should be ignored by whatever
// stage is now looking at the ZBI.  An earlier stage already "consumed"
// this information, but avoided copying data around to remove it from
// the ZBI item stream.
//
// 'SKIP'
#define ZBI_TYPE_DISCARD (0x50494b53)

// A virtual disk image.  This is meant to be treated as if it were a
// storage device.  The payload (after decompression) is the contents of
// the storage device, in whatever format that might be.
//
// 'RDSK'
#define ZBI_TYPE_STORAGE_RAMDISK (0x4b534452)

// The /boot filesystem in BOOTFS format, specified in
// <lib/zbi-format/internal/bootfs.h>.  This represents an internal
// contract between Zircon userboot (//docs/userboot.md), which handles
// the contents of this filesystem, and platform tooling, which prepares
// them.
//
// 'BFSB'
#define ZBI_TYPE_STORAGE_BOOTFS (0x42534642)

// Storage used by the kernel (such as a compressed image containing the
// actual kernel).  The meaning and format of the data is specific to the
// kernel, though it always uses the standard (private) storage
// compression protocol. Each particular KERNEL_{ARCH} item image and its
// STORAGE_KERNEL item image are intimately tied and one cannot work
// without the exact correct corresponding other.
//
// 'KSTR'
#define ZBI_TYPE_STORAGE_KERNEL (0x5254534b)

// Device-specific factory data, stored in BOOTFS format.
//
// TODO(https://fxbug.dev/42109921): This should not use the "STORAGE" infix.
//
// 'BFSF'
#define ZBI_TYPE_STORAGE_BOOTFS_FACTORY (0x46534642)

// A kernel command line fragment, a UTF-8 string that need not be
// NUL-terminated.  The kernel's own option parsing accepts only printable
// 'ASCI'I and treats all other characters as equivalent to whitespace. Multiple
// ZBI_TYPE_CMDLINE items can appear.  They are treated as if concatenated with
// ' ' between each item, in the order they appear: first items in the bootable
// ZBI containing the kernel; then items in the ZBI synthesized by the boot
// loader.  The kernel interprets the [whole command line](../../../../docs/kernel_cmdline.md).
//
// 'CMDL'
#define ZBI_TYPE_CMDLINE (0x4c444d43)

// The crash log from the previous boot, a UTF-8 string.
//
// 'BOOM'
#define ZBI_TYPE_CRASHLOG (0x4d4f4f42)

// Physical memory region that will persist across warm boots. See zbi_nvram_t
// for payload description.
//
// 'NVLL'
#define ZBI_TYPE_NVRAM (0x4c4c564e)

// Platform ID Information.
//
// 'PLID'
#define ZBI_TYPE_PLATFORM_ID (0x44494c50)

// Board-specific information.
//
// mBSI
#define ZBI_TYPE_DRV_BOARD_INFO (1230193261)  // 0x49534200 | TYPE_DRIVER_METADATA_PREFIX

// CPU configuration. See zbi_topology_node_t for a description of the payload.
#define ZBI_TYPE_CPU_TOPOLOGY (0x43505533)

// Device memory configuration. See zbi_mem_range_t for a description of the
// payload.
//
// 'MEMC'
#define ZBI_TYPE_MEM_CONFIG (0x434d454d)

// Kernel driver configuration.  The zbi_header_t.extra field gives a
// ZBI_KERNEL_DRIVER_* type that determines the payload format.
// See <lib/zbi-format/driver-config.h> for details.
//
// 'KDRV'
#define ZBI_TYPE_KERNEL_DRIVER (0x5652444b)

// 'ACPI' Root Table Pointer, a uint64_t physical address.
//
// 'RSDP'
#define ZBI_TYPE_ACPI_RSDP (0x50445352)

// 'SMBI'
//
// 'SMBI'OS entry point, a uint64_t physical address.
#define ZBI_TYPE_SMBIOS (0x49424d53)

// EFI system table, a uint64_t physical address.
//
// 'EFIS'
#define ZBI_TYPE_EFI_SYSTEM_TABLE (0x53494645)

// EFI memory attributes table. An example of this format can be found in UEFI 2.10 section 4.6.4,
// but the consumer of this item is responsible for interpreting whatever the bootloader supplies
// (in particular the "version" field may differ as the format evolves).
//
// 'EMAT'
#define ZBI_TYPE_EFI_MEMORY_ATTRIBUTES_TABLE (0x54414d45)

// Framebuffer parameters, a zbi_swfb_t entry.
//
// 'SWFB'
#define ZBI_TYPE_FRAMEBUFFER (0x42465753)

// The image arguments, data is a trivial text format of one "key=value" per line
// with leading whitespace stripped and "#" comment lines and blank lines ignored.
// It is processed by bootsvc and parsed args are shared to others via Arguments service.
// TODO: the format can be streamlined after the /config/additional_boot_args compat support is
// removed.
//
// 'IARG'
#define ZBI_TYPE_IMAGE_ARGS (0x47524149)

// A copy of the boot version stored within the sysconfig
// partition
//
// 'BVRS'
#define ZBI_TYPE_BOOT_VERSION (0x53525642)

// MAC address for Ethernet, Wifi, Bluetooth, etc.  zbi_header_t.extra
// is a board-specific index to specify which device the MAC address
// applies to.  zbi_header_t.length gives the size in bytes, which
// varies depending on the type of address appropriate for the device.
//
// mMAC
#define ZBI_TYPE_DRV_MAC_ADDRESS (1128353133)  // 0x43414d00 | TYPE_DRIVER_METADATA_PREFIX

// A partition map for a storage device, a zbi_partition_map_t header
// followed by one or more zbi_partition_t entries.  zbi_header_t.extra
// is a board-specific index to specify which device this applies to.
//
// mPRT
#define ZBI_TYPE_DRV_PARTITION_MAP (1414680685)  // 0x54525000 | TYPE_DRIVER_METADATA_PREFIX

// Private information for the board driver.
//
// mBOR
#define ZBI_TYPE_DRV_BOARD_PRIVATE (1380926061)  // 0x524f4200 | TYPE_DRIVER_METADATA_PREFIX

// 'HWRB'
#define ZBI_TYPE_HW_REBOOT_REASON (0x42525748)

// The serial number, an unterminated ASCII string of printable non-whitespace
// characters with length zbi_header_t.length.
//
// 'SRLN'
#define ZBI_TYPE_SERIAL_NUMBER (0x4e4c5253)

// This type specifies a binary file passed in by the bootloader.
// The first byte specifies the length of the filename without a NUL terminator.
// The filename starts on the second byte.
// The file contents are located immediately after the filename.
//
// Layout: | name_len |        name       |   payload
//           ^(1 byte)  ^(name_len bytes)     ^(length of file)
//
// 'BTFL'
#define ZBI_TYPE_BOOTLOADER_FILE (0x4c465442)

// The devicetree blob from the legacy boot loader, if any.  This is used only
// for diagnostic and development purposes.  Zircon kernel and driver
// configuration is entirely driven by specific ZBI items from the boot
// loader.  The boot shims for legacy boot loaders pass the raw devicetree
// along for development purposes, but extract information from it to populate
// specific ZBI items such as ZBI_TYPE_KERNEL_DRIVER et al.
#define ZBI_TYPE_DEVICETREE (0xd00dfeed)

// An arbitrary number of random bytes attested to have high entropy.  Any
// number of items of any size can be provided, but no data should be provided
// that is not true entropy of cryptographic quality.  This is used to seed
// secure cryptographic pseudo-random number generators.
//
// 'RAND'
#define ZBI_TYPE_SECURE_ENTROPY (0x444e4152)

// This provides a data dump and associated logging from a boot loader,
// shim, or earlier incarnation that wants its data percolated up by the
// booting Zircon kernel. See zbi_debugdata_t for a description of the
// payload.
//
// 'DBGD'
#define ZBI_TYPE_DEBUGDATA (0x44474244)

// Each RISC-V hart has an associated "ISA string" encoding its supported
// extensions: see Chapter 27 (ISA Extension Naming Conventions) in
// https://riscv.org/wp-content/uploads/2019/12/riscv-spec-20191213.pdf.
// This item gives the NUL-separated (and -terminated) concatenation of
// all such strings in the system.
//
// The first character of the table must be NUL; that is, the first entry
// in the table must be the empty and invalid, which is pointed to by a
// default-constructructed index. Beyond that, there are no guarantees to
// order the strings in the table or the extent of their (de)duplication.
//
// A given hart's index into the table is encoded within its associated
// node structure in the ZBI_TYPE_CPU_TOPOLOGY item.
//
// 'VISA'
#define ZBI_TYPE_RISCV64_ISA_STRTAB (0x41534956)

// LSW of sha256("bootdata")
#define ZBI_CONTAINER_MAGIC (0x868cf7e6)

// LSW of sha256("bootitem")
#define ZBI_ITEM_MAGIC (0xb5781729)

// Flags associated with an item.  A valid flags value must always include
// ZBI_FLAGS_VERSION. Values should also contain ZBI_FLAGS_CRC32 for any item
// where it's feasible to compute the CRC32 at build time.  Other flags are
// specific to each type.

// This flag is always required.

#define ZBI_FLAGS_VERSION_SHIFT (16)
#define ZBI_FLAGS_VERSION (1 << ZBI_FLAGS_VERSION_SHIFT)

// ZBI items with the CRC32 flag must have a valid crc32.
// Otherwise their crc32 field must contain ZBI_ITEM_NO_CRC32

#define ZBI_FLAGS_CRC32_SHIFT (17)
#define ZBI_FLAGS_CRC32 (1 << ZBI_FLAGS_CRC32_SHIFT)

// Value for zbi_header_t.crc32 when ZBI_FLAGS_CRC32 is not set.
#define ZBI_ITEM_NO_CRC32 (0x4a87e8d6)

// Each header must be 8-byte aligned.  The length field specifies the
// actual payload length and does not include the size of padding.
#define ZBI_HEADER_SIZEOF (32)

// ZBI_TYPE_* constant.
#define ZBI_HEADER_TYPE (0)

// Size of the payload immediately following this header.  This
// does not include the header itself nor any alignment padding
// after the payload.
#define ZBI_HEADER_LENGTH (4)

// Type-specific extra data.  Each type specifies the use of this
// field.  When not explicitly specified, it should be zero.
#define ZBI_HEADER_EXTRA (8)

// Flags for this item.
#define ZBI_HEADER_FLAGS (12)

// For future expansion.  Set to 0.
#define ZBI_HEADER_RESERVED0 (16)
#define ZBI_HEADER_RESERVED1 (20)

// Must be ZBI_ITEM_MAGIC.
#define ZBI_HEADER_MAGIC (24)

// Must be the CRC32 of payload if ZBI_FLAGS_CRC32 is set,
// otherwise must be ZBI_ITEM_NO_CRC32.
#define ZBI_HEADER_CRC32 (28)

#endif  // FIDL_ZBI_DATA_ASM_ZBI_H_