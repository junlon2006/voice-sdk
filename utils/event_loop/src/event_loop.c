/**********************************************************************
 * Copyright (C) 2020-2020  junlon2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **********************************************************************
 *
 * Description : event_loop.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.21
 *
 **********************************************************************/
#include "event_loop.h"
#include "list_head.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>

#define _consume_higest_list               _consume_one_list
#define _consume_medium_list               _consume_one_list
#define _consume_lowest_list               _consume_one_list
#define THREAD_TRY_COND_TIMROUT_MSC        (1000)

typedef int (*_interruptable_handler)(void *args);

typedef struct {
  list_head link;
  int       priority;
  void      *event;
} EventLoopItem;

typedef struct {
  list_head                 highest_list;
  list_head                 medium_list;
  list_head                 lowest_list;
  int                       highest_cnt;
  int                       medium_cnt;
  int                       lowest_cnt;
  EventLoopEventHandler     event_handler;
  EventLoopEventFreeHandler free_handler;
  _interruptable_handler    highest_interrupt_handler;
  _interruptable_handler    medium_interrupt_handler;
  _interruptable_handler    lowest_interrupt_handler;
  pthread_t                 pid;
  pthread_mutex_t           mutex;
  pthread_cond_t            cond;
  int                       running;
} EventLoop;

static void _consume_one_list(list_head *header,
                              EventLoopEventHandler event_handler,
                              EventLoopEventFreeHandler free_handler,
                              _interruptable_handler interrupt,
                              void *interrupt_args,
                              pthread_mutex_t *mutex,
                              int *total_cnt, int *running) {
  list_head *p;
  EventLoopItem *item;
  do {
    pthread_mutex_lock(mutex);
    if (NULL != interrupt && interrupt(interrupt_args)) {
      pthread_mutex_unlock(mutex);
      break;
    }
    if (NULL == (p = list_get_head(header))) {
      pthread_mutex_unlock(mutex);
      break;
    }
    item = list_entry(p, EventLoopItem, link);
    list_del(&item->link);
    *total_cnt = *total_cnt - 1;
    pthread_mutex_unlock(mutex);
    if (NULL != event_handler && *running) {
      event_handler(item->event);
    }
    if (NULL != free_handler) {
      free_handler(item->event);
    }
    free(item);
  } while (p != NULL);
}

static int _highest_interruptable_handler(void *args) {
  return 0;
}

static int _medium_interruptable_handler(void *args) {
  EventLoop *event_loop = (EventLoop*)args;
  return (0 < event_loop->highest_cnt);
}

static int _lowest_interruptable_handler(void *args) {
  EventLoop *event_loop = (EventLoop*)args;
  return (0 < event_loop->highest_cnt || 0 < event_loop->medium_cnt);
}

static void _consume_event_loop(EventLoop *event_loop) {
  _consume_higest_list(&event_loop->highest_list, event_loop->event_handler,
                       event_loop->free_handler,
                       event_loop->highest_interrupt_handler, event_loop,
                       &event_loop->mutex,
                       &event_loop->highest_cnt, &event_loop->running);
  _consume_medium_list(&event_loop->medium_list, event_loop->event_handler,
                       event_loop->free_handler,
                       event_loop->medium_interrupt_handler, event_loop,
                       &event_loop->mutex,
                       &event_loop->medium_cnt, &event_loop->running);
  _consume_lowest_list(&event_loop->lowest_list, event_loop->event_handler,
                       event_loop->free_handler,
                       event_loop->lowest_interrupt_handler, event_loop,
                       &event_loop->mutex,
                       &event_loop->lowest_cnt, &event_loop->running);
}

static void _reset_event_handler(EventLoop *event_loop) {
  event_loop->event_handler = NULL;
}

static void _reset_interrupt_handler(EventLoop *event_loop) {
  event_loop->highest_interrupt_handler = NULL;
  event_loop->medium_interrupt_handler = NULL;
  event_loop->lowest_interrupt_handler = NULL;
}

static void _free_all(EventLoop *event_loop) {
  _reset_event_handler(event_loop);
  _reset_interrupt_handler(event_loop);
  _consume_event_loop(event_loop);
  pthread_mutex_destroy(&event_loop->mutex);
  pthread_cond_destroy(&event_loop->cond);
  free(event_loop);
}

static void _set_ts(struct timespec *ts) {
  int64_t timeout;
  clock_gettime(CLOCK_MONOTONIC, ts);
  timeout = (int64_t)ts->tv_sec * (int64_t)1000000000 + (int64_t)ts->tv_nsec;
  timeout += (int64_t)(THREAD_TRY_COND_TIMROUT_MSC * (int64_t)1000000);
  ts->tv_sec = (timeout / 1000000000);
  ts->tv_nsec = (timeout % 1000000000);
}

static int _all_list_empty(EventLoop *event_loop) {
  return (0 == event_loop->highest_cnt &&
          0 == event_loop->medium_cnt &&
          0 == event_loop->lowest_cnt);
}

