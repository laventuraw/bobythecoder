
/*
 * EventOS V0.2.0
 * Copyright (c) 2021, EventOS Team, <event-os@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.event-os.cn
 * https://github.com/event-os/eventos
 * https://gitee.com/event-os/eventos
 * 
 */

#ifndef EVENTOS_H_
#define EVENTOS_H_

#include "eos_def.h"
#include "eventos_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------------
EventOS Default Configuration
----------------------------------------------------------------------------- */
#ifndef EOS_MAX_PRIORITY
#define EOS_MAX_PRIORITY                        8       // 默认最多8个优先级
#endif

#ifndef EOS_MAX_OBJECTS
#define EOS_MAX_OBJECTS                         256     // 默认最多256个对象
#endif

#ifndef EOS_USE_ASSERT
#define EOS_USE_ASSERT                          1       // 默认打开断言
#endif

#ifndef EOS_USE_SM_MODE
#define EOS_USE_SM_MODE                         0       // 默认关闭状态机
#endif

#ifndef EOS_USE_PUB_SUB
#define EOS_USE_PUB_SUB                         0       // 默认关闭发布-订阅机制
#endif

#ifndef EOS_USE_TIME_EVENT
#define EOS_USE_TIME_EVENT                      0       // 默认关闭时间事件
#endif

#ifndef EOS_USE_EVENT_DATA
#define EOS_USE_EVENT_DATA                      0       // 默认关闭时间事件
#endif

#ifndef EOS_USE_EVENT_BRIDGE
#define EOS_USE_EVENT_BRIDGE                    0       // 默认关闭事件桥
#endif

#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------------
Basic type
----------------------------------------------------------------------------- */
typedef enum eos_bool
{
    EOS_False = 0,
    EOS_True = !EOS_False,
} eos_bool_t;

#define EOS_NULL                        ((void *)0)

#define EOS_TIME_FOREVER                UINT32_MAX

#if (EOS_TEST_PLATFORM == 32)
typedef uint32_t                        eos_pointer_t;
#else
#include <stdint.h>
typedef uint64_t                        eos_pointer_t;
#endif

/* -----------------------------------------------------------------------------
EventOS
----------------------------------------------------------------------------- */
extern volatile int32_t critical_count;
#define eos_interrupt_disable() do {                                           \
    __disable_irq();                                                           \
    critical_count ++;                                                         \
} while (0)

#define eos_interrupt_enable() do {                                            \
    critical_count --;                                                         \
    EOS_ASSERT(critical_count >= 0);                                           \
    if (critical_count == 0) {                                                 \
        __enable_irq();                                                        \
    }                                                                          \
} while (0)

// EventOS initialization.
void eos_init(void);
// Run EventOS.
void eos_run(void);
uint32_t eos_time(void);
// System tick function.
void eos_tick(void);
// 进入中断
void eos_interrupt_enter(void);
// 退出中断
void eos_interrupt_exit(void);
#if (EOS_USE_PREEMPTIVE != 0)
// 禁止任务切换
void eos_sheduler_lock(void);
// 关闭禁止任务切换
void eos_sheduler_unlock(void);
#endif

void eos_kernel_init(void);
void eos_kernel_start(void);

void eos_task_idle_init(void);

void eos_enter_critical(void);
void eos_exit_critical(void);
eos_u16_t eos_critical_level(void);

eos_base_t eos_hw_interrupt_disable(void);
void eos_hw_interrupt_enable(eos_base_t level);

void eos_system_timer_init(void);
void eos_system_timer_task_init(void);

eos_u32_t eos_tick_get(void);
void eos_tick_set(eos_u32_t tick);
void eos_tick_increase(void);
eos_u32_t eos_tick_from_millisecond(eos_s32_t ms);
eos_u32_t eos_tick_get_millisecond(void);

/* defunct */
void eos_task_defunct_enqueue(eos_task_handle_t task);
eos_task_handle_t eos_task_defunct_dequeue(void);

/*
 * interrupt service
 */

/*
 * eos_interrupt_enter and eos_interrupt_leave only can be called by BSP
 */
void eos_interrupt_enter(void);
void eos_interrupt_leave(void);

