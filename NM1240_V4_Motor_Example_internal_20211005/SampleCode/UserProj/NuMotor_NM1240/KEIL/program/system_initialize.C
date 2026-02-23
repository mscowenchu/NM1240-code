
/*************************************************************************************
* 
* Functions to initialize the system including
*  MCU
*  System environment
**************************************************************************************/

#include <stdio.h>  
#include "NM1240.h"
#include "system_initialize.h"
#include "system_parameter.h"
#include "variable_typedefine.h"
#include "motor_functions.h"
#include "protocol.h"

void SYS_Init_Clock(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    
    #if SYSTEM_CLOCK == 48000000
        /* Enable 48MHz HIRC */
        CLK->PWRCTL = CLK->PWRCTL | CLK_PWRCTL_HIRCEN_Msk;
    #endif
  
    #if SYSTEM_CLOCK == 60000000
        /* Enable 60MHz HIRC */
        CLK->PWRCTL |= CLK_PWRCTL_HIRC_SEL_Msk | CLK_PWRCTL_HIRCEN_Msk;
    #endif

    /* Waiting for system clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* HCLK Clock source from HIRC */
    CLK->CLKSEL0 = CLK->CLKSEL0 | CLK_HCLK_SRC_HIRC;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}


//==Setting About EPWM for FOC======================================================================
/*--Initialize_EPWM()------------------------------------- 
Pin: PWM0~PWM5 = PA0~PA5
1. Set up EPWM as complementary mode, Center-aligned type (PWM0/1, PWM2/3, PWM4/5)
2. At initial, force EPWM IP output by Mask Data to turn off all MOS.
3. Interrupt: Enable EPWM Brake0 function and BRK0 Interrupt. 
   Selct BRK0 source from ACMP0 Output for detecting Idc Over current in this demo system.
4. Set duty = 0%
   Setup dead-time for the complementary PWM
5. Start EPWM counting and comparing
6. Set GPA as EPWM Output.
-------------------------------------------------------------------*/
#if P_FOC_CONTROL
  #ifdef P_2R_FOC
void Initialize_EPWM(void)
{
    /* Enable EPWM IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_EPWMCKEN_Msk;  
  
    /* SYS_ResetModule(EPWM_RST); */
    SYS->IPRST1 |= SYS_IPRST1_EPWMRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_EPWMRST_Msk;
  
    /* Set EPWM as Complementary mode, Center-aligned type and Auto-reload mode */
    EPWM->CTL = (EPWM->CTL & ~EPWM_CTL_MODE_Msk) | (1UL << EPWM_CTL_MODE_Pos);  //Set Complementary mode
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_CNTTYPE_Msk);   //Set center-aligned type
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_CNTMODE_Msk);    //Set Auto-reload mode (contineous PWM mode)
  
    /* Set EPWM Clock = HCLK = 60MHz */  
    EPWM->CLKDIV = (EPWM->CLKDIV & ~(EPWM_CLKDIV_CLKDIV_Msk)) | (EPWM_CLK_DIV_1 << EPWM_CLKDIV_CLKDIV_Pos);
  
    /* Set EPWM Period */
    EPWM->PERIOD = C_PWM_FULL_SCALE;  //C_PWM_FULL_SCALE is defined in system_parameter.h
    
    /* Set EPWM Duty; Set initial duty to 0% for 3 pairs */
    EPWM->CMPDAT[0] = 0;  //Set duty for PWM0/1
    EPWM->CMPDAT[2] = 0;  //Set duty for PWM2/3
    EPWM->CMPDAT[4] = 0;  //Set duty for PWM4/5
  
    /* Setup Dead-time */
    /* set dead-time and start dead-time counter (DTCNT) */
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT01_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT01_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT01_Msk;  
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT23_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT23_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT23_Msk;
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT45_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT45_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT45_Msk;

    /* Eanble MASK function and set EPWM IP output as MASK data */
    //--At initial state, force the EPWM MSKDAT[5:0] = PWM_OUT_OFF_MOS
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);      //Set PWM MASK enable and MASKDATA=PWM_OUT_OFF_MOS
    
    /* Set EPWM Output Polarity */
    EPWM->NPCTL &= ~(0x3F);     //Not enable negative polarity --> Means it is positive polarity

    /* Set EPWM BRK0 */
    //---Set ACMP0 Output as the BRK0 source.
    EPWM->BRKCTL = EPWM->BRKCTL | EPWM_BRKCTL_BRK0A0EN_Msk | EPWM_BRKCTL_BRK0EN_Msk;

    /* Enable EPWM Brake0 interrupt */
//    EPWM->INTSTS |= 0x3F073F01;    //Clear all interrupt status flags before enable Interrupt.
    EPWM->INTSTS = EPWM->INTSTS;    //Write status register itself to clear all interrupt status flags
    EPWM->INTEN |= (EPWM_INTEN_BRK0IEN_Msk);   //Enable BRK0 Interrupt
//  EPWM->INTEN |= (EPWM_INTEN_PIEN_Msk);    //Eanble PWM Interrupt at periodic point (not use yet) 
//  EPWM->INTEN |= (EPWM_INTEN_CIEN_Msk);    //Enable PWM Interrupt at center point (not use yet)
//  NVIC_EnableIRQ(EPWM_IRQn);        //Not enable it yet.
//  NVIC_EnableIRQ(BRAKE0_IRQn);      //Not enable it yet.

    /* Start EPWM Counting and comparing */
    EPWM->CTL |= EPWM_CTL_CNTCLR_Msk;  //Reset EPWM Counter
    EPWM->CTL |= EPWM_CH_0_MASK | EPWM_CH_2_MASK | EPWM_CH_4_MASK;    //Start counting, Set CNTEN0/2/4 to START EPWM counting and comparing
      
    /*---Setup GPA as EPWM Outputs ------------------------*/
    /* Preset data of PA0~PA5 as PWM_OUT_OFF_MOS to turn off MOS initially */
    Set_GPA_As_EPWM_Output();
}
  #endif  //---End of "#ifdef P_2R_FOC"---------------------


  #ifdef P_1R_FOC
/*--Initialize_EPWM_1R()------------------------------------- 
Pin: PWM0~PWM5 = PA0~PA5
1. Set up EPWM as complementary mode, Center-aligned type (PWM0/1, PWM2/3, PWM4/5), Auto-reload and Asymmetric mode
2. At initial, force EPWM IP output by Mask Data to turn off all MOS.
3. Interrupt: Enable EPWM Brake0 function and BRK0 Interrupt. 
   Selct BRK0 source from ACMP0 Output for detecting Idc Over current in this demo system.
4. Set duty = 0%
   Setup dead-time for the complementary PWM
5. Start EPWM counting and comparing
6. Set GPA as EPWM Output.
-------------------------------------------------------------------*/
void Initialize_EPWM_1R(void)
{
    /* Enable EPWM IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_EPWMCKEN_Msk;

    /* SYS_ResetModule(EPWM_RST); */
    SYS->IPRST1 |= SYS_IPRST1_EPWMRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_EPWMRST_Msk;
  
    /* Set EPWM as Complementary mode, Center-aligned type and Auto-reload mode */
    EPWM->CTL = (EPWM->CTL & ~EPWM_CTL_MODE_Msk) | (1UL << EPWM_CTL_MODE_Pos);  //Set Complementary mode
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_CNTTYPE_Msk);  //Set center-aligned type
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_CNTMODE_Msk);  //Set Auto-reload mode (contineous PWM mode)
  
    /* Set EPWM as Asymmetric mode in center-aligned type */
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_ASYMEN_Msk);  //Set Asymmetric mode in center-aligned type

    /* Set EPWM Clock = HCLK = 60MHz */
    EPWM->CLKDIV = (EPWM->CLKDIV & ~(EPWM_CLKDIV_CLKDIV_Msk)) | (EPWM_CLK_DIV_1 << EPWM_CLKDIV_CLKDIV_Pos);
  
    /* Set EPWM Period */
    EPWM->PERIOD = C_PWM_FULL_SCALE;  //C_PWM_FULL_SCALE is defined in system_parameter.h
  
    /* Set EPWM Duty; Set initial duty to 0% for 3 pairs */
    EPWM->CMPDAT[0] = 0;  //Set duty for PWM0/1
    EPWM->CMPDAT[2] = 0;  //Set duty for PWM2/3
    EPWM->CMPDAT[4] = 0;  //Set duty for PWM4/5
    
    /* Setup Dead-time */
    /* set dead-time and start dead-time counter (DTCNT) */
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT01_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT01_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT01_Msk;
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT23_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT23_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT23_Msk;
    EPWM->DTCTL = (EPWM->DTCTL & ~(EPWM_DTCTL_DTCNT45_Msk)) | (C_EPWM_DEAD_TIME << EPWM_DTCTL_DTCNT45_Pos);
    EPWM->CTL |= EPWM_CTL_DTCNT45_Msk;

    /* Eanble MASK function and set EPWM IP output as MASK data */
    //--At initial state, force the EPWM MSKDAT[5:0] = PWM_OUT_OFF_MOS
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Set PWM MASK enable and MASKDATA=PWM_OUT_OFF_MOS
  
    /* Set EPWM Output Polarity */
    EPWM->NPCTL &= ~(0x3F);  //Not enable negative polarity --> Means it is positive polarity

    /* Set EPWM BRK0 */
    //---Set ACMP0 Output as the BRK0 source.
    EPWM->BRKCTL = EPWM->BRKCTL | EPWM_BRKCTL_BRK0A0EN_Msk | EPWM_BRKCTL_BRK0EN_Msk;

    /* Enable EPWM Brake0 interrupt */
    EPWM->INTSTS = EPWM->INTSTS;  //Write status register itself to clear all interrupt status flags
    EPWM->INTEN |= (EPWM_INTEN_BRK0IEN_Msk);  //Enable BRK0 Interrupt
    EPWM->INTEN |= (EPWM_INTEN_PIEN_Msk);  //Eanble PWM Interrupt at periodic point
//  EPWM->INTEN |= (EPWM_INTEN_CIEN_Msk);  //Enable PWM Interrupt at center point (not use yet)
//  NVIC_EnableIRQ(EPWM_IRQn);  //Not enable it yet.
//  NVIC_EnableIRQ(BRAKE0_IRQn);  //Not enable it yet.

    /* Start EPWM Counting and comparing */
    EPWM->CTL |= EPWM_CTL_CNTCLR_Msk;  //Reset EPWM Counter
    EPWM->CTL |= EPWM_CH_0_MASK | EPWM_CH_2_MASK | EPWM_CH_4_MASK;  //Start counting, Set CNTEN0/2/4 to START EPWM counting and comparing
  
    /*---Setup GPA as EPWM Outputs ------------------------*/
    /* Preset data of PA0~PA5 as PWM_OUT_OFF_MOS to turn off MOS initially */
    Set_GPA_As_EPWM_Output();
}
  #endif //---End of "#ifdef P_1R_FOC"---------------------


