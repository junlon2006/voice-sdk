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
#include "pub.h"
#include "pipeline.h"

#define TAG "lasr"

typedef struct {
    PipelineNode pipeline;
} Lasr;

static int __pipeline_accept_data(struct PipelineNode *pipeline,
                                  char *buffer, int bytes_len) {
    LOGD(TAG, "recv data. len=%d", bytes_len);
    return 0;
}

static int __pipeline_accept_ctrl(struct PipelineNode *pipeline,
                                  PipelineEvent *event) {
    LOGT(TAG, "recv cmd. [%d]", event->type);
    return 0;
}

LasrHandle LasrCreate() {
    Lasr *lasr = (Lasr *)malloc(sizeof(Lasr));
    if (NULL_PTR_CHECK(lasr)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(lasr);

    PipelineNodeInit(&lasr->pipeline, __pipeline_accept_ctrl, __pipeline_accept_data, TAG);

    LOGT(TAG, "lasr create success");
    return lasr;
}

void LasrDestroy(LasrHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return;
    }

    free(hndl);
    LOGT(TAG, "lasr destroy success");
}

int LasrStart(LasrHandle hndl, int mode) {
    LOGT(TAG, "lasr start");
    return 0;
}

int LasrStop(LasrHandle hndl) {
    LOGT(TAG, "lasr stop");
    return 0;
}
