

#include "system_initialize.h"
#include "system_parameter.h"
#include "NM1240.h"
#include "variable_typedefine.h"
#include "PI_control.h"

// Flags to determin motor command from VSP(Variable resistor) or UART
uint8_t u8_flag_VSP_control, u8_flag_UART_control;
uint16_t u16_Duty_UART,u16_Duty_VSP;
// For 1ms Timer0 Interrupt use. To count global time 1ms, 10ms, 100ms
uint8_t u8_flag_1ms, u8_flag_10ms, u8_flag_10ms, u8_flag_100ms, u8_flag_500ms;
uint16_t u16_timer0_int_counter, u16_1ms_timer_counter, u16_10ms_time_counter, \
       u16_100ms_time_counter, u16_500ms_time_counter;

// For Backup Iu_ref_ADC data
int16_t  i16_bak_Iu_ref, i16_bak_Iv_ref, i16_bak_Iw_ref;

    
//--Global motor variable struct
AMotor MotorA; 

//--Global PI Controller varaible struct (PI controllers of Iq, Id, Speed)
PICs PI_Iq     = {0,0,C_I_Kp,C_I_Ki},     //{0,0, I_Kp_M0, I_Ki_M0},
     PI_Id     = {0,0,C_I_Kp,C_I_Ki},     //{0,0, I_Kp_M0, I_Ki_M0},
     PI_Speed  = {0,0,C_SP_Kp,C_SP_Ki};   //{0,0, W_Kp_M0, W_Ki_M0},