#endif
//end of EPWM initial for FOC============================================================================


//==Setting About EPWM for Six_step======================================================================
/*--Initialize_EPWM()------------------------------------- 
Pin: PWM0~PWM5 = PA0~PA5
1. Set up EPWM as Independent mode, Edge-aligned type (PWM0/1, PWM2/3, PWM4/5)
2. At initial, force EPWM IP output by Mask Data to turn off all MOS.
3. Interrupt: Enable EPWM Brake0 function and BRK0 Interrupt. 
   Selct BRK0 source from ACMP0 Output for detecting Idc Over current in this demo system.
4. Set GROUP MODE
5. Set duty = 0%
6. Start EPWM counting and comparing
7. Set GPA as EPWM Output.
-------------------------------------------------------------------*/
#if P_SIX_STEP_CONTROL 
void Initialize_EPWM(void)
{
    /* Enable EPWM IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_EPWMCKEN_Msk;  
  
    /* Reset IP */
    SYS->IPRST1 |= SYS_IPRST1_EPWMRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_EPWMRST_Msk;
  
    /* Set EPWM as Independent mode, Edge-aligned type and Auto-reload mode */
    EPWM->CTL = (EPWM->CTL & ~EPWM_CTL_MODE_Msk) | (0 << EPWM_CTL_MODE_Pos);  //Set Independent mode
    EPWM->CTL = (EPWM->CTL & ~EPWM_CTL_CNTTYPE_Msk);  //Set Edge-aligned type
    EPWM->CTL = (EPWM->CTL | EPWM_CTL_CNTMODE_Msk);  //Set Auto-reload mode (contineous PWM mode)

    /* Set EPWM Clock = HCLK = 60MHz */  
    EPWM->CLKDIV = (EPWM->CLKDIV & ~(EPWM_CLKDIV_CLKDIV_Msk)) | (EPWM_CLK_DIV_1 << EPWM_CLKDIV_CLKDIV_Pos);
  
    /* Set EPWM Period */
    EPWM->PERIOD = C_PWM_FULL_SCALE;  //C_PWM_FULL_SCALE is defined in system_parameter.h
    
    /* Set EPWM_ENABLE_GROUP_MODE */
    EPWM->CTL |= EPWM_CTL_GROUPEN_Msk;
  
    /* Set EPWM Duty; Set initial duty to 0% for 3 pairs */
    EPWM->CMPDAT[0] = 0;  //Set duty for PWM0/1

    /* Eanble MASK function and set EPWM IP output as MASK data */
    //--At initial state, force the EPWM MSKDAT[5:0] = PWM_OUT_OFF_MOS
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Set PWM MASK enable and MASKDATA=PWM_OUT_OFF_MOS
  
    /* Set EPWM Output Polarity */
    EPWM->NPCTL &= ~(0x3F);  //Not enable negative polarity --> Means it is positive polarity

    /* Set EPWM BRK0 */
    //---Set ACMP0 Output as the BRK0 source.
    EPWM->BRKCTL = EPWM->BRKCTL | EPWM_BRKCTL_BRK0A0EN_Msk | EPWM_BRKCTL_BRK0EN_Msk;

    /* Enable EPWM Brake0 interrupt */
    //EPWM->INTSTS |= 0x3F073F01;  //Clear all interrupt status flags before enable Interrupt.
    EPWM->INTSTS = EPWM->INTSTS;  //Write status register itself to clear all interrupt status flags
    EPWM->INTEN |= (EPWM_INTEN_BRK0IEN_Msk);  //Enable BRK0 Interrupt
//  EPWM->INTEN |= (EPWM_INTEN_PIEN_Msk);  //Eanble PWM Interrupt at periodic point (not use yet)
//  EPWM->INTEN |= (EPWM_INTEN_CIEN_Msk);  //Enable PWM Interrupt at center point (not use yet)
//  NVIC_EnableIRQ(EPWM_IRQn);  //Not enable it yet.
//  NVIC_EnableIRQ(BRAKE0_IRQn);  //Not enable it yet.

    /* Start EPWM Counting and comparing */
    EPWM->CTL |= EPWM_CTL_CNTCLR_Msk;  //Reset EPWM Counter
    EPWM->CTL |= EPWM_CH_0_MASK | EPWM_CH_2_MASK | EPWM_CH_4_MASK;  //Start counting, Set CNTEN0/2/4 to START EPWM counting and comparing

    /*---Setup GPA as EPWM Outputs ------------------------*/
    /* Preset data of PA0~PA5 as PWM_OUT_OFF_MOS to turn off MOS initially */
    Set_GPA_As_EPWM_Output();
}
#endif
//end of EPWM initial for six_step============================================================================


/*--Set_EPWM_MASK_Enable()-------------------------------------
 Eanble MASK function and set EPWM IP output as MASK data
 At initial state, force the EPWM MSKDAT[5:0] = PWM_OUT_OFF_MOS
Note: Must disable Phase Change Function if only use MASK function.
-------------------------------------------------------------------*/
void Set_EPWM_MASK_Enable(uint8_t MASK_DATA)
{
    //Just use Mask Function, so diable Phase Change Function
    /* Set TRGSEL=0x7 ==> Disable Auto Phase Change Function  */
    EPWM->PHCHG = EPWM->PHCHG | EPWM_PHCHG_TRGSEL_Msk;

    /* Set MASK_Enable and MASK_Data */
    EPWM->PHCHG = (EPWM->PHCHG & ~(0x3F)) | MASK_DATA;  //Set MSKDAT[5:0] = PWM_OUT_OFF_MOS
    EPWM->PHCHG = EPWM->PHCHG | 0x3F00;  //Enable MASKEN5~MASKEN0(bit13~bit8)

    /* If not disable phase change function, then must set PHCHGNXT with MASKEN as PHCHG */
//  EPWM->PHCHGNXT = (EPWM->PHCHGNXT & ~(0x3F)) | MASK_DATA;  //Set MSKDAT[5:0] = PWM_OUT_OFF_MOS
//  EPWM->PHCHGNXT = EPWM->PHCHGNXT | 0x3F00;  //Enable MASKEN5~MASKEN0(bit13~bit8)
}


void Set_EPWM_MASK_Disable(void)
{
    /* Disable PWM MASK function */
    //--At initial state, force the EPWM MSKDAT[5:0] = PWM_OUT_OFF_MOS
    EPWM->PHCHG = (EPWM->PHCHG & ~(0x3F)) | PWM_OUT_OFF_MOS;  //Set MSKDAT[5:0] = PWM_OUT_OFF_MOS
    EPWM->PHCHG = EPWM->PHCHG & (~0x3F00);  //Disable MASKEN5~MASKEN0(bit13~bit8)
}


/*--------------------------------------------------------------
Switch GPIO pins as EPWM output
1. Pre-set DOUT = PWM_OUT_OFF_MOS to turn off MOS
2. GPIO output as Push-pull mode
3. Switch port pins to PWM output.
---------------------------------------------------------------*/
void Set_GPA_As_EPWM_Output(void)
{
    /* Preset data of PA0~PA5 as PWM_OUT_OFF_MOS to turn off MOS initially */
    PA->DOUT &=  PWM_OUT_OFF_MOS;

    /* Set PA0~PA5 as output mode */
    PA->MODE = (PA->MODE & ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk |
                             GPIO_MODE_MODE3_Msk | GPIO_MODE_MODE4_Msk | GPIO_MODE_MODE5_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE4_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE5_Pos);

    /* Set GPA multi-function pins as EPWM Output */
    SYS->GPA_MFP = (SYS->GPA_MFP & ~(SYS_GPA_MFP_PA0MFP_Msk | SYS_GPA_MFP_PA1MFP_Msk | SYS_GPA_MFP_PA2MFP_Msk |
                                     SYS_GPA_MFP_PA3MFP_Msk | SYS_GPA_MFP_PA4MFP_Msk | SYS_GPA_MFP_PA5MFP_Msk)) |
                    SYS_GPA_MFP_PA0_EPWM_CH0 | SYS_GPA_MFP_PA1_EPWM_CH1 | SYS_GPA_MFP_PA2_EPWM_CH2 |
                    SYS_GPA_MFP_PA3_EPWM_CH3 | SYS_GPA_MFP_PA4_EPWM_CH4 | SYS_GPA_MFP_PA5_EPWM_CH5;
}


/*--------------------------------------------------------------
Switch EPWM pins as GPIO mode
1. Pre-set DOUT = PWM_OUT_OFF_MOS to turn off MOS
2. GPIO output as Push-pull mode
3. Switch port pins to GPA output.
---------------------------------------------------------------*/
void Set_EPWM_Output_As_GPA_Output(uint8_t OUT_DATA)
{
    /* Preset data of PA0~PA5 as PWM_OUT_OFF_MOS to turn off MOS initially */
    PA->DOUT = (PA->DOUT & PWM_OUT_OFF_MOS) | (OUT_DATA & 0x3F);  //usually it should be set as PWM_OUT_OFF_MOS;

    /* Set PA0~PA5 as output mode */
    PA->MODE = (PA->MODE & ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk |
                             GPIO_MODE_MODE3_Msk | GPIO_MODE_MODE4_Msk | GPIO_MODE_MODE5_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE4_Pos) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE5_Pos);

    /* Set GPA multi-function pins as GPIO function */
    SYS->GPA_MFP = (SYS->GPA_MFP & ~(SYS_GPA_MFP_PA0MFP_Msk | SYS_GPA_MFP_PA1MFP_Msk | SYS_GPA_MFP_PA2MFP_Msk |
                                     SYS_GPA_MFP_PA3MFP_Msk | SYS_GPA_MFP_PA4MFP_Msk | SYS_GPA_MFP_PA5MFP_Msk)) |
                    SYS_GPA_MFP_PA0_GPIO | SYS_GPA_MFP_PA1_GPIO | SYS_GPA_MFP_PA2_GPIO |
                    SYS_GPA_MFP_PA3_GPIO | SYS_GPA_MFP_PA4_GPIO | SYS_GPA_MFP_PA5_GPIO;
}


