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
 * Description : lasr.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#include "lasr.h"
#include "pipeline.h"
#include "ringbuf.h"

#define TAG                   "lasr"
#define LASR_RINGBUF_SIZE     (ONE_FRAME_BYTES * 60)  /* 960ms 16K 16bit */
#define LASR_WORKER_STACKSIZE (16 * 1024) /* Linux PTHREAD_STACK_MIN */

typedef struct {
    PipelineNode     pipeline;
    bool             worker_running;
    bool             lasr_stopping;
    uni_sem_t        sem_worker_thread;
    uni_sem_t        sem_starting;
    uni_sem_t        sem_started;
    uni_sem_t        sem_stopping;
    uni_sem_t        sem_stopped;
    uni_sem_t        sem_retrive_audio_data;
    RingBufferHandle ringbuf;
    CbEventRouter    cb_event;
} Lasr;

static void __send_event(Lasr *lasr, Event *event) {
    if (lasr->cb_event) {
        lasr->cb_event(event);
    }
}

static int __pipeline_accept_data(struct PipelineNode *pipeline,
                                  char *buffer, int bytes_len) {
    Lasr *lasr = (Lasr *)pipeline;
    assert(bytes_len == ONE_FRAME_BYTES);

    if (RingBufferGetFreeSize(lasr->ringbuf) < ONE_FRAME_BYTES) {
        LOGW(TAG, "not enough audio buffer, lasr process too slow");
        return 0;
    }

    RingBufferWrite(lasr->ringbuf, buffer, bytes_len);
    uni_sem_signal(&lasr->sem_retrive_audio_data);

    //LOGD(TAG, "recv data. len=%d", bytes_len);
    return 0;
}

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    Lasr *lasr = (Lasr *)pipeline;

    switch (event->type) {
        case PIPELINE_START: {
            LOGT(TAG, "recv lasr start cmd");
            RingBufferClear(lasr->ringbuf);
            uni_sem_signal(&lasr->sem_starting);
            uni_sem_wait(&lasr->sem_started, UNI_WAIT_FOREVER);
        }
        break;
        case PIPELINE_STOP: {
            LOGT(TAG, "recv lasr stop cmd");
            uni_sem_signal(&lasr->sem_stopping);
            lasr->lasr_stopping = true;
            uni_sem_wait(&lasr->sem_stopped, UNI_WAIT_FOREVER);
        }
        break;
        default:
        LOGE(TAG, "unsupport cmd[%d]", event->type);
    }
    return 0;
}

static void __lasr_stop() {
    LOGT(TAG, "lasr stop");
}

static void __free_lasr_result_event(Event *event) {
    free(event->content);
    free(event);
}

#define MOCK_LASR_RESULT  "alex"
static void __do_lasr(Lasr *lasr, char *buf, int len) {
    static int frame = 0;
    if (frame++ % 120 == 0) {
        LOGT(TAG, "do lasr[%d]", frame);
        Event *event              = malloc(sizeof(Event));
        event->msg_id             = UNI_MSG_LASR_RESULT;
        event->content            = malloc(strlen(MOCK_LASR_RESULT) + 1);
        strcpy(event->content, MOCK_LASR_RESULT);
        event->free_event_handler = __free_lasr_result_event;
        __send_event(lasr, event);
    }
}

static void __lasr(Lasr *lasr) {
    char buf[ONE_FRAME_BYTES];
    while (true) {
        if (lasr->lasr_stopping) {
            lasr->lasr_stopping = false;
            uni_sem_wait(&lasr->sem_stopping, UNI_WAIT_FOREVER);
            __lasr_stop();
            uni_sem_signal(&lasr->sem_stopped);
            break;
        }

        uni_sem_wait(&lasr->sem_retrive_audio_data, 100);
        if (RingBufferGetDataSize(lasr->ringbuf) < ONE_FRAME_BYTES) {
            LOGW(TAG, "cannot retrieve enough audio data");
            continue;
        }

        RingBufferRead(buf, ONE_FRAME_BYTES, lasr->ringbuf);
        __do_lasr(lasr, buf, ONE_FRAME_BYTES);
    }
}

static void* __lasr_worker(void *args) {
    Lasr *lasr = (Lasr *)args;

    while (lasr->worker_running) {
        uni_sem_wait(&lasr->sem_starting, UNI_WAIT_FOREVER);
        /* step1. do some sync process */
        uni_sem_signal(&lasr->sem_started);

        /* step2. do lasr */
        __lasr(lasr);
    }

    LOGW(TAG, "worker exit");
    uni_sem_signal(&lasr->sem_worker_thread);

    return NULL;
}

static void __worker_launch(Lasr *lasr) {
    lasr->ringbuf = RingBufferCreate(LASR_RINGBUF_SIZE);
    assert(NULL != lasr->ringbuf);

    uni_sem_new(&lasr->sem_worker_thread, 0);
    uni_sem_new(&lasr->sem_retrive_audio_data, 0);
    uni_sem_new(&lasr->sem_starting, 0);
    uni_sem_new(&lasr->sem_started, 0);
    uni_sem_new(&lasr->sem_stopping, 0);
    uni_sem_new(&lasr->sem_stopped, 0);

    lasr->worker_running = true;
    int ret = uni_thread_new(TAG, __lasr_worker, lasr, LASR_WORKER_STACKSIZE);
    assert(ret == OK);
}

static void __register_event_router(Lasr *lasr, CbEventRouter event_router) {
    lasr->cb_event = event_router;
}

LasrHandle LasrCreate(CbEventRouter event_router) {
    Lasr *lasr = (Lasr *)malloc(sizeof(Lasr));
    if (NULL_PTR_CHECK(lasr)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    /* step1. reset lasr handle */
    MZERO(lasr);

    /* step2. init pipeline */
    PipelineNodeInit(&lasr->pipeline, __pipeline_accept_ctrl, __pipeline_accept_data, TAG);

    /* step3. register event router */
    __register_event_router(lasr, event_router);

    /* step3. launch worker thread */
    __worker_launch(lasr);

    LOGT(TAG, "lasr create success");
    return lasr;
}

static void __destroy_woker(Lasr *lasr) {
    lasr->worker_running = false;
    uni_sem_wait(&lasr->sem_worker_thread, UNI_WAIT_FOREVER);
    uni_sem_free(&lasr->sem_worker_thread);
    uni_sem_free(&lasr->sem_starting);
    uni_sem_free(&lasr->sem_started);
    uni_sem_free(&lasr->sem_stopping);
    uni_sem_free(&lasr->sem_stopped);
    uni_sem_free(&lasr->sem_retrive_audio_data);

    RingBufferDestroy(lasr->ringbuf);
}

void LasrDestroy(LasrHandle hndl) {
    Lasr *lasr = (Lasr *)hndl;
    if (NULL_PTR_CHECK(lasr)) {
        return;
    }

    __destroy_woker(lasr);

    free(hndl);
    LOGT(TAG, "lasr destroy success");
}