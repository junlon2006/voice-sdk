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

#define TAG "vui"

typedef struct Vui {
    AudioInHandle audioin;
    AecHandle     aec;
    LasrHandle    lasr;
    RasrHandle    rasr;
    uint8_t       aec_on : 1;
    uint8_t       rasr_on : 1;
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

VuiHandle VuiCeate(void) {
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

int VuiStart(VuiHandle hndl, VuiMode mode) {
    LOGD(TAG, "vui start");
    return 0;
}

int VuiStop(VuiHandle hndl) {
    LOGD(TAG, "vui stop");
    return 0;
}