static void _try_cond_timedwait(EventLoop *event_loop) {
  struct timespec ts;
  if (_all_list_empty(event_loop)) {
    pthread_mutex_lock(&event_loop->mutex);
    if (_all_list_empty(event_loop)) {
      _set_ts(&ts);
      pthread_cond_timedwait(&event_loop->cond, &event_loop->mutex, &ts);
    }
    pthread_mutex_unlock(&event_loop->mutex);
  }
}

static void *_event_handler(void *args) {
  EventLoop *event_loop = (EventLoop*)args;
  pthread_detach(pthread_self());
  while (event_loop->running) {
    _consume_event_loop(event_loop);
    _try_cond_timedwait(event_loop);
  }
  _free_all(event_loop);
  return NULL;
}

static void _mzero_event_loop(EventLoop *event_loop) {
  memset(event_loop, 0, sizeof(EventLoop));
}

static void _list_init(EventLoop *event_loop) {
  list_init(&event_loop->highest_list);
  list_init(&event_loop->medium_list);
  list_init(&event_loop->lowest_list);
}

static void _register_event_handler(EventLoop *event_loop,
                                    EventLoopEventHandler event_handler) {
  event_loop->event_handler = event_handler;
}

static void _register_free_handler(EventLoop *event_loop,
                                   EventLoopEventFreeHandler free_handler) {
  event_loop->free_handler = free_handler;
}

static void _register_interuppt_handler(EventLoop *event_loop) {
  event_loop->highest_interrupt_handler = _highest_interruptable_handler;
  event_loop->medium_interrupt_handler = _medium_interruptable_handler;
  event_loop->lowest_interrupt_handler = _lowest_interruptable_handler;
}

static void _worker_thread_create(EventLoop *event_loop, unsigned int stack_size) {
  pthread_condattr_t attr;
  pthread_attr_t ar;
  pthread_attr_init(&ar);
  pthread_attr_setstacksize(&ar, stack_size);
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  pthread_cond_init(&event_loop->cond, &attr);
  pthread_mutex_init(&event_loop->mutex, NULL);
  event_loop->running = 1;
  pthread_create(&event_loop->pid, NULL, _event_handler, event_loop);
}

EventLoopHandle EventLoopCreate(EventLoopEventHandler event_handler,
                                EventLoopEventFreeHandler free_handler,
                                unsigned int stack_size) {
  EventLoop *event_loop = NULL;
  if (NULL == (event_loop = (EventLoop*)malloc(sizeof(EventLoop)))) {
    return NULL;
  }
  _mzero_event_loop(event_loop);
  _list_init(event_loop);
  _register_event_handler(event_loop, event_handler);
  _register_free_handler(event_loop, free_handler);
  _register_interuppt_handler(event_loop);
  _worker_thread_create(event_loop, stack_size);
  return event_loop;
}

int EventLoopDestroy(EventLoopHandle handle) {
  EventLoop *event_loop = (EventLoop*)handle;
  event_loop->running = 0;
  pthread_join(event_loop->pid, NULL);
  return 0;
}

int EventLoopAdd(EventLoopHandle handle, void *event, int priority) {
  EventLoopItem *item = NULL;
  EventLoop *event_loop = (EventLoop *)handle;
  if (NULL == (item = (EventLoopItem*)malloc(sizeof(EventLoopItem)))) {
    return -1;
  }
  item->priority = priority;
  item->event = event;
  pthread_mutex_lock(&event_loop->mutex);
  if (EVENT_LOOP_PRIORITY_HIGHEST == priority) {
    list_add_tail(&item->link, &event_loop->highest_list);
    event_loop->highest_cnt++;
  } else if (EVENT_LOOP_PRIORITY_MEDIUM == priority) {
    list_add_tail(&item->link, &event_loop->medium_list);
    event_loop->medium_cnt++;
  } else if (EVENT_LOOP_PRIORITY_LOWEST == priority) {
    list_add_tail(&item->link, &event_loop->lowest_list);
    event_loop->lowest_cnt++;
  }
  pthread_cond_signal(&event_loop->cond);
  pthread_mutex_unlock(&event_loop->mutex);
  return 0;
}

int EventLoopClear(EventLoopHandle handle) {
  EventLoop *event_loop = (EventLoop *)handle;
  _consume_higest_list(&event_loop->highest_list, NULL,
                       event_loop->free_handler, NULL, NULL,
                       &event_loop->mutex, &event_loop->highest_cnt,
                       &event_loop->running);
  _consume_medium_list(&event_loop->medium_list, NULL,
                       event_loop->free_handler, NULL, NULL,
                       &event_loop->mutex, &event_loop->medium_cnt,
                       &event_loop->running);
  _consume_lowest_list(&event_loop->lowest_list, NULL,
                       event_loop->free_handler, NULL, NULL,
                       &event_loop->mutex, &event_loop->lowest_cnt,
                       &event_loop->running);
  return 0;
}
