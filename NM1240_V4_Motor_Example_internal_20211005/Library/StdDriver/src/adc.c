/**************************************************************************//**
 * @file     adc.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/07/09 15:54 $
 * @brief    NM1240 ADC driver source file
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NM1240.h"

/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_ADC_Driver ADC Driver
  @{
*/


/** @addtogroup NM1240_ADC_EXPORTED_FUNCTIONS ADC Exported Functions
  @{
*/

/**
  * @brief      This function make ADC_module be ready to convert.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32InputMode Decides the input mode. This parameter is not used in NM1240.
  * @return     None.
  * @details    This function is used to set analog input mode and enable A/D Converter.
  *             Before starting A/D conversion function, ADCEN bit (ADC_CTL[0]) should be set to 1.
  */
void ADC_Open(ADC_T *adc, uint32_t u32InputMode)
{
    adc->CTL |= ADC_CTL_ADCEN_Msk;
}

/**
  * @brief      Disable ADC_module.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @return     None.
  * @details    Clear ADCEN bit (ADC_CTL[0]) to disable A/D converter analog circuit power consumption.
  */
void ADC_Close(ADC_T *adc)
{
    adc->CS_CTL = (ADC0CS_VSS)       /* Switching to channel Vss to save power */
				| (ADC1CS_VSS);       /* Switching to channel Vss to save power */

    adc->CTL &= ~ADC_CTL_ADCEN_Msk;
    SYS_ResetModule(ADC_RST);
}

/**
  * @brief      Configure channel and trigger source.
  * @param[in]  adc The pointer of the specified ADC module.
  * @param[in]  u32Channel Decides the sample module number, valid value are: (use '|' for multiple selections)
  *                          - \ref ADC0CS_ADC0_P0
  *                          - \ref ADC0CS_ADC0_P1
  *                          - \ref ADC0CS_ADC0_P2
  *                          - \ref ADC0CS_ADC0_P3
  *                          - \ref ADC0CS_ADC0_P4
  *                          - \ref ADC0CS_ADC0_P8 
  *                          - \ref ADC0CS_BAND_GAP
  *                          - \ref ADC0CS_VSS
  *                          - \ref ADC0CS_ADC0_P9
  *                          - \ref ADC0CS_OP1_O
  *                          - \ref ADC0CS_ADC1_P0
  *                          - \ref ADC0CS_DAC0
  *                          - \ref ADC0CS_DAC1
  *                          - \ref ADC0CS_ADC0_P5
  *                          - \ref ADC0CS_ADC0_P6
  *                          - \ref ADC0CS_ADC0_P7
  *                          - \ref ADC1CS_ADC1_P0
  *                          - \ref ADC1CS_ADC1_P1
  *                          - \ref ADC1CS_ADC1_P2
  *                          - \ref ADC1CS_ADC0_P0
  *                          - \ref ADC1CS_ADC0_P4
  *                          - \ref ADC1CS_ADC0_P8
  *                          - \ref ADC1CS_TEMP_SENSOR
  *                          - \ref ADC1CS_VSS
  *                          - \ref ADC1CS_ADC1_P9
  *                          - \ref ADC1CS_OP1_O
  *                          - \ref ADC1CS_DAC0
  *                          - \ref ADC1CS_ADC1_CH3
  *                          - \ref ADC1CS_ADC1_CH4
  *                          - \ref ADC1CS_ADC1_CH5
  *                          - \ref ADC1CS_ADC1_CH6
  *                          - \ref ADC1CS_ADC1_CH7
  * @param[in] u32TriggerSrc Decides the trigger source, valid values are: (use '|' for multiple selections)
  *                          - \ref ADC0_SOFTWARE_TRIGGER
  *                          - \ref ADC0_STADC_RISING_TRIGGER
  *                          - \ref ADC0_STADC_FALLING_TRIGGER
  *                          - \ref ADC0_STADC_RISING_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM0_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM0_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM0_RISING_TRIGGER
  *                          - \ref ADC0_EPWM0_PERIOD_TRIGGER
  *                          - \ref ADC0_EPWM1_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM1_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM1_RISING_TRIGGER
  *                          - \ref ADC0_EPWM1_PERIOD_TRIGGER
  *                          - \ref ADC0_EPWM2_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM2_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM2_RISING_TRIGGER
  *                          - \ref ADC0_EPWM2_PERIOD_TRIGGER
  *                          - \ref ADC0_EPWM3_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM3_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM3_RISING_TRIGGER
  *                          - \ref ADC0_EPWM3_PERIOD_TRIGGER
  *                          - \ref ADC0_EPWM4_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM4_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM4_RISING_TRIGGER
  *                          - \ref ADC0_EPWM4_PERIOD_TRIGGER
  *                          - \ref ADC0_EPWM5_FALLING_TRIGGER
  *                          - \ref ADC0_EPWM5_CENTRAL_TRIGGER
  *                          - \ref ADC0_EPWM5_RISING_TRIGGER
  *                          - \ref ADC0_EPWM5_PERIOD_TRIGGER
  *                          - \ref ADC0_TIMER0_TRIGGER
  *                          - \ref ADC0_TIMER1_TRIGGER
  *                          - \ref ADC0_ECAPPHG_TRIGGER
  *                          - \ref ADC0_ADC0IF_TRIGGER
  *                          - \ref ADC0_ADC1IF_TRIGGER
  *                          - \ref ADC0_SOFTWARE_TRIGGER
  *                          - \ref ADC1_STADC_RISING_TRIGGER
  *                          - \ref ADC1_STADC_FALLING_TRIGGER
  *                          - \ref ADC1_STADC_RISING_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM0_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM0_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM0_RISING_TRIGGER
  *                          - \ref ADC1_EPWM0_PERIOD_TRIGGER
  *                          - \ref ADC1_EPWM1_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM1_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM1_RISING_TRIGGER
  *                          - \ref ADC1_EPWM1_PERIOD_TRIGGER
  *                          - \ref ADC1_EPWM2_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM2_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM2_RISING_TRIGGER
  *                          - \ref ADC1_EPWM2_PERIOD_TRIGGER
  *                          - \ref ADC1_EPWM3_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM3_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM3_RISING_TRIGGER
  *                          - \ref ADC1_EPWM3_PERIOD_TRIGGER
  *                          - \ref ADC1_EPWM4_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM4_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM4_RISING_TRIGGER
  *                          - \ref ADC1_EPWM4_PERIOD_TRIGGER
  *                          - \ref ADC1_EPWM5_FALLING_TRIGGER
  *                          - \ref ADC1_EPWM5_CENTRAL_TRIGGER
  *                          - \ref ADC1_EPWM5_RISING_TRIGGER
  *                          - \ref ADC1_EPWM5_PERIOD_TRIGGER
  *                          - \ref ADC1_TIMER0_TRIGGER
  *                          - \ref ADC1_TIMER1_TRIGGER
  *                          - \ref ADC1_ECAPPHG_TRIGGER
  *                          - \ref ADC1_ADC0IF_TRIGGER
  *                          - \ref ADC1_ADC1IF_TRIGGER
  * @return     None.
  * @details    Each of ADC control logic modules A0~7 which is configurable for ADC converter channel ADC0_CH0~7 and trigger source.
  *             And each of ADC control logic modules B0~7 which is configurable for ADC converter channel ADC1_CH0~7 and trigger source.
  * @note       Sample module 0 channels 0~7 share the same hardware trigger configuration in ADC_TRGSOR[7:0] and sample module 1 channels 0~7 share the same hardware trigger configuration in ADC_TRGSOR[23:16].
  */
