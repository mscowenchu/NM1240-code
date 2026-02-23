
#include "motor_functions.h"
#include "motor_six_step.h"


//====PhaseCurrent_OC_Check()==================================================
//1. Check if the 3 phase current (Iu/Iv/Iw) is over the limit setting
//=======================================================================
void PhaseCurrent_OC_Check(AMotor* Motor)
{
    int16_t iu = 0;
    int16_t iv = 0;
    int16_t iw = 0;

    iu = ABS(Motor->info.i16_Iu);
    iv = ABS(Motor->info.i16_Iv);
    iw = ABS(Motor->info.i16_Iw);

    if(iu > Motor->spec.i16_Iphase_OC_Q15_max) 
    {
        Stop_Motor_PWMOUT_OFF();
        Motor->sym.u8_Iphase_OC_error |= 2;
        Motor->other.u8_flag_error_record |= 0x02;  //bit1 for Iu_error
    }
    else if(iv > Motor->spec.i16_Iphase_OC_Q15_max)
    {
        Stop_Motor_PWMOUT_OFF();
        Motor->sym.u8_Iphase_OC_error |= 4;
        Motor->other.u8_flag_error_record |= 0x04;  //bit2 for Iv_error
    }
    else if(iw > Motor->spec.i16_Iphase_OC_Q15_max) 
    {
        Stop_Motor_PWMOUT_OFF();
        Motor->sym.u8_Iphase_OC_error |= 8;
        Motor->other.u8_flag_error_record |= 0x08;  //bit3 for Iw_error
    }
} //----End of "PhaseCurrent_OC_Check()"
//=========================================================================


//====HallSpeedEstimator()==================================================
//1. According with hall pulse width which is measure by ECAP to calculate the rotor speed.
//2. Calculate the step-theda in every 15KHz PWM at this rotor speed.
//=======================================================================
void HallSpeedEstimator(AMotor* Motor)
{
    int32_t speed_now = 0, speed_old; 
    int32_t i32_Capture_Value_Q15, i32_Last_Capture_Value_Q15;
    int32_t i32_Hall_Wr_EST_Constant;
    uint8_t u8_cmd_direction, u8_rotor_direction;

    speed_old = Motor->info.i16_rotor_speed;
    u8_cmd_direction = Motor->cmd.u8_direction;
    u8_rotor_direction = Motor->info.u8_rotor_direction;

    i32_Capture_Value_Q15 = Motor->other.i32_Capture_Value_Q15;  
    i32_Last_Capture_Value_Q15 = Motor->other.i32_Last_Capture_Value_Q15;  
    i32_Hall_Wr_EST_Constant = Motor->other.i32_Hall_Wr_EST_Constant;
  
    if((i32_Capture_Value_Q15 > 0x7FF0)  || (Motor->other.u8_flag_ECAP_overflow == 1)) //0x7FF0-> a value lower than 2^15
    {// If Hall pulse is too long
        speed_now = 0;
    }
    //---Check if Capture_Value has large change -----
//  else if((i32_Capture_Value_Q15 * 16) < (i32_Last_Capture_Value_Q15))//If hall pulse change too much
//  {//check if speed suddenly changes 16 times than before
//      speed_now = speed_old;    //keep old speed or alarm warnning (depends on system)
//  }
    //-----------------------------------------------
    else
    {
    //---Wr(rpm) = Hall_Wr_EST_Constant/Hall_width(ECAP_HLD) ----
        if((i32_Capture_Value_Q15 < 0x7FF0) && (i32_Capture_Value_Q15 > 0))
        {
            HDIV0->DIVIDEND = (int32_t) i32_Hall_Wr_EST_Constant; 
            HDIV0->DIVISOR = i32_Capture_Value_Q15;      //Start HDIV0 while DIVISOR is written
            __nop();
            __nop();
            speed_now = (int16_t) HDIV0->QUOTIENT;
        }
        else
        {
            speed_now = 0;    //8;  set a minimum speed
        }
    }//--- End of if () ----------------------------------

    if(speed_now <= 1)    //If too slow, return back to motor start state.
    {
        Motor->other.u8_operation_state = C_OPEN_LOOP;
        Fix_Phase_Angle_M0(85);
    }

    //--check if need do speed average----(demo +/1 100rpm for the speed hysteresis)
    if((Motor->other.u8_flag_do_speed_average == 0) && (speed_now > (C_RPM_Do_Speed_Average + 100)))
        Motor->other.u8_flag_do_speed_average = 1;
    else if((Motor->other.u8_flag_do_speed_average == 1) && (speed_now < (C_RPM_Do_Speed_Average - 100)))
        Motor->other.u8_flag_do_speed_average = 0;

    //--If motor is reversed running, speed is negative---
    if(u8_rotor_direction == 0)
        speed_now = -speed_now;

    //--Check if need to do speed average---- (Depends on system requirement)
    if(Motor->other.u8_flag_do_speed_average)
        speed_now = (speed_now + Motor->info.i16_rotor_speed*3)>>2;

    //--Update new rotor speed------
    Motor->info.i16_rotor_speed = speed_now;

  //--Update new step theda for every PWM period------  
  //---If rotor direction is opposited to CMD direction: force it as square wave 
    if(u8_cmd_direction == 1)  //If CMD is FW
    {
      if(u8_rotor_direction == 0) //If rotor is RW
        Motor->other.i32_Hall_Step_Theda_Q26 = 0;  
      else
        Motor->other.i32_Hall_Step_Theda_Q26 = Motor->other.i32_Hall_Step_Theda_EST_Constatnt * speed_now;    
    }
    else
    {
      if(u8_rotor_direction == 1)
        Motor->other.i32_Hall_Step_Theda_Q26 = 0;
      else
        Motor->other.i32_Hall_Step_Theda_Q26 = Motor->other.i32_Hall_Step_Theda_EST_Constatnt * speed_now;    
    }

    Motor->other.i32_Last_Capture_Value_Q15 = i32_Capture_Value_Q15;
} //----End of "HallSpeedEstimator()"
//=========================================================================