/*----Stop_Motor_PWMOUT_OFF()----------------------------------------------------------
Stop motor by setting PWM output to turn off all MOSFET
1. Set PWM as MASK output with "PWM_OUT_OFF_MOS"
2. (Optional) set PWM port as GPIO mode
3. Reset related flags
4. Re-initialize the variables
---------------------------------------------------------------*/
void Stop_Motor_PWMOUT_OFF(void)
{
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Use MASK function to let PWM output to turn off MOS

    //--It is optional to switch PWM pins to GPIO.
    Set_EPWM_Output_As_GPA_Output(PWM_OUT_OFF_MOS);  //Switch PWM output pins as GPIO output.

    MotorA.cmd.u8_start = 0;
    MotorA.other.u8_CMD_Motor_Action = C_CMD_STOP;
    MotorA.other.u8_Motor_Running_State = C_State_Initial;

    /* Reset some motor parameters */
    Initialize_Motor_Parameters();  //Initialize some parameters
    Initialize_PI_Parameters();

    // For Re-store the 3 Iu_ref_ADC data
    MotorA.info.i16_Iu_ref_ADC = i16_bak_Iu_ref;
    MotorA.info.i16_Iv_ref_ADC = i16_bak_Iv_ref;
    MotorA.info.i16_Iw_ref_ADC = i16_bak_Iw_ref;
}
//====End of "Stop_Motor_PWMOUT_OFF()"========================================================


/*-----Start_Motor_PWMOUT_ON()---------------------------------------------------------
Set EPWM port to output PWM signals 
1. Firstly set PWM outputs are masked by MASK_DATA to keep turn off MOS
2. Set initial PWM duty
3. Switch GPIO PA port as EPWM output pins
4. Disable MASK function --> PWM start output
---------------------------------------------------------------*/
void Start_Motor_PWMOUT_ON(int16_t duty0, int16_t duty2, int16_t duty4)
{
    /* Set EPWM MASK enable with PWM_OUT_OFF_MOS */
    Set_EPWM_MASK_Enable(PWM_OUT_OFF_MOS);  //Use MASK function to let PWM output to turn off MOS

    /* Set EPWM Initial Duty for 3 pairs before motor start */ //20180814 : add to load CMP and CMPU
    EPWM->CMPDAT[0] = duty0 + (duty0 << 16);  //Set duty for PWM0/1
    EPWM->CMPDAT[2] = duty2 + (duty2 << 16);  //Set duty for PWM2/3
    EPWM->CMPDAT[4] = duty4 + (duty4 << 16);  //Set duty for PWM4/5

    //--It is optional to switch PWM pins to GPIO.
    Set_GPA_As_EPWM_Output();  //Switch GPIO pins as EPWM output  //20180828

    /* Disable MASK --> PWM will be output */
    Set_EPWM_MASK_Disable();
}
//====End of "Start_Motor_PWMOUT_ON"========================================================



//==Setting About ECAP======================================================================
/*--Initialize_ECAP()---------------------------------------------- 
//For 3 hall sensor feedback: PF2~PF0 as IC2:0 to detect Hall edge
Pin:  PF0 = Hu, PF1 = Hv, PF2 = Hw
1. Use ECAP0/1/2 to detect Hu/Hv/Hw 
2. Set Noise filter
3. Set ECAP Functions: 
  3.1: if(P_CAP_60_DEGREE): Counter up counting from 0 to any edge or to until match  maximum value.
  Set both edge trigger, Compare-match mode, Clear CNT by matched or edges, compare-match reg = C_CAP_Compare_Value
  
  3.2: if(P_CAP_180_DEGREE): Counter up counting from re-load-value to Hu egde or until overflow.
  Set both edge trigger, Reload-mode, Reload CNT by CAPTF0, Reload reg = C_CAP_Reload_Value

4. Set ECAP Interrupt by : CAPTF0/1/2, CAPOVF, CAPCMPF

Note1: With "P_USE_19BIT: If counting over 19 bits to cause compare-mathed(CAPCMPF=1) or Overflow(CAPOVF=1)
Note2: Compare-match and Reload register share the same register at ECAP->CNTCMP.
-------------------------------------------------------------------*/
void Initialize_ECAP(void)
{
    /* Enable ECAP IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_ECAPCKEN_Msk;

    /* Reset IP */
    SYS->IPRST1 |= SYS_IPRST1_CAPRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_CAPRST_Msk;

    /* Enable Input Capture */
    ECAP->CTL0 |= ECAP_CTL0_CAPEN_Msk;  

    /* Enable ECAP Input Channel 0/1/2 (IC0/1/2) */
    ECAP->CTL0 |= ECAP_CTL0_IC0EN_Msk;
    ECAP->CTL0 |= ECAP_CTL0_IC1EN_Msk;
    ECAP->CTL0 |= ECAP_CTL0_IC2EN_Msk;

    /* Enable Noise Filter */
    //--Filter clock pre-div: CLK/4
    ECAP->CTL0 = (ECAP->CTL0 & (~(ECAP_CTL0_NFCLKS_Msk | ECAP_CTL0_CAPNFDIS_Msk))) |
                 (ECAP_NOISE_FILTER_CLKDIV_4 << ECAP_CTL0_NFCLKS_Pos);

    /* Select ECAP IC0/1/2 source from ECAP0/1/2 */
    ECAP->CTL0 = (ECAP->CTL0 & ~(ECAP_CTL0_CAP0SEL_Msk | ECAP_CTL0_CAP1SEL_Msk | ECAP_CTL0_CAP2SEL_Msk)) |
                 (ECAP_CAP_INPUT_SRC_ECAPX << ECAP_CTL0_CAP0SEL_Pos) |
                 (ECAP_CAP_INPUT_SRC_ECAPX << ECAP_CTL0_CAP1SEL_Pos) |
                 (ECAP_CAP_INPUT_SRC_ECAPX << ECAP_CTL0_CAP2SEL_Pos);

    /* Select IC0/1/2 detect both rising and falling edge */
    ECAP->CTL1 = (ECAP->CTL1 & ~(ECAP_CTL1_CAPEDG0_Msk | ECAP_CTL1_CAPEDG0_Msk | ECAP_CTL1_CAPEDG0_Msk)) |
                 (ECAP_RISING_FALLING_EDGE << ECAP_CTL1_CAPEDG0_Pos) |
                 (ECAP_RISING_FALLING_EDGE << ECAP_CTL1_CAPEDG1_Pos) |
                 (ECAP_RISING_FALLING_EDGE << ECAP_CTL1_CAPEDG2_Pos);

    /* Set ECAP clock source and divider */
    //--set clock source from ECAP_CLK(=HCLK)--//
    ECAP->CTL1 = (ECAP->CTL1 & ~ECAP_CTL1_CNTSRC_Msk) | (ECAP_CAPTURE_TIMER_CLK_SRC_CAP_CLK << ECAP_CTL1_CNTSRC_Pos);
    ECAP->CTL1 = (ECAP->CTL1 & ~ECAP_CTL1_CAPDIV_Msk) | (C_CAP_CTL1_CAPDIV << ECAP_CTL1_CAPDIV_Pos);


    /*------------------------------------------------------------------------------------------
    Set ECAP Functions     
    For P_CAP_60_DEGREE = 1: Use CAP counter from 0x000000 to C_CAP_Compare_Value
                Every 60-degree Hall will reset CAP counter.
    For P_CAP_180_DEGREE = 1: Ues CAP counter from C_CAP_Reload_Value to 0xFFFFFF
                Every 180-degree (Hu changed) will reload CAP counter with C_CAP_Reload_Value
    ------------------------------------------------------------------------------------------*/
    //--Set ECAP: Compare function and Enable Capture counter cleard by CAPTF0~2 or Compare-matched --//
    if(CAP_60_DEGREE == 1)  //(for CAP_60_DEGREE = 1)
    {
        ECAP->CTL0 |= ECAP_CTL0_CMPEN_Msk;  //Enable Compare function
        ECAP->CTL0 &= ~ECAP_CTL0_RLDEN_Msk;  //Disable Re-load function
        ECAP->CTL0 |= ECAP_CTL0_CPTCLR_Msk;  //Enable Counter cleared by Capture Event(CAPTF0~2)
        ECAP->CTL0 |= ECAP_CTL0_CMPCLR_Msk;  //Enable Counter cleared by Compare-matched
        /* Set initial value of registers */
        ECAP->CNTCMP = C_CAP_Compare_Value;  //Set Compare value
    }

    if(CAP_180_DEGREE == 1)  //(for CAP_180_DEGREE = 1)
    {
        ECAP->CTL0 &= ~ECAP_CTL0_CMPEN_Msk;  //Disable Compare function
        ECAP->CTL0 |= ECAP_CTL0_RLDEN_Msk;  //Enable Re-load function
        ECAP->CTL0 &= ~ECAP_CTL0_CPTCLR_Msk;  //Disable Counter cleared by Capture Event(CAPTF0~2)
        ECAP->CTL0 &= ~ECAP_CTL0_CMPCLR_Msk;  //Disable Counter cleared by Compare-matched
        /* Set Reload by IC0(CAPTF0) */
        ECAP->CTL1 = (ECAP->CTL1 & ~ECAP_CTL1_CPRLDS_Msk) | (0x0 << ECAP_CTL1_CPRLDS_Pos);  //Set Reload by IC0(CAPTF0)
        /* Set initial value of registers */
        ECAP->CNTCMP = C_CAP_Reload_Value;  //Set Reload value (Reload and Compare-reg share the same register)  
    }


    /* Reset ECAP counter and Hold registers */
    ECAP->CNT = 0;  //Clear counter at initial
    ECAP->HLD[0] = 0; ECAP->HLD[1] = 0; ECAP->HLD[2] = 0;  //Reset 3 Counter Hold Registers  


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init ECAP I/O Multi-function PF0~2 as ECAP, Input.                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPF multi-function pins GPF0 ~ GPF2 for ECAP input channel IC0 ~ IC2 */
    SYS->GPF_MFP = (SYS->GPF_MFP & ~(SYS_GPF_MFP_PF0MFP_Msk | SYS_GPF_MFP_PF1MFP_Msk | SYS_GPF_MFP_PF2MFP_Msk)) |
                    SYS_GPF_MFP_PF0_ECAP0 | SYS_GPF_MFP_PF1_ECAP1 | SYS_GPF_MFP_PF2_ECAP2;
    /* GPIO_SetMode(PF, BIT0, GPIO_MODE_INPUT); */
    PF->MODE = (PF->MODE & ~GPIO_MODE_MODE0_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);
    /* GPIO_SetMode(PF, BIT1, GPIO_MODE_INPUT); */
    PF->MODE = (PF->MODE & ~GPIO_MODE_MODE1_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE1_Pos);
    /* GPIO_SetMode(PF, BIT2, GPIO_MODE_INPUT); */
    PF->MODE = (PF->MODE & ~GPIO_MODE_MODE2_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE2_Pos);

    /* Clear All Flags at initial */
    ECAP->STS = ECAP->STS;

    /* Set ECAP Interrupt by : CAPTF0/1/2, CAPOVF, CAPCMPF */
    ECAP->CTL0 |= ECAP_CTL0_CAPTF0IEN_Msk | ECAP_CTL0_CAPTF1IEN_Msk | ECAP_CTL0_CAPTF2IEN_Msk |
                  ECAP_CTL0_CAPOVIEN_Msk | ECAP_CTL0_CAPCMPIEN_Msk;
//  NVIC_EnableIRQ(ECAP_IRQn);  //Not enable it yet

    /* Set ECAP Counter Start up-counting */
    ECAP->CTL0 |= ECAP_CTL0_CPTST_Msk;  //Start up-counting
}
//====End of Revelant to ECAP ===========================================================


