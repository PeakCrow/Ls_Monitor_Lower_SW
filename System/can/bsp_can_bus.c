#include "bsp_can_bus.h"

/* 使用最新的hal库提供的can外设文件 */
CAN_HandleTypeDef hCAN1;
CAN_HandleTypeDef hCAN2;

CAN_RxHeaderTypeDef can_rx_msg;
uint8_t g_canrxbuf[8] = {0};

#define BSP_CAN_LOG_ECHO TRUE

/**
  * @FunctionName: bsp_InitCan1Bus
  * @Author:       trx
  * @DateTime:     2022年5月22日 17:36:06 
  * @Purpose:      can1总线外设初始化
  * @param:        void
  * @return:       none
*/
void bsp_InitCan1Bus(void)
{
    CAN_FilterTypeDef  sFilterConfig;
    CAN_FilterTypeDef  sFilterConfig2;
    
    /*CAN单元初始化*/
    hCAN1.Instance                       = CANx_BUS_1;             /* CAN外设 */
    hCAN1.Init.Prescaler                 = CAN1_BUS_BAUDE_RATE;    /* BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+6+7)/6=500kbps */
    hCAN1.Init.Mode                      = CAN_MODE_NORMAL;        /* 正常工作模式 */
    hCAN1.Init.SyncJumpWidth             = CAN_SJW_1TQ;            /* BTR-SJW 重新同步跳跃宽度 1个时间单元 */
    hCAN1.Init.TimeSeg1                  = CAN_BS1_6TQ;            /* BTR-TS1 时间段1 占用了6个时间单元 */
    hCAN1.Init.TimeSeg2                  = CAN_BS2_7TQ;            /* BTR-TS1 时间段2 占用了7个时间单元 */
    hCAN1.Init.TimeTriggeredMode         = DISABLE;                /* MCR-TTCM  关闭时间触发通信模式使能 */
    hCAN1.Init.AutoBusOff                = DISABLE;                /* MCR-ABOM  自动离线管理 */
    hCAN1.Init.AutoWakeUp                = DISABLE;                /* MCR-AWUM  使用自动唤醒模式 */
    hCAN1.Init.AutoRetransmission        = ENABLE;                 /* MCR-NART  禁止报文�    �动重传 	DISABLE-自动重传 */
    hCAN1.Init.ReceiveFifoLocked         = DISABLE;                /* MCR-RFLM  接收FIFO 锁    ��模式	DISABLE-溢出时新报文会覆盖原有报文 */
    hCAN1.Init.TransmitFifoPriority      = DISABLE;                /* MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 */
    
    HAL_StatusTypeDef ret = HAL_CAN_Init(&hCAN1);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_Init CAN1 ret not ok %d !\n",ret);
    }
    
    /*CAN过滤器初始化*/
    sFilterConfig.FilterMode            = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig.FilterScale           = CAN_FILTERSCALE_32BIT; /* 过滤器位宽为单个32位。*/
    /* 使能报文标识符过滤器按照标识符的内容进行对比过滤，扩展ID不是如下的旧抛弃掉，是的话，会存入FIFO0 */
    /* id左移3位，是为了将0位保留位、1位RTR位、2位IDE位补0 */
    /* 右移16位是将整个id的高16位取出赋值给过滤器高位 */
    sFilterConfig.FilterIdHigh          = (((uint32_t)0x1314<<3)&0xFFFF0000)>>16;       /* 要过滤的ID高位 */
    sFilterConfig.FilterIdLow           = (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
    sFilterConfig.FilterMaskIdHigh      = 0xFFFF;               /* 过滤器高16位每位必须匹配 */
    sFilterConfig.FilterMaskIdLow       = 0xFFFF;               /* 过滤器低16位每位必须匹配 */
    sFilterConfig.FilterFIFOAssignment  = CAN_RX_FIFO0;         /* 过滤器被关联到FIFO 0 */
    sFilterConfig.FilterActivation      = ENABLE;               /* 使能过滤器 */
    sFilterConfig.FilterBank            = 14;
    ret = HAL_CAN_ConfigFilter(&hCAN1, &sFilterConfig);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter0 CAN1 ret not ok %d !\n",ret);
    }
    
    /*CAN过滤器初始化*/
    sFilterConfig2.FilterMode           = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig2.FilterScale          = CAN_FILTERSCALE_32BIT;  /* 过滤器位宽为单个32位。*/
    sFilterConfig2.FilterIdHigh         = (((uint32_t)0x1315<<3)&0xFFFF0000)>>16;       /* 要过滤的ID高位 */
    sFilterConfig2.FilterIdLow          = (((uint32_t)0x1315<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
    sFilterConfig2.FilterMaskIdHigh     = 0xFFFF;                 /* 过滤器高16位每位必须匹配 */
    sFilterConfig2.FilterMaskIdLow      = 0xFFFF;                 /* 过滤器低16位每位必须匹配 */
    sFilterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO1;       /* 过滤器被关联到FIFO 1 */
    sFilterConfig2.FilterActivation     = ENABLE;                 /* 使能过滤器 */
    sFilterConfig2.FilterBank           = 10;
    ret = HAL_CAN_ConfigFilter(&hCAN1, &sFilterConfig2);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter1 CAN1 ret not ok %d !\n",ret);
    }

    /*CAN过滤器初始化*/
    sFilterConfig2.FilterMode           = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig2.FilterScale          = CAN_FILTERSCALE_32BIT;  /* 过滤器位宽为单个32位。*/
    sFilterConfig2.FilterIdHigh         = 0;                      /* 要过滤的ID高位 */
    sFilterConfig2.FilterIdLow          = 0;                      /* 要过滤的ID低位 */
    sFilterConfig2.FilterMaskIdHigh     = 0;                      /* 过滤器高16位每位必须匹配 */
    sFilterConfig2.FilterMaskIdLow      = 0;                      /* 过滤器低16位每位必须匹配 */
    sFilterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO0;       /* 过滤器被关联到FIFO 1 */
    sFilterConfig2.FilterActivation     = ENABLE;                 /* 使能过滤器 */
    sFilterConfig2.FilterBank           = 0;
    ret = HAL_CAN_ConfigFilter(&hCAN1, &sFilterConfig2);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter2 CAN1 ret not ok %d !\n",ret);
    }
    
    
    HAL_CAN_Start(&hCAN1);
    
    /* 配置邮箱0接收过程中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO0_MSG_PENDING config error!\r\n");
    }

    /* 配置邮箱0接收完全中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO0_FULL) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO0_FULL config error!\r\n");
    }

    /* 配置邮箱接0收溢出中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO0_OVERRUN) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO0_OVERRUN config error!\r\n");
    }

    /* 配置邮箱接收过程中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO1_MSG_PENDING config error!\r\n");
    }

    /* 配置邮箱接收完全中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO1_FULL) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO1_FULL config error!\r\n");
    }

    /* 配置邮箱1接收溢出中断 */
    if(HAL_CAN_ActivateNotification(&hCAN1,CAN_IT_RX_FIFO1_OVERRUN) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_RX_FIFO1_OVERRUN config error!\r\n");
    }
    
    /* 配置邮箱发送空中断 */
    if (HAL_CAN_ActivateNotification(&hCAN1, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
    {
        mini_printf("CAN1 CAN_IT_TX_MAILBOX_EMPTY config error!\r\n");
    }
}

/**
  * @FunctionName: bsp_Can1_Send_buf
  * @Author:       trx
  * @DateTime:     2022年5月25日 20:58:40 
  * @Purpose:      can发送函数，支持一次发送3个以上的报文，不然一次最多只能发送3条
  * @param:        _id                要发送的报文id
  * @param:        _buf[]             报文的数据
  * @param:        _dlc               报文数据的长度，0-8可选
  * @return:       HAL_OK:代表发送成功;HAL_ERROR:发送失败
*/
HAL_StatusTypeDef bsp_Can1_Send_buf(uint32_t _id,uint8_t _buf[],uint8_t _dlc)
{
    CAN_TxHeaderTypeDef can_tx_msg;
    if(IS_CAN_STDID(_id))
    {
        can_tx_msg.StdId = _id;
        can_tx_msg.IDE = CAN_ID_STD;
    }
    else if(IS_CAN_EXTID(_id))
    {
        can_tx_msg.ExtId = _id;
        can_tx_msg.IDE = CAN_ID_EXT;
    }
    else
    {
        PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    if (_dlc >8)
    {
        return HAL_ERROR;
    }
    else
    {
        can_tx_msg.DLC = _dlc;
    }
    can_tx_msg.RTR = CAN_RTR_DATA;          /* 默认都是数据帧 */
    can_tx_msg.TransmitGlobalTime = DISABLE;
    
    if(HAL_CAN_IsTxMessagePending(&hCAN1,CAN_TX_MAILBOX0) == HAL_OK)/* 代表没有挂起的发送报文  */
    {
        HAL_CAN_AddTxMessage(&hCAN1,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX0);
    }
    else if(HAL_CAN_IsTxMessagePending(&hCAN1,CAN_TX_MAILBOX0) != HAL_OK)
    {
        HAL_CAN_AddTxMessage(&hCAN1,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX1);
    }
    else if(HAL_CAN_IsTxMessagePending(&hCAN1,CAN_TX_MAILBOX0) != HAL_OK)
    {
        HAL_CAN_AddTxMessage(&hCAN1,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX2);
    }
    else
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

/**
  * @FunctionName: CAN1_TX_IRQHandler
  * @Author:       trx
  * @DateTime:     2025年5月5日 21:19:29
  * @Purpose:      can 报文发送 中断
  * @param:        none
  * @return:       none
*/
void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hCAN1);
}

/**
  * @FunctionName: CAN1_RX0_IRQHandler
  * @Author:       trx
  * @DateTime:     2022年5月26日 18:15:34 
  * @Purpose:      can接收中断函数，底层库提供
  * @param:        void
  * @return:       none
*/
void CAN1_RX0_IRQHandler(void)
{
    memset(&can_rx_msg,0,sizeof(can_rx_msg));
    memset(&g_canrxbuf,0,sizeof(g_canrxbuf));
    
    HAL_CAN_IRQHandler(&hCAN1);              /* 需要在中断函数中调用此函数来清除中断标志位 */
    HAL_CAN_GetRxMessage(&hCAN1, CAN_FILTER_FIFO0,&can_rx_msg,g_canrxbuf);
#if TRUE == BSP_CAN_LOG_ECHO
    mini_printf("\r\n");
    mini_printf("can1 rx0 receive blow: \r\n");
    mini_printf("CAN_RxHeaderTypeDef.StdId is           0x%x\r\n",can_rx_msg.StdId);
    mini_printf("CAN_RxHeaderTypeDef.ExtId is           0x%x\r\n",can_rx_msg.ExtId);
    mini_printf("CAN_RxHeaderTypeDef.IDE is             %d\r\n",can_rx_msg.IDE);
    mini_printf("CAN_RxHeaderTypeDef.RTR is             %d\r\n",can_rx_msg.RTR);
    mini_printf("CAN_RxHeaderTypeDef.DLC is             %d\r\n",can_rx_msg.DLC);
    mini_printf("CAN_RxHeaderTypeDef.Timestamp is       %d\r\n",can_rx_msg.Timestamp);
    for(uint8_t i = 0;i < can_rx_msg.DLC;i++)
    {
        mini_printf("CAN_Infomation[%d] = 0x%02x\r\n",i,g_canrxbuf[i]);
    }
#endif
}


/**
  * @FunctionName: bsp_Can1_Receive_buf
  * @Author:       trx
  * @DateTime:     2022年5月26日 20:37:22 
  * @Purpose:      can报文接收函数，目前只配置了一个id
                    而且好像fifo1无法正常使用
                    并且fifo0与fifo1都无法进入邮箱填满函数
  * @param:        _id   ：要接收报文的id
  * @param:        _buf[]：报文数据数组
  * @return:       DLC:返回报文的数据长度
*/
uint8_t bsp_Can1_Receive_buf(uint32_t _id,uint8_t _buf[])
{
    uint8_t i;
    uint8_t g_canrxbuf[8] = {0};
    if(IS_CAN_STDID(_id))
    {
        if(can_rx_msg.StdId == _id)
        {
            for(i = 0;i < can_rx_msg.DLC;i++)
            _buf[i] = g_canrxbuf[i];
        }
        else
            PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    else if(IS_CAN_EXTID(_id))
    {
    if(can_rx_msg.ExtId == _id)
    {
        for(i = 0;i < can_rx_msg.DLC;i++)
        {
            _buf[i] = g_canrxbuf[i];
        }
    }
    else
        PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    return can_rx_msg.DLC;
}


/**
  * @FunctionName: bsp_InitCan2Bus
  * @Author:       trx
  * @DateTime:     2022年5月22日 17:36:06 
  * @Purpose:      can1总线外设初始化
  * @param:        void
  * @return:       none
*/
void bsp_InitCan2Bus(void)
{
    CAN_FilterTypeDef  sFilterConfig;
    CAN_FilterTypeDef  sFilterConfig2;
    
    /*CAN单元初始化*/
    hCAN2.Instance                       = CANx_BUS_2;             /* CAN外设 */
    hCAN2.Init.Prescaler                 = CAN2_BUS_BAUDE_RATE;    /* BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+6+7)/6=500kbps */
    hCAN2.Init.Mode                      = CAN_MODE_NORMAL;        /* 正常工作模式 */
    hCAN2.Init.SyncJumpWidth             = CAN_SJW_1TQ;            /* BTR-SJW 重新同步跳跃宽度 1个时间单元 */
    hCAN2.Init.TimeSeg1                  = CAN_BS1_6TQ;            /* BTR-TS1 时间段1 占用了6个时间单元 */
    hCAN2.Init.TimeSeg2                  = CAN_BS2_7TQ;            /* BTR-TS1 时间段2 占用了7个时间单元 */
    hCAN2.Init.TimeTriggeredMode         = DISABLE;                /* MCR-TTCM  关闭时间触发通信模式使能 */
    hCAN2.Init.AutoBusOff                = ENABLE;                 /* MCR-ABOM  自动离线管理 */
    hCAN2.Init.AutoWakeUp                = ENABLE;                 /* MCR-AWUM  使用自动唤醒模式 */
    hCAN2.Init.AutoRetransmission        = DISABLE;                /* MCR-NART  禁止报文蠠  ˥ʨ重传 	DISABLE-自动重传 */
    hCAN2.Init.ReceiveFifoLocked         = DISABLE;                /* MCR-RFLM  接收FIFO 锁    Κ模式	DISABLE-溢出时新报文会覆盖原有报文 */
    hCAN2.Init.TransmitFifoPriority      = ENABLE;                 /* MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 */
    
    HAL_StatusTypeDef ret = HAL_CAN_Init(&hCAN2);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_Init CAN2 ret not ok %d !\n",ret);
    }
    
    /*CAN过滤器初始化*/
    sFilterConfig.FilterMode            = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig.FilterScale           = CAN_FILTERSCALE_32BIT; /* 过滤器位宽为单个32位。*/
    /* 使能报文标识符过滤器按照标识符的内容进行对比过滤，扩展ID不是如下的旧抛弃掉，是的话，会存入FIFO0 */
    /* id左移3位，是为了将0位保留位、1位RTR位、2位IDE位补0 */
    /* 右移16位是将整个id的高16位取出赋值给过滤器高位 */
    sFilterConfig.FilterIdHigh          = (((uint32_t)0x1314<<3)&0xFFFF0000)>>16;       /* 要过滤的ID高位 */
    sFilterConfig.FilterIdLow           = (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
    sFilterConfig.FilterMaskIdHigh      = 0xFFFF;               /* 过滤器高16位每位必须匹配 */
    sFilterConfig.FilterMaskIdLow       = 0xFFFF;               /* 过滤器低16位每位必须匹配 */
    sFilterConfig.FilterFIFOAssignment  = CAN_FILTER_FIFO0;     /* 过滤器被关联到FIFO 0 */
    sFilterConfig.FilterActivation      = ENABLE;               /* 使能过滤器 */
    sFilterConfig.FilterBank            = 14;
    ret = HAL_CAN_ConfigFilter(&hCAN2, &sFilterConfig);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter0 CAN2 ret not ok %d !\n",ret);
    }
    
    /*CAN过滤器初始化*/
    sFilterConfig2.FilterMode           = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig2.FilterScale          = CAN_FILTERSCALE_32BIT;  /* 过滤器位宽为单个32位。*/
    sFilterConfig2.FilterIdHigh         = (((uint32_t)0x1315<<3)&0xFFFF0000)>>16;       /* 要过滤的ID高位 */
    sFilterConfig2.FilterIdLow          = (((uint32_t)0x1315<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
    sFilterConfig2.FilterMaskIdHigh     = 0xFFFF;                 /* 过滤器高16位每位必须匹配 */
    sFilterConfig2.FilterMaskIdLow      = 0xFFFF;                 /* 过滤器低16位每位必须匹配 */
    sFilterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO1;       /* 过滤器被关联到FIFO 1 */
    sFilterConfig2.FilterActivation     = ENABLE;                 /* 使能过滤器 */
    sFilterConfig2.FilterBank           = 10;
    ret = HAL_CAN_ConfigFilter(&hCAN2, &sFilterConfig2);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter1 CAN2 ret not ok %d !\n",ret);
    }

    /*CAN过滤器初始化*/
    sFilterConfig2.FilterMode           = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
    sFilterConfig2.FilterScale          = CAN_FILTERSCALE_32BIT;  /* 过滤器位宽为单个32位。*/
    sFilterConfig2.FilterIdHigh         = 0;       /* 要过滤的ID高位 */
    sFilterConfig2.FilterIdLow          = 0; /* 要过滤的ID低位 */
    sFilterConfig2.FilterMaskIdHigh     = 0;                 /* 过滤器高16位每位必须匹配 */
    sFilterConfig2.FilterMaskIdLow      = 0;                 /* 过滤器低16位每位必须匹配 */
    sFilterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO0;       /* 过滤器被关联到FIFO 1 */
    sFilterConfig2.FilterActivation     = ENABLE;                 /* 使能过滤器 */
    sFilterConfig2.FilterBank           = 0;
    ret = HAL_CAN_ConfigFilter(&hCAN2, &sFilterConfig2);
    if(HAL_OK != ret)
    {
        PRINT("HAL_CAN_ConfigFilter2 CAN2 ret not ok %d !\n",ret);
    }
    
    
    HAL_CAN_Start(&hCAN2);
    
    /* 配置邮箱0接收过程中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO0_MSG_PENDING config error!\r\n");
    }

    /* 配置邮箱0接收完全中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO0_FULL) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO0_FULL config error!\r\n");
    }

    /* 配置邮箱接0收溢出中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO0_OVERRUN) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO0_OVERRUN config error!\r\n");
    }

    /* 配置邮箱接收过程中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO1_MSG_PENDING config error!\r\n");
    }

    /* 配置邮箱接收完全中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO1_FULL) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO1_FULL config error!\r\n");
    }

    /* 配置邮箱1接收溢出中断 */
    if(HAL_CAN_ActivateNotification(&hCAN2,CAN_IT_RX_FIFO1_OVERRUN) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_RX_FIFO1_OVERRUN config error!\r\n");
    }
    
    /* 配置邮箱发送空中断 */
    if (HAL_CAN_ActivateNotification(&hCAN2, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
    {
        mini_printf("CAN2 CAN_IT_TX_MAILBOX_EMPTY config error!\r\n");
    }
}
/**
  * @FunctionName: bsp_Can2_Send_buf
  * @Author:       trx
  * @DateTime:     2025年3月9日 22:42:50
  * @Purpose:      can发送函数，支持一次发送3个以上的报文，不然一次最多只能发送3条
  * @param:        _id                要发送的报文id
  * @param:        _buf[]             报文的数据
  * @param:        _dlc               报文数据的长度，0-8可选
  * @return:       HAL_OK:代表发送成功;HAL_ERROR:发送失败
*/
HAL_StatusTypeDef bsp_Can2_Send_buf(uint32_t _id,uint8_t _buf[],uint8_t _dlc)
{
    CAN_TxHeaderTypeDef can_tx_msg;
    if(IS_CAN_STDID(_id))
    {
        can_tx_msg.StdId = _id;
        can_tx_msg.IDE = CAN_ID_STD;
    }
    else if(IS_CAN_EXTID(_id))
    {
        can_tx_msg.ExtId = _id;
        can_tx_msg.IDE = CAN_ID_EXT;
    }
    else
    {
        PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    if (_dlc >8)
    {
        return HAL_ERROR;
    }
    else
    {
        can_tx_msg.DLC = _dlc;
    }
    can_tx_msg.RTR = CAN_RTR_DATA;          /* 默认都是数据帧 */
    can_tx_msg.TransmitGlobalTime = DISABLE;

    /* 代表没有挂起的发送报文  */
    if(HAL_CAN_IsTxMessagePending(&hCAN2,CAN_TX_MAILBOX0) == HAL_OK)
    {
        HAL_CAN_AddTxMessage(&hCAN2,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX0);
    }
    else if(HAL_CAN_IsTxMessagePending(&hCAN2,CAN_TX_MAILBOX0) != HAL_OK)
    {
        HAL_CAN_AddTxMessage(&hCAN2,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX1);
    }
    else if(HAL_CAN_IsTxMessagePending(&hCAN2,CAN_TX_MAILBOX0) != HAL_OK)
    {
        HAL_CAN_AddTxMessage(&hCAN2,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX2);
    }
    else
    {
        return HAL_ERROR;
    }
    return HAL_OK;

}

/**
  * @FunctionName: CAN2_TX_IRQHandler
  * @Author:       trx
  * @DateTime:     2025年5月5日 22:00:01
  * @Purpose:      can 报文发送 中断
  * @param:        none
  * @return:       none
*/
void CAN2_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hCAN2);
}

/**
  * @FunctionName: bsp_Can2_Receive_buf
  * @Author:       trx
  * @DateTime:     2025年3月9日 22:59:39
  * @Purpose:      can报文接收函数，目前只配置了一个id
                    而且好像fifo1无法正常使用
                    并且fifo0与fifo1都无法进入邮箱填满函数
  * @param:        _id   ：要接收报文的id
  * @param:        _buf[]：报文数据数组
  * @return:       DLC:返回报文的数据长度
*/
uint8_t bsp_Can2_Receive_buf(uint32_t _id,uint8_t _buf[])
{
    uint8_t i;
    uint8_t g_canrxbuf[8] = {0};
    if(IS_CAN_STDID(_id))
    {
        if(can_rx_msg.StdId == _id)
        {
            for(i = 0;i < can_rx_msg.DLC;i++)
            _buf[i] = g_canrxbuf[i];
        }
        else
            PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    else if(IS_CAN_EXTID(_id))
    {
    if(can_rx_msg.ExtId == _id)
    {
        for(i = 0;i < can_rx_msg.DLC;i++)
        {
            _buf[i] = g_canrxbuf[i];
        }
    }
    else
        PRINT("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
    }
    return can_rx_msg.DLC;
}


/**
  * @FunctionName: CAN2_RX0_IRQHandler
  * @Author:       trx
  * @DateTime:     2022年5月26日 18:15:34 
  * @Purpose:      can接收中断函数，底层库提供
  * @param:        void
  * @return:       none
*/
void CAN2_RX0_IRQHandler(void)
{
    memset(&can_rx_msg,0,sizeof(can_rx_msg));
    memset(&g_canrxbuf,0,sizeof(g_canrxbuf));
    
    HAL_CAN_IRQHandler(&hCAN2);              /* 需要在中断函数中调用此函数来清除中断标志位 */
    HAL_CAN_GetRxMessage(&hCAN2, CAN_FILTER_FIFO0,&can_rx_msg,g_canrxbuf);
#if TRUE == BSP_CAN_LOG_ECHO
    mini_printf("\r\n");
    mini_printf("can2 rx0 receive blow: \r\n");
    mini_printf("CAN_RxHeaderTypeDef.StdId is           0x%x\r\n",can_rx_msg.StdId);
    mini_printf("CAN_RxHeaderTypeDef.ExtId is           0x%x\r\n",can_rx_msg.ExtId);
    mini_printf("CAN_RxHeaderTypeDef.IDE is             %d\r\n",can_rx_msg.IDE);
    mini_printf("CAN_RxHeaderTypeDef.RTR is             %d\r\n",can_rx_msg.RTR);
    mini_printf("CAN_RxHeaderTypeDef.DLC is             %d\r\n",can_rx_msg.DLC);
    mini_printf("CAN_RxHeaderTypeDef.Timestamp is       %d\r\n",can_rx_msg.Timestamp);
    for(uint8_t i = 0;i < can_rx_msg.DLC;i++)
    {
        mini_printf("CAN_Infomation[%d] = 0x%02x\r\n",i,g_canrxbuf[i]);
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_MspInit
  * @Author:       trx
  * @DateTime:     2022年5月25日22:14:29 
  * @Purpose:      can总线引脚初始化，包含有中断配置
  * @param:        hCAN1：can外设句柄(全局变量)
  * @return:       none
*/
void HAL_CAN_MspInit(CAN_HandleTypeDef * hCAN1)
{
    GPIO_InitTypeDef    gpio_init;

    if(hCAN1->Instance == CANx_BUS_1)
    {
        CAN1_CLK_ENABLE();          /* can外设时钟使能 */
        __HAL_RCC_CAN2_CLK_ENABLE();
        CAN1_GPIO_CLK_ENABLE();     /* 引脚时钟使能 */
        
        gpio_init.Pin       = CAN1_TX_PIN;
        gpio_init.Mode      = GPIO_MODE_AF_PP;
        gpio_init.Pull      = GPIO_PULLDOWN;
        gpio_init.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(CAN1_TX_GPIO_PORT,&gpio_init);

        gpio_init.Pin       = CAN1_RX_PIN;
        gpio_init.Mode      = GPIO_MODE_AF_PP;
        gpio_init.Speed     = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(CAN1_RX_GPIO_PORT,&gpio_init);
        
        gpio_init.Pin       = CAN_STB_PIN;
        gpio_init.Mode      = GPIO_MODE_OUTPUT_PP;
        gpio_init.Speed     = GPIO_SPEED_FREQ_LOW;
        gpio_init.Pull      = GPIO_NOPULL;
        HAL_GPIO_Init(CAN_STB_GPIO_PORT,&gpio_init);
        PBout(11) = 0;

        HAL_NVIC_SetPriority(CAN1_RX_IRQN,0,0);/* 初始化中断优先级 */
        HAL_NVIC_EnableIRQ(CAN1_RX_IRQN);
        HAL_NVIC_SetPriority(CAN1_TX_IRQn,0,0);/* 初始化中断优先级 */
        HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    }else if(hCAN1->Instance == CANx_BUS_2)
    {
        CAN2_CLK_ENABLE();          /* can外设时钟使能 */
        __HAL_RCC_CAN2_CLK_ENABLE();
        CAN2_GPIO_CLK_ENABLE();     /* 引脚时钟使能 */
        
        gpio_init.Pin       = CAN2_TX_PIN;
        gpio_init.Mode      = GPIO_MODE_AF_PP;
        gpio_init.Pull      = GPIO_PULLDOWN;
        gpio_init.Alternate = GPIO_AF9_CAN2;
        HAL_GPIO_Init(CAN2_TX_GPIO_PORT,&gpio_init);

        gpio_init.Pin       = CAN2_RX_PIN;
        gpio_init.Mode      = GPIO_MODE_AF_PP;
        gpio_init.Speed     = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Alternate = GPIO_AF9_CAN2;
        HAL_GPIO_Init(CAN2_RX_GPIO_PORT,&gpio_init);
        
        gpio_init.Pin       = CAN_STB_PIN;
        gpio_init.Mode      = GPIO_MODE_OUTPUT_PP;
        gpio_init.Speed     = GPIO_SPEED_FREQ_LOW;
        gpio_init.Pull      = GPIO_NOPULL;
        HAL_GPIO_Init(CAN_STB_GPIO_PORT,&gpio_init);
        PBout(11) = 0;

        HAL_NVIC_SetPriority(CAN2_RX_IRQN,0,0);/* 初始化中断优先级 */
        HAL_NVIC_EnableIRQ(CAN2_RX_IRQN);
        HAL_NVIC_SetPriority(CAN2_TX_IRQn,0,0);/* 初始化中断优先级 */
        HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    }else
    {
        /* do nothing */
    }
}

/**
  * @FunctionName: HAL_CAN_MspDeInit
  * @Author:       trx
  * @DateTime:     2022年5月25日23:55:24 
  * @Purpose:      can外设非初始化
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hCAN1)
{
    if(hCAN1->Instance==CANx_BUS_1)
    {
        CAN1_FPRCE_RESET();
        CAN1_RELEASE_RESET();

        /**CAN GPIO Configuration
        PB8     ------> CAN_RX
        PB9     ------> CAN_TX 
        */
        HAL_GPIO_DeInit(CAN1_TX_GPIO_PORT, CAN1_TX_PIN);
        HAL_GPIO_DeInit(CAN1_RX_GPIO_PORT, CAN1_RX_PIN);

        /* Peripheral interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN1_RX_IRQN);
    }else if(hCAN1->Instance==CANx_BUS_2)
    {
        CAN2_FPRCE_RESET();
        CAN2_RELEASE_RESET();

        /**CAN GPIO Configuration
        PB8     ------> CAN_RX
        PB9     ------> CAN_TX 
        */
        HAL_GPIO_DeInit(CAN2_TX_GPIO_PORT, CAN2_TX_PIN);
        HAL_GPIO_DeInit(CAN2_RX_GPIO_PORT, CAN2_RX_PIN);

        /* Peripheral interrupt Deinit */
        HAL_NVIC_DisableIRQ(CAN2_RX_IRQN);
    }else
    {
        /* do nothing */
    }
} 

/**
  * @FunctionName: HAL_CAN_RxFifo0FullCallback
  * @Author:       trx
  * @DateTime:     2022年5月26日 20:07:04 
  * @Purpose:      can接收fifo0填满回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 fifo0数据接收满\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 fifo0数据接收满\r\n");
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_RxFifo0MsgPendingCallback
  * @Author:       trx
  * @DateTime:     2022年5月26日 20:07:03 
  * @Purpose:      can接收fifo0正在接收数据回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 持续接收数据fifo0\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 持续接收数据fifo0\r\n");
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_RxFifo1MsgPendingCallback
  * @Author:       trx
  * @DateTime:     2025年5月5日 21:21:32
  * @Purpose:      can接收fifo0正在接收数据回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 持续接收数据fifo1\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 持续接收数据fifo1\r\n");
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_TxMailbox0CompleteCallback
  * @Author:       trx
  * @DateTime:     2025年5月5日 21:21:32
  * @Purpose:      fifo0 发送完成 数据回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 HAL_CAN_TxMailbox0CompleteCallback\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 HAL_CAN_TxMailbox0CompleteCallback\r\n");
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_TxMailbox0CompleteCallback
  * @Author:       trx
  * @DateTime:     2025年5月5日 21:21:32
  * @Purpose:      fifo1 发送完成 数据回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 HAL_CAN_TxMailbox1CompleteCallback\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 HAL_CAN_TxMailbox1CompleteCallback\r\n");
    }
#endif
}

/**
  * @FunctionName: HAL_CAN_TxMailbox2CompleteCallback
  * @Author:       trx
  * @DateTime:     2025年5月5日 21:21:32
  * @Purpose:      fifo2 发送完成 数据回调函数
  * @param:        hCAN1：can外设句柄
  * @return:       none
*/
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
#if TRUE == BSP_CAN_LOG_ECHO
    if(hcan == &hCAN1)
    {
        mini_printf("CAN1 HAL_CAN_TxMailbox2CompleteCallback\r\n");
    }
    
    if(hcan == &hCAN2)
    {
        mini_printf("CAN2 HAL_CAN_TxMailbox2CompleteCallback\r\n");
    }
#endif
}
