#ifndef PI_CONTROL_H
#define PI_CONTROL_H

#include "variable_typedefine.h"



//===Functions related to PI control ====================================
extern int16_t PI_Iq_current(AMotor* Motor, PICs* C, int16_t FW_Item);
extern int16_t PI_Id_current(AMotor* Motor, PICs* C, int16_t FW_Item);
extern int16_t PI_speed(AMotor* Motor, PICs* C, int16_t FW_Item);
extern int16_t PI_speed_six_step(AMotor* Motor, PICs* C, int16_t FW_Item);
extern int32_t PI_controller(int16_t out_limit, int32_t ui_limit, int32_t Error, PICs* C, int16_t FW_Item);
//=================================================================================

#endif