//==Setting About ADC======================================================================
/*--Initialize_ADC()---------------------------------------------- 
1. Set ADC clock as 16MHz. (But ADC conversion takes about 1us with independent clock)
  Change it from 12MHz to 16MHz (20180808)
2. Set S/H time = 6 ADC_CLK
3. Enable ADC0 Interrupt (but not enable NVIC_ADC yet.)
-------------------------------------------------------------------*/
void Initialize_ADC(void)
{
    /* Enable ADC IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_ADCCKEN_Msk;  

    /* Reset IP */
    SYS->IPRST1 |= SYS_IPRST1_ADCRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_ADCRST_Msk;

    /* Enable ADC */
    ADC->CTL |= ADC_CTL_ADCEN_Msk;  

    /* Configure ADC sample time to 5 sample count */
    ADC->SMPCNT = (ADC->SMPCNT & (~ADC_SMPCNT_ADCSMPCNT_Msk)) | (5 << ADC_SMPCNT_ADCSMPCNT_Pos);

    /* Enable ADC0 INTERRUPT */
    ADC->CTL |= ADC_CTL_ADC0IEN_Msk;
//  NVIC_EnableIRQ(ADC0_IRQn);  //Not enable it yet


    //For 2R system: 
    /*Config ADC1_P1 and ADC0_P3: HARDWARE TIRGGER by EPWM0_Period and SIMUTANEOUS MODE. */
    //Set_HW_ADC_Read_IuIv_by_Simu_Mode();  //call it in doing motor system intialization

    //For 1R system: 
    /*Config ADC0_OP1_O and ADC1_OP_1: Independent 2SH Mode, Hardware Triggered by EPWM rising or falling. */
    //Set_HW_ADC_Read_OP1_O_1R_by_Indpt_Mode();  //call it in doing motor system intialization
}

/*----ADC0_SW_Read--------------------------------------------------------------------------
1. Set u32ChannelNum SOFTWARE TIRGGER to read ADC0_DATn
2. u32ChannelNum is ADC0 channel
3. Start ADC0 conversion
4. Return 12-bit ADC0_DATn
--------------------------------------------------------------------------------*/
uint16_t ADC0_SW_Read(uint32_t u32ChannelNum)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC0CSEN_Msk) | (0x1UL << u32ChannelNum << ADC_CS_CTL_ADC0CSEN_Pos);

    /* Set ADC0 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC0IF) before conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Software trigger ADC0 */
    ADC->CTL |= ADC_CTL_ADC0SWTRG_Msk;

    /* Wait ADC0 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC0IF_Msk));

    /* Clear Flag(ADC0IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC0IF_Msk;

    /* Return conversion data */
    return ADC0_GET_CONV_DATA(u32ChannelNum);;
}

/*----ADC1_SW_Read--------------------------------------------------------------------------
1. Set u32ChannelNum SOFTWARE TIRGGER to read ADC1_DATn
2. u32ChannelNum is ADC1 channel
3. Start ADC1 conversion
4. Return 12-bit ADC1_DATn
--------------------------------------------------------------------------------*/
uint16_t ADC1_SW_Read(uint32_t u32ChannelNum)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC1 channel select ADC1_P7(PC.7) */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC1CSEN_Msk) | (0x1UL << u32ChannelNum << ADC_CS_CTL_ADC1CSEN_Pos);

    /* Set ADC1 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC1IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC1IF_Msk;

    /* Software trigger ADC1 */
    ADC->CTL |= ADC_CTL_ADC1SWTRG_Msk;

    /* Wait ADC1 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC1IF_Msk));

    /* Clear Flag(ADC1IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC1IF_Msk;

    /* Return conversion data */
    return ADC1_GET_CONV_DATA(u32ChannelNum);
}


/*----SW_Trg_ADC1_P7_Return_Reslut--------------------------------------------------------------------------
1. Set ADC1_P7(PC.7) SOFTWARE TIRGGER to read VR voltage(Speed command)
2. ADC1_P7(PC.7) is at ADC1 channel 15
3. Start ADC conversion
3. Return 12-bit ADC1_DAT15
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC1_P7_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC1 channel select ADC1_P7(PC.7) */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC1CSEN_Msk) | (ADC1CS_ADC1_P7);

    /* Set ADC1 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC1IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC1IF_Msk;

    /* Software trigger ADC1 */
    ADC->CTL |= ADC_CTL_ADC1SWTRG_Msk;

    /* Wait ADC1 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC1IF_Msk));

    /* Clear Flag(ADC1IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC1IF_Msk;

    /* Return ADC1 channel 15 (ADC1_P7) conversion data */
    return (ADC->ADC1_DAT15);
}


/*----SW_Trg_ADC1_P1_Return_Reslut--------------------------------------------------------------------------
1. Set ADC1_P1(PD.2) SOFTWARE TIRGGER to manually read Iu
2. ADC1_P1(PD.2) is at ADC1 channel 1
3. Start ADC conversion
4. Return 12-bit ADC1_DAT1
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC1_P1_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC1 channel select ADC1_P1(PD.2) */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC1CSEN_Msk) | (ADC1CS_ADC1_P1);

    /* Set ADC1 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC1IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC1IF_Msk;

    /* Software trigger ADC1 */
    ADC->CTL |= ADC_CTL_ADC1SWTRG_Msk;

    /* Wait ADC1 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC1IF_Msk));

    /* Clear Flag(ADC1IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC1IF_Msk;

    /* Return ADC1 channel 15 (ADC1_P1) conversion data */
    return (ADC->ADC1_DAT1);
}


/*----SW_Trg_ADC0_P3_Return_Reslut--------------------------------------------------------------------------
1. Set ADC0_P3(PC.0) SOFTWARE TIRGGER to manually read Iv
2. ADC0_P3(PC.0) is at ADC0 channel 3
3. Start ADC conversion
3. Return 12-bit ADC0_DAT3
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC0_P3_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_P3(PC.0) */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC0CSEN_Msk) | (ADC0CS_ADC0_P3);

    /* Set ADC0 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC0IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Software trigger ADC0 */
    ADC->CTL |= ADC_CTL_ADC0SWTRG_Msk;

    /* Wait ADC0 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC0IF_Msk));

    /* Clear Flag(ADC0IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC0IF_Msk;

    /* Return ADC0 channel 15 (ADC0_P3) conversion data */
    return (ADC->ADC0_DAT3);
}


/*----SW_Trg_ADC0_P4_Return_Reslut--------------------------------------------------------------------------
1. Use ADC0_P4(PC.1) to SOFTWARE TIRGGER to manually read I_BUS
2. ADC0_P4 is at ADC0 channel 4
3. Start ADC conversion
4. Return 12-bit ADC0_DAT4
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC0_P4_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_P4(PC.1) */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC0CSEN_Msk) | (ADC0CS_ADC0_P4);

    /* Set ADC0 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC0IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Software trigger ADC0 */
    ADC->CTL |= ADC_CTL_ADC0SWTRG_Msk;

    /* Wait ADC0 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC0IF_Msk));

    /* Clear Flag(ADC0IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC0IF_Msk;

    /* Return ADC0 channel 4 (ADC0_P4) conversion data */
    return (ADC->ADC0_DAT4);
}


/*----SW_Trg_ADC1_TempSensor_Return_Reslut--------------------------------------------------------------------------
1. Set ADC1_TEMP_SENSOR SOFTWARE TIRGGER to read Temperature Sensor voltage
2. ADC1_TEMP_SENSOR is at ADC1 channel 6
3. Start ADC conversion
4. Return 12-bit ADC1_DAT6
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC1_TempSensor_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC1 channel select ADC1_TEMP_SENSOR */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC1CSEN_Msk) | (ADC1CS_TEMP_SENSOR);

    /* Set ADC1 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC1IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC1IF_Msk;

    /* Software trigger ADC1 */
    ADC->CTL |= ADC_CTL_ADC1SWTRG_Msk;

    /* Wait ADC1 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC1IF_Msk));

    /* Clear Flag(ADC1IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC1IF_Msk;

    /* Return ADC1 channel 6 (TEMP_SENSOR) conversion data */
    return (ADC->ADC1_DAT6);
}


/*----SW_Trg_ADC0_BandGap_Return_Reslut--------------------------------------------------------------------------
1. Set ADC0_BAND_GAP SOFTWARE TIRGGER to read Band_Gap voltage
2. ADC0_BAND_GAP is at ADC0 channel 6
3. Start ADC conversion
4. Return 12-bit ADC0_DAT6
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC0_BandGap_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_BAND_GAP */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC0CSEN_Msk) | (ADC0CS_BAND_GAP);

    /* Set ADC0 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC0IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Software trigger ADC0 */
    ADC->CTL |= ADC_CTL_ADC0SWTRG_Msk;

    /* Wait ADC0 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC0IF_Msk));

    /* Clear Flag(ADC0IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC0IF_Msk;

    /* Return ADC0 channel 6 (ADC0_BAND_GAP) conversion data */
    return (ADC->ADC0_DAT6);
}





