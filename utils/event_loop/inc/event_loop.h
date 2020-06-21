/**********************************************************************
 * Copyright (C) 2020-2020  junlon2006
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
 **********************************************************************
 *
 * Description : event_loop.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.21
 *
 **********************************************************************/
#ifndef VOICE_SDK_UTILS_EVENT_LOOP_INC_EVENT_LOOP_H_
#define VOICE_SDK_UTILS_EVENT_LOOP_INC_EVENT_LOOP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_LOOP_PRIORITY_HIGHEST  (1 << 0)
#define EVENT_LOOP_PRIORITY_MEDIUM   (1 << 1)
#define EVENT_LOOP_PRIORITY_LOWEST   (1 << 2)

typedef void* EventLoopHandle;
typedef void (*EventLoopEventHandler)(void *event);
typedef void (*EventLoopEventFreeHandler)(void *event);

EventLoopHandle EventLoopCreate(EventLoopEventHandler event_handler,
                                EventLoopEventFreeHandler free_handler,
                                unsigned int stack_size);
int             EventLoopDestroy(EventLoopHandle handle);
int             EventLoopAdd(EventLoopHandle handle, void *event, int priority);
int             EventLoopClear(EventLoopHandle handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_UTILS_EVENT_LOOP_INC_EVENT_LOOP_H_ */
