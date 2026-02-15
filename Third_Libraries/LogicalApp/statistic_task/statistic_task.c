#include "statistic_task.h"

/* 启动任务的 TCB 和栈定义 */
TX_THREAD   AppTaskStartTCB;
uint64_t    AppTaskStartStk[4096/8];


TX_THREAD   AppTaskIdleTCB;
uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];
TX_THREAD   AppTaskStatTCB;
uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];


__IO float     OSCPUUsage;           /* CPU百分比 */
__IO uint8_t   OSStatRdy;            /* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;            /* 空闲任务计数 */
uint32_t       OSIdleCtrMax;         /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun;         /* 1秒内空闲任务当前计数 */
/*
* 
*    函 数 名: DispTaskInfo
*    功能说明: 将uCOS-III任务信息通过串口打印出来
*    形    参 ：无
*    返 回 值: 无
* 
*/
void DispTaskInfo(void)
{
    TX_THREAD      *p_tcb;            /* 定义一个任务控制块指针 */
    unsigned int    cpu_x100;

    p_tcb = &AppTaskStartTCB;
    
    /* 打印标题 */
    App_printf("===============================================================\r\n");
    /* 注意：很多嵌入式工程的printf默认不支持%f会导致看起来空白只剩%%
     * 因此这里用定点格式输出：xxx.yy%%
     */
    if(OSCPUUsage <= 0.0f)
    {
        cpu_x100 = 0u;
    }
    else
    {
        float tmp = OSCPUUsage * 100.0f + 0.5f;
        if(tmp >= 10000.0f)
        {
            cpu_x100 = 10000u;
        }
        else
        {
            cpu_x100 = (unsigned int)tmp;
        }
    }
    App_printf("OS CPU Usage = %3u.%02u%%\r\n", cpu_x100 / 100u, cpu_x100 % 100u);
    App_printf("===============================================================\r\n");
    App_printf(" 任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名\r\n");
    App_printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

    /* 遍历任务控制块列(TCB list)，打印所有的任务的优先级和名称 */
    while (p_tcb != (TX_THREAD *)0) 
    {
        ULONG current_stack_used;
        ULONG max_stack_used;

        /* 计算当前栈使用量：栈是向下增长的，所以是 stack_end - stack_ptr */
        /* 注意：如果任务从未运行, stack_ptr 可能等于 stack_end, 结果为 0 */
        if((ULONG)p_tcb->tx_thread_stack_end >= (ULONG)p_tcb->tx_thread_stack_ptr)
        {
            current_stack_used = (ULONG)p_tcb->tx_thread_stack_end - (ULONG)p_tcb->tx_thread_stack_ptr;
        }
        else
        {
            current_stack_used = 0u;    /* 异常情况 */
        }

        /* 计算最大栈使用量 */
        if((ULONG)p_tcb->tx_thread_stack_end >= (ULONG)p_tcb->tx_thread_stack_highest_ptr)
        {
            max_stack_used = (ULONG)p_tcb->tx_thread_stack_end - (ULONG)p_tcb->tx_thread_stack_highest_ptr;
        }
        else
        {
            max_stack_used = 0; /* 异常情况 */
        }

        /* 修正：最大栈使用不应该小于当前栈使用 */
        /* 因为 highest_ptr 可能更新不及时,所以取两者的较大值 */
        if(max_stack_used < current_stack_used)
        {
            max_stack_used = current_stack_used;
        }
        
        App_printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    current_stack_used,
                    max_stack_used,
                    p_tcb->tx_thread_name);

        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
    }
}

void DispTaskInfo_Cmd(char argc, char *argv)
{
    DispTaskInfo();
}

NR_SHELL_CMD_EXPORT(taskinfo, DispTaskInfo_Cmd,"display all threadx task info,result to be fixed");