/*
 * the number of nested interrupts.
 */
eos_u8_t eos_interrupt_get_nest(void);

eos_err_t eos_get_errno(void);
void eos_set_errno(eos_err_t no);
int *_eos_errno(void);

/* -----------------------------------------------------------------------------
Task
----------------------------------------------------------------------------- */
/**
 * Double List structure
 */
struct eos_list_node
{
    struct eos_list_node *next;             /**< point to next node. */
    struct eos_list_node *prev;             /**< point to prev node. */
};
typedef struct eos_list_node eos_list_t;    /**< Type for lists. */

/**
 *  The object type can be one of the follows with specific
 *  macros enabled:
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Timer
 *  - Unknown
 *  - Static
 */
enum eos_obj_type
{
    EOS_Object_Null          = 0x00,        /**< The object is not used. */
    EOS_Object_Thread        = 0x01,        /**< The object is a task. */
    EOS_Object_Semaphore     = 0x02,        /**< The object is a semaphore. */
    EOS_Object_Mutex         = 0x03,        /**< The object is a mutex. */
    EOS_Object_Event         = 0x04,        /**< The object is a event. */
    EOS_Object_Timer         = 0x0a,        /**< The object is a timer. */
    EOS_Object_Unknown       = 0x0e,        /**< The object is unknown. */
    EOS_Object_Static        = 0x80         /**< The object is a static object. */
};

/**
 * The information of the kernel object
 */
struct eos_obj_info
{
    enum eos_obj_type type;                 /**< object class type */
    eos_list_t object_list;                 /**< object list */
    eos_size_t object_size;                 /**< object size */
};

/**
 * Base structure of Kernel object
 */
typedef struct eos_obj
{
    eos_u8_t type;                          /**< type of kernel object */
    eos_u8_t flag;                          /**< flag of kernel object */
    eos_list_t list;                        /**< list node of kernel object */
} eos_obj_t;
typedef eos_obj_t *eos_object_t;    /**< Type for kernel objects. */

/*
 * Defines the prototype to which the application task hook function must
 * conform.
 */
typedef void (* eos_func_t)(void *parameter);

/*
 * Definition of the event class.
 */
typedef struct eos_event
{
    const char *topic;                      // The event topic.
    uint32_t eid                    : 16;   // The event ID.
    uint32_t size                   : 16;   // The event content's size.
} eos_event_t;

/**
 * Thread structure
 */
typedef struct eos_task
{
    /* eos object */
    eos_u8_t type;                                   /**< type of object */
    eos_u8_t flags;                                  /**< task's flags */

    eos_list_t list;                                   /**< the object list */
    eos_list_t tlist;                                  /**< the task list */

    /* stack point and entry */
    void *sp;                                     /**< stack point */
    void *entry;                                  /**< entry */
    void *parameter;                              /**< parameter */
    void *stack_addr;                             /**< stack address */
    eos_u32_t stack_size;                             /**< stack size */

    /* error code */
    eos_err_t error;                                  /**< error code */

    eos_u8_t status;                                   /**< task status */

    /* priority */
    eos_u8_t  current_priority;                       /**< current priority */
    eos_u32_t number_mask;

    eos_ubase_t init_tick;                              /**< task's initialized tick */
    eos_ubase_t remaining_tick;                         /**< remaining tick */

#ifdef EOS_USING_CPU_USAGE
    eos_u64_t  duration_tick;                          /**< cpu usage tick */
#endif

    struct eos_timer task_timer;                       /**< built-in task timer */

    void (*cleanup)(struct eos_task *tid);             /**< cleanup function when task exit */

    /* light weight process if present */
    eos_ubase_t user_data;                             /**< private user data beyond this task */
} eos_task_t;

typedef struct eos_task *eos_task_handle_t;
/**
 * task control command definitions
 */
#define EOS_TASK_CTRL_STARTUP          0x00                /**< Startup task. */
#define EOS_TASK_CTRL_CLOSE            0x01                /**< Close task. */
#define EOS_TASK_CTRL_CHANGE_PRIORITY  0x02                /**< Change task priority. */
#define EOS_TASK_CTRL_INFO             0x03                /**< Get task information. */
#define EOS_TASK_CTRL_BIND_CPU         0x04                /**< Set task bind cpu. */