//20180808
/*----SW_Trg_ADC0_OP1_O_Return_Reslut--------------------------------------------------------------------------
1. Set ADC0_OP1_O SOFTWARE TIRGGER to manually read OP1_O from one shunt R
2. ADC0_OP1_O is at ADC0 channel 9
3. Start ADC conversion
4. Return 12-bit ADC0_DAT9
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC0_OP1_O_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_OP1_O */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC0CSEN_Msk) | (ADC0CS_OP1_O);

    /* Set ADC0 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC0IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC0IF_Msk;

    /* Software trigger ADC0 */
    ADC->CTL |= ADC_CTL_ADC0SWTRG_Msk;

    /* Wait ADC0 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC0IF_Msk));

    /* Clear Flag(ADC0IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC0IF_Msk;

    /* Return ADC0 channel 9 (ADC0_OP1_O) conversion data */
    return (ADC->ADC0_DAT9);
}


//20180808
/*----SW_Trg_ADC1_OP1_O_Return_Reslut--------------------------------------------------------------------------
1. Set ADC1_OP1_O SOFTWARE TIRGGER to manually read OP1_O from one shunt R
2. ADC1_OP1_O is at ADC0 channel 9
2. Start ADC conversion
3. Return 12-bit ADC1_DAT9
--------------------------------------------------------------------------------*/
uint16_t SW_Trg_ADC1_OP1_O_Return_Reslut(void)
{
    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC1 channel select ADC1_OP1_O */
    ADC->CS_CTL = (ADC->CS_CTL & ~ADC_CS_CTL_ADC1CSEN_Msk) | (ADC1CS_OP1_O);

    /* Set ADC1 to software trigger */
    ADC->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;

    /*-----------------------------------------------------------------------------
    / Software start procedure
    ------------------------------------------------------------------------------*/
    /* Clear Flag(ADC1IF) befpore conversion*/
    ADC->STATUS = ADC_STATUS_ADC1IF_Msk;

    /* Software trigger ADC1 */
    ADC->CTL |= ADC_CTL_ADC1SWTRG_Msk;

    /* Wait ADC1 completed by polling */
    while (!(ADC->STATUS & ADC_STATUS_ADC1IF_Msk));

    /* Clear Flag(ADC1IF) after conversion*/
    ADC->STATUS |= ADC_STATUS_ADC1IF_Msk;

    /* Return ADC1 channel 9 (ADC1_OP1_O) conversion data */
    return (ADC->ADC1_DAT9);
}

/*---Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period()----------------------------------------------
For 2R use(2 Shunt)
Config ADC1_P1 and ADC0_P3: HARDWARE TIRGGER by EPWM0_Period and SIMUlTANEOUS MODE.

Set ADC for 2R two current measurement.
  Iu from PD.2=ADC1_P1
  Iv from PC.0=ADC0_P3
--------------------------------------------------------------------------------*/
void Set_ADC1_P1_ADC0_P3_Simu_HW_Trg_by_PWM0Period(void)
{
    /* Check and waiting if ADC is busy before setting ADCMODE as HW trigger */
    while(ADC->STATUS & (ADC_STATUS_ADC0BUSY_Msk | ADC_STATUS_ADC1BUSY_Msk));
  
    /* Configure ADC conversion mode to SIMUTANEOUS MODE */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x2 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_P3, ADC1 channel select ADC1_P1 */
    ADC->CS_CTL = (ADC->CS_CTL & ~(ADC_CS_CTL_ADC0CSEN_Msk | ADC_CS_CTL_ADC1CSEN_Msk)) | (ADC1CS_ADC1_P1 | ADC0CS_ADC0_P3);

    /* Set ADC0 and ADC1 to hardware trigger */
    ADC->CTL |= ADC_CTL_ADC0HWTRGEN_Msk | ADC_CTL_ADC1HWTRGEN_Msk;

    /* Set Triggered by PWM0_PERIOD */
    ADC->TRGSOR = (ADC->TRGSOR & ~(ADC_TRGSOR_ADC0TRGSOR_Msk | ADC_TRGSOR_ADC0STADCSEL_Msk | ADC_TRGSOR_ADC0PWMTRGSEL_Msk)) |
                  (ADC0_EPWM0_PERIOD_TRIGGER);
    
    /* For Simultaneous mode, it does not need to set ADC1 hardware trigger source. */
//  ADC->TRGSOR = (ADC->TRGSOR & ~(ADC_TRGSOR_ADC1TRGSOR_Msk | ADC_TRGSOR_ADC1STADCSEL_Msk | ADC_TRGSOR_ADC1PWMTRGSEL_Msk)) |
//                (ADC1_EPWM0_PERIOD_TRIGGER);
}


/*---Set_ADC0_OP1_O_ADC1_OP1_O_Independent_2SH_Trg_by_Hardware()-----------------------
For 1R use(1 Shunt) : Set ADC to measure phase current from OP1 Output.
Config ADC0_OP1_O and ADC1_OP1_O: Independent Mode, Hardware Triggered by EPWM0 Period.
--------------------------------------------------------------------------------*/
void Set_ADC0_OP1_O_ADC1_OP1_O_Independent_2SH_Trg_by_Hardware(void)
{
    /* Check and waiting if ADC is busy before setting ADCMODE as HW trigger */
    while(ADC->STATUS & (ADC_STATUS_ADC0BUSY_Msk | ADC_STATUS_ADC1BUSY_Msk));

    /* Configure ADC conversion mode to Independent Mode */
    ADC->CTL = (ADC->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos);

    /* ADC0 channel select ADC0_OP1_O, ADC1 channel select ADC1_OP1_O */
    ADC->CS_CTL = (ADC->CS_CTL & ~(ADC_CS_CTL_ADC0CSEN_Msk | ADC_CS_CTL_ADC1CSEN_Msk)) | (ADC0CS_OP1_O | ADC1CS_OP1_O);

    /* Set ADC0 and ADC1 to hardware trigger */
    ADC->CTL |= ADC_CTL_ADC0HWTRGEN_Msk | ADC_CTL_ADC1HWTRGEN_Msk;

    /* Trigger Delay Setting  */ //Delay time = (4*C_ADC_DELAY_TIME)*(1/60) us.
    ADC->TRGDLY = ((C_ADC_DELAY_TIME << ADC_TRGDLY_ADC0DELAY_Pos) | (C_ADC_DELAY_TIME << ADC_TRGDLY_ADC1DELAY_Pos));

    /* In system program, the trigger source will be dynamically changed from PWM0/2/4 falling/rising
    1. Clear ADC0/1 Trigger Source and PWM Triger (ADC0TRGSOR=0, ADC0PWMTRGSEL=0)
    2. ADC0_OP1_O and ADC1_OP1_O are triggerred by which PWM falling edges: Depends on ZONE in FOC */
    ADC->TRGSOR &= ~(ADC_TRGSOR_ADC1TRGSOR_Msk | ADC_TRGSOR_ADC1PWMTRGSEL_Msk | ADC_TRGSOR_ADC0TRGSOR_Msk | ADC_TRGSOR_ADC0PWMTRGSEL_Msk);

    switch(MotorA.cmd.u8_zone)  //Set ADC0_OP1_O first, then ADC1_OP1_O
    {
        case 5:     //L M S = V W U, -Iu, Iv
            ADC->TRGSOR |= ADC0_EPWM0_FALLING_TRIGGER | ADC1_EPWM4_FALLING_TRIGGER;
        break;
    
        case 4:     //L M S = W V U, -Iu, Iw
            ADC->TRGSOR |= ADC0_EPWM0_FALLING_TRIGGER | ADC1_EPWM2_FALLING_TRIGGER;
        break;

        case 6:     //L M S = W U V, -Iv, Iw
            ADC->TRGSOR |= ADC0_EPWM2_FALLING_TRIGGER | ADC1_EPWM0_FALLING_TRIGGER;
        break;

        case 2:     //L M S = U W V, -Iv, Iu
            ADC->TRGSOR |= ADC0_EPWM2_FALLING_TRIGGER | ADC1_EPWM4_FALLING_TRIGGER;
        break;
    
        case 3:     //L M S = U V W, -Iw, Iu
            ADC->TRGSOR |= ADC0_EPWM4_FALLING_TRIGGER | ADC1_EPWM2_FALLING_TRIGGER;
        break;

        case 1:     //L M S = V U W, -Iw, Iv
            ADC->TRGSOR |= ADC0_EPWM4_FALLING_TRIGGER | ADC1_EPWM0_FALLING_TRIGGER;
        break;

        default:   //2. Temporally set ADC0/1 Trigger Source from EPWM0/2 Falling Edge
            ADC->TRGSOR |= ADC0_EPWM0_FALLING_TRIGGER | ADC1_EPWM2_FALLING_TRIGGER;
        break;
    }
}


/*---Set_GPIO_as_ADC_Input()--------------------------------------------------------
/ Base on this demo system, config the following GPIO as ADC Input
  Set GPD2=ADC1_P1, GPC0=ADC0_P3 for Iu and Iv
  Set GPC1=ADC0_P4 for I_BUS
  Set GPC7=ADC1_P7 for Speed(Variable resistor)
--------------------------------------------------------------------------------*/
void Set_GPIO_as_ADC_Input(void)
{
    /* Set GPD2=ADC1_P1, GPC0=ADC0_P3, GPE2=ADC1_OP1_O, GPC7=ADC1_P7 */
    SYS->GPD_MFP = (SYS->GPD_MFP & (~SYS_GPD_MFP_PD2MFP_Msk)) | SYS_GPD_MFP_PD2_ADC1_P1;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC0MFP_Msk)) | SYS_GPC_MFP_PC0_ADC0_P3;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC1MFP_Msk)) | SYS_GPC_MFP_PC1_ADC0_P4;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC7MFP_Msk)) | SYS_GPC_MFP_PC7_ADC1_P7;
  
    /* Set GPIO as Input and Digital-Off mode */
    //---Set PD.2(ADC1_P1) & PC.0(ADC0_P3) as input and digital-off */
    PD->MODE = (PD->MODE & ~GPIO_MODE_MODE2_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE2_Pos);
    PD->DINOFF |= GPIO_DINOFF_DINOFF2_Msk;
    
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE0_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF0_Msk;

    //---Set PC.1(ADC0_P4) as input and digital-off */
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE1_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE1_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF1_Msk;

    //---Set PC.7(ADC1_P7) as input and digital-off */
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE7_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE7_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF7_Msk;
}
//======End of Setting Revelant to ADC=========================================================================



