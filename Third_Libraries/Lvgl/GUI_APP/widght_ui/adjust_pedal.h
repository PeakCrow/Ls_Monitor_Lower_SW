#ifndef ADJUST_PEDAL_H
#define ADJUST_PEDAL_H
#include "lvgl.h"
#include "monitor.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _driverx_pos {
    float driver1_pos;
    float driver2_pos;
    float driver3_pos;
    float current_pos;
}st_driver_pos;

extern st_driver_pos DriverX_Pos;

void Adjust_Pedal_Ui(lv_obj_t *parent);

#ifdef __cplusplus
}
#endif

#endif // ADJUST_PEDAL-H
