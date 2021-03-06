/*
 * Author Jerry Lundström <jerry@dns-oarc.net>
 * Copyright (c) 2017, OARC, Inc.
 * All rights reserved.
 *
 * This file is part of sllq.
 *
 * sllq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sllq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sllq.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __sllq_h
#define __sllq_h

#include <pthread.h>
#include <signal.h>
#if SLLQ_ENABLE_ASSERT
#include <assert.h>
#define sllq_assert(x) assert(x)
#else
#define sllq_assert(x)
#endif

/* clang-format off */

#define SLLQ_VERSION_STR    "1.0.0"
#define SLLQ_VERSION_MAJOR  1
#define SLLQ_VERSION_MINOR  0
#define SLLQ_VERSION_PATCH  0

#define SLLQ_OK             0
#define SLLQ_ERROR          1
#define SLLQ_ERRNO          2
#define SLLQ_ENOMEM         3
#define SLLQ_EINVAL         4
#define SLLQ_ETIMEDOUT      5
#define SLLQ_EBUSY          6
#define SLLQ_EAGAIN         7
#define SLLQ_EMPTY          8
#define SLLQ_FULL           9

#define SLLQ_ERROR_STR      "generic error"
#define SLLQ_ERRNO_STR      "system error"
#define SLLQ_ENOMEM_STR     "out of memory"
#define SLLQ_EINVAL_STR     "invalid argument"
#define SLLQ_ETIMEDOUT_STR  "timed out"
#define SLLQ_EBUSY_STR      "busy"
#define SLLQ_EAGAIN_STR     "try again"
#define SLLQ_EMPTY_STR      "queue is empty"
#define SLLQ_FULL_STR       "queue is full"

/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

const char* sllq_version_str(void);
int         sllq_version_major(void);
int         sllq_version_minor(void);
int         sllq_version_patch(void);

/* clang-format off */
#define SLLQ_ITEM_T_INIT { \
    0, 0, 0, \
    0, \
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER \
}
/* clang-format on */
typedef struct sllq_item sllq_item_t;
struct sllq_item {
    unsigned short want_read : 1;
    unsigned short want_write : 1;
    unsigned short have_data : 1;

    void* data;

    pthread_mutex_t mutex;
    pthread_cond_t  cond;
};

typedef enum sllq_mode sllq_mode_t;
enum sllq_mode {
    SLLQ_MUTEX,
    SLLQ_PIPE
};

/* clang-format off */
#define SLLQ_T_INIT { \
    SLLQ_MUTEX, \
    0, 0, 0, 0, 0 \
    -1, -1 \
}
/* clang-format on */
typedef struct sllq sllq_t;
struct sllq {
    sllq_mode_t mode;

    /* MUTEX mode */
    sllq_item_t* item;
    size_t       size;
    size_t       mask;
    size_t       read;
    size_t       write;

    /* PIPE mode */
    int read_pipe;
    int write_pipe;
};

typedef void (*sllq_item_callback_t)(void* data);

sllq_t* sllq_new(void);
void sllq_free(sllq_t* queue);

sllq_mode_t sllq_mode(const sllq_t* queue);
int sllq_set_mode(sllq_t* queue, sllq_mode_t mode);
size_t sllq_size(const sllq_t* queue);
int sllq_set_size(sllq_t* queue, size_t size);

int sllq_init(sllq_t* queue);
int sllq_destroy(sllq_t* queue);

int sllq_flush(sllq_t* queue, sllq_item_callback_t callback);

int sllq_push(sllq_t* queue, void* data, const struct timespec* abstime);
int sllq_shift(sllq_t* queue, void** data, const struct timespec* abstime);

const char* sllq_strerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif /* __sllq_h */
