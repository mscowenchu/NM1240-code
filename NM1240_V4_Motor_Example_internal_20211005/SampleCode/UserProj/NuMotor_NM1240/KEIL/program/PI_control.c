#include "PI_control.h"

int16_t PI_Iq_current(AMotor* Motor, PICs* C, int16_t FW_Item)
{
    int32_t output = 0;
    int32_t limit;  //Vd,Vq max value
    int32_t ui_limit;  //limit << 15;
    int32_t Error;

    limit = Motor->ctrl.i32_Iq_control_limit_dig;
    ui_limit = Motor->ctrl.i32_Iq_control_limit_dig_ui;

    Error = Motor->info.i16_Iq_error;
    C->e1=Error;

    C->ui_31 = C->ui_31 + C->ki_15*C->e1;

    if(C->ui_31 >= ui_limit) 
        C->ui_31 = ui_limit;
    else if(C->ui_31 <= -ui_limit) 
        C->ui_31 = -ui_limit;
   
    output = ((C->kp_15*Error + C->ui_31)>>15) + FW_Item;
    
    if(output >= limit) 
        output = limit;
    else if(output <= -limit) 
        output = -limit;

    return output;
}


int16_t PI_Id_current(AMotor* Motor, PICs* C, int16_t FW_Item)
{
    int32_t output;
    int16_t limit;  //Vd,Vq max value
    int32_t ui_limit;  //limit << 15;
    int32_t Error;
  
    limit = Motor->ctrl.i32_Id_control_limit_dig;
    ui_limit = Motor->ctrl.i32_Id_control_limit_dig_ui;

    Error = Motor->info.i16_Id_error;
    C->e1=Error;

    C->ui_31 += C->ki_15*C->e1;

    if(C->ui_31 >= ui_limit) 
        C->ui_31 = ui_limit;
    else if(C->ui_31 <= -ui_limit)
        C->ui_31 = -ui_limit;

    output = ((C->kp_15*Error + C->ui_31)>>15) + FW_Item;

    if(output >= limit) 
        output = limit;
    else if(output <= -limit) 
        output = -limit;

    return output;
}


int16_t PI_speed(AMotor* Motor, PICs* C, int16_t FW_Item)
{
    int32_t output;
    int16_t limit;  //Iq max value
    int32_t ui_limit;  //limit << 15;
    int32_t Error;

    limit = Motor->ctrl.i32_Speed_control_limit_dig;  //Iq max value
    ui_limit = Motor->ctrl.i32_Speed_control_limit_dig_ui;     
    
    Error = Motor->info.i16_speed_error;
    C->e1=Error;

    C->ui_31 += C->ki_15*C->e1;

    if(C->ui_31 >= ui_limit) 
        C->ui_31 = ui_limit;
    else if(C->ui_31 <= -ui_limit) 
        C->ui_31 = -ui_limit;

    output = ((C->kp_15*Error + C->ui_31)>>15) + FW_Item;

    if(output >= limit) 
        output = limit;
    else if(output <= -limit) 
        output = -limit;

    return output;
}


int16_t PI_speed_six_step(AMotor* Motor, PICs* C, int16_t FW_Item)
{
    int32_t output;
    int16_t limit;        
    int32_t ui_limit;      
    int32_t Error;

    limit = (1 << 15)-1;       
    ui_limit = (1 << 30)-1;     
    
    Error = Motor->info.i16_speed_error;
    C->e1=Error;

    C->ui_31 += C->ki_15*C->e1;

    if(C->ui_31 >= ui_limit) 
        C->ui_31 = ui_limit;
    else if(C->ui_31 <= -ui_limit) 
        C->ui_31 = -ui_limit;
          
    output = ((C->kp_15*Error + C->ui_31)>>15) + FW_Item;
    
    if(output >= limit) 
        output = limit;
    else if(output <= -limit) 
        output = -limit;

    return output;
}


/*------------------------------------------------------
General PI controller 
------------------------------------------------------*/
int32_t PI_controller(int16_t out_limit, int32_t ui_limit, int32_t Error, PICs* C, int16_t FW_Item)
{
    int32_t output;

    C->e1=Error;

    C->ui_31 += C->ki_15*C->e1;

    if(C->ui_31 >= ui_limit) 
        C->ui_31 = ui_limit;
    else if(C->ui_31 <= -ui_limit) 
        C->ui_31 = -ui_limit;

    output = ((C->kp_15*Error + C->ui_31)>>15) + FW_Item;

    if(output >= out_limit) 
        output = out_limit;
    else if(output <= -out_limit) 
        output = -out_limit;

    return output;
}
