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
#include "event_loop.h"

#define TAG "main"

typedef struct {
    EventLoopHandle vui_event_loop;
    VuiHandle       vui_hndl;
} App;

static App g_app = {0};

static void __vui_event_handler(void *event) {
    Event *ev = (Event *)event;

    if (ev->msg_id == UNI_MSG_LASR_RESULT) {
        LasrResult *lasr_result = (LasrResult *)ev->content;
        LOGT(TAG, "msg=%d, wuw[%d]=%s, out of date[%d]", ev->msg_id,
             lasr_result->vui_id, lasr_result->keyword, VuiEventOutOfDate(g_app.vui_hndl, lasr_result->vui_id));
        LOGT(TAG, "relaunch vui");
        VuiRelaunch(g_app.vui_hndl, UNI_LASR_RASR_MODE);
        LOGT(TAG, "relaunch vui done");
    }
}

static void __vui_event_free_handler(void *event) {
    Event *ev = (Event *)event;
    if (ev && ev->free_event_handler) {
        ev->free_event_handler(ev);
    }
}

static void __eventRouter(Event *event) {
    /* use event_loop to async process */
    EventLoopAdd(g_app.vui_event_loop, event, EVENT_LOOP_PRIORITY_MEDIUM);
}

int main() {
    LogLevelSet(N_LOG_TRACK);

    g_app.vui_event_loop = EventLoopCreate(__vui_event_handler, __vui_event_free_handler, 16 * 1024);

    g_app.vui_hndl = VuiCreate(__eventRouter);

    VuiStart(g_app.vui_hndl, UNI_LASR_RASR_MODE);

    while (true) uni_sleep(10000);

    VuiDestroy(g_app.vui_hndl);

    LOGT(TAG, "exit");
    return 0;
}
