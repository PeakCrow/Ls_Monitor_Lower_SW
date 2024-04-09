#ifndef    _SYSTEM_TASK_CREATE_H_
#define    _SYSTEM_TASK_CREATE_H_

#include "sys.h"

/***********************************ThreadX操作系统组件属性外部声名*************************************************************/

#define DRIVER1_POS     (1<<0)
#define DRIVER2_POS     (1<<1)
#define DRIVER3_POS     (1<<2)
#define CAN1_RX_FLAG    (1<<3)
#define CAN2_RX_FLAG    (1<<4)
#define DRIVER_POS_ALL  (DRIVER1_POS | DRIVER2_POS | DRIVER3_POS)
#define CANx_RX_FLAG    (CAN1_RX_FLAG | CAN2_RX_FLAG)
#define HARDWARE_MAJOR_VERSION           1
#define HARDWARE_MINOR_VERSION           0
#define HARDWARE_PATCH_VERSION           0
/***********************************ThreadX操作系统组件属性外部声名*************************************************************/


#endif


