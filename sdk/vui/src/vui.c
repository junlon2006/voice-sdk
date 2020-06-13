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
    AudioInHandle audioin;
    AecHandle     aec;
    LasrHandle    lasr;
    RasrHandle    rasr;
    uint8_t       aec_on : 1;
    uint8_t       rasr_on : 1;
    uint8_t       status : 1;
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

VuiHandle VuiCreate(void) {
    Vui *vui = (Vui *)malloc(sizeof(Vui));
    if (NULL_PTR_CHECK(vui)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(vui);

    __load_vui_config(vui);

    vui->audioin                = AudioInCreate();
    if (vui->aec_on) vui->aec   = AecCreate();
    vui->lasr                   = LasrCreate();
    if (vui->rasr_on) vui->rasr = RasrCreate();

    if (NULL_PTR_CHECK(vui->audioin) ||
        (vui->aec_on && NULL_PTR_CHECK(vui->aec)) ||
        NULL_PTR_CHECK(vui->lasr) ||
        (vui->rasr_on && NULL_PTR_CHECK(vui->rasr))) {
        __destroy_vui_handle(vui);
        LOGE(TAG, "create vui handle failed");
        return NULL;
    }

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

#define LINKING(pre, rear) PipelineConnect((PipelineNode *)vui->pre, (PipelineNode *)vui->rear)
static void __module_linking(VuiHandle hndl, VuiMode mode) {
    Vui *vui = (Vui *)hndl;
    if (vui->aec_on) {
        LINKING(audioin, aec);
        if (__lasr_on(mode))      LINKING(aec, lasr);
        if (__rasr_on(vui, mode)) LINKING(aec, rasr);
    } else {
        if (__lasr_on(mode))      LINKING(audioin, lasr);
        if (__rasr_on(vui, mode)) LINKING(audioin, rasr); 
    }
}

int VuiStart(VuiHandle hndl, VuiMode mode) {
    if (NULL_PTR_CHECK(hndl)) {
        return -INVALID_IN_PARAM;
    }

    Vui *vui = (Vui *)hndl;
    if (VUI_STATUS_STARTED == vui->status) {
        return -VUI_START_ERROR;
    }

    __module_linking(hndl, mode);

    LOGD(TAG, "vui start");
    return 0;
}

int VuiStop(VuiHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return -INVALID_IN_PARAM;
    }

    Vui *vui = (Vui *)hndl;
    if (VUI_STATUS_STOPPED == vui->status) {
        return -VUI_STOP_ERROR;
    }

    LOGD(TAG, "vui stop");
    return 0;
}