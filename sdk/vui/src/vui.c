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
 * Description : vui.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.11
 *
 **************************************************************************/
#include "vui.h"
#include "lasr.h"
#include "rasr.h"
#include "aec.h"
#include "audioin.h"
#include "pub.h"
#include "pipeline.h"

#define TAG                  "vui"
#define VUI_STATUS_STARTED   1
#define VUI_STATUS_STOPPED   0

typedef struct Vui {
    PipelineNode  root;
    AudioInHandle audioin;
    AecHandle     aec;
    LasrHandle    lasr;
    RasrHandle    rasr;
    CbEventRouter cb_event;
    uint8_t       aec_on  : 1;
    uint8_t       rasr_on : 1;
    uint8_t       status  : 1;
    uint8_t       vui_id  : 5; /* filter out of date event */
} Vui;

static void __destroy_vui_handle(Vui *vui) {
    if (!NULL_PTR_CHECK(vui->audioin)) {
        AudioInDestroy(vui->audioin);
    }

    if (!NULL_PTR_CHECK(vui->aec)) {
        AecDestroy(vui->aec);
    }

    if (!NULL_PTR_CHECK(vui->lasr)) {
        LasrDestroy(vui->lasr);
    }

    if (!NULL_PTR_CHECK(vui->rasr)) {
        RasrDestroy(vui->rasr);
    }

    free(vui);
}

static void __load_vui_config(Vui *vui) {
    vui->aec_on  = 1;
    vui->rasr_on = 1;
    LOGT(TAG, "aec_on=%d, rasr_on=%d", vui->aec_on, vui->rasr_on);
}

static void __register_event_router(Vui *vui, CbEventRouter event_router) {
    vui->cb_event = event_router;
}

VuiHandle VuiCreate(CbEventRouter event_router) {
    Vui *vui = (Vui *)malloc(sizeof(Vui));
    if (NULL_PTR_CHECK(vui)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(vui);

    __load_vui_config(vui);
    __register_event_router(vui, event_router);

    vui->audioin                = AudioInCreate(event_router);
    if (vui->aec_on) vui->aec   = AecCreate(event_router);
    vui->lasr                   = LasrCreate(event_router);
    if (vui->rasr_on) vui->rasr = RasrCreate(event_router);

    if (NULL_PTR_CHECK(vui->audioin) ||
        (vui->aec_on && NULL_PTR_CHECK(vui->aec)) ||
        NULL_PTR_CHECK(vui->lasr) ||
        (vui->rasr_on && NULL_PTR_CHECK(vui->rasr))) {
        __destroy_vui_handle(vui);
        LOGE(TAG, "create vui handle failed");
        return NULL;
    }

    PipelineNodeInit(&vui->root, NULL, NULL, TAG);

    LOGT(TAG, "vui create success");
    return vui;
}

void VuiDestroy(VuiHandle hndl) {
    if (!NULL_PTR_CHECK(hndl)) {
        __destroy_vui_handle((Vui *)hndl);
    }

    LOGT(TAG, "vui destroy");
}

static bool __lasr_on(VuiMode mode) {
    return mode == UNI_AWAKEN_MODE || mode == UNI_LASR_ONLY_MODE || UNI_LASR_RASR_MODE;
}

static bool __rasr_on(Vui *vui, VuiMode mode) {
    assert(vui->rasr_on);
    return mode == UNI_RASR_ONLY_MODE || mode == UNI_LASR_RASR_MODE;
}

static void __linking_module_launch(Vui *vui) {
    PipelineNode *root = &vui->root;
    PipelineEvent event;
    PipelineStartContent content;
    content.vui_id     = vui->vui_id;
    event.type         = PIPELINE_START;
    event.content      = &content;
    PipelinePushCmd(root, &event);
}

#define LINKING(pre, rear) PipelineConnect((PipelineNode *) vui->pre, (PipelineNode *)vui->rear)
static void __module_linking(Vui *vui, VuiMode mode) {
    PipelineConnect(&vui->root, (PipelineNode *)vui->audioin);
    if (vui->aec_on) {
        LINKING(audioin, aec);
        if (__lasr_on(mode))      LINKING(aec, lasr);
        if (__rasr_on(vui, mode)) LINKING(aec, rasr);
    } else {
        if (__lasr_on(mode))      LINKING(audioin, lasr);
        if (__rasr_on(vui, mode)) LINKING(audioin, rasr);
    }
}

static void __set_current_vui_id(Vui *vui) {
    vui->vui_id++;
    LOGT(TAG, "vui_id=%d", vui->vui_id);
}

static void __set_vui_status(Vui *vui, uint8_t status) {
    vui->status = status;
    LOGD(TAG, "vui status[%d]", status);
}

int VuiStart(VuiHandle hndl, VuiMode mode) {
    if (NULL_PTR_CHECK(hndl)) {
        return -INVALID_IN_PARAM;
    }

    Vui *vui = (Vui *)hndl;
    if (VUI_STATUS_STARTED == vui->status) {
        return -VUI_START_ERROR;
    }

    __set_current_vui_id(vui);
    __module_linking(vui, mode);
    __linking_module_launch(vui);
    __set_vui_status(vui, VUI_STATUS_STARTED);

    LOGD(TAG, "vui start");
    return 0;
}

static void __linking_module_stop(Vui *vui) {
    PipelineEvent event;
    PipelineNode *root = &vui->root;
    event.type         = PIPELINE_STOP;
    event.content      = NULL;
    PipelinePushCmd(root, &event);
}

#define UNLINKING(node) if (node) PipelineClear((PipelineNode *)node)
static void __module_unlinking(Vui *vui) {
    UNLINKING(vui);
    UNLINKING(vui->audioin);
    UNLINKING(vui->aec);
    UNLINKING(vui->lasr);
    UNLINKING(vui->rasr);
}

int VuiStop(VuiHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return -INVALID_IN_PARAM;
    }

    Vui *vui = (Vui *)hndl;
    if (VUI_STATUS_STOPPED == vui->status) {
        return -VUI_STOP_ERROR;
    }

    __linking_module_stop(vui);
    __module_unlinking(vui);
    __set_vui_status(vui, VUI_STATUS_STOPPED);

    LOGD(TAG, "vui stop");
    return 0;
}

bool VuiEventOutOfDate(VuiHandle hndl, int8_t vui_id) {
    Vui *vui = (Vui *)hndl;
    if (NULL_PTR_CHECK(vui)) {
        return true;
    }

    return vui->vui_id != vui_id;
}
