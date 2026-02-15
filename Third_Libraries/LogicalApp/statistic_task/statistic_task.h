#ifndef _STATISTIC_TASK_H_
#define _STATISTIC_TASK_H_

#include "sys.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
#define  APP_CFG_TASK_IDLE_PRIO          31u
#define  APP_CFG_TASK_IDLE_STK_SIZE      1024u

#define  APP_CFG_TASK_STAT_PRIO          30u
#define  APP_CFG_TASK_STAT_STK_SIZE      1024u

/** 启动任务 */
extern TX_THREAD    AppTaskStartTCB;
extern uint64_t     AppTaskStartStk[4096/8];

/** 统计任务 */
extern TX_THREAD   AppTaskStatTCB;
extern uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];

/** 空闲任务 */
extern TX_THREAD   AppTaskIdleTCB;
extern uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];


void  DispTaskInfo          (void);
void  AppTaskIDLE           (ULONG thread_input);
void  AppTaskStat           (ULONG thread_input);
void  OSStatInit            (void);

#ifdef __cplusplus
}
#endif
#endif

