#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*os_job_fn_t)(void *arg);

/* 非同期実行 */
void os_post(os_job_fn_t fn, void *arg);

/* キュー */
typedef struct os_queue os_queue_t;

os_queue_t* os_queue_create(int capacity);
bool os_queue_send(os_queue_t *q, void *msg);
void* os_queue_recv(os_queue_t *q);

/* イベント */
typedef int os_event_id_t;
typedef void (*os_event_cb_t)(void *arg);

void os_event_subscribe(os_event_id_t id, os_event_cb_t cb, void *arg);
void os_event_publish(os_event_id_t id);

/* タイマー */
void os_schedule(uint32_t delay_ms, os_job_fn_t fn, void *arg);

/* 初期化 */
void os_init(void);

#endif