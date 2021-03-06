/**************************************************************************
 * Copyright (C) 2018-2019  Junlon2006
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
 * Description : pipeline.c
 * Author      : junlon2006@163.com
 * Date        : 2019.04.07
 *
 **************************************************************************/
#include "pipeline.h"
#include "log.h"
#include "list_head.h"
#include <stdio.h>

#define TAG  "pipeline"

int PipelineNodeInit(PipelineNode *node, PipelineAcceptCtrl cb_cmd,
                     PipelineAcceptData cb_data, const char *name) {
  if (NULL == node) {
    LOGE(TAG, "param invalid. node=%p", node);
    return -1;
  }

  node->self = node;
  node->data = cb_data;
  node->cmd  = cb_cmd;
  snprintf(node->name, sizeof(node->name), "%s", name);
  list_init(&node->link);
  list_init(&node->rear_list);
  return 0;
}

int PipelineConnect(PipelineNode *pre, PipelineNode *rear) {
  if (NULL == pre || NULL == rear) {
    LOGE(TAG, "param invalid. pre=%p, rear=%p", pre, rear);
    return -1;
  }

  list_add(&rear->link, &pre->rear_list);
  LOGD(TAG, "linking %s --> %s", pre->name, rear->name);
  return 0;
}

int PipelineDisConnect(PipelineNode *pre, PipelineNode *rear) {
  PipelineNode *p;
  if (NULL == pre || NULL == rear) {
    LOGE(TAG, "param invalid. pre=%p, rear=%p", pre, rear);
    return -1;
  }

  list_for_each_entry(p, &pre->rear_list, PipelineNode, link) {
    if (p == rear) {
      list_del(&p->link);
      break;
    }
  }
  return 0;
}

int PipelineClear(PipelineNode *pipeline) {
  PipelineNode *p, *n;
  if (NULL == pipeline) {
    LOGE(TAG, "param invalid. node=%p", pipeline);
  }

  list_for_each_entry_safe(p, n, &pipeline->rear_list, PipelineNode, link) {
    list_del(&p->link);
  }
  return 0;
}

int PipelinePushData(PipelineNode *node, char *buffer, int bytes_len) {
  PipelineNode *p;
  if (NULL == node || NULL == buffer) {
    LOGE(TAG, "param invalid. node=%p, buffer=%p", node, buffer);
    return -1;
  }

  list_for_each_entry(p, &node->rear_list, PipelineNode, link) {
    LOGD(TAG, "push data. from %s --> %s", node->name, p->name);
    p->data(p, buffer, bytes_len);
  }

  return 0;
}

int PipelinePushCmd(PipelineNode *node, PipelineEvent *event) {
  PipelineNode *p;
  if (NULL == node || NULL == event) {
    LOGE(TAG, "param invalid. node=%p, event=%p", node, event);
    return -1;
  }

  list_for_each_entry(p, &node->rear_list, PipelineNode, link) {
    LOGD(TAG, "push cmd[%d]. from %s --> %s", event->type, node->name, p->name);
    p->cmd(p, event);
  }

  return 0;
}