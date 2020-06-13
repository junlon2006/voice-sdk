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

#define TAG "audioin"

AudioInHandle AudioInCreate(void) {
    LOGD(TAG, "audioIn create");
    return NULL;
}

void AudioInDestroy(AudioInHandle hndl) {
    LOGD(TAG, "audioIn destroy");
}

int AudioInStart(AudioInHandle hndl) {
    LOGD(TAG, "audioIn start");
    return 0;
}

int AudioInStop(AudioInHandle hndl) {
    LOGD(TAG, "audioIn stop");
    return 0;
}