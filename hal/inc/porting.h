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
 * Description : porting.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.11
 *
 **************************************************************************/
#ifndef VOICE_SDK_HAL_INC_PORTING_H_
#define VOICE_SDK_HAL_INC_PORTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UNI_WAIT_FOREVER   -1
#define NULL_PTR_CHECK(p)  (NULL == p)

typedef struct {
    void *hdl;
} uni_hdl_t;

typedef uni_hdl_t uni_mutex_t;
typedef uni_hdl_t uni_sem_t;
typedef uni_hdl_t uni_thread_t;

/**
 * Alloc a mutex.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in uni_mutex_t will refer a kernel obj internally.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_mutex_new(uni_mutex_t *mutex);

/**
 * Free a mutex.
 *
 * @param[in]  mutex  mutex object, mem refered by hdl pointer in
 *                    uni_mutex_t will be freed internally.
 *
 * @return  none.
 */
int uni_mutex_free(uni_mutex_t *mutex);

/**
 * Lock a mutex.
 *
 * @param[in]  mutex    mutex object, it contains kernel obj pointer.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_mutex_lock(uni_mutex_t *mutex);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex  mutex object, it contains kernel obj pointer.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_mutex_unlock(uni_mutex_t *mutex);

/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in uni_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_sem_new(uni_sem_t *sem, unsigned int count);

/**
 * Destroy a semaphore.
 *
 * @param[in]  sem  pointer of semaphore object, mem refered by hdl pointer
 *                  in uni_sem_t will be freed internally.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_sem_free(uni_sem_t *sem);

/**
 * Acquire a semaphore.
 *
 * @param[in]  sem      semaphore object, it contains kernel obj pointer
 *                      which uni_sem_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *                      value:  UNI_WAIT_FOREVER: wait mutex for ever.
 *                              0: return immediately if not get mutex.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_sem_wait(uni_sem_t *sem, unsigned int timeout);

/**
 * Release a semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which uni_sem_new alloced.
 *                  It will wakeup one task which is waiting for the sem according to task priority.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_sem_signal(uni_sem_t *sem);

/**
 * Create a task.
 *
 * @param[in]  name       task name.
 * @param[in]  fn         function to run.
 * @param[in]  arg        argument of the function.
 * @param[in]  stacksize  stack-size in bytes.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_thread_new(const char *name, void* (*fn)(void *), void *arg, int stack_size);

/**
 * Sleep.
 *
 * @param[in]  msec       sleep msecond.
 *
 * @return  0: success, otherwise: fail.
 */
int uni_sleep(unsigned int msec);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_HAL_INC_PORTING_H_ */