void ADC_ConfigChannel(ADC_T *adc,
                           uint32_t u32Channel,
                           uint32_t u32TriggerSrc)
{
	  uint32_t ADC0TriggerSrc, ADC1TriggerSrc;
	  adc->CS_CTL |= u32Channel;
	
	  ADC0TriggerSrc = u32TriggerSrc & (ADC_TRGSOR_ADC0STADCSEL_Msk | ADC_TRGSOR_ADC0PWMTRGSEL_Msk | ADC_TRGSOR_ADC0TRGSOR_Msk);
	  ADC1TriggerSrc = u32TriggerSrc & (ADC_TRGSOR_ADC1STADCSEL_Msk | ADC_TRGSOR_ADC1PWMTRGSEL_Msk | ADC_TRGSOR_ADC1TRGSOR_Msk);

    if (ADC0TriggerSrc == ADC0_SOFTWARE_TRIGGER)
        adc->CTL &= ~ADC_CTL_ADC0HWTRGEN_Msk;
    else
    {
	      adc->TRGSOR = (adc->TRGSOR & ~(ADC_TRGSOR_ADC0STADCSEL_Msk | ADC_TRGSOR_ADC0PWMTRGSEL_Msk | ADC_TRGSOR_ADC0TRGSOR_Msk))
		                 | ADC0TriggerSrc;
		    adc->CTL |= ADC_CTL_ADC0HWTRGEN_Msk;
		}
    if (ADC1TriggerSrc == ADC1_SOFTWARE_TRIGGER)
        adc->CTL &= ~ADC_CTL_ADC1HWTRGEN_Msk;
    else
    {
        adc->TRGSOR = (adc->TRGSOR & ~(ADC_TRGSOR_ADC1STADCSEL_Msk | ADC_TRGSOR_ADC1PWMTRGSEL_Msk | ADC_TRGSOR_ADC1TRGSOR_Msk))
			               | ADC1TriggerSrc;
		    adc->CTL |= ADC_CTL_ADC1HWTRGEN_Msk;
		}
}

