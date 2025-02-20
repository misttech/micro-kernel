
// Copyright 2025 Mist Tecnologia Ltda
// Copyright (c) 2008-2014 Travis Geiselbrecht
// Copyright (c) 2012 Shantanu Gupta
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef MK_INCLUDE_KERNEL_MUTEX_H_
#define MK_INCLUDE_KERNEL_MUTEX_H_

#include <stdint.h>

#include <kernel/thread.h>
#include <lk/compiler.h>
#include <lk/debug.h>

__BEGIN_CDECLS

#define MUTEX_MAGIC (0x6D757478)  // 'mutx'

typedef struct mutex {
  uint32_t magic;
  thread_t *holder;
  int count;
  wait_queue_t wait;
} mutex_t;

#define MUTEX_INITIAL_VALUE(m)                    \
  {                                               \
      .magic = MUTEX_MAGIC,                       \
      .holder = NULL,                             \
      .count = 0,                                 \
      .wait = WAIT_QUEUE_INITIAL_VALUE((m).wait), \
  }

/* Rules for Mutexes:
 * - Mutexes are only safe to use from thread context.
 * - Mutexes are non-recursive.
 */

void mutex_init(mutex_t *);
void mutex_destroy(mutex_t *);
status_t mutex_acquire_timeout(mutex_t *,
                               lk_time_t); /* try to acquire the mutex with a timeout value */
status_t mutex_release(mutex_t *);

static inline status_t mutex_acquire(mutex_t *m) { return mutex_acquire_timeout(m, INFINITE_TIME); }

/* does the current thread hold the mutex? */
static bool is_mutex_held(const mutex_t *m) { return m->holder == get_current_thread(); }

__END_CDECLS

#ifdef __cplusplus

#include <lk/cpp.h>

// C++ wrapper object
class Mutex {
 public:
  constexpr Mutex() = default;
  ~Mutex() { mutex_destroy(&lock_); }

  status_t acquire(lk_time_t timeout = INFINITE_TIME) {
    return mutex_acquire_timeout(&lock_, timeout);
  }
  status_t release() { return mutex_release(&lock_); }
  bool is_held() { return is_mutex_held(&lock_); }

  // suppress default constructors
  DISALLOW_COPY_ASSIGN_AND_MOVE(Mutex);

 private:
  mutex_t lock_ = MUTEX_INITIAL_VALUE(lock_);

  // AutoLock has access to the inner lock
  friend class AutoLock;
};

// RAII wrapper around the mutex
class AutoLock {
 public:
  explicit AutoLock(mutex_t *mutex) : mutex_(mutex) { mutex_acquire(mutex_); }
  AutoLock(mutex_t &mutex) : AutoLock(&mutex) {}

  explicit AutoLock(Mutex *mutex) : AutoLock(&mutex->lock_) {}
  AutoLock(Mutex &mutex) : AutoLock(&mutex) {}

  ~AutoLock() { release(); }

  // early release the mutex before the object goes out of scope
  void release() {
    if (likely(mutex_)) {
      mutex_release(mutex_);
      mutex_ = nullptr;
    }
  }

  // suppress default constructors
  DISALLOW_COPY_ASSIGN_AND_MOVE(AutoLock);

 private:
  mutex_t *mutex_ = nullptr;
};

#endif  // __cplusplus

#endif  // MK_INCLUDE_KERNEL_MUTEX_H_