/*
 * task interface
 */
eos_err_t eos_task_init(struct eos_task *task,
                        const char *name,
                        void (*entry)(void *parameter),
                        void *parameter,
                        void *stack_start,
                        eos_u32_t stack_size,
                        eos_u8_t priority,
                        eos_u32_t tick);
eos_err_t eos_task_detach(eos_task_handle_t task);
eos_task_handle_t eos_task_self(void);
eos_task_handle_t eos_task_find(char *name);
eos_err_t eos_task_startup(eos_task_handle_t task);
eos_err_t eos_task_yield(void);
eos_err_t eos_task_delay(eos_u32_t tick);
eos_err_t eos_task_delay_until(eos_u32_t *tick, eos_u32_t inc_tick);
eos_err_t eos_task_mdelay(eos_s32_t ms);
eos_err_t eos_task_control(eos_task_handle_t task, int cmd, void *arg);
eos_err_t eos_task_suspend(eos_task_handle_t task);
eos_err_t eos_task_resume(eos_task_handle_t task);

// 启动任务，main函数或者任务函数中调用。
void eos_task_start(eos_task_t * const me,
                    const char *name,
                    eos_func_t func,
                    uint8_t priority,
                    void *stack_addr,
                    uint32_t stack_size,
                    void *parameter);
// 退出当前任务，任务函数中调用。
void eos_task_exit(void);
// 任务内延时，任务函数中调用，不允许在定时器的回调函数调用，不允许在空闲回调函数中调用。
void eos_delay_ms(uint32_t time_ms);
// 任务等待某特定事件，其他事件均忽略。
bool eos_task_wait_specific_event(  eos_event_t * const e_out,
                                    const char *topic, uint32_t time_ms);
// 任务阻塞式等待事件
bool eos_task_wait_event(eos_event_t * const e_out, uint32_t time_ms);

/* -----------------------------------------------------------------------------
Timer
----------------------------------------------------------------------------- */

/**
 * clock & timer macros
 */
#define EOS_TIMER_FLAG_DEACTIVATED       0x0             /**< timer is deactive */
#define EOS_TIMER_FLAG_ACTIVATED         0x1             /**< timer is active */
#define EOS_TIMER_FLAG_ONE_SHOT          0x0             /**< one shot timer */
#define EOS_TIMER_FLAG_PERIODIC          0x2             /**< periodic timer */

#define EOS_TIMER_FLAG_HARD_TIMER        0x0             /**< hard timer,the timer's callback function will be called in tick isr. */
#define EOS_TIMER_FLAG_SOFT_TIMER        0x4             /**< soft timer,the timer's callback function will be called in timer task. */

#define EOS_TIMER_CTRL_SET_TIME          0x0             /**< set timer control command */
#define EOS_TIMER_CTRL_GET_TIME          0x1             /**< get timer control command */
#define EOS_TIMER_CTRL_SET_ONESHOT       0x2             /**< change timer to one shot */
#define EOS_TIMER_CTRL_SET_PERIODIC      0x3             /**< change timer to periodic */
#define EOS_TIMER_CTRL_GET_STATE         0x4             /**< get timer run state active or deactive*/
#define EOS_TIMER_CTRL_GET_REMAIN_TIME   0x5             /**< get the remaining hang time */

#ifndef EOS_TIMER_SKIP_LIST_LEVEL
#define EOS_TIMER_SKIP_LIST_LEVEL        1
#endif

/* 1 or 3 */
#ifndef EOS_TIMER_SKIP_LIST_MASK
#define EOS_TIMER_SKIP_LIST_MASK         0x3
#endif

/*
 * Definition of the timer class.
 */
typedef struct eos_timer
{
    struct eos_timer *next;
    uint32_t time;
    uint32_t time_out;
    eos_func_t callback;
    uint32_t oneshoot               : 1;
    uint32_t running                : 1;
} eos_timer_t;

/**
 * timer structure
 */
