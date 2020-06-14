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
} AuidoIn;

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    LOGT(TAG, "recv cmd. [%d]", event->type);
    PipelinePushCmd(pipeline, event);
    return 0;
}

AudioInHandle AudioInCreate(void) {
    AuidoIn *audioin = (AuidoIn *)malloc(sizeof(AuidoIn));
    if (NULL_PTR_CHECK(audioin)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(audioin);

    PipelineNodeInit(&audioin->pipeline, __pipeline_accept_ctrl, NULL, TAG);

    LOGT(TAG, "audioIn create success");
    return audioin;
}

void AudioInDestroy(AudioInHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return;
    }

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