/*----Initialize_Variabls()-------------------------------------------------------
  Initialize all variables related to MotorA 
---------------------------------------------------------------------------------*/
void Initialize_Variabls(void)
{
    // motor specification
    MotorA.spec.i16_freq_pwm = C_MOTOR_PWM_FREQ;
    MotorA.spec.i16_speed_loop_time = C_SPEED_LOOP_TIME_ms;
    MotorA.spec.i16_dead_time = C_MOTOR_DEAD_TIME;
    MotorA.spec.i16_speed_slope_cnt_target = C_SPEED_SLOPE_CNT_TARGET;
    MotorA.spec.i16_speed_slope_delta_rpm = C_SPEED_SLOPE_DELTA_RPM;
    MotorA.spec.i16_current_gain = C_1A_Q15;
    MotorA.spec.i16_Idc_over_current = C_IDC_OVER_CURRENT_1_0_A;
    MotorA.spec.i16_Idc_OC_Q15_max = C_IDC_OC_Q15_MAX;
    MotorA.spec.i16_Iphase_OC_Q15_max = C_Phase_OVER_CURRENT_1_0_A * C_1A_Q15;
    MotorA.spec.i16_Vq_maximum = C_MAX_Vq_Q15;
    MotorA.spec.i16_Iq_current_limit_in_A = C_Max_Phase_Current;
    MotorA.spec.i16_Iq_current_limit = C_MAX_Iq_CURRENT_LIMIT;
    MotorA.spec.i16_speed_maximum = C_MAX_MOTOR_SPEED;
    MotorA.spec.i16_motor_pole = C_MOTOR_POLE;

    // Real-time feedback information/signals
    MotorA.info.i16_Iu = 0;
    MotorA.info.i16_Iv = 0;
    MotorA.info.i16_Iw = 0;
    MotorA.info.i16_Ialfa = 0;
    MotorA.info.i16_Ibeta = 0;
    MotorA.info.i16_Id = 0;
    MotorA.info.i16_Iq = 0;
    MotorA.info.i16_Id_error = 0;
    MotorA.info.i16_Iq_error = 0;

    MotorA.info.i16_Iu_ref_ADC = 0;
    MotorA.info.i16_Iv_ref_ADC = 0;
    MotorA.info.i16_Iw_ref_ADC = 0;

    MotorA.info.i16_rotor_speed = 0;
    MotorA.info.i16_speed_error = 0;
    MotorA.info.i16_hall_angle = 0;

    MotorA.info.i32_sin_Q15 = 0;
    MotorA.info.i32_cos_Q15 = 0;

    MotorA.info.u8_Hall_Position = 0;
    MotorA.info.u8_Last_Hall_Position = 0;
    MotorA.info.u8_rotor_direction = 0;
  
    MotorA.info.i16_IDC_Bus = 0;
    MotorA.info.i16_IDC_Bus_avg = 0;
    MotorA.info.i16_VDC_Bus = 0;
    MotorA.info.i16_VDC_Bus_avg = 0;

    // command
    MotorA.cmd.i16_Id = 0;
    MotorA.cmd.i16_Iq = 0;
    MotorA.cmd.i16_Vd = 0;
    MotorA.cmd.i16_Vq = 0;
    MotorA.cmd.i16_Valfa = 0;
    MotorA.cmd.i16_Vbeta = 0;
    MotorA.cmd.i16_Va = 0;
    MotorA.cmd.i16_Vb = 0;
    MotorA.cmd.i16_Vc = 0;
    MotorA.cmd.i16_Duty0 = 0;  //Duty0 of PWM0H
    MotorA.cmd.i16_Duty2 = 0;  //Duty2 of PWM2H
    MotorA.cmd.i16_Duty4 = 0;  //Duty4 of PWM4H
  
    MotorA.cmd.i16_rotor_speed = 0;
    MotorA.cmd.i16_rotor_speed_target = 0;
    MotorA.cmd.i16_angle = 0;

    MotorA.cmd.i16_Iq_target = 0;
    MotorA.cmd.i16_Vq_target = 0;

    MotorA.cmd.u8_start = 0;
//  MotorA.cmd.u8_stop = 1;
    MotorA.cmd.u8_direction = 1;  //1: means FW, 0: means RW


    // variables for controller 
    MotorA.ctrl.i32_Iq_control_limit_dig = MotorA.spec.i16_Vq_maximum;  //Maximum Vq limit
    MotorA.ctrl.i32_Iq_control_limit_dig_ui = MotorA.ctrl.i32_Iq_control_limit_dig << 15;  //Q30

    MotorA.ctrl.i32_Id_control_limit_dig = C_Max_Vd_Q15;   
    MotorA.ctrl.i32_Id_control_limit_dig_ui = MotorA.ctrl.i32_Id_control_limit_dig << 15; 

    MotorA.ctrl.i32_Speed_control_limit_dig = MotorA.spec.i16_Iq_current_limit;    
    MotorA.ctrl.i32_Speed_control_limit_dig_ui = MotorA.ctrl.i32_Speed_control_limit_dig << 15;
  
    MotorA.ctrl.i16_maximum_speed_limit = C_MAX_MOTOR_SPEED;

    // sym infomation, 
//  MotorA.sym.u8_motor_run = 0;
//  MotorA.sym.u8_motor_stop = 0;
    MotorA.sym.u8_zero_current_error = 0;
    MotorA.sym.u8_Idc_OC_error = 0;
    MotorA.sym.u8_Iphase_OC_error = 0;
  
    // Other Variables
    MotorA.other.i32_Capture_Value_Q15 = 0;
    MotorA.other.i32_Last_Capture_Value_Q15 = 0;
    MotorA.other.i32_Hall_Step_Theda_Q26 = 0;
    MotorA.other.i32_Hall_Accumulated_Theda_Q26 = 0;


    MotorA.other.i32_Hall_Step_Theda_EST_Constatnt = (32768 << 11)/4*C_MOTOR_POLE/C_MOTOR_PWM_FREQ/(120/4);

    //MotorA.other.i32_Hall_Wr_EST_Constant = ((SYSTEM_CLOCK/C_CAP_GAIN*60/CAPDIV))*2 / i16_motor_pole;  // = 72000000/16*60/32*2/Pole = 16875000/DIV/Pole =2109375 (if Pole=8)
    MotorA.other.i32_Hall_Wr_EST_Constant = ((SYSTEM_CLOCK/C_CAP_GAIN*60/(C_CAPDIV/2))) / C_MOTOR_POLE;  // = 72000000/16*60/32*2/Pole = 16875000/DIV/Pole =2109375 (if Pole=8)
    if(CAP_180_DEGREE)
        MotorA.other.i32_Hall_Wr_EST_Constant = MotorA.other.i32_Hall_Wr_EST_Constant / 2;  //=703125 for pole=8
    if(CAP_60_DEGREE)
        MotorA.other.i32_Hall_Wr_EST_Constant = MotorA.other.i32_Hall_Wr_EST_Constant / 6;  //=468750 for pole=8

    MotorA.other.u8_flag_ECAP_overflow = 0;

    MotorA.other.ui16_initial_hall_change_cnt = 0;  //cnt++ at hall changes when motor starts

    MotorA.other.u8_operation_state = C_OPEN_LOOP;  //0: operation not in sine-mode, 2: in sine-mode
    MotorA.other.u8_Motor_Running_State = C_State_Initial;  
    MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP;
    MotorA.other.u8_flag_error_record = 0;  //Initially set 0: means no error.
  
    MotorA.other.ui16_speed_slope_counter = 0;  //cnt++ in every PWM_ADC INT
    MotorA.other.ui16_VSP_12bit_ADC = 0;  //VSP(Variable resistor) voltage 12bit ADC data at PC.2(ADC1_CH2)

    MotorA.other.u8_flag_need_read_VSP = 0;  //In timer0 10ms or 100ms loop, set it if need to read VSP
}
//====End of "Initialize_Variabls()"=================================