//==Setting About OP======================================================================
/*---Initialize_OP1()--------------------------------------------------------
/ Base on this demo system, config PE.2 as OP1_O, PE.3 as OP1_N and PE.4 as OP1_P
PE.2=OP1_O
PE.3=OP1_N
PE.4=OP1_P
--------------------------------------------------------------------------------*/
void Initialize_OP1(void)
{
    /* Enable IP clock */
    /* CLK_EnableModuleClock(OP_MODULE); */
    CLK->APBCLK |= CLK_APBCLK_OPCKEN_Msk;

    /* SYS_ResetModule(OP_RST); */
    SYS->IPRST1 |= SYS_IPRST1_OPRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_OPRST_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Initiate I/O Multi-function                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/
    /* The analog input port pins must be configured as input type before the OP function is enabled. */
    /* GPIO_SetMode(PE, BIT2, GPIO_MODE_OUTPUT); */
    /* GPIO_SetMode(PE, BIT3 | BIT4, GPIO_MODE_INPUT); */
    PE->MODE = (PE->MODE & ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk | GPIO_MODE_MODE4_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos) | (GPIO_MODE_INPUT << GPIO_MODE_MODE3_Pos) | (GPIO_MODE_INPUT << GPIO_MODE_MODE4_Pos);

    /* Set GPE2, GPE3, GPE4 multi-function pins for OP1_O, OP1_N, OP1_P */
    SYS->GPE_MFP = (SYS->GPE_MFP & ~(SYS_GPE_MFP_PE2MFP_Msk | SYS_GPE_MFP_PE3MFP_Msk | SYS_GPE_MFP_PE4MFP_Msk)) |
                   (SYS_GPE_MFP_PE2_OP1_O | SYS_GPE_MFP_PE3_OP1_N | SYS_GPE_MFP_PE4_OP1_P);

    /* Disable digital input path of analog pin to prevent leakage */
    PE->DINOFF |= GPIO_DINOFF_DINOFF2_Msk | GPIO_DINOFF_DINOFF3_Msk | GPIO_DINOFF_DINOFF4_Msk;

    /*---------------------------------------------------------------------------------------------------------*/
    /*  Eanble OP1                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/
    OP->CTL |= OP_CTL_OP1EN_Msk;
}
//====End of Setting Revelant to OP1=====================================================================================

//==Setting About ACMP======================================================================
/*---Initialize_ACMP0()--------------------------------------------------------
//Config ACMP 
//Base on this demo system, use OP1_O to detect I_OC 
 * Select CPPSEL=ACMP_CTL_POSSEL_OP1_O
 * Select CPNSEL=DAC0
 * ACMPHYSEN = 10b = Vhys_90mV
 * Enable filter and NFCLKS=10=PCLK/4
 * Polarity = 0 = normal polarity
 * PBRKSEL = 0 = ACMP result direct ouput to PWM Brake0
 * EDGESEL = 10b = Rising trigger Interrupt
 * PRESET = 0 = Preset ACMP_output Low
--------------------------------------------------------------------------------*/
void Initialize_ACMP0(uint16_t DAC0_value)
{
    /* Enable IP clock */
    /* CLK_EnableModuleClock(ACMP_MODULE); */
    CLK->APBCLK |= CLK_APBCLK_ACMPCKEN_Msk;

    /* SYS_ResetModule(ACMP_RST); */
    SYS->IPRST1 |= SYS_IPRST1_ACMPRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_ACMPRST_Msk;


    /*---------------------------------------------------------------------------------------------------------*/
    /*  Config ACMP0                                                                                  */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PRESET, CPPSEL, CPNSEL, NFCK, ACMPHYSEN*/
    ACMP->CTL[0] |= ACMP_CTL_POSSEL_OP1_O;   //Set CPPSEL=OP1_O
    ACMP->CTL[0] |= ACMP_CTL_NEGSEL_DAC0;   //Set CPNSEL=DAC0
    ACMP->CTL[0] |= ACMP_CTL_NFCLKS_PCLKDIV_4;  //Set noise filter: NFCLKS=PCLK/4
    ACMP->CTL[0] |= ACMP_CTL_HYSTERESIS_90mV;   //Set HYSTERESIS=90mV
    ACMP->CTL[0] &= ~ACMP_CTL_PRESET_Msk;  //Preset ACMP0 Output = Low
    ACMP->CTL[0] |= ACMP_CTL_ACMPEN_Msk;   //Enable ACMP0
    ACMP->CTL[0] &= ~ACMP_CTL_POLARITY_Msk;  //Set ACMP0_O normal polarity to PWMBRK
//  CLEAR_EPWM_BK_RESTART_CNT();      //Due to poloarity setting may trigger EPWM Brake, so had better add this command line
    ACMP->CTL[0] &= ~ACMP_CTL_PBRKSEL_Msk;  //Set ACMP0_O direct output to PWMBRK

    /* Set ACMP reference voltage as DAC0*/
    //DAC0 = AVDD x DAC0[11:0]/4096.
    ACMP->DACVAL = (ACMP->DACVAL & ~ACMP_DACVAL_DAC0_Msk) | (DAC0_value << ACMP_DACVAL_DAC0_Pos);

    /*-- Due to poloarity setting may trigger EPWM Brake, so had better add this command line after set ACMP --*/
    CLEAR_EPWM_BK_RESTART_CNT();
}
//====End of Setting Revelant to ACMP=====================================================================================



//==Setting About Timer0======================================================================
/*---Initialize_Timer0()--------------------------------------------------------
Initialize timer0
Set timer0 to product a 1ms periodic interrupt
--------------------------------------------------------------------------------*/
void Initialize_Timer0(void)
{
    /* Enable IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_TMR0CKEN_Msk;

    /* Select IP clock source */
    CLK->CLKSEL1 = CLK->CLKSEL1 | CLK_TMR0_SRC_HIRC;
  
    /* Reset Timer0 IP */
    SYS->IPRST1 |= SYS_IPRST1_TMR0RST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_TMR0RST_Msk;


    /*---------------------------------------------------------------------------------------------------------
      Config Timer0                                                                                
    Time-out period = (Period of Timer clock source) * (8-bit PSC + 1) * (24-bit CMPDAT).
    ---------------------------------------------------------------------------------------------------------*/
    /* Set timer frequency to 1kHZ */
    TIMER0->CTL = (TIMER0->CTL & ~TIMER_CTL_PSC_Msk) | (3 << TIMER_CTL_PSC_Pos); //Prescale=3+1=4
    TIMER0->CMP = (SYSTEM_CLOCK/(4*1000));  //Set 24-bit CMPDAT value=60M/((3+1)*1000)=18000
    TIMER0->CTL = ((TIMER0->CTL & ~TIMER_CTL_OPMODE_Msk) | TIMER_PERIODIC_MODE); //Set Timer0 in Periodic mode
    
    /* Enable timer0 interrupt */
    TIMER0->CTL = TIMER0->CTL | TIMER_CTL_INTEN_Msk;
//  NVIC_EnableIRQ(TMR0_IRQn);    //Not enable yet.

    /* Start Timer 0 */
    TIMER0->CTL = TIMER0->CTL | TIMER_CTL_CNTEN_Msk;
  
    /* Clear flags of TIF */
    TIMER0->EINTSTS = TIMER0->EINTSTS | TIMER_INTSTS_TIF_Msk;
}
//====End of Setting Revelant to Timer0=====================================================================================


/*--Initialize_USCI1_SPI1_for_DAC()-------------------------------------------
Set USCI1_SPI1 as Master mode for DAC module
Pin:
  PE5 = SPI1_CLK; PE7 = SPI0_MOSI; PD6 = I/O for CS

Note:
 The USCI usage is exclusive
 If user configure the USCI port as USPI function, that port cannot use UUART or UI2C function. 
-------------------------------------------------------------------*/
void Initialize_USCI1_SPI1_for_DAC(void)
{
    /* Enable USCI0 IP clock */
    CLK->APBCLK = CLK->APBCLK | CLK_APBCLK_USCI1CKEN_Msk;  

    /* SYS_ResetModule(SYS_IPRST1_USCI0RST_Msk); */
    SYS->IPRST1 |= SYS_IPRST1_USCI1RST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_USCI1RST_Msk;

    /*---------------------------------------------------------------------------------------------------------
    Config USCI1_SPI                                                                                
    Master, MSB first, clock idle low, 16-bit transaction, drive output on falling clock edge 
    and latch input on rising edge.
  ---------------------------------------------------------------------------------------------------------*/
    /* Select USCI_SPI0 protocol */
    USPI1->CTL &= ~USPI_CTL_FUNMODE_Msk;
    USPI1->CTL = 1 << USPI_CTL_FUNMODE_Pos;      //0=Disable, 1=SPI, 2=UART, 4=I2C

    /*USPI_MODE_1: SCLK idle low; data transmit with rising edge and receive with falling edge */
    USPI1->PROTCTL &= USPI_MASTER;    //Write USPI_MASTER(bit0)=0 to select master mode of SPI0
    USPI1->PROTCTL |= USPI_MODE_1;    //Set SPI0: data transmit with rising edge and receive with falling edge

    /* Set USCI_SPI0 clock rate = f_PCLK / 2*(1+1) = 10 MHz */
    USPI1->BRGEN = (USPI1->BRGEN & (~USPI_BRGEN_CLKDIV_Msk)) | (0x02 << USPI_BRGEN_CLKDIV_Pos);
 
    /* Configure USCI_SPI_SS pin as low-active, Data output not inversed, MSB first */
    USPI1->LINECTL = (USPI1->LINECTL & (~USPI_LINECTL_CTLOINV_Msk) & (~USPI_LINECTL_DATOINV_Msk));// | USPI_SS_ACTIVE_LOW;
    USPI1->LINECTL &= ~USPI_LINECTL_LSB_Msk;  //Set bit0=0 to select MSB first
    
    /*Not enable SS(Slave Select) control function (use I/O to active DAC module) */
    USPI1->PROTCTL &= ~USPI_PROTCTL_AUTOSS_Msk;  
  
    /* Enable USCI_SPI1 protocol */
    USPI1->PROTCTL |= USPI_PROTCTL_PROTEN_Msk;  //Enable SPI Protocol
  
    /* Set GPD multi-function pins for PE5 = SPI1_CLK; PE7 = SPI1_MOSI; PD6 = I/O for CS */
    SYS->GPE_MFP = (SYS->GPE_MFP & ~(SYS_GPE_MFP_PE5MFP_Msk)) | SYS_GPE_MFP_PE5_SPI1_CLK;
    SYS->GPE_MFP = (SYS->GPE_MFP & ~(SYS_GPE_MFP_PE7MFP_Msk)) | SYS_GPE_MFP_PE7_SPI1_MOSI;
    SYS->GPD_MFP = (SYS->GPD_MFP & ~(SYS_GPD_MFP_PD6MFP_Msk)) | SYS_GPD_MFP_PD6_GPIO;
  
    /* Set GPIO pin as SPI function PIN */
    /* Set PE5=SPI1_CLK=Ouput mode, PE7=SPI1_MOSI=Output mode, PD6=Output mode. */
    //Consider to use "GPIO_MODE_OUTPUT"
    PE->MODE = (PE->MODE & ~(GPIO_MODE_MODE5_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE5_Pos);
    PE->MODE = (PE->MODE & ~(GPIO_MODE_MODE7_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE7_Pos);
    PD->MODE = (PD->MODE & ~(GPIO_MODE_MODE6_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE6_Pos);


    /* Enable TX end interrupt */
    USPI1->INTEN |= USPI_INTEN_TXENDIEN_Msk;
//  NVIC_EnableIRQ(USCI1_IRQn);      //Not enable yet
}
//==End of "Initial_USCI1_SPI_for_DAC"=======================================



//==Setting About GPIO for the demo system=========================================
/*---Initialize_GPIO()--------------------------------------------------------
Initialize GPIO
PA6=Input: for reading switch "RUN" or testpin1 
PF4=Output: for controling LED output

Set GPIO to be ADC input for the demo system
  Set GPD2=ADC1_P1, GPC0=ADC0_P3 for Iu and Iv
  Set GPC1=ADC0_P4 for I_BUS
  Set GPC7=ADC1_P7 for Speed(Variable resistor)
--------------------------------------------------------------------------------*/
void Initialize_GPIO(void)
{
    /* PA6=Input: for reading switch "RUN" or testpin */
    PA->MODE = (PA->MODE & ~(GPIO_MODE_MODE6_Msk)) | (GPIO_MODE_INPUT << GPIO_MODE_MODE6_Pos);
    /* PF4=Output: for controling LED2 output */
    PF->MODE = (PF->MODE & ~(GPIO_MODE_MODE4_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE4_Pos);
    /* PF3=Output: for controling LED2 output */
    PF->MODE = (PF->MODE & ~(GPIO_MODE_MODE3_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos);
    
    /* PE1=Output: for controling LED1 output */
    PE->MODE = (PE->MODE & ~(GPIO_MODE_MODE1_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos);

    /*   Set GPC0=ADC0_CH3, GPB4=EACD1_CH0 for Iu and Iv
    Set GPC2=ADC1_CH2 for I_BUS
    Set GPC5=ADC1_CH5 for Speed(Variable resistor)    */  
    Set_GPIO_as_ADC_Input();
}
//====End of Setting About GPIO for the demo system=====================================================================================



//==Setting About Hardware Dividor=========================================
/*---Initialize_HDIV()--------------------------------------------------------
Enable Hardware Dividor
--------------------------------------------------------------------------------*/
void Initialize_HDIV(void)
{
    /* Enable IP clock */
    /* CLK_EnableModuleClock(HDIV_MODULE); */
    CLK->AHBCLK |= CLK_AHBCLK_HDIVCKEN_Msk;

    /* Example for usage */
//  int32_t quo;
//  int16_t rem;  
//  quo = HDIV_DIV(-1000,10);    //quo will be -100
//  rem = HDIV_MOD(10001,10);    //rem will be 1
}
//====End of Setting About HDIV=====================================================================================


//==Setting About Temperature Sensor=========================================
/*---Initialize_Temperature_Sensor()--------------------------------------------------------
Enable Temperature sensor
--------------------------------------------------------------------------------*/
void Initialize_Temperature_Sensor(void)
{
    /* Enable Temperature sensor power */
    SYS->IVSCTL |= SYS_IVSCTL_VTEMPEN_Msk;
}
//====End of Setting About Temperature Sensor=====================================================================================





//==Setting About Interrupt(NVIC)=========================================
/*---Initialize_NVIC_Priority()--------------------------------------------------------
Set Interrupt Priority (Lower level number with higher priority)
Level 0: Not use yet.
Level 1: BRAKE0 Interrupt for Brake condition process. (Highest)
Level 1: ADC Interrupt for FOC loop
Level 1: GDMA0 Interrupt for GDMA receiving command from PC
Level 2: ECAP Interrupt  for Hall sensor 
Level 3: Timer0 Interrupt for user interface, speed control loop

--------------------------------------------------------------------------------*/
void Initialize_NVIC_Priority(void)
{
    NVIC_SetPriority(BRAKE0_IRQn, 1);  //with higher nature priority than ADC0
    NVIC_SetPriority(ADC0_IRQn,   1);
    NVIC_SetPriority(GDMA0_IRQn,  1);
    NVIC_SetPriority(ECAP_IRQn,   2);
    NVIC_SetPriority(TMR0_IRQn,   3);
}


/*---Enable_NVIC_For_System_Requirement()-------------------------------------------
Enable the NVIC for the system requirement
  NVIC_EnableIRQ(ADC0_IRQn);    //for Iu and Iv detection --> FOC current control
  NVIC_EnableIRQ(BRAKE0_IRQn);    //for EPWM Brake Interrrupt
  NVIC_EnableIRQ(ECAP_IRQn);    //for Hall signal detection --> HallSpeedEstimator()
  NVIC_EnableIRQ(TMR0_IRQn);    //for System basic periodic routine
--------------------------------------------------------------------------------*/
void Enable_NVIC_For_System_Requirement(void)
{
    NVIC->ICPR[0U] = 0xFFFFFFFF;   //Clear all pending interrupt.
    /*-- Enable NVIC ---*/    
    NVIC_EnableIRQ(ADC0_IRQn);    //for Iu and Iv detection --> FOC current control
    NVIC_EnableIRQ(BRAKE0_IRQn);  //for EPWM Brake Interrrupt
    NVIC_EnableIRQ(ECAP_IRQn);    //for Hall signal detection --> HallSpeedEstimator()
    NVIC_EnableIRQ(TMR0_IRQn);    //for System basic periodic routine
    if((u8_flag_VSP_control == 0) && (u8_flag_UART_control == 1))
        NVIC_EnableIRQ(GDMA0_IRQn);   //for GDMA receiving command from PC
}
//====End of Setting Revelant to NVIC setting====================================================================================





//20180817
//==Setting About Motor System=========================================
/*---Initialize_Motor_System()--------------------------------------------------------
For 2R FOC:
Check some signals depends on system need
For example: DC bus voltage, environment temperature and others. (Not implemented in this demo system)
1. Initialize the variables used in the demo system
2. Do zero current calibration:
  To get the bias value(i16_Iu_ref_ADC, i16_Iv_ref_ADC) of ADC at zero current.
3. Enable PWM automatically trigger ADC to read Iu & Iv and Enable ADC Interrupt
  The running PWM will continually trigger ADC to detect Iu and Iv and request ADC Interrupt.
4. Enable ECAP Interrupt: 
  If Hall signals have change the ECAP interrupt will be triggered and call HallSpeedEstimator().
--------------------------------------------------------------------------------*/
void Initialize_Motor_System(void)
{
    Initialize_Variabls();  //Initialize the variable value for the system        
  
    Iu_Iv_Zero_Current_Calibration();  //ADC Zero current calibration to get i16_Iu_ref_ADC

    Check_control_mode();  //Check VSP or UART mode

    /*--Enable PWM automatically trigger ADC to read Iu & Iv and Enable ADC Interrupt--*/
    Set_HW_ADC_Read_IuIv_by_Simu_Mode();  //Set ADC to read Iu & Iv by simultaneous mode triggered by PWM timing

    Initialize_NVIC_Priority();  //Set NVIC priority for this demo system
    
    Enable_NVIC_For_System_Requirement();  //Enable the needed NVIC
  
    Initialize_varaibles_for_TMR0_int();  //Initialize some variabls used in TMR0 Interrupt
  
    Detect_Initial_Hall_Position_M0();  //Detect Initial hall position
    
    Load_from_data_flash();  //Check whether the data flash is empty.
}



/*---Iu_Iv_Zero_Current_Calibration()-------------------------------------------
1. Before function is executed, the system must be in power-on and motor is ceased, 
  the Iu and Iv should be 0A.
2. Read the values of Iu_0A and Iv_0A as the initial zero current biasd value.
3. If got an unreasonable value --> Do error process. 
--------------------------------------------------------------------------------*/
void Iu_Iv_Zero_Current_Calibration(void)
{
    unsigned char i;
    unsigned int ADCA_0_current_12bit;
    unsigned int ADCB_0_current_12bit;

    ADCA_0_current_12bit = 0;
    ADCB_0_current_12bit = 0;

    /*--Read 32 times and average it. ---*/
    for(i=0;i<32;i++)  
    {
        ADCA_0_current_12bit = ADCA_0_current_12bit + SW_Read_Iu_by_ADC();
        ADCB_0_current_12bit = ADCB_0_current_12bit + SW_Read_Iv_by_ADC();  
    }

    ADCA_0_current_12bit = ADCA_0_current_12bit >> 5;
    ADCB_0_current_12bit = ADCB_0_current_12bit >> 5;

    MotorA.info.i16_Iu_ref_ADC = ADCA_0_current_12bit;
    MotorA.info.i16_Iv_ref_ADC = ADCB_0_current_12bit;

    /*--Check if the value is reasonable or not ---*/
    if(MotorA.info.i16_Iu_ref_ADC >= (C_Zero_I_ADC + C_Zero_I_Band))
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
    else if(MotorA.info.i16_Iu_ref_ADC <= (C_Zero_I_ADC - C_Zero_I_Band))
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
   
    if(MotorA.info.i16_Iv_ref_ADC >= (C_Zero_I_ADC + C_Zero_I_Band))
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
    else if(MotorA.info.i16_Iv_ref_ADC <= (C_Zero_I_ADC - C_Zero_I_Band))
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843

    /*-- If with error then do something to protect the system --*/
    if(MotorA.sym.u8_zero_current_error == 1)
    {
        Stop_Motor_PWMOUT_OFF();  //If got error, stop motor.
    
        while(MotorA.sym.u8_zero_current_error == 1);
    }

    /*-- Backup the 3 phase reference data value --*/
    i16_bak_Iu_ref = MotorA.info.i16_Iu_ref_ADC;
    i16_bak_Iv_ref = MotorA.info.i16_Iv_ref_ADC;
    i16_bak_Iw_ref = MotorA.info.i16_Iw_ref_ADC;
}
//====End of Setting About Motor System====================================================================================





/*---Initialize_varaibles_for_TMR0_int()--------------------------------------------------------

--------------------------------------------------------------------------------*/
void Initialize_varaibles_for_TMR0_int(void)
{
    u8_flag_1ms = 0;
    u8_flag_10ms = 0;
    u8_flag_100ms = 0;
    u8_flag_500ms = 0;

    u16_timer0_int_counter = 0;
    u16_1ms_timer_counter = 0;
    u16_10ms_time_counter = 0;
    u16_100ms_time_counter = 0;
    u16_500ms_time_counter = 0;
}


/*---Initialize_Motor_System_1R()--------------------------------------------------------
For 1R FOC: 
Check some signals depends on system need
For example: DC bus voltage, environment temperature and others. (Not implemented in this demo system)
1. Initialize the variables used in the demo system
2. Do zero current calibration:
  To get the bias value(i16_Iu_ref_ADC, i16_Iv_ref_ADC) of ADC at zero current.
3. Enable PWM automatically trigger ADC to read Iu & Iv and Enable ADC Interrupt
  The running PWM will continually trigger ADC to detect Iu and Iv and request ADC Interrupt.
4. Enable ECAP Interrupt: 
  If Hall signals have change the ECAP interrupt will be triggered and call HallSpeedEstimator().
--------------------------------------------------------------------------------*/
void Initialize_Motor_System_1R(void)
{
    Initialize_Variabls();  //Initialize the variable value for the system        
  
    OP1_O_Zero_Current_Calibration();  //ADC Zero current calibration to get i16_Iu_ref_ADC
    
    Check_control_mode();  //Check VSP or UART mode
  
    /*--Enable PWM two falling edges trigger ADC to read PGAO and NOT enable ADC INTERRUPT--*/
    Set_HW_ADC_Read_OP1_O_1R_by_Indpt_Mode();  //Set ADC to read 2 of Iu/v/w by independent mode triggered by PWM falling edge

    Initialize_NVIC_Priority_1R();  //Set NVIC priority for this demo system
    
    Enable_NVIC_For_System_Requirement_1R();  //Enable the needed NVIC
  
    Initialize_varaibles_for_TMR0_int();  //Initialize some variabls used in TMR0 Interrupt
  
    Detect_Initial_Hall_Position_M0();  //Detect Initial hall position
    
    Load_from_data_flash();
}


/*---OP1_O_Zero_Current_Calibration()-------------------------------------------
1. Before function is executed, the system must be in power-on and motor is ceased,
  the motor current should be in zero amp.
2. Read the values of OP1_O_0A as the initial zero current biasd value.
3. If got an unreasonable value --> Do error process. 
--------------------------------------------------------------------------------*/
void OP1_O_Zero_Current_Calibration(void)
{
    unsigned char i;
    unsigned int ADC0_current_12bit;
    unsigned int ADC1_current_12bit;

    ADC0_current_12bit = 0;
    ADC1_current_12bit = 0;

    /*--Read 32 times and average it. ---*/
    for(i=0;i<32;i++)  
    {
        ADC0_current_12bit = ADC0_current_12bit + SW_Read_OP1_O_by_ADC0();
        delay_Xmsec(10);
        ADC1_current_12bit = ADC1_current_12bit + SW_Read_OP1_O_by_ADC1(); 
        delay_Xmsec(10);
    }

    ADC0_current_12bit = ADC0_current_12bit >> 5;
    ADC1_current_12bit = ADC1_current_12bit >> 5;

    MotorA.info.i16_Iu_ref_ADC = ADC0_current_12bit;
    MotorA.info.i16_Iv_ref_ADC = ADC1_current_12bit;


    /*--Check if the value is reasonable or not ---*/
    if(MotorA.info.i16_Iu_ref_ADC >= (C_Zero_I_ADC_1R + C_Zero_I_Band)) 
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
    else if(MotorA.info.i16_Iu_ref_ADC <= (C_Zero_I_ADC_1R - C_Zero_I_Band)) 
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
   
    if(MotorA.info.i16_Iv_ref_ADC >= (C_Zero_I_ADC_1R + C_Zero_I_Band))
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843
    else if(MotorA.info.i16_Iv_ref_ADC <= (C_Zero_I_ADC_1R - C_Zero_I_Band)) 
        MotorA.sym.u8_zero_current_error = 1;  //2.5V = 2048 , 2.75V = 2252, 2.25V = 1843


    /*-- Backup the 2 reference data value --*/
    i16_bak_Iu_ref = MotorA.info.i16_Iu_ref_ADC;
    i16_bak_Iv_ref = MotorA.info.i16_Iv_ref_ADC;
    
    /*-- If with error then do something to protect the system --*/
    if(MotorA.sym.u8_zero_current_error == 1)
    {
        Stop_Motor_PWMOUT_OFF();  //If got error, stop motor.

        while(MotorA.sym.u8_zero_current_error == 1);
    }
}



/*---Initialize_GPIO_1R()--------------------------------------------------------
Initialize GPIO for 1R system
PB6=Input: for reading switch "RUN" or testpin1 
PB7=Output: for controling LED output

Set GPIO to be ADC input for the demo system
  Set GPD2=ADC1_P1, GPC0=ADC0_P3 for Iu and Iv
  Set GPC1=ADC0_P4 for I_BUS
  Set GPC7=ADC1_P7 for Speed(Variable resistor)
  
--------------------------------------------------------------------------------*/
void Initialize_GPIO_1R(void)
{
     /*   PB6=Input: for reading switch "RUN" or testpin
          PB7=Output: for controling LED output */
    PB->MODE = (PB->MODE & ~(GPIO_MODE_MODE6_Msk)) | (GPIO_MODE_QUASI << GPIO_MODE_MODE6_Pos);
    PB->MODE = (PB->MODE & ~(GPIO_MODE_MODE7_Msk)) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE7_Pos);


    /* Set GPD2=ADC1_P1, GPC0=ADC0_P3, GPE2=ADC1_OP1_O, GPC7=ADC1_P7 */
    SYS->GPD_MFP = (SYS->GPD_MFP & (~SYS_GPD_MFP_PD2MFP_Msk)) | SYS_GPD_MFP_PD2_ADC1_P1;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC0MFP_Msk)) | SYS_GPC_MFP_PC0_ADC0_P3;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC1MFP_Msk)) | SYS_GPC_MFP_PC1_ADC0_P4;
    SYS->GPC_MFP = (SYS->GPC_MFP & (~SYS_GPC_MFP_PC7MFP_Msk)) | SYS_GPC_MFP_PC7_ADC1_P7;
  
    /* Set GPIO as Input and Digital-Off mode */
    //---Set PD.2(ADC1_P1) & PC.0(ADC0_P3) as input and digital-off */
    PD->MODE = (PD->MODE & ~GPIO_MODE_MODE2_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE2_Pos);
    PD->DINOFF |= GPIO_DINOFF_DINOFF2_Msk;
    
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE0_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF0_Msk;

    //---Set PC.1(ADC0_P4) as input and digital-off */
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE1_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE1_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF1_Msk;

    //---Set PC.7(ADC1_P7) as input and digital-off */
    PC->MODE = (PC->MODE & ~GPIO_MODE_MODE7_Msk) | (GPIO_MODE_INPUT << GPIO_MODE_MODE7_Pos);
    PC->DINOFF |= GPIO_DINOFF_DINOFF7_Msk;
}
//---End of Setting About GPIO for the demo system--------------------------------


//==Setting About Interrupt(NVIC)=========================================
/*---Initialize_NVIC_Priority_1R()--------------------------------------------------------
Set Interrupt Priority (Lower level number with higher priority)
Level 0: Not use yet.
Level 1: BRAKE0 Interrupt for Brake condition process. (Highest)
Level 2: EPWM Interrupt for FOC loop (Default higher than ECAP)
Level 2: ECAP Interrupt  for Hall sensor 
Level 2: Timer0 Interrupt for user interface, speed control loop
Level 3: USECI0 Interrupt for UART data link with PC
--------------------------------------------------------------------------------*/
void Initialize_NVIC_Priority_1R(void)
{  
    NVIC_SetPriority(BRAKE0_IRQn,   1);  //with higher nature priority than ADC0
    NVIC_SetPriority(EPWM_IRQn,    1);  //with higher nature priority than ECAP and TMR0
    NVIC_SetPriority(ECAP_IRQn,   2);  //with higher nature priority than TMR0
    NVIC_SetPriority(TMR0_IRQn,   2);  
}


/*---Enable_NVIC_For_System_Requirement_1R()-------------------------------------------
Enable the NVIC for the system requirement
  NVIC_EnableIRQ(EPWM_IRQn);    //for 1R phase current detection --> FOC current control
  NVIC_EnableIRQ(BRAKE0_IRQn);  //for EPWM Brake Interrrupt
  NVIC_EnableIRQ(ECAP_IRQn);    //for Hall signal detection --> HallSpeedEstimator()
  NVIC_EnableIRQ(TMR0_IRQn);    //for System basic periodic routine
  NVIC_EnableIRQ(USCI0_IRQn);    //for UART data link with PC
--------------------------------------------------------------------------------*/
void Enable_NVIC_For_System_Requirement_1R(void)
{
    NVIC->ICPR[0U] = 0xFFFFFFFF;   //Clear all pending interrupt.
    /*-- Enable NVIC ---*/    
    NVIC_EnableIRQ(EPWM_IRQn);    //for Iu and Iv detection --> FOC current control
    NVIC_EnableIRQ(BRAKE0_IRQn);  //for EPWM Brake Interrrupt
    NVIC_EnableIRQ(ECAP_IRQn);    //for Hall signal detection --> HallSpeedEstimator()
    NVIC_EnableIRQ(TMR0_IRQn);    //for System basic periodic routine
    if((u8_flag_VSP_control == 0) && (u8_flag_UART_control == 1))
        NVIC_EnableIRQ(GDMA0_IRQn);   //for GDMA receiving command from PC
}

//====End of Setting Revelant to NVIC setting====================================================================================


//#endif
//---End of "#ifdef  P_1R_FOC"-------------------------------------------------------------


/*---Check_control_mode()-------------------------------------------
This motor system can be controlled by VSP or UART
if VSP initial value < 2048 , system enter VSP control mode
if VSP initial value > 2048 , system enter UART control mode
----------------------------------------------------------------------------------------*/

void Check_control_mode()
{
  uint16_t VSP = SW_Get_Speed_Command_From_VR(); //ADC1_SW_Read(ADC1_ADC1_P7_DAT)
  if(VSP<2048)
  {
    u8_flag_VSP_control = 1;
    u8_flag_UART_control = 0;
  }
  if(VSP>2048)
  {
    u8_flag_VSP_control = 0;
    u8_flag_UART_control = 1;
  }
}
//--END of Check_control_mode---------------------------------------------------------------
