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
#include "event_list.h"

#define TAG "main"

static EventListHandle g_event_list = NULL;
static VuiHandle       g_vui = NULL;

static void __event_list_event_handler(void *event) {
    Event *ev = (Event *)event;

    if (ev->msg_id == UNI_MSG_LASR_RESULT) {
        LasrResult *lasr_result = (LasrResult *)ev->content;
        LOGT(TAG, "msg=%d, wuw[%d]=%s, out of date[%d]", ev->msg_id,
             lasr_result->vui_id, lasr_result->keyword, VuiEventOutOfDate(g_vui, lasr_result->vui_id));
        LOGT(TAG, "relaunch vui");
        VuiStop(g_vui);
        VuiStart(g_vui, UNI_LASR_RASR_MODE);
        LOGT(TAG, "relaunch vui done");
    }
}

static void __event_list_event_free_handler(void *event) {
    Event *ev = (Event *)event;
    if (ev && ev->free_event_handler) {
        ev->free_event_handler(ev);
    }
}

static void __eventRouter(Event *event) {
    /* use event_list to async process */
    EventListAdd(g_event_list, event, EVENT_LIST_PRIORITY_MEDIUM);
}

int main() {
    LogLevelSet(N_LOG_TRACK);

    g_event_list = EventListCreate(__event_list_event_handler, __event_list_event_free_handler);

    g_vui = VuiCreate(__eventRouter);

    VuiStart(g_vui, UNI_LASR_RASR_MODE);

    while (true) uni_sleep(10000);

    VuiDestroy(g_vui);

    LOGT(TAG, "exit");
    return 0;
}