typedef struct eos_timer
{
    eos_obj_t super;

    eos_list_t row[EOS_TIMER_SKIP_LIST_LEVEL];

    void (*timeout_func)(void *parameter);
    void *parameter;

    eos_u32_t init_tick;
    eos_u32_t timeout_tick;
} eos_timer_t;
typedef struct eos_timer *eos_timer_handle_t;

// 启动软定时器，允许在中断中调用。
void eos_timer_start(eos_timer_t * const me,
                     const char *name,
                     uint32_t time_ms,
                     bool oneshoot,
                     eos_func_t callback);
// 删除软定时器，允许在中断中调用。
void eos_timer_delete(const char *name);
// 暂停软定时器，允许在中断中调用。
void eos_timer_pause(const char *name);
// 继续软定时器，允许在中断中调用。
void eos_timer_continue(const char *name);
// 重启软定时器的定时，允许在中断中调用。
void eos_timer_reset(const char *name);

void eos_timer_init(eos_timer_handle_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void *parameter,
                   eos_u32_t time,
                   eos_u8_t flag);
eos_err_t eos_timer_detach(eos_timer_handle_t timer);
eos_err_t eos_timer_start(eos_timer_handle_t timer);
eos_err_t eos_timer_stop(eos_timer_handle_t timer);
eos_err_t eos_timer_control(eos_timer_handle_t timer, int cmd, void *arg);

eos_u32_t eos_timer_next_timeout_tick(void);
void eos_timer_check(void);


/* -----------------------------------------------------------------------------
Semaphore
----------------------------------------------------------------------------- */
/**
 * IPC flags and control command definitions
 */
#define EOS_IPC_FLAG_FIFO                0x00            /**< FIFOed IPC. @ref IPC. */
#define EOS_IPC_FLAG_PRIO                0x01            /**< PRIOed IPC. @ref IPC. */

#define EOS_WAITING_FOREVER              -1              /**< Block forever until get resource. */
#define EOS_WAITING_NO                   0               /**< Non-block. */

/**
 * Base structure of IPC object
 */
struct eos_ipc_object
{
    eos_obj_t super;                            /**< inherit from eos_object */

    eos_list_t suspend_task;                    /**< tasks pended on this resource */
};

#ifdef EOS_USING_SEMAPHORE
/**
 * Semaphore structure
 */
struct eos_semaphore
{
    struct eos_ipc_object super;                        /**< inherit from ipc_object */

    eos_u16_t value;                         /**< value of semaphore. */
};
typedef struct eos_semaphore *eos_sem_t;
#endif

#ifdef EOS_USING_SEMAPHORE
/*
 * semaphore interface
 */
eos_err_t eos_sem_init(eos_sem_t sem,
                       const char *name,
                       eos_u32_t value,
                       eos_u8_t flag);
eos_err_t eos_sem_detach(eos_sem_t sem);

eos_err_t eos_sem_take(eos_sem_t sem, eos_s32_t time);
eos_err_t eos_sem_trytake(eos_sem_t sem);
eos_err_t eos_sem_release(eos_sem_t sem);
eos_err_t eos_sem_reset(eos_sem_t sem, eos_ubase_t value);
#endif

/* -----------------------------------------------------------------------------
Mutex
----------------------------------------------------------------------------- */

#ifdef EOS_USING_MUTEX
/**
 * Mutual exclusion (mutex) structure
 */
struct eos_mutex
{
    struct eos_ipc_object super;                        /**< inherit from ipc_object */

    eos_u16_t value;                         /**< value of mutex */

    eos_u8_t prio_bkp;             /**< priority of last task hold the mutex */
    eos_u8_t hold;                          /**< numbers of task hold the mutex */

    struct eos_task *owner;                         /**< current owner of mutex */
};
typedef struct eos_mutex *eos_mutex_t;
#endif


#ifdef EOS_USING_MUTEX
/*
 * mutex interface
 */
eos_err_t eos_mutex_init(eos_mutex_t mutex, const char *name, eos_u8_t flag);
eos_err_t eos_mutex_detach(eos_mutex_t mutex);

eos_err_t eos_mutex_take(eos_mutex_t mutex, eos_s32_t time);
eos_err_t eos_mutex_trytake(eos_mutex_t mutex);
eos_err_t eos_mutex_release(eos_mutex_t mutex);
#endif

