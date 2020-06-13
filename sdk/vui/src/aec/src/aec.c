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
 * Description : aec.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#include "aec.h"
#include "pub.h"

#define TAG "aec"

typedef struct {
    int param;
} Aec;

AecHandle AecCreate() {
    Aec *aec = (Aec *)malloc(sizeof(Aec));
    if (NULL_PTR_CHECK(aec)) {
        LOGE(TAG, OUT_MEM_STRING);
        return NULL;
    }

    MZERO(aec);

    LOGT(TAG, "aec create success");
    return aec;
}

void AecDestroy(AecHandle hndl) {
    if (NULL_PTR_CHECK(hndl)) {
        return;
    }

    free(hndl);
    LOGT(TAG, "aec destroy success");
}

int AecStart(AecHandle hndl) {
    LOGT(TAG, "aec start");
    return 0;
}

int AecStop(AecHandle hndl) {
    LOGT(TAG, "aec stop");
    return 0;
}
