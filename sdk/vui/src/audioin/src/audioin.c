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
 * Description : audioin.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#include "audioin.h"
#include "pipeline.h"
#include "ringbuf.h"
#include "audio_wrapper.h"

#define TAG                      "audioin"
#define AUDIOIN_WORKER_STACKSIZE (16 * 1024) /* Linux PTHREAD_STACK_MIN */
#define AUDIO_DATA_RINGBUF_SIZE  (8 * 1024)

typedef struct {
    PipelineNode       pipeline;
    bool               worker_running;
    uni_sem_t          sem_worker_thread;
    bool               pipepile_status;
    bool               stopping;
    uni_sem_t          sem_stopped;
    CbEventRouter      cb_event;
    AudioWrapperHandle audio_wrapper;
    RingBufferHandle   ringbuf;
} AudioIn;

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    AudioIn *audioin = (AudioIn *)pipeline;
    LOGT(TAG, "recv cmd. [%d]", event->type);

    switch (event->type) {
        case PIPELINE_START: {
            PipelinePushCmd(pipeline, event);
            audioin->pipepile_status = PIPELINE_START;
            AudioWrapperStart(audioin->audio_wrapper);
        }
        break;
        case PIPELINE_STOP: {
            AudioWrapperStop(audioin->audio_wrapper);
            audioin->pipepile_status = PIPELINE_STOP;
            audioin->stopping        = true;
            uni_sem_wait(&audioin->sem_stopped, UNI_WAIT_FOREVER);
            RingBufferClear(audioin->ringbuf);

            PipelinePushCmd(pipeline, event);
        }
        break;
        default:
        LOGW(TAG, "recv unsupport cmd[%d]", event->type);
        break;
    }

    return 0;
}

static void* __audioin_worker(void *args) {
    AudioIn *audioin = (AudioIn *)args;
    char buf[ONE_FRAME_BYTES];

    while (audioin->worker_running) {
        if (audioin->stopping) {
            audioin->stopping = false;
            uni_sem_signal(&audioin->sem_stopped);
        }

        if (audioin->pipepile_status == PIPELINE_START) {
            if (RingBufferGetDataSize(audioin->ringbuf) < ONE_FRAME_BYTES) {
                uni_sleep(10);
                continue;
            }

            RingBufferRead(buf, sizeof(buf), audioin->ringbuf);
            PipelinePushData(&audioin->pipeline, buf, sizeof(buf));
        } else {
            uni_sleep(10);
        }
    }

    LOGW(TAG, "worker exit");
    uni_sem_signal(&audioin->sem_worker_thread);

    return NULL;
}

static void __cb_data(const char *data, unsigned int len, void *user_data) {
    AudioIn *audioin = (AudioIn *)user_data;
    if (RingBufferGetFreeSize(audioin->ringbuf) < len) {
        LOGW(TAG, "no empty ringbuf");
        return;
    }

    RingBufferWrite(audioin->ringbuf, (char *)data, len);
}

static void __worker_launch(AudioIn *audioin) {
    /* step1. portaudio init */
    audioin->audio_wrapper = AudioWrapperCreate(__cb_data, audioin);
    assert(audioin->audio_wrapper != NULL);
    audioin->ringbuf = RingBufferCreate(AUDIO_DATA_RINGBUF_SIZE);
    assert(audioin->ringbuf != NULL);

    /* step2. audioin worker task */
    uni_sem_new(&audioin->sem_worker_thread, 0);
    uni_sem_new(&audioin->sem_stopped, 0);
    audioin->worker_running = true;
    int ret = uni_thread_new(TAG, __audioin_worker, audioin, AUDIOIN_WORKER_STACKSIZE);
    assert(ret == OK);
}

static void __register_event_router(AudioIn *audioin, CbEventRouter event_router) {
    audioin->cb_event = event_router;
}

AudioInHandle AudioInCreate(CbEventRouter event_router) {
    AudioIn *audioin = (AudioIn *)malloc(sizeof(AudioIn));
    if (NULL_PTR_CHECK(audioin)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    /* step1. reset audioin handle */
    MZERO(audioin);

    /* step2. init pipeline */
    PipelineNodeInit(&audioin->pipeline, __pipeline_accept_ctrl, NULL, TAG);

    /* step3. register event router */
    __register_event_router(audioin, event_router);

    /* step4. launch worker thread */
    __worker_launch(audioin);

    LOGT(TAG, "audioIn create success");
    return audioin;
}

static void __destroy_woker(AudioIn *audioin) {
    AudioWrapperDestroy(audioin->audio_wrapper);

    audioin->worker_running = false;
    uni_sem_wait(&audioin->sem_worker_thread, UNI_WAIT_FOREVER);
    uni_sem_free(&audioin->sem_worker_thread);
    uni_sem_free(&audioin->sem_stopped);

    RingBufferDestroy(audioin->ringbuf);
}

void AudioInDestroy(AudioInHandle hndl) {
    AudioIn *audioin = (AudioIn *)hndl;
    if (NULL_PTR_CHECK(audioin)) {
        return;
    }

    __destroy_woker(audioin);

    free(hndl);
    LOGT(TAG, "audioIn destroy");
}