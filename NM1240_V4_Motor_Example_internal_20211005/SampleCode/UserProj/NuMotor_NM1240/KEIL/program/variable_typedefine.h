/*---------------------------------------------------------------
Define the variables related to motor control use.

----------------------------------------------------------------*/

#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include "system_initialize.h"
#include "system_parameter.h"
#include "NM1240.h"

//typedef union
//{
//    int32_t DWORD;
//    struct
//    {
//        uint8_t first;
//        uint8_t second;
//        uint8_t third;
//        uint8_t forth;
//    }byte;
//}DWORD;
  
// all speed is indicated in rpm

typedef struct tag_MotorSpec                // motor specification
{
int16_t i16_freq_pwm;                       //pwm frequence
int16_t i16_speed_loop_time;                //speed loop time in ms
int16_t i16_dead_time;                      //dead time uinit: Dead-time counter with 72Mhz clock

int16_t i16_speed_slope_cnt_target;         //speed_command slope counter target value
int16_t i16_speed_slope_delta_rpm;          //a step which cmd.rotor_speed increase/decrease in a time

int16_t i16_current_gain;                   //current_gain => the Q15 value of 1A in this system
int16_t i16_Idc_over_current;               //Software Idc over current value in Amp. (unit=A)
int16_t i16_Idc_OC_Q15_max;                 //Software Idc over current value in Q15. (unit=A*current_gain)
int16_t i16_Iphase_OC_Q15_max;              //Software Iphase OC value in Q15 (unit=A*current_gain)
  
int16_t i16_Vq_maximum;                     //Maximum Vq value ( less than 32767)
int16_t i16_Iq_current_limit_in_A;          //Maximum Iq operating current in Amp. (unit=A)  
int16_t i16_Iq_current_limit;               //Maximum Iq operating current in Q15. (unit=A*current_gain)  
int16_t i16_speed_maximum;                  //Maximum oerating rotor speed (unit=rpm)

//int16_t i16_dc_bus_ov;                    //DC BUS over voltage  (V)
//int16_t i16_dc_bus_uv;                    //DC BUS under voltage (V)
//int16_t i16_dc_bus_gain;                  //DC BUS GAIN xx -> 1V
  
int16_t i16_motor_pole;                     //Motor Pole (usually 2,4,6,8,10...)
}AMotorSpec;

typedef struct tag_MotorInfo                // Real-time feedback information/signals
{  
int16_t i16_Iu;                             //Phase Current Iu (Ia)
int16_t i16_Iv;                             //Phase Current Iv (Ib)
int16_t i16_Iw;                             //Phase Current Iw (Ic)
int16_t i16_Ialfa;                          //Clark Ialfa
int16_t i16_Ibeta;                          //Clark Ibeta
int16_t i16_Id;                             //Park Id
int16_t i16_Iq;                             //Park Iq
int16_t i16_Id_error;
int16_t i16_Iq_error;

int16_t i16_Iu_ref_ADC;                     //Indicate the ADC value of zero Iu current 
int16_t i16_Iv_ref_ADC;                     //Indicate the ADC value of zero Iu current
int16_t i16_Iw_ref_ADC;                     //Indicate the ADC value of zero Iu current
  
//int16_t  i16_hall_speed;                  //Rotor speed for motor with hall
int16_t i16_rotor_speed;                    //Rotor speed for motor with hall  
int16_t i16_speed_error;  
int16_t i16_hall_angle;                     //hall_angle in the range of sin table(0~1023 in this demo code)

int32_t i32_sin_Q15;                        //Store look-up table SIN(cmd.angle)
int32_t i32_cos_Q15;                        //Store look-up table SIN(cmd.angle+512)
  
//int16_t  i16_operation_state;             //0: open loop without hall_speed , 2: close loop with hall_speed    

uint8_t u8_Hall_Position;    
uint8_t u8_Last_Hall_Position;
uint8_t u8_rotor_direction;                 //Set 1=FW & 0=RW In Check_Rotor_FW_RW_M0()
  
int16_t i16_IDC_Bus;                        //DC bus current in Q15
int16_t i16_IDC_Bus_avg;                    //Average of IDC in Q15
int16_t i16_VDC_Bus;                        //DC bus voltage in Q15
int16_t i16_VDC_Bus_avg;                    //Average voltage in Q15  
}AMotorInfo;

typedef struct tag_MotorCommand             // command
{
  //FOC outputs and be the commands for the next block
int16_t i16_Id;
int16_t i16_Iq;
int16_t i16_Vd;
int16_t i16_Vq;
int16_t i16_Valfa;
int16_t i16_Vbeta;
int16_t i16_Va;
int16_t i16_Vb;
int16_t i16_Vc;
int16_t i16_Duty0;                          //Duty0 of PWM0H
int16_t i16_Duty2;                          //Duty2 of PWM2H
int16_t i16_Duty4;                          //Duty4 of PWM4H

int16_t i16_rotor_speed;                    //dynamic Speed command
int16_t i16_rotor_speed_target;             //Final target of speed command
int16_t i16_angle;                          //angle command for the index of look-up sin/cos table
  
int16_t i16_Iq_target;                      //Final target of Iq command (for user Iq command type)
int16_t i16_Vq_target;                      //Final traget of Vq command (for user Vq command type)
  
uint8_t u8_start;                           //motor start command
//uint8_t u8_stop;                          //motor stop command
uint8_t u8_direction;                       //motor direction command
uint8_t u8_zone;                            //zone number in SVPWM operation
}AMotorCommand;


