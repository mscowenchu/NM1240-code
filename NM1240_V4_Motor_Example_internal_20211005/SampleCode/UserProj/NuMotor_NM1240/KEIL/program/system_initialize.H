#ifndef _system_initialize_h
#define _system_initialize_h
#include "variable_typedefine.h"
#include "NM1240.h"
#include "system_parameter.h"


//---Functions-----------------------------------------------------
extern void SYS_Init_Clock(void);

extern void Initialize_EPWM(void);
extern void Initialize_EPWM_1R(void);
extern void Set_EPWM_MASK_Enable(uint8_t MASK_DATA);
extern void Set_EPWM_MASK_Disable(void);
extern void Set_GPA_As_EPWM_Output(void);
extern void Set_EPWM_Output_As_GPA_Output(uint8_t OUT_DATA);
extern void Stop_Motor_PWMOUT_OFF(void);
extern void Start_Motor_PWMOUT_ON(int16_t duty0, int16_t duty2, int16_t duty4);
extern void Initialize_Variabls(void);
extern void Initialize_Motor_Parameters(void);
extern void Initialize_PI_Parameters(void);

extern void Initialize_ECAP(void);

extern void Initialize_ADC(void);
extern uint16_t ADC0_SW_Read(uint32_t u32ChannelNum);
extern uint16_t ADC1_SW_Read(uint32_t u32ChannelNum);
extern void Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period(void);
extern void Set_ADC0_OP1_O_ADC1_OP1_O_Independent_2SH_Trg_by_Hardware(void);
extern uint16_t SW_Trg_ADC1_P7_Return_Reslut(void);
extern uint16_t SW_Trg_ADC1_P1_Return_Reslut(void);
extern uint16_t SW_Trg_ADC0_P3_Return_Reslut(void);
extern uint16_t SW_Trg_ADC0_P4_Return_Reslut(void);
extern uint16_t SW_Trg_ADC1_TempSensor_Return_Reslut(void);
extern uint16_t SW_Trg_ADC0_BandGap_Return_Reslut(void);
extern uint16_t SW_Trg_ADC0_OP1_O_Return_Reslut(void);
extern uint16_t SW_Trg_ADC1_OP1_O_Return_Reslut(void);
extern void Set_GPIO_as_ADC_Input(void);

extern void Initialize_OP1(void);

extern void Initialize_ACMP0(uint16_t DAC0_value);

extern void Initialize_Timer0(void);

extern void Initialize_USCI1_SPI1_for_DAC(void);

extern void Initialize_GPIO(void);

extern void Initialize_HDIV(void);

extern void Initialize_Temperature_Sensor(void);

extern void Initialize_GDMA(void);

extern void Initialize_NVIC_Priority(void);
extern void Enable_NVIC_For_System_Requirement(void);

extern void Initialize_Motor_System(void);
extern void Iu_Iv_Zero_Current_Calibration(void);

extern void Initialize_varaibles_for_TMR0_int(void);

extern void Initialize_Motor_System_1R(void);
extern void OP1_O_Zero_Current_Calibration(void);

extern void Initialize_NVIC_Priority_1R(void);
extern void Enable_NVIC_For_System_Requirement_1R(void);

//extern uint16_t SW_Trg_ADC0OP0_Return_Reslut(void);
//extern uint16_t SW_Trg_ADC1OP1_Return_Reslut(void);
//extern void Set_ADC0OP0_ADC1OP1_Simu_HW_Trg_by_PWM0Period(void);

extern void Check_control_mode(void);
  
//---Re-Name Functions for ADC---------------------------------------------------------------
/* Define a readable name for speed command */
#define SW_Get_Speed_Command_From_VR            SW_Trg_ADC1_P7_Return_Reslut
//#define SW_Get_Speed_Command_From_VR()            ADC1_SW_Read(ADC1_ADC1_P7_DAT)
/* Define a readable name for I_shunt */
#define SW_Get_I_BUS_From_Ishunt                SW_Trg_ADC0_P4_Return_Reslut
//#define SW_Get_I_BUS_From_Ishunt()                ADC0_SW_Read(ADC0_ADC0_P4_DAT)


/* Define a readable name for read Iu, Iv by ADC */
#define SW_Read_Iu_by_ADC                       SW_Trg_ADC1_P1_Return_Reslut
#define SW_Read_Iv_by_ADC                       SW_Trg_ADC0_P3_Return_Reslut
//#define SW_Read_Iu_by_ADC()                       ADC1_SW_Read(ADC1_ADC1_P1_DAT)
//#define SW_Read_Iv_by_ADC()                       ADC0_SW_Read(ADC0_ADC0_P3_DAT)

/* Define a readable name for setting ADC read Iu & Iv */
#define Set_HW_ADC_Read_IuIv_by_Simu_Mode       Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period

/* Define a readable name for read OP1_O(one shunt) by ADC0, ADC1 */
#define SW_Read_OP1_O_by_ADC0                   SW_Trg_ADC0_OP1_O_Return_Reslut
#define SW_Read_OP1_O_by_ADC1                   SW_Trg_ADC1_OP1_O_Return_Reslut
//#define SW_Read_OP1_O_by_ADC0()                   ADC0_SW_Read(ADC0_OP1_O_DAT)
//#define SW_Read_OP1_O_by_ADC1()                   ADC1_SW_Read(ADC1_OP1_O_DAT)

/* Define a readable name in 1R system for setting ADC to read phase current*/
#define Set_HW_ADC_Read_OP1_O_1R_by_Indpt_Mode  Set_ADC0_OP1_O_ADC1_OP1_O_Independent_2SH_Trg_by_Hardware

static __INLINE void CLEAR_EPWM_BK_RESTART_CNT(void)
{
    /* Clear Brake 0/1 interrupt flag */
    EPWM->INTSTS = EPWM_INTSTS_BRK0IF_Msk;
    EPWM->INTSTS = EPWM_INTSTS_BRK1IF_Msk;

    /* Start EPWM Counting and comparing */
    EPWM->CTL |= EPWM_CTL_CNTCLR_Msk;  //Reset EPWM Counter
    EPWM->CTL |= EPWM_CH_0_MASK | EPWM_CH_2_MASK | EPWM_CH_4_MASK;  //Start counting, Set CNTEN0/2/4 to START EPWM counting and comparing
}

#endif
