#include "motor_six_step.h"
#include "motor_functions.h"
#include "protocol.h"


void SIX_STEP_CHANGE_PHASE_FW(void) 
{
    uint8_t Hall_Position;

    Hall_Position = (HALL_PORT_M0 & 0x07);  // Reserve PF2~PF0
  
    switch(Hall_Position)
    {
        case HALL_STATE1_FW_M0:
            Trigger_update_micro_fig();  //For trigger microcosm waveform of GUI
            
            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE1_FW;  //HALL_PORT_M0 = 5

            if(CAP_180_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed  
            }
            else if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE2_FW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE2_FW;  //HALL_PORT_M0 = 1
      
            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE3_FW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE3_FW;  //HALL_PORT_M0 = 3
      
            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE4_FW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE4_FW;  //HALL_PORT_M0 = 2

            if(CAP_180_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }
            else if(CAP_60_DEGREE)
            {
              MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
              HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }
        break;

        case HALL_STATE5_FW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE5_FW;  //HALL_PORT_M0 = 6
      
            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE6_FW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE6_FW;  //HALL_PORT_M0 = 4

            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;
    }
}


void SIX_STEP_CHANGE_PHASE_RW(void)
{
    uint8_t Hall_Position;

    Hall_Position = (HALL_PORT_M0 & 0x07);  //Reserve PF2~PF0

    switch(Hall_Position)
    {
        case HALL_STATE1_RW_M0:
            Trigger_update_micro_fig();  //For trigger microcosm waveform of GUI
            
            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE1_RW;  //HALL_PORT_M0 = 5
            if(CAP_180_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }
            else if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }
        break;

        case HALL_STATE2_RW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE2_RW;  //HALL_PORT_M0 = 1

            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE3_RW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE3_RW;  //HALL_PORT_M0 = 3

            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE4_RW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE4_RW;  //HALL_PORT_M0 = 2
      
            if(CAP_180_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)(((ECAP_HLD0-C_CAP_Reload_Value) & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }
            else if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD0 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE5_RW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE5_RW;  //HALL_PORT_M0 = 6
      
            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD1 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;

        case HALL_STATE6_RW_M0:

            EPWM->PHCHG = SIX_STEP_PWM_MASK_STATE6_RW;  //HALL_PORT_M0 = 4

            if(CAP_60_DEGREE)
            {
                MotorA.other.i32_Capture_Value_Q15 = (signed short int)((ECAP_HLD2 & 0x00FFFFFF)>>C_CAP_SHIFT);
                HallSpeedEstimator(&MotorA);  //Update new info.i32_rotor_speed
            }

        break;
    }
}