typedef struct tag_MotorController          // variables for controller
{
int32_t i32_Iq_control_limit_dig;           //Iq control limit = maximum Vq command = C_MAX_Vq_Q15
int32_t i32_Iq_control_limit_dig_ui;

int32_t i32_Id_control_limit_dig;           //Id control limit = maximum Vd command
int32_t i32_Id_control_limit_dig_ui;
  
int32_t i32_Speed_control_limit_dig;        //speed controller limit = maximum Iq command = current_limit * current_gain
int32_t i32_Speed_control_limit_dig_ui;     //speed_limit_dig <<16: limit integrator in PI
  
int16_t i16_maximum_speed_limit;            //Maximum speed command
}AMotorController;


typedef struct tag_MotorSym                 // sym infomation,
{
uint8_t u8_zero_current_error;              //zero_current_error
uint8_t u8_Idc_OC_error;                    //Idc over_current_error
uint8_t u8_Iphase_OC_error;                 //Iphase over_current_error

}AMotorSym;


typedef struct tag_MotorOther               // Other Variables
{
// static 
int32_t i32_Capture_Value_Q15;              //Store ECAP Hold Value when hall has edge change
int32_t i32_Last_Capture_Value_Q15;
int32_t i32_Hall_Step_Theda_Q26;            //cmd.angle=(Accumulated_Theda+Step_Theda)>>16 in ever PWM INT
int32_t i32_Hall_Accumulated_Theda_Q26;
  
int32_t i32_Hall_Step_Theda_EST_Constatnt;  //=(32767 << 11)*(Motor->spec.motor_pole)/PWM_freq/120;
int32_t i32_Hall_Wr_EST_Constant;           // = (72000000*60*2/Pole/CAPDIV) >> 9

uint8_t u8_flag_ECAP_overflow;              //In CAP INT, clear it first but set it if INT is caused by overflow

uint16_t ui16_initial_hall_change_cnt;      //When motor start, counting how many times of hall changes
uint8_t u8_operation_state;                 //state=0:not confirm rotor speed yet, state=2:got rotor speed by hall
uint8_t u8_Motor_Running_State;             //Denote motor is in which state. (臱笆祘Α场だ穦莱瞷琌或state)
uint8_t u8_CMD_Motor_Action;                //System use it to command motor to run or brake (╰参ノ硂跑计㏑)
uint8_t u8_flag_error_record;               //bit0=Idc_OC, bit1=Iu_error, bit2=Iv_error, bit3=Iw_error
  
uint8_t u8_flag_do_speed_average;           //Set 1 if need to do speed average
  
uint16_t ui16_speed_slope_counter;          //cnt++ every PWM, check if(cnt > spec.speed_slope * 1) cmd.speed++
uint16_t ui16_VSP_12bit_ADC;                //VSP(Variable resistor) voltage 12bit ADC data at PC.2(ADC1_CH2)

uint8_t u8_flag_need_read_VSP;              //In timer0 10ms or 100ms loop, set it if need to read VSP

}AMotorOther;


//--motor struct------------------
typedef struct tag_Motor
{
    AMotorSpec        spec; 
    AMotorInfo        info;
    AMotorCommand     cmd;
    AMotorController  ctrl;
    AMotorSym         sym;
    AMotorOther       other;
}AMotor; 
//----------------------------------

extern uint8_t u8_flag_VSP_control, u8_flag_UART_control;
extern uint16_t u16_Duty_UART,u16_Duty_VSP;
// For 1ms Timer0 Interrupt use. To count global time 1ms, 10ms, 100ms
extern uint8_t u8_flag_1ms, u8_flag_10ms, u8_flag_10ms, u8_flag_100ms, u8_flag_500ms;
extern uint16_t u16_timer0_int_counter, u16_1ms_timer_counter, u16_10ms_time_counter, \
              u16_100ms_time_counter, u16_500ms_time_counter;

// For Backup Iu_ref_ADC data
extern int16_t i16_bak_Iu_ref, i16_bak_Iv_ref, i16_bak_Iw_ref;

//--Global motor variable struct
extern  AMotor MotorA;

//--PI Controller varaible struct
typedef struct
{
    int32_t e1;
    int32_t ui_31;

    int32_t kp_15,ki_15;
}PICs;

extern PICs PI_Iq, PI_Id, PI_Speed;         //PI controllers of Iq, Id, Speed 


extern __IO int32_t  t1, t2, t3, t4;        //For test use only.

#define ABS(a) ((a)>=0 ? (a) : -(a))

#endif


