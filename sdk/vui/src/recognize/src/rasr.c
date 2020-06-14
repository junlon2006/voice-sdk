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

#define TAG "rasr"

typedef struct {
    PipelineNode  pipeline;
    CbEventRouter cb_event;
} Rasr;

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

static void __register_event_router(Rasr *rasr, CbEventRouter event_router) {
    rasr->cb_event = event_router;
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

    LOGT(TAG, "rasr create success");
    return rasr;
}

void RasrDestroy(RasrHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return;
    }

    free(hndl);
    LOGT(TAG, "rasr destroy success");
}