void DispTaskInfoDetailed(void)
{
    TX_THREAD *p_tcb;
    p_tcb = &AppTaskStartTCB;

    App_printf("===========================================\r\n");
    App_printf("详细任务栈信息(Detailed Stack Info)\r\n");
    App_printf("===========================================\r\n");

    while(p_tcb != (TX_THREAD *)0)
    {
        App_printf("\r\n任务名：%s\r\n",p_tcb->tx_thread_name);
        App_printf("  优先级：%d\r\n",p_tcb->tx_thread_priority);
        App_printf("  状态：0x%X\r\n",p_tcb->tx_thread_state);
        App_printf("  栈大小： %d bytes\r\n",p_tcb->tx_thread_stack_size);
        App_printf("  栈起始地址(stack_start): 0x%08X\r\n",(unsigned int)p_tcb->tx_thread_stack_start);
        App_printf("  栈结束地址(stack_end):   0x%08X\r\n",(unsigned int)p_tcb->tx_thread_stack_end);
        App_printf("  当前栈指针(stack_ptr):   0x%08X\r\n",(unsigned int)p_tcb->tx_thread_stack_ptr);
        App_printf("  最高水位(highest_ptr):   0x%08X\r\n",(unsigned int)p_tcb->tx_thread_stack_highest_ptr);

        ULONG current_used = 0;
        ULONG max_used = 0;

        if((ULONG)p_tcb->tx_thread_stack_end >= (ULONG)p_tcb->tx_thread_stack_ptr)
        {
            current_used = ((ULONG)p_tcb->tx_thread_stack_end - (ULONG)p_tcb->tx_thread_stack_ptr);
        }

        if((ULONG)p_tcb->tx_thread_stack_end >= (ULONG)p_tcb->tx_thread_stack_ptr)
        {
            max_used = ((ULONG)p_tcb->tx_thread_stack_end - (ULONG)p_tcb->tx_thread_stack_highest_ptr);
        }
            

        App_printf("  当前使用：%d bytes\r\n", current_used);
        App_printf("  最大使用：%d bytes\r\n", max_used);
        App_printf("  剩余空间：%d bytes\r\n", p_tcb->tx_thread_stack_size - max_used);

        p_tcb = p_tcb->tx_thread_created_next;
        if(p_tcb == &AppTaskStartTCB) break;
    }
    App_printf("\r\n");
    
}

void DispTaskInfoDetailed_Cmd(char argc,char *argv)
{
    DispTaskInfoDetailed();
}

NR_SHELL_CMD_EXPORT(taskdetail,DispTaskInfoDetailed_Cmd,"display detailed threadx task stack info");

/*
* 
*    函 数 名: AppTaskStatistic
*    功能说明: 统计任务，用于实现CPU利用率的统计。为了测试更加准确，可以开启注释调用的全局中断开关
*    形    参 : thread_input 创建该任务时传递的形参 
*    返 回 值: 无
*   优 先 级: 30
* 
*/
void AppTaskStat(ULONG thread_input)
{
    (void)thread_input;

    while (OSStatRdy == FALSE) 
    {
        tx_thread_sleep(200);     /* 等待统计任务就绪 */
    }

    /* OSIdleCtrMax 在 OSSStatInit()测得同一个统计窗口 例如 100ms */
    /* 这里不要做比例缩放 否则 OSIdleCtrRun/OSIdleCtrMax 会被放大 100倍 导致 CPUUsage 负数 */
    //OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
    {
        OSCPUUsage = 0u;
    }
    
    __disable_irq();
    //OSIdleCtr = OSIdleCtrMax * 100uL;  /* 设置初始CPU利用率 0% */
    OSIdleCtr = 0ul;  /* 设置初始CPU利用率 0% */
    __enable_irq();
    
    for (;;) 
    {
        __disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* 获得100ms内空闲计数 */
        OSIdleCtr    = 0uL;          /* 复位空闲计数 */
        __enable_irq();            /* 计算100ms内的CPU利用率 */

        /* 计算 CPU 利用率：与 OSIdleCtrMax 的测量窗口保持一致 */
        if(OSIdleCtrMax > 0ul)
        {
            /* 有些情况下 OSIdleCtrRun 可能略大于 OSIdleCtrMax (计时窗口抖动/线程调度差异) 做下一钳制 */
            if(OSIdleCtrRun > OSIdleCtrMax)
            {
                OSIdleCtrRun = OSIdleCtrMax;
            }

            OSCPUUsage = 100.0f - (100.0f * (float)OSIdleCtrRun / (float)OSIdleCtrMax);
            if(OSCPUUsage < 0.0f)
            {
                OSCPUUsage = 0.0f;
            }
            else if(OSCPUUsage > 100.0f)
            {
                OSCPUUsage = 100.0f;
            }
        }
        else
        {
            OSCPUUsage = 0.0f;
        }

        tx_thread_sleep(100);        /* 每100ms统计一次 */
    }
}

/*
* 
*    函 数 名: AppTaskIDLE
*    功能说明: 空闲任务
*    形    参 : thread_input 创建该任务时传递的形参
*    返 回 值: 无
*    优 先 级: 31
* 
*/
void AppTaskIDLE(ULONG thread_input)
{    
  TX_INTERRUPT_SAVE_AREA

  (void)thread_input;
    
    while(1)
    {
       TX_DISABLE
       OSIdleCtr++;
       TX_RESTORE
    }
}
/**
  * @FunctionName: OSStatInit
  * @Author:       trx
  * @DateTime:     2022年5月24日23:52:39 
  * @Purpose:      具体的操作内容
  * @param:        void
  * @return:       none
*/
void  OSStatInit (void)
{
    OSStatRdy = FALSE;
    
    tx_thread_sleep(2u);        /* 时钟同步 */
    
    __disable_irq();
    OSIdleCtr    = 0uL;         /* 清空闲计数 */
    __enable_irq();
    
    tx_thread_sleep(100);       /* 统计100ms内，最大空闲计数 */
    
    __disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* 保存最大空闲计数 */
    OSStatRdy    = TRUE;
    __enable_irq();
}