/**
  * @brief      Set trigger delay time.
  * @param[in]  adc The pointer of the specified ADC module.
  * @param[in]  u32ModuleNum Decides the sample module number, valid value are:
  *                          - \ref ADC_ADC0
  *                          - \ref ADC_ADC1
  * @param[in]  u32TriggerDelayTime Decides the trigger delay time, valid range are between 0~0xFF.
  * @return     None.
  * @details    User can configure the trigger delay time by setting ADCnDELAY (ADC_TRGDLY[7:0] and ADC_TRGDLY[23:16], n=0~1).
  *             Trigger delay time = (4 * u32TriggerDelayTime) x system clock period.
  * @note       Sample module 0 channels 0~7 share the same configuration in ADC0DELAY(ADC_TRGDLY[7:0]) and sample module 1 channels 0~7 share the same configuration in ADC1DELAY(ADC_TRGDLY[23:16]).
  */
void ADC_SetTriggerDelayTime(ADC_T *adc,
                            uint32_t u32ModuleNum,
                            uint32_t u32TriggerDelayTime)
{
    if (u32ModuleNum == ADC_ADC0)
        adc->TRGDLY = (adc->TRGDLY & (~ADC_TRGDLY_ADC0DELAY_Msk)) | ((u32TriggerDelayTime & 0xFF) << ADC_TRGDLY_ADC0DELAY_Pos);
    else if (u32ModuleNum == ADC_ADC1)
        adc->TRGDLY = (adc->TRGDLY & (~ADC_TRGDLY_ADC1DELAY_Msk)) | ((u32TriggerDelayTime & 0xFF) << ADC_TRGDLY_ADC1DELAY_Pos);
}

/**
  * @brief      Set ADC sampling time.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32SampleCnt Decides the extend sampling time, the range is from 1~1024 ADC clock. Valid value are from 0 to 15.
  *                     PCLK * (ADCSMPCNT * 4 + 2)
  * @return     None.
  * @details    When A/D converting at high conversion rate, the sampling time of analog input voltage may not enough if input channel loading is heavy,
  *             user can extend A/D sampling time after trigger source is coming to get enough sampling time.
  * @note       All sample module 0 channels 0~7 and sample module 1 channels 0~7 share the same configuration in ADCSMPCNT(ADC_SMPCNT[3:0]).
  */
void ADC_SetSampleCnt(ADC_T *adc, uint32_t u32ModuleNum, uint32_t u32SampleCnt)
{
    adc->SMPCNT = (adc->SMPCNT & (~ADC_SMPCNT_ADCSMPCNT_Msk)) | ((u32SampleCnt & 0xF) << ADC_SMPCNT_ADCSMPCNT_Pos);
}

/**
  * @brief      Get the data valid flag of the user-specified sample module.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Channel Decides the channel number, valid value are
  *                     - \ref ADC_VALSTS_ADC0VALID_CHn (n=0~15)
  *                     - \ref ADC_VALSTS_ADC1VALID_CHn (n=0~15)
  * @return     Return the data valid flag of the user-specified channel.
  * @details    This macro is used to read ADC_VALSTS to get the data valid flag of the channel.
  * @note       Since the valid bit will be cleared by hardware after the ADCx_DATn register is read,
  *             user MUST call this function BEFORE any other functions that could to read ADCx_DATn register.
  *             That includes ADC_GET_CONV_DATA().
  */
uint32_t ADC_Get_Data_Valid_Flag(ADC_T *adc, uint32_t u32Channel)
{
    uint32_t valid_flag;
    valid_flag = (adc->VALSTS >> u32Channel) & 0x1;
    return (valid_flag);
}

/**
  * @brief      Get the user-specified interrupt flags.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Msk The combination of interrupt status bits. Each bit corresponds to a interrupt status.
  *                     Valid values are:
  *                     - \ref ADC_STATUS_ADC0IF_Msk
  *                     - \ref ADC_STATUS_ADC1IF_Msk

  * @return     Return the user-specified interrupt flags.
  * @details    This macro is used to get the user-specified interrupt flags.
  */
uint32_t ADC_GET_INT_FLAG(ADC_T *adc, uint32_t u32Msk)
{
	  return (((adc)->STATUS & u32Msk) ? 1 : 0);
}

/*@}*/ /* end of group NM1240_ADC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_ADC_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
