#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    OS_MODE_ASYNC,
    OS_MODE_MANUAL
} os_mode_t;


typedef void (*os_job_fn_t)(void *arg);


/* 初期化 */
void os_init(os_mode_t mode);
os_mode_t os_get_mode(void);



/* 非同期実行 */
void os_post(os_job_fn_t fn, void *arg);

/* キュー */
#if 0
typedef struct os_queue os_queue_t;

os_queue_t* os_queue_create(int capacity);
bool os_queue_send(os_queue_t *q, void *msg);
void* os_queue_recv(os_queue_t *q);
#endif

/* イベント */
typedef int os_event_id_t;
typedef void (*os_event_cb_t)(void *arg);

void os_event_subscribe(os_event_id_t id, os_event_cb_t cb, void *arg);
void os_event_publish(os_event_id_t id);

/* タイマー */
void os_schedule(uint32_t delay_ms, os_job_fn_t fn, void *arg);



/* 手動スケジューリング（OS_MODE_MANUAL） */
void os_sim_run_one(void);
void os_sim_run_all(void);
bool os_sim_has_pending(void);
uint32_t os_sim_now_ms(void);
void os_sim_advance_time(uint32_t ms);

#endif