//========Detect_Initial_Hall_Position_M0()===============================
// Detect the initial Hall Postion.
// Set the corresponding values of MotorA.info.i16_hall_angle and Hall_Accumulated_Theda_Q26.
//========================================================================

void Detect_Initial_Hall_Position_M0(void)
{
    int16_t   Initial_Shift_Angle;
    int32_t   Hall_Accumulated_Theda_Q26;
    uint8_t  Hall_Position, Last_Hall_Position, Forward_Direction_CMD_M0;

    Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
    Last_Hall_Position = Hall_Position;
    Forward_Direction_CMD_M0 = MotorA.cmd.u8_direction;

    Initial_Shift_Angle = 1024*0/360;

    if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)
    {
        Hall_Accumulated_Theda_Q26 = MotorA.other.i32_Hall_Accumulated_Theda_Q26;

        if(Forward_Direction_CMD_M0 == 1)      //For Forward running
        {
            switch(Hall_Position)
            {
                case HALL_STATE1_FW_M0:  // prepare to turn on CAP1 Falling for Hall_V

                    MotorA.info.i16_hall_angle = HALL_STATE1_F_ANGLE_M0 + Initial_Shift_Angle;  //Update new info.hall_angle

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE2_FW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE2_F_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE3_FW_M0:
                    MotorA.info.i16_hall_angle = HALL_STATE3_F_ANGLE_M0 + Initial_Shift_Angle;
                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE4_FW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE4_F_ANGLE_M0 + Initial_Shift_Angle;  //Update new info.hall_angle

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE5_FW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE5_F_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE6_FW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE6_F_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                default:

                    Stop_Motor_PWMOUT_OFF();

                break;
            }//---end of switch(Hall_Position)
        }//--- end of "if(Forward_Direction_CMD_M0 == 1)  //For Forward running"
        else //if(Forward_Direction_CMD_M0 == 0)      //For Revesed running
        {
            switch(Hall_Position)
            {

                case HALL_STATE1_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE1_R_ANGLE_M0 + Initial_Shift_Angle;  //Update new info.hall_angle
                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE2_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE2_R_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE3_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE3_R_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE4_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE4_R_ANGLE_M0 + Initial_Shift_Angle;  //Update new info.hall_angle

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE5_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE5_R_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                case HALL_STATE6_RW_M0:

                    MotorA.info.i16_hall_angle = HALL_STATE6_R_ANGLE_M0 + Initial_Shift_Angle;

                    if(MotorA.info.i16_hall_angle > 1023)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                    else if (MotorA.info.i16_hall_angle < 0)
                        MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                    Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

                break;

                default:

                    Stop_Motor_PWMOUT_OFF();

                break;
            }//---end of switch(Hall_Position)
        }//--- end of "else //if(Forward_Direction_CMD_M0 == 0)  //For Rewind running"

        MotorA.other.i32_Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26;
        MotorA.info.u8_Hall_Position = Hall_Position;
        MotorA.info.u8_Last_Hall_Position = Last_Hall_Position;
    }//--end of if "if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)"
}
//====End of Detect_Initial_Hall_Position_M0()================================================