/* -----------------------------------------------------------------------------
Event
----------------------------------------------------------------------------- */
// 事件的直接发送 -----------------------------------------
// 直接发送事件。允许在中断中调用。
void eos_event_send(const char *task, const char *topic);
// 延迟发送事件。
void eos_event_send_delay(const char *task, const char *topic, uint32_t time_delay_ms);
// 周期发送事件。
void eos_event_send_period(const char *task, const char *topic, uint32_t time_period_ms);

// 事件的发布 --------------------------------------------
// 发布主题事件。允许在中断中调用。
void eos_event_publish(const char *topic);
// 延时发布某事件。允许在中断中调用。
void eos_event_publish_delay(const char *topic, uint32_t time_delay_ms);
// 周期发布某事件。允许在中断中调用。
void eos_event_publish_period(const char *topic, uint32_t time_period_ms);
// 取消某延时或者周期事件的发布。允许在中断中调用。
void eos_event_time_cancel(const char *topic);

// 事件的订阅 --------------------------------------------
// 事件订阅，仅在任务函数、状态函数或者事件回调函数中使用。
void eos_event_sub(const char *topic);
// 事件取消订阅，仅在任务函数、状态函数或者事件回调函数中使用。
void eos_event_unsub(const char *topic);

// 事件的接收 --------------------------------------------
// 事件接收。仅在任务函数、状态函数或者事件回调函数中使用。
bool eos_event_topic(eos_event_t const * const e, const char *topic);

/* -----------------------------------------------------------------------------
Database
----------------------------------------------------------------------------- */
#define EOS_DB_ATTRIBUTE_LINK_EVENT      ((uint8_t)0x40U)
#define EOS_DB_ATTRIBUTE_PERSISTENT      ((uint8_t)0x20U)
#define EOS_DB_ATTRIBUTE_VALUE           ((uint8_t)0x01U)
#define EOS_DB_ATTRIBUTE_STREAM          ((uint8_t)0x02U)

// 事件数据库的初始化
void eos_db_init(void *const memory, uint32_t size);
// 事件数据库的注册。
void eos_db_register(const char *topic, uint32_t size, uint8_t attribute);
// 块数据的读取。
void eos_db_block_read(const char *topic, void * const data);
// 块数据的写入。允许在中断中调用。
void eos_db_block_write(const char *topic, void * const data);
// 流数据的读取。
int32_t eos_db_stream_read(const char *topic, void *const buffer, uint32_t size);
// 流数据的写入。允许在中断中调用。
void eos_db_stream_write(const char *topic, void *const buffer, uint32_t size);

/* -----------------------------------------------------------------------------
Reactor
----------------------------------------------------------------------------- */
// 事件处理句柄的定义
struct eos_reactor;
typedef void (* eos_event_handler)( struct eos_reactor *const me,
                                    eos_event_t const * const e);

/*
 * Definition of the Reactor class.
 */
typedef struct eos_reactor
{
    eos_task_t super;
    eos_event_handler event_handler;
} eos_reactor_t;

void eos_reactor_init(  eos_reactor_t * const me,
                        const char *name,
                        uint8_t priority,
                        void *stack, uint32_t size);
void eos_reactor_start(eos_reactor_t * const me, eos_event_handler event_handler);

#define EOS_HANDLER_CAST(handler)       ((eos_event_handler)(handler))

/* -----------------------------------------------------------------------------
State machine
----------------------------------------------------------------------------- */
/*
 * Definition of the EventOS reture value.
 */
typedef enum eos_ret
{
    EOS_Ret_Null = 0,                       // 无效值
    EOS_Ret_Handled,                        // 已处理，不产生跳转
    EOS_Ret_Super,                          // 到超状态
    EOS_Ret_Tran,                           // 跳转
} eos_ret_t;

#if (EOS_USE_SM_MODE != 0)
// 状态函数句柄的定义
struct eos_sm;
typedef eos_ret_t (* eos_state_handler)(struct eos_sm *const me,
                                        eos_event_t const * const e);
#endif

