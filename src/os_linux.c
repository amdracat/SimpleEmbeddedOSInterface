#include "os.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/* =========================
 * 内部ジョブキュー
 * ========================= */

typedef struct job {
    os_job_fn_t fn;
    void *arg;
    struct job *next;
} job_t;

static job_t *job_head = NULL;
static job_t *job_tail = NULL;

static pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t job_cond = PTHREAD_COND_INITIALIZER;

/* =========================
 * イベント管理
 * ========================= */

#define MAX_EVENT 32
#define MAX_SUBSCRIBER 8

typedef struct {
    os_event_cb_t cb;
    void *arg;
} subscriber_t;

static subscriber_t subscribers[MAX_EVENT][MAX_SUBSCRIBER];

/* =========================
 * キュー
 * ========================= */

struct os_queue {
    void **buffer;
    int capacity;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/* =========================
 * ワーカースレッド
 * ========================= */

static void* worker_thread(void *arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&job_mutex);

        while (job_head == NULL) {
            pthread_cond_wait(&job_cond, &job_mutex);
        }

        job_t *job = job_head;
        job_head = job->next;
        if (job_head == NULL) job_tail = NULL;

        pthread_mutex_unlock(&job_mutex);

        job->fn(job->arg);
        free(job);
    }
    return NULL;
}

/* =========================
 * API実装
 * ========================= */

void os_init(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker_thread, NULL);
}

/* 非同期実行 */
void os_post(os_job_fn_t fn, void *arg) {
    job_t *job = malloc(sizeof(job_t));
    job->fn = fn;
    job->arg = arg;
    job->next = NULL;

    pthread_mutex_lock(&job_mutex);

    if (job_tail) {
        job_tail->next = job;
    } else {
        job_head = job;
    }
    job_tail = job;

    pthread_cond_signal(&job_cond);
    pthread_mutex_unlock(&job_mutex);
}

/* キュー */

os_queue_t* os_queue_create(int capacity) {
    os_queue_t *q = malloc(sizeof(os_queue_t));
    q->buffer = malloc(sizeof(void*) * capacity);
    q->capacity = capacity;
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

bool os_queue_send(os_queue_t *q, void *msg) {
    pthread_mutex_lock(&q->mutex);

    if (q->count == q->capacity) {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }

    q->buffer[q->tail] = msg;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return true;
}

void* os_queue_recv(os_queue_t *q) {
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    void *msg = q->buffer[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->count--;

    pthread_mutex_unlock(&q->mutex);
    return msg;
}

/* イベント */

void os_event_subscribe(os_event_id_t id, os_event_cb_t cb, void *arg) {
    for (int i = 0; i < MAX_SUBSCRIBER; i++) {
        if (subscribers[id][i].cb == NULL) {
            subscribers[id][i].cb = cb;
            subscribers[id][i].arg = arg;
            return;
        }
    }
}

static void event_dispatch(void *arg) {
    int id = (int)(intptr_t)arg;

    for (int i = 0; i < MAX_SUBSCRIBER; i++) {
        if (subscribers[id][i].cb) {
            subscribers[id][i].cb(subscribers[id][i].arg);
        }
    }
}

void os_event_publish(os_event_id_t id) {
    os_post(event_dispatch, (void*)(intptr_t)id);
}

/* タイマー */

typedef struct {
    uint32_t delay;
    os_job_fn_t fn;
    void *arg;
} timer_arg_t;

static void* timer_thread(void *arg) {
    timer_arg_t *t = arg;
    usleep(t->delay * 1000);
    os_post(t->fn, t->arg);
    free(t);
    return NULL;
}

void os_schedule(uint32_t delay_ms, os_job_fn_t fn, void *arg) {
    pthread_t tid;

    timer_arg_t *t = malloc(sizeof(timer_arg_t));
    t->delay = delay_ms;
    t->fn = fn;
    t->arg = arg;

    pthread_create(&tid, NULL, timer_thread, t);
    pthread_detach(tid);
}
