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
#include "pub.h"

#define TAG "rasr"

RasrHandle RasrCreate() {
    LOGD(TAG, "rasr create");
    return NULL;
}

void RasrDestroy(RasrHandle hndl) {
    LOGD(TAG, "rasr destroy");
}

int RasrStart(RasrHandle hndl) {
    LOGD(TAG, "rasr start");
    return 0;
}

int RasrStop(RasrHandle hndl) {
    LOGD(TAG, "rasr stop");
    return 0;
}