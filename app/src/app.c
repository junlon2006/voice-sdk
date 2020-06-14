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
 * Description : app.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.11
 *
 **************************************************************************/
#include "app.h"
#include "pub.h"
#include "vui.h"

#define TAG "main"

int main() {
    while (1) {
    LOGT(TAG, "vui create");
    VuiHandle vui = VuiCreate();

    VuiStart(vui, UNI_LASR_RASR_MODE);

    VuiStop(vui);

    LOGT(TAG, "vui destroy");
    VuiDestroy(vui);

    uni_sleep(100);
    LOGT(TAG, "exit");
    }

    return 0;
}