//====Check_Rotor_FW_RW_M0()==================================================
//According Hall signal sequence to determine rotor is in FW or RW running
//=======================================================================
uint8_t Check_Rotor_FW_RW_M0(uint8_t u8_Last_Hall_Pos, uint8_t u8_Hall_Pos, uint8_t u8_Last_Direction)
{
    uint8_t u8_Rotor_Direction_M0;

    u8_Rotor_Direction_M0 = u8_Last_Direction;  //Initially set it is in unknown state.

    switch(u8_Last_Hall_Pos)
    {
        case HALL_STATE1_FW_M0:
            if(u8_Hall_Pos == HALL_STATE2_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE6_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        case HALL_STATE2_FW_M0:
            if(u8_Hall_Pos == HALL_STATE3_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE1_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        case HALL_STATE3_FW_M0:
            if(u8_Hall_Pos == HALL_STATE4_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE2_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        case HALL_STATE4_FW_M0:
            if(u8_Hall_Pos == HALL_STATE5_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE3_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        case HALL_STATE5_FW_M0:
            if(u8_Hall_Pos == HALL_STATE6_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE4_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        case HALL_STATE6_FW_M0:
            if(u8_Hall_Pos == HALL_STATE1_FW_M0)
                u8_Rotor_Direction_M0 = 1;  //rotor in FW
            else if(u8_Hall_Pos == HALL_STATE5_FW_M0)
                u8_Rotor_Direction_M0 = 0;  //rotor in RW
        break;

        default:
            u8_Rotor_Direction_M0 = u8_Last_Direction;    //unknown
        break;
    }//--end of "switch"--------

    return(u8_Rotor_Direction_M0);
}
//===End of "Check_Rotor_FW_RW_M0()"========================================


//====Fix_Phase_Angle_M0()================================================
//1. Fix phase angle depends on the Hall_Position and CMD_Direction
//2. i16_Shift_Angle: if needs some angle shift.
//3. Update the MotorA.other.i32_Hall_Accumulated_Theda_Q26
//=======================================================================
void Fix_Phase_Angle_M0(int16_t i16_Shift_Angle)
{
    uint8_t Forward_Direction_CMD_M0, Hall_Position;

    Hall_Position = MotorA.info.u8_Hall_Position;
    Forward_Direction_CMD_M0 = MotorA.cmd.u8_direction;

    if(Forward_Direction_CMD_M0 == 1)  //For Forward running
    {
        switch(Hall_Position)
        {
            case HALL_STATE1_FW_M0:  // prepare to turn on CAP1 Falling for Hall_V

                MotorA.info.i16_hall_angle = HALL_STATE1_F_ANGLE_M0 + i16_Shift_Angle;  //Update new info.i16_hall_angle

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;
  
            break;

            case HALL_STATE2_FW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE2_F_ANGLE_M0 + i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE3_FW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE3_F_ANGLE_M0 + i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE4_FW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE4_F_ANGLE_M0 + i16_Shift_Angle;  //Update new info.i16_hall_angle

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE5_FW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE5_F_ANGLE_M0 + i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE6_FW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE6_F_ANGLE_M0 + i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            default:

                Stop_Motor_PWMOUT_OFF();

            break;
        }//---end of switch(Hall_Position)
    }//--- end of "if(Forward_Direction_CMD_M0 == 1)  //For Forward running"
    else //if(Forward_Direction_CMD_M0 == 0)  //For Reversed running
    {
        switch(Hall_Position)
        {
            case HALL_STATE1_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE1_R_ANGLE_M0 - i16_Shift_Angle;  //Update new info.i16_hall_angle

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE2_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE2_R_ANGLE_M0 - i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE3_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE3_R_ANGLE_M0 - i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE4_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE4_R_ANGLE_M0 - i16_Shift_Angle;  //Update new info.i16_hall_angle

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;
        
            break;

            case HALL_STATE5_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE5_R_ANGLE_M0 - i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            case HALL_STATE6_RW_M0:

                MotorA.info.i16_hall_angle = HALL_STATE6_R_ANGLE_M0 - i16_Shift_Angle;

                if(MotorA.info.i16_hall_angle > 1023)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
                else if (MotorA.info.i16_hall_angle < 0)
                    MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

                MotorA.other.i32_Hall_Accumulated_Theda_Q26 = (int32_t) (MotorA.info.i16_hall_angle) << 16;

            break;

            default:

                Stop_Motor_PWMOUT_OFF();
          
            break;
        }//---end of switch(Hall_Position)
    }//--- end of "else //if(Forward_Direction_CMD_M0 == 0)  //For Reversed running"
}
//====End of "void Fix_Phase_Angle_M0(void)"======================


//========cmd_speed_slope_with_hall()=============================
// Setup the slope of speed command 
// How many EADC0 interrupt occurs the cmd.i16_rotor_speed++
//================================================================
void cmd_speed_slope_with_hall(AMotor* Motor)
{
    uint16_t counter, delta_rpm;
    int16_t  temp_cnt_value;
    int32_t Hall_Step_Theda_Q26;

    if(Motor->cmd.u8_start == 0)
        return;

    temp_cnt_value = Motor->spec.i16_speed_slope_cnt_target;  //How many interrupt occurs the cmd.speed++
    delta_rpm = Motor->spec.i16_speed_slope_delta_rpm;  //a step which cmd.rotor_speed increase/decrease in a time
    counter = Motor->other.ui16_speed_slope_counter;
  
    if(Motor->other.u8_operation_state == C_OPEN_LOOP )
    {   // Increasing Speed Slope at first several steps

        Hall_Step_Theda_Q26 = Motor->other.i32_Hall_Step_Theda_Q26;
    
        if(Motor->cmd.i16_rotor_speed_target > Motor->cmd.i16_rotor_speed)
        {
            if(++counter >= temp_cnt_value)
            {
                Motor->cmd.i16_rotor_speed += delta_rpm;
                counter = 0;
                Hall_Step_Theda_Q26 = Motor->other.i32_Hall_Step_Theda_EST_Constatnt * (int32_t)Motor->cmd.i16_rotor_speed;
            }
        }
        else if(Motor->cmd.i16_rotor_speed_target < Motor->cmd.i16_rotor_speed)
        {
            if(++counter >= temp_cnt_value)
            {
                Motor->cmd.i16_rotor_speed -= delta_rpm;
                counter = 0;
                Hall_Step_Theda_Q26 = Motor->other.i32_Hall_Step_Theda_EST_Constatnt * (int32_t)Motor->cmd.i16_rotor_speed;
            }
        }

        Motor->other.i32_Hall_Step_Theda_Q26 = Hall_Step_Theda_Q26;
    }
    else  // Increasing Speed Slope after close loop
    {
        if(Motor->cmd.i16_rotor_speed_target > Motor->cmd.i16_rotor_speed)  
        {
            if(++counter >= temp_cnt_value)
            {
                Motor->cmd.i16_rotor_speed += delta_rpm;
                counter = 0;
            }
        }
        else if(Motor->cmd.i16_rotor_speed_target < Motor->cmd.i16_rotor_speed)  
        {
            if(++counter >= temp_cnt_value)
            {
                Motor->cmd.i16_rotor_speed -= delta_rpm;
                counter = 0;
            }
        }
    }

    Motor->other.ui16_speed_slope_counter = counter;
}
//--End of cmd_speed_slope_with_hall()------------


//========read_VSP_Check_ON_OFF_Motor()======================
//1. VSP signal is read by "SW_Get_Speed_Command_From_VR" in EADC0_IRQHandler at PC.7
//2. if motor is in start --> check need to stop  or not
//3. if motor is in stop  --> check need to start or not
//================================================================
void read_VSP_Check_ON_OFF_Motor(AMotor* Motor)
{
    int32_t VSP;

    if(Motor->other.u8_flag_error_record != 0)  //If with error, force PWM MASK to turn OFF external MOS
    {
        Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Use MASK function to let PWM output to turn off MOS
        return;
    }

    VSP = Motor->other.ui16_VSP_12bit_ADC;

    if(Motor->cmd.u8_start == 1)//check if need to OFF motor from START motor
    {
        if(VSP <= C_MIN_VSP_12b_ADC)
        {
            Stop_Motor_PWMOUT_OFF();
        }
    }
    else  //check if need to START from OFF motor 
    {
        if(VSP >= C_MIN_VSP_12b_ADC)  
        {
            #if P_FOC_CONTROL
            //--Prepare for motor start and presest PWM duty in PW->CMPDAT[0] [1] [2]
            //Start_Motor_PWMOUT_ON(50, 50, 50); // C_PWM_FULL_SCALE    
            Start_Motor_PWMOUT_ON(C_PWM_FULL_SCALE/2, C_PWM_FULL_SCALE/2, C_PWM_FULL_SCALE/2); // C_PWM_FULL_SCALE
            #endif
            //uint8_t    Hall_Position;
            #if P_SIX_STEP_CONTROL
            //--It is optional to switch PWM pins to GPIO.
            Set_GPA_As_EPWM_Output(); //Switch PWM output pins as GPIO output.
            //Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
            if(MotorA.cmd.u8_direction==1)
                SIX_STEP_CHANGE_PHASE_FW();
            else if(MotorA.cmd.u8_direction==0)
                SIX_STEP_CHANGE_PHASE_RW();
            #endif

            MotorA.cmd.u8_start = 1; 
            MotorA.other.u8_CMD_Motor_Action = C_CMD_RUN; 
            MotorA.other.u8_Motor_Running_State = C_State_Motor_In_Run;
        }
    }
}
//--End of read_VSP_Check_ON_OFF_Motor()------------


//========read_UART_Check_ON_OFF_Motor()======================
//1. if motor is in start --> check need to stop  or not
//2. if motor is in stop  --> check need to start or not
//================================================================
void read_UART_Check_ON_OFF_Motor(AMotor* Motor)
{
    if(Motor->other.u8_flag_error_record != 0)   //If with error, force PWM MASK to turn OFF external MOS
    {
        Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Use MASK function to let PWM output to turn off MOS
        return;
    }

    if(Motor->cmd.u8_start == 0 )//check if need to OFF motor from START motor
        Stop_Motor_PWMOUT_OFF();
      
    if(Motor->cmd.u8_start == 1 && MotorA.other.u8_CMD_Motor_Action == C_CMD_STOP )
    {
        #if P_FOC_CONTROL
        //--Prepare for motor start and presest PWM duty in PW->CMPDAT[0] [1] [2]
        //Start_Motor_PWMOUT_ON(50, 50, 50); // C_PWM_FULL_SCALE    
        Start_Motor_PWMOUT_ON(C_PWM_FULL_SCALE/2, C_PWM_FULL_SCALE/2, C_PWM_FULL_SCALE/2); // C_PWM_FULL_SCALE
        #endif
        //uint8_t    Hall_Position;
        #if P_SIX_STEP_CONTROL
        //--It is optional to switch PWM pins to GPIO.
        Set_GPA_As_EPWM_Output(); //Switch PWM output pins as GPIO output.
        //Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
        if(MotorA.cmd.u8_direction==1)
            SIX_STEP_CHANGE_PHASE_FW();
        else if(MotorA.cmd.u8_direction==0)
            SIX_STEP_CHANGE_PHASE_RW();
        #endif

        MotorA.other.u8_CMD_Motor_Action = C_CMD_RUN; 
        MotorA.other.u8_Motor_Running_State = C_State_Motor_In_Run;
    }
}
//--End of read_UART_Check_ON_OFF_Motor()------------


//========read_VSP_to_duty_cmd()===========================================================
//1. read the VSP value
//2. convert the VSP value(0~4096) to (0~C_PWM_FULL_SCALE)
//3. Directly adjust PWM duty(0~100%) by VSP
//======================================================================================
void read_VSP_to_duty_cmd(AMotor* Motor)
{
    int32_t VSP, cmd_duty;
    uint16_t dDuty;
  
    VSP = Motor->other.ui16_VSP_12bit_ADC;
  
    /* If motor in start state (C_CMD_RUN) */
    if(Motor->cmd.u8_start == 1)
    {  
        if(MotorA.other.u8_flag_error_record != 0) //if system error , set EPWM->CMPDAT[0]=0;
        {
            EPWM->CMPDAT[0]=0;
            u16_Duty_VSP = 0;
        }

        //--Define duty Command: from ui16_VSP_12bit_ADC value.
        cmd_duty = C_PWM_FULL_SCALE*VSP>>12;

        if(cmd_duty > C_MAX_PWM_DUTY)
            cmd_duty = C_MAX_PWM_DUTY;
        else if(cmd_duty < 0)
            cmd_duty = 0;

        if(EPWM->CMPDAT[0]<cmd_duty) //before PWM duty achieve duty command,PWM_duty=PWM_duty+ dDuty
        {
            dDuty=dDuty_VSP_10ms;
            u16_Duty_VSP += dDuty;

            if(u16_Duty_VSP >= cmd_duty)
                u16_Duty_VSP = cmd_duty;

            EPWM->CMPDAT[0] = u16_Duty_VSP;
            Motor->cmd.i16_Duty0 = Motor->cmd.i16_Duty2 = Motor->cmd.i16_Duty4 = cmd_duty;
        }
        else 
        {
            EPWM->CMPDAT[0] = cmd_duty ;
            Motor->cmd.i16_Duty0 = Motor->cmd.i16_Duty2 = Motor->cmd.i16_Duty4 = cmd_duty;
        }
    }
}
//--End of read_VSP_to_duty_cmd()------------


//========read_VSP_to_speed_cmd()===========================================================
//1. if motor is in start: Convert "other.ui16_VSP_12bit_ADC" to cmd.i16_rotor_speed_traget 
//==========================================================================================
void read_VSP_to_speed_cmd(AMotor* Motor)
{
    int32_t VSP, cmd_speed_target;
  
    VSP = Motor->other.ui16_VSP_12bit_ADC;
  
    /* If motor in start state (C_CMD_RUN) */
    if(Motor->other.u8_CMD_Motor_Action == C_CMD_RUN)
    {
        //--Define Speed Command: from ui16_VSP_12bit_ADC value.
        cmd_speed_target = ((VSP - C_MIN_VSP_12b_ADC) * C_VSP_CMD_SLOPE >> 15) + C_MIN_CMD_SPEED_rpm;

        if(cmd_speed_target > C_MAX_CMD_SPEED_rpm)
            cmd_speed_target = C_MAX_CMD_SPEED_rpm;
        else if(cmd_speed_target < C_MIN_CMD_SPEED_rpm)
            cmd_speed_target = C_MIN_CMD_SPEED_rpm;

        if(MotorA.cmd.u8_direction == 0)
            cmd_speed_target = -cmd_speed_target;

        Motor->cmd.i16_rotor_speed_target = cmd_speed_target;
    }
}
//--End of read_VSP_to_speed_cmd()------------


//========read_VSP_to_Iq_cmd()======================
//1. Convert the ui16_VSP_12bit_ADC to Iq command directly.
//2. For test use only.
//================================================================
void read_VSP_to_Iq_cmd(AMotor* Motor)
{
    /* If motor in start state (C_CMD_RUN) */
    if(Motor->cmd.u8_start == 1)
    {
        Motor->cmd.i16_Iq = Motor->other.ui16_VSP_12bit_ADC;
    
        if(MotorA.cmd.u8_direction == 0)
            Motor->cmd.i16_Iq = -Motor->cmd.i16_Iq;  // If want to run RW
    }
}
//--End of read_VSP_to_Iq_cmd()------------


//========read_VSP_to_Vq_cmd()======================
//1. Convert the ui16_VSP_12bit_ADC to Vq command directly.
//2. For test use only.
//================================================================
void read_VSP_to_Vq_cmd(AMotor* Motor)
{
    int32_t VSP;

    VSP = Motor->other.ui16_VSP_12bit_ADC;
  
    /* If motor in start state (C_CMD_RUN) */
    if(Motor->cmd.u8_start == 1)
    {
        Motor->cmd.i16_Vq = VSP * 2;
    }
}
//--End of read_VSP_to_Vq_cmd()------------


//****DAC_Output_M0************************************
//Output internal variables to DAC by SPI1
//*****************************************************
uint16_t temp_data, tempA, tempB, tempC, tempD;
void DAC_Output_M0(void)
{
//  tempD = ((t1 * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((t3 * 4 + 32768) >> 4)&0x0FFF;
//  tempB = ((t2 * 4 + 32768) >> 4)&0x0FFF;
    //---DAC Module Output-------------------------------------
//  tempA = ((MotorA.info.i16_Iq*16  + 32768) >> 4)&0x0FFF;
//  tempA =  (MotorA.cmd.i16_Duty0*8) & 0x0FFF;
//  tempA = ((MotorA.cmd.i16_Vq * 1 + 32768) >> 4)&0x0FFF;
    tempA = ((MotorA.info.i16_Iu*4  + 32768) >> 4)&0x0FFF;
//  tempA = ((MotorA.info.i16_Iq_error*4  + 32768) >> 4)&0x0FFF;
//  tempA = (MotorA.info.i16_IDC_Bus) & 0x0FFF;

//  tempB = ((MotorA.info.i16_rotor_speed * 8 + 32768) >> 4)&0x0FFF;
//  tempB = (MotorA.cmd.i16_Duty2*8) & 0x0FFF;
//  tempB = ((MotorA.info.i32_sin_Q15 * 1 + 32768) >> 4 ) & 0x0FFF;
//  tempB = ((MotorA.cmd.i16_Vd * 4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.cmd.i16_Valfa * 4 + 32768) >> 4)&0x0FFF;
    tempB = ((MotorA.info.i16_Iv*4  + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.info.i16_Ibeta*4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.info.i16_Iq*4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.cmd.i16_rotor_speed_target*16 + 32768) >> 4)&0x0FFF;

//  tempC = ((MotorA.cmd.i16_Iq*4 + 32768) >> 4)&0x0FFF;
//  tempC = (MotorA.cmd.i16_angle*4) & 0x0FFF;
//  tempC = ((MotorA.info.i16_Ibeta*4 + 32768) >> 4)&0x0FFF;
//  tempC = (MotorA.info.u8_Hall_Position*500) & 0x0FFF;
//  tempC = (MotorA.cmd.i16_Duty4*8) & 0x0FFF;
//  tempC = ((MotorA.info.i32_cos_Q15 * 1 + 32768) >> 4 ) & 0x0FFF;
//  tempC = ((MotorA.cmd.i16_Vc * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((MotorA.cmd.i16_Vbeta * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((MotorA.info.i16_rotor_speed * 16 + 32768) >> 4)&0x0FFF;
    tempC = ((MotorA.info.i16_Iw*4  + 32768) >> 4)&0x0FFF;
//  tempC = ((MotorA.cmd.i16_rotor_speed*16 + 32768) >> 4)&0x0FFF;
  
//  tempD = ((MotorA.info.i16_rotor_speed*4 + 32768) >> 4)&0x0FFF;
    tempD = (MotorA.cmd.i16_angle*4) & 0x0FFF; 
//  tempD = ((MotorA.info.i32_sin_Q15 * 1 + 32768) >> 4 ) & 0x0FFF; 
//  tempD = (MotorA.other.ui16_VSP_12bit_ADC*1) & 0x0FFF;
//  tempD = (MotorA.info.u8_Hall_Position*500) & 0x0FFF;
//  tempD = ((MotorA.info.i16_Iq_error*4 + 32768) >> 4)&0x0FFF;
//  tempD = ((MotorA.cmd.i16_Vq * 4 + 32768) >> 4)&0x0FFF;

{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chA_control | tempA;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chB_control | tempB;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chC_control | tempC;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chD_control | tempD;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
    //---------------------------------------------------------
}
//=End of "DAC_Output_M0()"=========================================


//****DAC_Output_M0************************************
//Output internal variables to DAC by SPI0
//*****************************************************
void DAC_Module_Check(void)
{
    //---DAC Module Output-------------------------------------
    tempA = 0xFFF;
    tempB = 0x800;
    tempC = 0x400;
    tempD = 0x200;

    /* set DAC in power down mode and all output in Hi-Z. */  
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;
    SPI1_TX = DAC_PWD | 0x800;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;

    //--Tx data to DAC module: checking the 4 outputs voltage level by oscilloscope   
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;
    SPI1_TX = DAC_chA_control | tempA;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;

    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;
    SPI1_TX = DAC_chB_control | tempB;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;

    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;
    SPI1_TX = DAC_chC_control | tempC;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;  
  
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;
    SPI1_TX = DAC_chD_control | tempD;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
    //---------------------------------------------------------
}
//=End of "DAC_Output_M0()"=========================================


//*********************************************************************
// for system time delay
//  "j<6700": delay time is about 1ms for 72MHz
//*********************************************************************
void delay_Xmsec(unsigned int z)  //delay time 
{
    uint32_t i;
    uint32_t j;

    for(i=0; i<z; i++)
    {
        for(j=0; j<6700; j++)
        {
        }
    }
}


//20180824
//****DAC_Output_M0_1R************************************
//Output internal variables to DAC by SPI0
//*****************************************************
extern uint32_t ui32_duty0, ui32_duty2, ui32_duty4;
void DAC_Output_M0_1R(void)
{
//  tempD = ((t1 * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((t3 * 4 + 32768) >> 4)&0x0FFF;
//  tempB = ((t2 * 4 + 32768) >> 4)&0x0FFF;    
    //---DAC Module Output-------------------------------------
//  tempA = ((MotorA.info.i16_Iq*16  + 32768) >> 4)&0x0FFF; 
//  tempA =  (MotorA.cmd.i16_Duty0*8) & 0x0FFF; 
//  tempA = ((MotorA.cmd.i16_Vq * 1 + 32768) >> 4)&0x0FFF; 
    tempA = ((MotorA.info.i16_Iu*4  + 32768) >> 4)&0x0FFF; 
//  tempA = ((MotorA.info.i16_Iq_error*4  + 32768) >> 4)&0x0FFF; 
  
//  tempB = ((MotorA.info.i16_rotor_speed * 8 + 32768) >> 4)&0x0FFF;  
//  tempB =  (MotorA.cmd.i16_Duty2*8) & 0x0FFF; 
//  tempB =  ((MotorA.info.i32_sin_Q15 * 1 + 32768) >> 4 ) & 0x0FFF; 
//  tempB = ((MotorA.cmd.i16_Vd * 4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.cmd.i16_Valfa * 4 + 32768) >> 4)&0x0FFF;
    tempB = ((MotorA.info.i16_Iv*4  + 32768) >> 4)&0x0FFF;  
//  tempB = ((MotorA.info.i16_Ibeta*4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.info.i16_Iq*4 + 32768) >> 4)&0x0FFF;
//  tempB = ((MotorA.cmd.i16_rotor_speed_target*16 + 32768) >> 4)&0x0FFF;

  
//  tempC = ((MotorA.cmd.i16_Iq*4 + 32768) >> 4)&0x0FFF;
//  tempC =  (MotorA.cmd.i16_angle*4) & 0x0FFF;   
//  tempC = ((MotorA.info.i16_Ibeta*4 + 32768) >> 4)&0x0FFF;
//  tempC =  (MotorA.info.u8_Hall_Position*500) & 0x0FFF;
//  tempC =  (MotorA.cmd.i16_Duty4*8) & 0x0FFF; 
//  tempC =  (ui32_duty4*8) & 0x0FFF; 
//  tempC =  ((MotorA.info.i32_cos_Q15 * 1 + 32768) >> 4 ) & 0x0FFF; 
//  tempC = ((MotorA.cmd.i16_Vc * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((MotorA.cmd.i16_Vbeta * 4 + 32768) >> 4)&0x0FFF;
//  tempC = ((MotorA.info.i16_rotor_speed * 16 + 32768) >> 4)&0x0FFF;  
    tempC= ((MotorA.info.i16_Iw*4  + 32768) >> 4)&0x0FFF;  
//  tempC = ((MotorA.cmd.i16_rotor_speed*16 + 32768) >> 4)&0x0FFF;
  
//  tempD = ((MotorA.info.i16_rotor_speed*8 + 32768) >> 4)&0x0FFF;
    tempD =  (MotorA.cmd.i16_angle*4) & 0x0FFF; 
//  tempD =  ((MotorA.info.i32_sin_Q15 * 1 + 32768) >> 4 ) & 0x0FFF; 
//  tempD =  (MotorA.other.ui16_VSP_12bit_ADC*1) & 0x0FFF;
//  tempD =  (MotorA.info.u8_Hall_Position*500) & 0x0FFF;
//  tempD = ((MotorA.info.i16_Iq_error*4 + 32768) >> 4)&0x0FFF;
//  tempD = ((MotorA.cmd.i16_Vq * 4 + 32768) >> 4)&0x0FFF;
//  tempD =  (t4 * 650) & 0x0FFF; 

{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chA_control | tempA;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chB_control | tempB;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chC_control | tempC;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
{
    DAC_CS = 1;

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 0;

    SPI1_TX = DAC_chD_control | tempD;  //load data to TX buffer

    /* wait until GO_BUSY=0 */
    while(GO_BUSY0 == 1);

    DAC_CS = 1;
}
    //---------------------------------------------------------
}
//=End of "DAC_Output_M0_1R()"=========================================


//20180828
//****Enable_CLKO_to_PA0************************************
//For Test Only. Do not enable PWM when this function is in use.
//For testing the quality of HCLK frequency
//PA0 will output HCLK/2^(N+1)
//**********************************************************
void Enable_CLKO_to_PA0(void)
{
    /*-- Set GPA0 pin as CLKO output clock --*/
    SYS->GPA_MFP = (SYS->GPA_MFP & ~SYS_GPA_MFP_PA0MFP_Msk) | SYS_GPA_MFP_PA0_CLKO;  /* GPA0 CLKO mode */
    PA->MODE = (PA->MODE & ~GPIO_MODE_MODE0_Msk) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos);  /* GPA0 output mode */

    /* CLK_EnableModuleClock(CLKO_MODULE); */
    CLK->APBCLK |= CLK_APBCLK_CLKOCKEN_Msk;

    /*-- Set Clock Divider Clock Source from HCLK --*/
    CLK->CLKSEL1 = (CLK->CLKSEL1 &~ CLK_CLKSEL1_CLKOSEL_Msk) | CLK_CLKO_SRC_HCLK;

    /* CKO = clock source / 2^(u32ClkDiv + 1) */
    CLK->CLKOCTL = (CLK->CLKOCTL & ~(CLK_CLKOCTL_FREQSEL_Msk | CLK_CLKOCTL_DIV1EN_Msk)) |
                   ((7) << CLK_CLKOCTL_FREQSEL_Pos) |  //u32ClkDiv = 7
                   ((0) << CLK_CLKOCTL_DIV1EN_Pos);  //0: CLKO = HCLK/2^(N+1), 1: CLKO = HCLK

    /* Enable CKO clock source */
    CLK->CLKOCTL |= CLK_CLKOCTL_CLKOEN_Msk;

}
//-------------------------------------------------------------------------------
