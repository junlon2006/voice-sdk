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
 * Description : rasr.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#include "rasr.h"
#include "pipeline.h"
#include "ringbuf.h"

#define TAG                   "rasr"
#define RASR_RINGBUF_SIZE     (ONE_FRAME_BYTES * 60)  /* 960ms 16K 16bit */
#define RASR_WORKER_STACKSIZE (16 * 1024) /* Linux PTHREAD_STACK_MIN */

typedef struct {
    PipelineNode     pipeline;
    CbEventRouter    cb_event;
    int8_t           vui_id;
    RingBufferHandle ringbuf;
    uni_sem_t        sem_worker_thread;
    uni_sem_t        sem_starting;
    uni_sem_t        sem_started;
    uni_sem_t        sem_stopping;
    uni_sem_t        sem_stopped;
    uni_sem_t        sem_retrive_audio_data;
    bool             worker_running;
    bool             rasr_stopping;
} Rasr;

static int __pipeline_accept_data(struct PipelineNode *pipeline,
                                  char *buffer, int bytes_len) {
    Rasr *rasr = (Rasr *)pipeline;
    assert(bytes_len == ONE_FRAME_BYTES);

    if (RingBufferGetFreeSize(rasr->ringbuf) < ONE_FRAME_BYTES) {
        LOGW(TAG, "not enough audio buffer, rasr process too slow");
        return 0;
    }

    RingBufferWrite(rasr->ringbuf, buffer, bytes_len);
    uni_sem_signal(&rasr->sem_retrive_audio_data);
    return 0;
}

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    Rasr *rasr = (Rasr *)pipeline;

    switch (event->type) {
        case PIPELINE_START: {
            LOGT(TAG, "recv rasr start cmd");
            rasr->vui_id = ((PipelineStartContent *)event->content)->vui_id;
            RingBufferClear(rasr->ringbuf);
            uni_sem_signal(&rasr->sem_starting);
            uni_sem_wait(&rasr->sem_started, UNI_WAIT_FOREVER);
        }
        break;
        case PIPELINE_STOP: {
            LOGT(TAG, "recv rasr stop cmd");
            uni_sem_signal(&rasr->sem_stopping);
            rasr->rasr_stopping = true;
            uni_sem_wait(&rasr->sem_stopped, UNI_WAIT_FOREVER);
        }
        break;
        default:
        LOGE(TAG, "unsupport cmd[%d]", event->type);
    }

    return 0;
}

static void __register_event_router(Rasr *rasr, CbEventRouter event_router) {
    rasr->cb_event = event_router;
}

static void __do_rasr(Rasr *rasr, char *buf, int len) {
    //TODO rasr
}

static void __rasr_stop() {
    LOGT(TAG, "lasr stop");
}

static void __rasr(Rasr *rasr) {
    char buf[ONE_FRAME_BYTES];
    while (true) {
        if (rasr->rasr_stopping) {
            rasr->rasr_stopping = false;
            uni_sem_wait(&rasr->sem_stopping, UNI_WAIT_FOREVER);
            __rasr_stop();
            uni_sem_signal(&rasr->sem_stopped);
            break;
        }

        uni_sem_wait(&rasr->sem_retrive_audio_data, 100);
        if (RingBufferGetDataSize(rasr->ringbuf) < ONE_FRAME_BYTES) {
            LOGW(TAG, "cannot retrieve enough audio data");
            continue;
        }

        RingBufferRead(buf, ONE_FRAME_BYTES, rasr->ringbuf);
        __do_rasr(rasr, buf, ONE_FRAME_BYTES);
    }
}

static void* __rasr_worker(void *args) {
    Rasr *rasr = (Rasr *)args;

    while (rasr->worker_running) {
        uni_sem_wait(&rasr->sem_starting, UNI_WAIT_FOREVER);

        /* step1. do some sync process */
        uni_sem_signal(&rasr->sem_started);

        /* step2. do rasr */
        __rasr(rasr);
    }

    LOGW(TAG, "worker exit");
    uni_sem_signal(&rasr->sem_worker_thread);

    return NULL;
}

static void __worker_launch(Rasr *rasr) {
    rasr->ringbuf = RingBufferCreate(RASR_RINGBUF_SIZE);
    assert(NULL != rasr->ringbuf);

    uni_sem_new(&rasr->sem_worker_thread, 0);
    uni_sem_new(&rasr->sem_retrive_audio_data, 0);

    rasr->worker_running = true;
    int ret = uni_thread_new(TAG, __rasr_worker, rasr, RASR_WORKER_STACKSIZE);
    assert(ret == OK);
}

RasrHandle RasrCreate(CbEventRouter event_router) {
    Rasr *rasr = (Rasr *)malloc(sizeof(Rasr));
    if (NULL_PTR_CHECK(rasr)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(rasr);

    PipelineNodeInit(&rasr->pipeline, __pipeline_accept_ctrl, __pipeline_accept_data, TAG);

    __register_event_router(rasr, event_router);

    __worker_launch(rasr);

    LOGT(TAG, "rasr create success");
    return rasr;
}

static void __destroy_woker(Rasr *rasr) {
    rasr->worker_running = false;

    uni_sem_wait(&rasr->sem_worker_thread, UNI_WAIT_FOREVER);
    uni_sem_free(&rasr->sem_worker_thread);
    uni_sem_free(&rasr->sem_starting);
    uni_sem_free(&rasr->sem_started);
    uni_sem_free(&rasr->sem_stopping);
    uni_sem_free(&rasr->sem_stopped);
    uni_sem_free(&rasr->sem_retrive_audio_data);

    RingBufferDestroy(rasr->ringbuf);
}

void RasrDestroy(RasrHandle hndl) {
    Rasr *rasr = (Rasr *)hndl;
    if (NULL_PTR_CHECK(hndl)) {
        return;
    }

    __destroy_woker(rasr);

    free(hndl);
    LOGT(TAG, "rasr destroy success");
}