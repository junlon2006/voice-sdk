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
 * Description : pub.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#ifndef VOICE_SDK_HAL_INC_PUB_H_
#define VOICE_SDK_HAL_INC_PUB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "errcode.h"
#include "porting.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

enum {

    UNI_MSG_AUDIOIN_BASE = 0,    /* msg id for audioin */

    UNI_MSG_AEC_BASE     = 1000, /* msg id for aec */

    UNI_MSG_LASR_BASE    = 2000,  /* msg id for lasr */
    UNI_MSG_LASR_RESULT,

    UNI_MSG_RASR_BASE    = 3000,  /* msg id for rasr */

    UNI_MSG_APP_BASE     = 4000,  /* msg id for app */
};

typedef struct Event {
    uint16_t msg_id;
    void     *content;
    void     (*free_event_handler)(struct Event *event);
} Event;

typedef void (*CbEventRouter)(Event *event);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_HAL_INC_PUB_H_ */