#if (EOS_USE_SM_MODE != 0)
// 状态机类
typedef struct eos_sm
{
    eos_task_t super;
    volatile eos_state_handler state;
} eos_sm_t;
#endif

#if (EOS_USE_SM_MODE != 0)
// 状态机初始化函数
void eos_sm_init(   eos_sm_t * const me,
                    const char *name,
                    uint8_t priority,
                    void *stack, uint32_t size);
void eos_sm_start(eos_sm_t * const me, eos_state_handler state_init);

eos_ret_t eos_tran(eos_sm_t * const me, eos_state_handler state);
eos_ret_t eos_super(eos_sm_t * const me, eos_state_handler state);
eos_ret_t eos_state_top(eos_sm_t * const me, eos_event_t const * const e);

#define EOS_TRAN(target)            eos_tran((eos_sm_t * )me, (eos_state_handler)target)
#define EOS_SUPER(super)            eos_super((eos_sm_t * )me, (eos_state_handler)super)
#define EOS_STATE_CAST(state)       ((eos_state_handler)(state))
#endif

/* -----------------------------------------------------------------------------
Assert
----------------------------------------------------------------------------- */
#ifndef EOS_USE_ASSERT

#define EOS_TAG(name_)
#define EOS_ASSERT(test_)                       ((void)0)
#define EOS_ASSERT_ID(id_, test_)               ((void)0)
#define EOS_ASSERT_NAME(id_, test_)             ((void)0)
#define EOS_ASSERT_INFO(test_, ...)             ((void)0)

#else

/* User defined module name. */
#define EOS_TAG(name_)                                                         \
    static char const ___tag_name[] = name_;

/* General assert */
#define EOS_ASSERT(test_) ((test_)                                             \
    ? (void)0 : eos_port_assert(___tag_name, EOS_NULL, (uint32_t)__LINE__))

/* General assert with ID */
#define EOS_ASSERT_ID(id_, test_) ((test_)                                     \
    ? (void)0 : eos_port_assert(___tag_name, EOS_NULL, (uint32_t)(id_)))

/* General assert with name string or event topic. */
#define EOS_ASSERT_NAME(test_, name_) ((test_)                                 \
    ? (void)0 : eos_port_assert(___tag_name, name_, (uint32_t)(__LINE__)))
        
/* Assert with printed information. */
#define EOS_ASSERT_INFO(test_, ...) ((test_)                                   \
    ? (void)0 : elog_assert_info(___tag_name, __VA_ARGS__))

#endif

/* -----------------------------------------------------------------------------
Log
----------------------------------------------------------------------------- */
#if (EOS_USE_LOG != 0)
#include "elog.h"

#define EOS_PRINT(...)            elog_printf(__VA_ARGS__)
#define EOS_DEBUG(...)            elog_debug(___tag_name, __VA_ARGS__)
#define EOS_INFO(...)             elog_info(___tag_name, __VA_ARGS__)
#define EOS_WARN(...)             elog_warn(___tag_name, __VA_ARGS__)
#define EOS_ERROR(...)            elog_error(___tag_name, __VA_ARGS__)
#endif

/* -----------------------------------------------------------------------------
Trace
----------------------------------------------------------------------------- */
#if (EOS_USE_STACK_USAGE != 0)
// 任务的堆栈使用率
uint8_t eos_task_stack_usage(uint8_t priority);
#endif

#if (EOS_USE_CPU_USAGE != 0)
// 任务的CPU使用率
uint8_t eos_task_cpu_usage(uint8_t priority);
// 监控函数，放进一个单独的定时器中断函数，中断频率为SysTick的10-20倍。
void eos_cpu_usage_monitor(void);
#endif

/* -----------------------------------------------------------------------------
Port
----------------------------------------------------------------------------- */
void eos_port_task_switch(void);
void eos_port_assert(const char *tag, const char *name, uint32_t id);

/* -----------------------------------------------------------------------------
Hook
----------------------------------------------------------------------------- */
// 空闲回调函数
void eos_hook_idle(void);

// 结束EventOS的运行的时候，所调用的回调函数。
void eos_hook_stop(void);

// 启动EventOS的时候，所调用的回调函数
void eos_hook_start(void);

#ifdef __cplusplus
}
#endif

#endif
