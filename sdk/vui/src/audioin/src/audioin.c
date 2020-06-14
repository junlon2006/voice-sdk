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
#include "pub.h"
#include "pipeline.h"

#define TAG "audioin"

typedef struct {
    PipelineNode pipeline;
    bool         worker_running;
    uni_sem_t    sem_worker_thread;
} AudioIn;

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    LOGT(TAG, "recv cmd. [%d]", event->type);
    PipelinePushCmd(pipeline, event);
    return 0;
}

static void* __audioin_worker(void *args) {
    AudioIn *audioin = (AudioIn *)args;

    char buf[512];
    while (audioin->worker_running) {
        PipelinePushData(&audioin->pipeline, buf, sizeof(buf));
        uni_sleep(16);
    }

    LOGW(TAG, "worker exit");
    uni_sem_signal(&audioin->sem_worker_thread);

    return NULL;
}

static void __worker_launch(AudioIn *audioin) {
    uni_sem_new(&audioin->sem_worker_thread, 0);
    audioin->worker_running = true;
    int ret = uni_thread_new(TAG, __audioin_worker, audioin, 16 * 1024);
    assert(ret == OK);
}

AudioInHandle AudioInCreate(void) {
    AudioIn *audioin = (AudioIn *)malloc(sizeof(AudioIn));
    if (NULL_PTR_CHECK(audioin)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    /* step1. reset audioin handle */
    MZERO(audioin);

    /* step2. init pipeline */
    PipelineNodeInit(&audioin->pipeline, __pipeline_accept_ctrl, NULL, TAG);

    /* step3. launch worker thread */
    __worker_launch(audioin);

    LOGT(TAG, "audioIn create success");
    return audioin;
}

static void __destroy_woker(AudioIn *audioin) {
    audioin->worker_running = false;
    uni_sem_wait(&audioin->sem_worker_thread, UNI_WAIT_FOREVER);
    uni_sem_free(&audioin->sem_worker_thread);
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

int AudioInStart(AudioInHandle hndl) {
    LOGT(TAG, "audioIn start");
    return 0;
}

int AudioInStop(AudioInHandle hndl) {
    LOGT(TAG, "audioIn stop");
    return 0;
}