/*----Initialize_Motor_Parameters()-------------------------------------------------------
  Depends on the system requirement to re-initialize the variables.
---------------------------------------------------------------------------------*/
void Initialize_Motor_Parameters(void)
{
//  motor specification  (Need not to reset because all spec is the same )
//  MotorA.spec.i16_freq_pwm = C_MOTOR_PWM_FREQ;
//  MotorA.spec.i16_speed_loop_time = C_SPEED_LOOP_TIME_ms;
//  MotorA.spec.i16_dead_time = C_MOTOR_DEAD_TIME;      
//  MotorA.spec.i16_speed_slope_cnt_target = C_SPEED_SLOPE_CNT_TARGET;
//  MotorA.spec.i16_speed_slope_delta_rpm = C_SPEED_SLOPE_DELTA_RPM;
//  MotorA.spec.i16_current_gain = C_1A_Q15;
//  MotorA.spec.i16_Idc_over_current = C_IDC_OVER_CURRENT_1_0_A;
//  MotorA.spec.i16_Idc_OC_Q15_max = C_IDC_OC_Q15_MAX;
//  MotorA.spec.i16_Iphase_OC_Q15_max = C_Phase_OVER_CURRENT_1_0_A * C_1A_Q15;
//  MotorA.spec.i16_Vq_maximum = C_MAX_Vq_Q15; 
//  MotorA.spec.i16_Iq_current_limit_in_A = C_Max_Phase_Current;
//  MotorA.spec.i16_Iq_current_limit = C_MAX_Iq_CURRENT_LIMIT;
//  MotorA.spec.i16_speed_maximum = C_MAX_MOTOR_SPEED;
//  MotorA.spec.i16_motor_pole = C_MOTOR_POLE;

    //Real-time feedback information/signals (The real time information is keeping update)
//  MotorA.info.i16_Iu = 0;
//  MotorA.info.i16_Iv = 0;
//  MotorA.info.i16_Iw = 0;
//  MotorA.info.i16_Ialfa = 0;
//  MotorA.info.i16_Ibeta = 0;
//  MotorA.info.i16_Id = 0;
//  MotorA.info.i16_Iq = 0;
//  MotorA.info.i16_Id_error = 0;
//  MotorA.info.i16_Iq_error = 0;

//  MotorA.info.i16_Iu_ref_ADC = 0;
//  MotorA.info.i16_Iv_ref_ADC = 0;
//  MotorA.info.i16_Iw_ref_ADC = 0;

//  MotorA.info.i16_rotor_speed = 0;
//  MotorA.info.i16_speed_error = 0;
//  MotorA.info.i16_hall_angle = 0;

//  MotorA.info.i32_sin_Q15 = 0;
//  MotorA.info.i32_cos_Q15 = 0;

//  MotorA.info.u8_Hall_Position = 0;
//  MotorA.info.u8_Last_Hall_Position = 0;
//  MotorA.info.u8_rotor_direction = 0;
//  
//  MotorA.info.i16_IDC_Bus = 0;
//  MotorA.info.i16_IDC_Bus_avg = 0;
//  MotorA.info.i16_VDC_Bus = 0;
//  MotorA.info.i16_VDC_Bus_avg = 0;

    //command (Command items may need reset)
    MotorA.cmd.i16_Id = 0;
    MotorA.cmd.i16_Iq = 0;
    MotorA.cmd.i16_Vd = 0;
    MotorA.cmd.i16_Vq = 0;
    MotorA.cmd.i16_Valfa = 0;
    MotorA.cmd.i16_Vbeta = 0;
    MotorA.cmd.i16_Va = 0;
    MotorA.cmd.i16_Vb = 0;
    MotorA.cmd.i16_Vc = 0;
    MotorA.cmd.i16_Duty0 = 0;  //Duty0 of PWM0H
    MotorA.cmd.i16_Duty2 = 0;  //Duty2 of PWM2H
    MotorA.cmd.i16_Duty4 = 0;  //Duty4 of PWM4H
  
    MotorA.cmd.i16_rotor_speed = 0;
    MotorA.cmd.i16_rotor_speed_target = 0;
    MotorA.cmd.i16_angle = 0;

    MotorA.cmd.i16_Iq_target = 0;
    MotorA.cmd.i16_Vq_target = 0;

    MotorA.cmd.u8_start = 0;
    MotorA.cmd.u8_direction = 1;  //1: means FW, 0: means RW


    //variables for controller (same as initialize, so need not to reset)
//  MotorA.ctrl.i32_Iq_control_limit_dig = MotorA.spec.i16_Vq_maximum;  //Maximum Vq limit
//  MotorA.ctrl.i32_Iq_control_limit_dig_ui = MotorA.ctrl.i32_Iq_control_limit_dig >> 15; //Q30

//  MotorA.ctrl.i32_Id_control_limit_dig = C_Max_Vd_Q15;
//  MotorA.ctrl.i32_Id_control_limit_dig_ui = MotorA.ctrl.i32_Id_control_limit_dig >> 15; 

//  MotorA.ctrl.i32_Speed_control_limit_dig = MotorA.spec.i16_Iq_current_limit;
//  MotorA.ctrl.i32_Speed_control_limit_dig_ui = MotorA.ctrl.i32_Speed_control_limit_dig << 15;
//  
//  MotorA.ctrl.i16_maximum_speed_limit = C_MAX_MOTOR_SPEED;


    //sym infomation, (This part had better be clear in the error-handle function)
//  MotorA.sym.u8_motor_run = 0;
//  MotorA.sym.u8_motor_stop = 0;
    MotorA.sym.u8_zero_current_error = 0;
    MotorA.sym.u8_Idc_OC_error = 0;
    MotorA.sym.u8_Iphase_OC_error = 0;
  
    //Other Variables
//  MotorA.other.i32_Capture_Value_Q15 = 0;    
//  MotorA.other.i32_Last_Capture_Value_Q15 = 0;
//  MotorA.other.i32_Hall_Step_Theda_Q26 = 0;
//  MotorA.other.i32_Hall_Accumulated_Theda_Q26 = 0;

//  MotorA.other.i32_Hall_Step_Theda_EST_Constatnt = (32768 << 11)/4*C_MOTOR_POLE/C_MOTOR_PWM_FREQ/(120/4);
//  
//  MotorA.other.i32_Hall_Wr_EST_Constant = ((SYSTEM_CLOCK/C_CAP_GAIN*60/(C_CAPDIV/2))) / C_MOTOR_POLE;  // = 72000000/16*60/32*2/Pole = 16875000/DIV/Pole =2109375 (if Pole=8)
//  if(CAP_180_DEGREE)    
//      MotorA.other.i32_Hall_Wr_EST_Constant = MotorA.other.i32_Hall_Wr_EST_Constant / 2; //=703125 for pole=8
//  if(CAP_60_DEGREE)    
//      MotorA.other.i32_Hall_Wr_EST_Constant = MotorA.other.i32_Hall_Wr_EST_Constant / 6; //=468750 for pole=8

//  MotorA.other.u8_flag_ECAP_overflow = 0;  
//  
//  MotorA.other.ui16_initial_hall_change_cnt = 0;  //cnt++ at hall changes when motor starts

//  MotorA.other.u8_operation_state = C_OPEN_LOOP;      //0: operation not in sine-mode, 2: in sine-mode
    MotorA.other.u8_Motor_Running_State = C_State_Initial;  
    MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP;

//  MotorA.other.u8_flag_error_record = 0;
  
    MotorA.other.ui16_speed_slope_counter = 0;  //cnt++ in every PWM_ADC INT
//  MotorA.other.ui16_VSP_12bit_ADC = 0;  //VSP(Variable resistor) voltage 12bit ADC data at PC.2(ADC1_CH2)

//  MotorA.other.u8_flag_need_read_VSP = 0;  //In timer0 10ms or 100ms loop, set it if need to read VSP
}
//====End of "Initialize_Motor_Parameters()"=================================


void Initialize_PI_Parameters(void)
{
    PI_Iq.ui_31 = 0;
    PI_Id.ui_31 = 0;
    PI_Speed.ui_31 = 0;
}
