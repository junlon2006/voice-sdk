
/**************************************************************************
 * Copyright (C) 2020-2020  Junlon2006
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
 **************************************************************************
 *
 * Description : app.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.11
 *
 **************************************************************************/
#include "app.h"
#include "log.h"
#include "ringbuf.h"
#include "porting.h"
#include <stdio.h>

#define TAG "main"

static void* __read_task(void *args) {
    RingBufferHandle ringbuf = (RingBufferHandle)args;
    int data_size;
    char buf[1024];
    while (1) {
        data_size = RingBufferGetDataSize(ringbuf);
        if (data_size > 0) {
            RingBufferRead(buf, data_size, ringbuf);
            LOGT(TAG, "read data[%d]", data_size);
        } else {
            uni_sleep(10);
        }
    }
    return NULL;
}

static void* __write_task(void *args) {
    RingBufferHandle ringbuf = (RingBufferHandle)args;
    int free_size;
    char buf[64];
    while (1) {
        free_size = RingBufferGetFreeSize(ringbuf);
        if (free_size >= sizeof(buf)) {
            RingBufferWrite(ringbuf, buf, sizeof(buf));
        } else {
            uni_sleep(1000);
        }
    }
    return NULL;
}

int main() {
    RingBufferHandle ringbuf = RingBufferCreate(1024);
    uni_thread_new("test", __read_task, ringbuf, 1024 * 512);
    uni_thread_new("test", __write_task, ringbuf, 1024 * 512);
    LOGT(TAG, "main");
    while (1) uni_sleep(10000);
    return 0;
}