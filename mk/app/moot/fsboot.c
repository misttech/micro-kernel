/*
 * Copyright 2016 Google Inc. All Rights Reserved.
 * Author: gkalsi@google.com (Gurjant Kalsi)
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <lib/bio.h>
#include <lib/bootimage.h>
#include <lib/fs.h>
#include <stdio.h>

#include <app/moot/fsboot.h>
#include <app/moot/stubs.h>
#include <lk/compiler.h>
#include <lk/err.h>
#include <lk/trace.h>

#define MAX_FPATH_LEN 64

#define LOCAL_TRACE 0

// Attempt to boot from the filesystem.
void attempt_fs_boot(void) {
  char *mount_path, *device_name;
  bootimage_t *bi;

  status_t retcode = moot_mount_default_fs(&mount_path, &device_name);
  if (retcode != NO_ERROR) {
    LTRACEF("Failed: Unable to mount default fs. retcode = %d\n", retcode);
    return;
  }

  char fpath[MAX_FPATH_LEN];
  snprintf(fpath, MAX_FPATH_LEN, "%s/system.img", mount_path);

  filehandle *handle;
  retcode = fs_open_file(fpath, &handle);
  if (retcode != NO_ERROR) {
    LTRACEF("Failed: to open recovery file: '%s'. retcode = %d\n", fpath, retcode);
    goto finish;
  }

  // Fill in the length of the bootimage
  struct file_stat stat;
  retcode = fs_stat_file(handle, &stat);
  if (retcode != NO_ERROR) {
    LTRACEF("Failed: to stat recovery file: '%s'. retcode = %d\n", fpath, retcode);
    goto finish;
  }

  // Get the address of the Bootimage.
  // TODO: At some point we might have a flash device/controller that doesn't
  // support linear mode. In that case we'll have to come up with a mechanism
  // that can verify a bootimg without memory mapping the file.
  unsigned char *address = 0;
  retcode = fs_file_ioctl(handle, FS_IOCTL_GET_FILE_ADDR, &address);
  fs_close_file(handle);

  if (retcode != NO_ERROR) {
    LTRACEF("Failed: to get file memmap for '%s'. retcode = %d\n", fpath, retcode);
    goto finish;
  }

  bdev_t *secondary_flash = bio_open(device_name);
  if (!secondary_flash) {
    LTRACEF(
        "Failed: Unable to open secondary flash at '%s'. "
        "retcode = %d\n",
        device_name, retcode);
    goto finish;
  }

  unsigned char *unused = 0;
  retcode = bio_ioctl(secondary_flash, BIO_IOCTL_GET_MEM_MAP, &unused);
  bio_close(secondary_flash);

  if (retcode != NO_ERROR) {
    LTRACEF(
        "Failed: to get file memmap for '%s'. "
        "retcode = %d\n",
        device_name, retcode);
    goto finish;
  }

  retcode = bootimage_open(address, stat.size, &bi);
  if (retcode != NO_ERROR) {
    LTRACEF("Failed: Unable to open bootimage. retcode = %d\n", retcode);
    goto finish;
  }

  size_t imglen;
  const void *imgptr;
  retcode = bootimage_get_file_section(bi, TYPE_LK, &imgptr, &imglen);
  if (retcode != NO_ERROR) {
    LTRACEF("Failed: Unable to find lk section. retcode = %d\n", retcode);
    goto finish;
  }

  // Flash the new image.
  bdev_t *system_flash = bio_open(moot_system_info.system_flash_name);
  if (!system_flash) {
    LTRACEF("Failed: Unable to open system flash at '%s'.\n", moot_system_info.system_flash_name);
    goto finish;
  }

  ssize_t n_bytes_erased = bio_erase(system_flash, moot_system_info.system_offset, imglen);
  if (n_bytes_erased < (ssize_t)imglen) {
    LTRACEF("Failed: Unable to erase system flash at '%s'. retcode = %ld\n",
            moot_system_info.system_flash_name, n_bytes_erased);
    bio_close(system_flash);
    goto finish;
  }

  ssize_t written = bio_write(system_flash, imgptr, moot_system_info.system_offset, imglen);
  bio_close(system_flash);

  if (written < (ssize_t)imglen) {
    LTRACEF("Failed: Unable to write system flash at '%s'. retcode = %ld\n",
            moot_system_info.system_flash_name, written);
    goto finish;
  }

finish:
  fs_unmount(mount_path);
}
