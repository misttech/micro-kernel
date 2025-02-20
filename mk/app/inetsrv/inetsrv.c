/*
 * Copyright (c) 2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include "inetsrv.h"

#include <app.h>
#include <lib/cksum.h>
#include <lib/minip.h>
#include <lib/tftp.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/thread.h>
#include <lk/compiler.h>
#include <lk/debug.h>
#include <lk/err.h>
#include <lk/trace.h>

static int chargen_worker(void *socket) {
  uint64_t count = 0;
  tcp_socket_t *s = socket;

  /* enough buffer to hold an entire defacto chargen sequences */
#define CHARGEN_BUFSIZE (0x5f * 0x5f)  // 9025 bytes

  uint8_t *buf = malloc(CHARGEN_BUFSIZE);
  if (!buf)
    return ERR_NO_MEMORY;

  /* generate the sequence */
  uint8_t c = '!';
  for (size_t i = 0; i < CHARGEN_BUFSIZE; i++) {
    buf[i] = c++;
    if (c == 0x7f)
      c = ' ';
  }

  lk_time_t t = current_time();
  for (;;) {
    ssize_t ret = tcp_write(s, buf, CHARGEN_BUFSIZE);
    // TRACEF("tcp_write returns %d\n", ret);
    if (ret < 0)
      break;

    count += ret;
  }
  t = current_time() - t;

  TRACEF("chargen worker exiting, wrote %llu bytes in %u msecs (%llu bytes/sec)\n", count,
         (uint32_t)t, count * 1000 / t);
  free(buf);
  tcp_close(s);

  return 0;
}

static int chargen_server(void *arg) {
  status_t err;
  tcp_socket_t *listen_socket;

  err = tcp_open_listen(&listen_socket, 19);
  if (err < 0) {
    TRACEF("error opening chargen listen socket\n");
    return -1;
  }

  for (;;) {
    tcp_socket_t *accept_socket;

    err = tcp_accept(listen_socket, &accept_socket);
    TRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
    if (err < 0) {
      TRACEF("error accepting socket, retrying\n");
      continue;
    }

    TRACEF("starting chargen worker\n");
    thread_detach_and_resume(thread_create("chargen_worker", &chargen_worker, accept_socket,
                                           DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  }
}

static int discard_worker(void *socket) {
  uint64_t count = 0;
  uint32_t crc = 0;
  tcp_socket_t *s = socket;

#define DISCARD_BUFSIZE 1024

  uint8_t *buf = malloc(DISCARD_BUFSIZE);
  if (!buf) {
    TRACEF("error allocating buffer\n");
  }

  lk_time_t t = current_time();
  for (;;) {
    ssize_t ret = tcp_read(s, buf, DISCARD_BUFSIZE);
    if (ret <= 0)
      break;

    crc = crc32(crc, buf, ret);

    count += ret;
  }
  t = current_time() - t;

  TRACEF("discard worker exiting, read %llu bytes in %u msecs (%llu bytes/sec), crc32 0x%x\n",
         count, (uint32_t)t, count * 1000 / t, crc);
  tcp_close(s);

  free(buf);

  return 0;
}

static int discard_server(void *arg) {
  status_t err;
  tcp_socket_t *listen_socket;

  err = tcp_open_listen(&listen_socket, 9);
  if (err < 0) {
    TRACEF("error opening discard listen socket\n");
    return -1;
  }

  for (;;) {
    tcp_socket_t *accept_socket;

    err = tcp_accept(listen_socket, &accept_socket);
    TRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
    if (err < 0) {
      TRACEF("error accepting socket, retrying\n");
      continue;
    }

    TRACEF("starting discard worker\n");
    thread_detach_and_resume(thread_create("discard_worker", &discard_worker, accept_socket,
                                           DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  }
}

static int echo_worker(void *socket) {
  tcp_socket_t *s = socket;

#define ECHO_BUFSIZE 1024

  uint8_t *buf = malloc(ECHO_BUFSIZE);
  if (!buf) {
    TRACEF("error allocating buffer\n");
    return ERR_NO_MEMORY;
  }

  for (;;) {
    ssize_t ret = tcp_read(s, buf, sizeof(buf));
    if (ret <= 0)
      break;

    tcp_write(s, buf, ret);
    if (ret <= 0)
      break;
  }

  TRACEF("echo worker exiting\n");
  tcp_close(s);
  free(buf);

  return 0;
}

static int echo_server(void *arg) {
  status_t err;
  tcp_socket_t *listen_socket;

  err = tcp_open_listen(&listen_socket, 7);
  if (err < 0) {
    TRACEF("error opening echo listen socket\n");
    return -1;
  }

  for (;;) {
    tcp_socket_t *accept_socket;

    err = tcp_accept(listen_socket, &accept_socket);
    TRACEF("tcp_accept returns returns %d, handle %p\n", err, accept_socket);
    if (err < 0) {
      TRACEF("error accepting socket, retrying\n");
      continue;
    }

    TRACEF("starting echo worker\n");
    thread_detach_and_resume(thread_create("echo_worker", &echo_worker, accept_socket,
                                           DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  }
}

static void inetsrv_init(const struct app_descriptor *app) {}

static void inetsrv_entry(const struct app_descriptor *app, void *args) {
  /* XXX wait for the stack to initialize */

  printf("starting internet servers\n");

  thread_detach_and_resume(
      thread_create("chargen", &chargen_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  thread_detach_and_resume(
      thread_create("discard", &discard_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  thread_detach_and_resume(
      thread_create("echo", &echo_server, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  tftp_server_init(NULL);
}

APP_START(inetsrv).init = inetsrv_init, .entry = inetsrv_entry, .flags = 0, APP_END
