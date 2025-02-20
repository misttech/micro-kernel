/*
 * Copyright (c) 2009 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <lib/fs.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lk/console_cmd.h>
#include <lk/debug.h>
#include <lk/err.h>

#if LK_DEBUGLEVEL > 1
static int cmd_fs(int argc, const console_cmd_args *argv);

STATIC_COMMAND_START
STATIC_COMMAND("fs", "fs debug commands", &cmd_fs)
STATIC_COMMAND_END(fs);

extern int fs_mount_type(const char *path, const char *device, const char *name);

static int cmd_fs_ioctl(int argc, const console_cmd_args *argv) {
  if (argc < 3) {
    printf("not enough arguments\n");
    return ERR_INVALID_ARGS;
  }

  int request = argv[2].u;

  switch (request) {
    case FS_IOCTL_GET_FILE_ADDR: {
      if (argc < 4) {
        printf("%s %s %lu <path>\n", argv[0].str, argv[1].str, argv[2].u);
        return ERR_INVALID_ARGS;
      }

      int err;
      filehandle *handle;
      err = fs_open_file(argv[3].str, &handle);
      if (err != NO_ERROR) {
        printf("error %d opening file\n", err);
        return err;
      }

      void *file_addr;
      err = fs_file_ioctl(handle, request, &file_addr);
      if (err != NO_ERROR) {
        fs_close_file(handle);
        return err;
      }

      printf("%s is mapped at %p\n", argv[3].str, file_addr);

      return fs_close_file(handle);
      break;
    }
    case FS_IOCTL_IS_LINEAR: {
      if (argc < 4) {
        printf("%s %s %lu <path>\n", argv[0].str, argv[1].str, argv[2].u);
        return ERR_INVALID_ARGS;
      }

      int err;
      filehandle *handle;
      err = fs_open_file(argv[3].str, &handle);
      if (err != NO_ERROR) {
        printf("error %d opening file\n", err);
        return err;
      }

      bool is_mapped;
      err = fs_file_ioctl(handle, request, (void **)&is_mapped);
      if (err != NO_ERROR) {
        fs_close_file(handle);
        return err;
      }

      if (is_mapped) {
        printf("file at %s is memory mapped\n", argv[3].str);
      } else {
        printf("file at %s is not memory mapped\n", argv[3].str);
      }

      return fs_close_file(handle);
      break;
    }
    default: {
      printf("error, unsupported ioctl: %d\n", request);
    }
  }

  return ERR_NOT_SUPPORTED;
}

static int cmd_fs(int argc, const console_cmd_args *argv) {
  int rc = 0;

  if (argc < 2) {
  notenoughargs:
    printf("not enough arguments:\n");
  usage:
    printf("%s mount <path> <type> [device]\n", argv[0].str);
    printf("%s unmount <path>\n", argv[0].str);
    printf("%s write <path> <string> [<offset>]\n", argv[0].str);
    printf("%s format <type> [device]\n", argv[0].str);
    printf("%s stat <path>\n", argv[0].str);
    printf("%s ioctl <request> [args...]\n", argv[0].str);
    printf("%s list\n", argv[0].str);
    return -1;
  }

  if (!strcmp(argv[1].str, "mount")) {
    int err;

    if (argc < 4)
      goto notenoughargs;

    err = fs_mount(argv[2].str, argv[3].str, (argc >= 5) ? argv[4].str : NULL);

    if (err < 0) {
      printf("error %d mounting device\n", err);
      return err;
    }
  } else if (!strcmp(argv[1].str, "unmount")) {
    int err;

    if (argc < 3)
      goto notenoughargs;

    err = fs_unmount(argv[2].str);
    if (err < 0) {
      printf("error %d unmounting device\n", err);
      return err;
    }
  } else if (!strcmp(argv[1].str, "format")) {
    int err;

    if (argc < 3) {
      goto notenoughargs;
    }

    err = fs_format_device(argv[2].str, (argc >= 4) ? argv[3].str : NULL, NULL);

    if (err != NO_ERROR) {
      printf("error %d formatting device\n", err);
      return err;
    }

  } else if (!strcmp(argv[1].str, "stat")) {
    int err;

    if (argc < 3) {
      goto notenoughargs;
    }

    struct fs_stat stat;
    err = fs_stat_fs(argv[2].str, &stat);

    if (err != NO_ERROR) {
      printf("error %d statting filesystem\n", err);
      return err;
    }

    printf("\ttotal bytes: %llu\n", stat.total_space);
    printf("\tfree bytes: %llu\n", stat.free_space);
    printf("\n");
    printf("\ttotal inodes: %d\n", stat.total_inodes);
    printf("\tfree inodes: %d\n", stat.free_inodes);

  } else if (!strcmp(argv[1].str, "ioctl")) {
    return cmd_fs_ioctl(argc, argv);
  } else if (!strcmp(argv[1].str, "write")) {
    int err;
    off_t off;
    filehandle *handle;
    struct file_stat stat;

    if (argc < 3)
      goto notenoughargs;

    err = fs_open_file(argv[2].str, &handle);
    if (err < 0) {
      printf("error %d opening file\n", err);
      return err;
    }

    err = fs_stat_file(handle, &stat);
    if (err < 0) {
      printf("error %d stat'ing file\n", err);
      fs_close_file(handle);
      return err;
    }

    if (argc < 5)
      off = stat.size;
    else
      off = argv[4].u;

    err = fs_write_file(handle, argv[3].str, off, strlen(argv[3].str));
    if (err < 0) {
      printf("error %d writing file\n", err);
      fs_close_file(handle);
      return err;
    }

    fs_close_file(handle);
  } else if (!strcmp(argv[1].str, "list")) {
    printf("Implemented file systems:\n");
    fs_dump_list();
  } else {
    printf("unrecognized subcommand\n");
    goto usage;
  }

  return rc;
}

#endif
