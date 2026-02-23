/**************************************************************************//**
 * @file     adc.h
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/07/09 15:54 $
 * @brief    NM1240 ADC Driver Header File
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#ifndef __ADC_H__
#define __ADC_H__


#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_ADC_Driver ADC Driver
  @{
*/

/** @addtogroup NM1240_ADC_EXPORTED_CONSTANTS ADC Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* ADC Sample Module Constant Definitions                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

#define ADC_ADC0                  (0)                                        /*!< ADC0 Module Select                       */
#define ADC_ADC1                  (16)                                       /*!< ADC1 Module Select                       */
#define STATUS_ADC0CH             (ADC_STATUS_ADC0CH_Pos)                    /*!< ADC0 Current Conversion Channel          */
#define STATUS_ADC1CH             (ADC_STATUS_ADC1CH_Pos)                    /*!< ADC0 Current Conversion Channel          */

#define ADC0CS_ADC0_P0          (0x1UL<<0)    /*!< ADC0 channel select ADC0_P0    */
#define ADC0CS_ADC0_P1          (0x1UL<<1)    /*!< ADC0 channel select ADC0_P1    */
#define ADC0CS_ADC0_P2          (0x1UL<<2)    /*!< ADC0 channel select ADC0_P2    */
#define ADC0CS_ADC0_P3          (0x1UL<<3)    /*!< ADC0 channel select ADC0_P3    */
#define ADC0CS_ADC0_P4          (0x1UL<<4)    /*!< ADC0 channel select ADC0_P4    */
#define ADC0CS_ADC0_P8          (0x1UL<<5)    /*!< ADC0 channel select ADC0_P8    */
#define ADC0CS_BAND_GAP         (0x1UL<<6)    /*!< ADC0 channel select BAND_GAP   */
#define ADC0CS_VSS              (0x1UL<<7)    /*!< ADC0 channel select VSS        */
#define ADC0CS_ADC0_P9          (0x1UL<<8)    /*!< ADC0 channel select ADC0_P9    */
#define ADC0CS_OP1_O            (0x1UL<<9)    /*!< ADC0 channel select OP1_O      */
#define ADC0CS_ADC1_P0          (0x1UL<<10)   /*!< ADC0 channel select ADC1_P0    */
#define ADC0CS_DAC0             (0x1UL<<11)   /*!< ADC0 channel select DAC0       */
#define ADC0CS_DAC1             (0x1UL<<12)   /*!< ADC0 channel select DAC1       */
#define ADC0CS_ADC0_P5          (0x1UL<<13)   /*!< ADC0 channel select ADC0_P5    */
#define ADC0CS_ADC0_P6          (0x1UL<<14)   /*!< ADC0 channel select ADC0_P6    */
#define ADC0CS_ADC0_P7          (0x1UL<<15)   /*!< ADC0 channel select ADC0_P7    */
#define ADC1CS_ADC1_P0          (0x1UL<<16)   /*!< ADC1 channel select ADC1_P0    */
#define ADC1CS_ADC1_P1          (0x1UL<<17)   /*!< ADC1 channel select ADC1_P1    */
#define ADC1CS_ADC1_P2          (0x1UL<<18)   /*!< ADC1 channel select ADC1_P2    */
#define ADC1CS_ADC0_P0          (0x1UL<<19)   /*!< ADC1 channel select ADC0_P0    */
#define ADC1CS_ADC0_P4          (0x1UL<<20)   /*!< ADC1 channel select ADC0_P4    */
#define ADC1CS_ADC1_P8          (0x1UL<<21)   /*!< ADC1 channel select ADC1_P8    */
#define ADC1CS_TEMP_SENSOR      (0x1UL<<22)   /*!< ADC1 channel select TEMP SENSOR*/
#define ADC1CS_VSS              (0x1UL<<23)   /*!< ADC1 channel select VSS        */
#define ADC1CS_ADC1_P9          (0x1UL<<24)   /*!< ADC1 channel select ADC1_P9    */
#define ADC1CS_OP1_O            (0x1UL<<25)   /*!< ADC1 channel select OP1_O      */
#define ADC1CS_DAC0             (0x1UL<<26)   /*!< ADC1 channel select DAC0       */
#define ADC1CS_ADC1_P3          (0x1UL<<27)   /*!< ADC1 channel select ADC1_P3    */
#define ADC1CS_ADC1_P4          (0x1UL<<28)   /*!< ADC1 channel select ADC1_P4    */
#define ADC1CS_ADC1_P5          (0x1UL<<29)   /*!< ADC1 channel select ADC1_P5    */
#define ADC1CS_ADC1_P6          (0x1UL<<30)   /*!< ADC1 channel select ADC1_P6    */
#define ADC1CS_ADC1_P7          (0x1UL<<31)   /*!< ADC1 channel select ADC1_P7    */

/*----------------------------------------------------------------------------------------------------------*/
/* ADC Data Register Offset Definitions                                                                        */
/*----------------------------------------------------------------------------------------------------------*/
#define ADC0_ADC0_P0_DAT              (0)     /*!< ADC0 DAT0 for ADC0_P0    */
#define ADC0_ADC0_P1_DAT              (1)     /*!< ADC0 DAT1 for ADC0_P1    */
#define ADC0_ADC0_P2_DAT              (2)     /*!< ADC0 DAT2 for ADC0_P2    */
#define ADC0_ADC0_P3_DAT              (3)     /*!< ADC0 DAT3 for ADC0_P3    */
#define ADC0_ADC0_P4_DAT              (4)     /*!< ADC0 DAT4 for ADC0_P4    */
#define ADC0_ADC0_P8_DAT              (5)     /*!< ADC0 DAT5 for ADC0_P8    */
#define ADC0_BAND_GAP_DAT             (6)     /*!< ADC0 DAT6 for BAND_GAP   */
#define ADC0_VSS_DAT                  (7)     /*!< ADC0 DAT7 for VSS        */
#define ADC0_ADC0_P9_DAT              (8)     /*!< ADC0 DAT8 for ADC0_P9    */
#define ADC0_OP1_O_DAT                (9)     /*!< ADC0 DAT9 for OP1_O      */
#define ADC0_ADC1_P0_DAT              (10)    /*!< ADC0 DAT10 for ADC1_P0   */
#define ADC0_DAC0_DAT                 (11)    /*!< ADC0 DAT11 for DAC0      */
#define ADC0_DAC1_DAT                 (12)    /*!< ADC0 DAT12 for DAC1      */
#define ADC0_ADC0_P5_DAT              (13)    /*!< ADC0 DAT13 for ADC0_P5   */
#define ADC0_ADC0_P6_DAT              (14)    /*!< ADC0 DAT14 for ADC0_P6   */
#define ADC0_ADC0_P7_DAT              (15)    /*!< ADC0 DAT15 for ADC0_P7   */
#define ADC1_ADC1_P0_DAT              (0)     /*!< ADC1 DAT0 for ADC1_P0    */
#define ADC1_ADC1_P1_DAT              (1)     /*!< ADC1 DAT1 for ADC1_P1    */
#define ADC1_ADC1_P2_DAT              (2)     /*!< ADC1 DAT2 for ADC1_P2    */
#define ADC1_ADC0_P0_DAT              (3)     /*!< ADC1 DAT3 for ADC0_P0    */
#define ADC1_ADC0_P4_DAT              (4)     /*!< ADC1 DAT4 for ADC0_P4    */
#define ADC1_ADC1_P8_DAT              (5)     /*!< ADC1 DAT5 for ADC1_P8    */
#define ADC1_TEMP_SENSOR_DAT          (6)     /*!< ADC1 DAT6 for TEMP_SENSOR*/
#define ADC1_VSS_DAT                  (7)     /*!< ADC1 DAT7 for VSS        */
#define ADC1_ADC1_P9_DAT              (8)     /*!< ADC1 DAT8 for ADC1_P9    */
#define ADC1_OP1_O_DAT                (9)     /*!< ADC1 DAT9 for OP1_O      */
#define ADC1_DAC0_DAT                 (10)    /*!< ADC1 DAT10 for DAC0      */
#define ADC1_ADC1_P3_DAT              (11)    /*!< ADC1 DAT11 for ADC1_P3   */
#define ADC1_ADC1_P4_DAT              (12)    /*!< ADC1 DAT12 for ADC1_P4   */
#define ADC1_ADC1_P5_DAT              (13)    /*!< ADC1 DAT13 for ADC1_P5   */
#define ADC1_ADC1_P6_DAT              (14)    /*!< ADC1 DAT14 for ADC1_P6   */
#define ADC1_ADC1_P7_DAT              (15)    /*!< ADC1 DAT15 for ADC1_P7   */

/*----------------------------------------------------------------------------------------------------------*/
/* ADC_TRGSOR Constant Definitions                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
#define ADC0_SOFTWARE_TRIGGER               (0x000000FFUL)     /*!< Software trigger (Disable hardware trigger) */
#define ADC1_SOFTWARE_TRIGGER               (0x00FF0000UL)     /*!< Software trigger (Disable hardware trigger) */

#define ADC0_STADC_RISING_TRIGGER           ((0x0UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0STADCSEL_Pos))     /*!< STADC Rising Edge trigger            */
#define ADC0_STADC_FALLING_TRIGGER          ((0x0UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0STADCSEL_Pos))     /*!< STADC Falling Edge trigger           */
#define ADC0_STADC_RISING_FALLING_TRIGGER   ((0x0UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0STADCSEL_Pos))     /*!< STADC Rising or Falling Edge trigger */

#define ADC0_EPWM0_FALLING_TRIGGER          ((0x1UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM0 Falling trigger                */
#define ADC0_EPWM0_CENTRAL_TRIGGER          ((0x1UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM0 Counter Central trigger        */
#define ADC0_EPWM0_RISING_TRIGGER           ((0x1UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM0 Rising trigger                 */
#define ADC0_EPWM0_PERIOD_TRIGGER           ((0x1UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM0 Period trigger                 */

#define ADC0_EPWM1_FALLING_TRIGGER          ((0x2UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM1 Falling trigger                */
#define ADC0_EPWM1_CENTRAL_TRIGGER          ((0x2UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM1 Counter Central trigger        */
#define ADC0_EPWM1_RISING_TRIGGER           ((0x2UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM1 Rising trigger                 */
#define ADC0_EPWM1_PERIOD_TRIGGER           ((0x2UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM1 Period trigger                 */

#define ADC0_EPWM2_FALLING_TRIGGER          ((0x3UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM2 Falling trigger                */
#define ADC0_EPWM2_CENTRAL_TRIGGER          ((0x3UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM2 Counter Central trigger        */
#define ADC0_EPWM2_RISING_TRIGGER           ((0x3UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM2 Rising trigger                 */
#define ADC0_EPWM2_PERIOD_TRIGGER           ((0x3UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM2 Period trigger                 */

#define ADC0_EPWM3_FALLING_TRIGGER          ((0x4UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM3 Falling trigger                */
#define ADC0_EPWM3_CENTRAL_TRIGGER          ((0x4UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM3 Counter Central trigger        */
#define ADC0_EPWM3_RISING_TRIGGER           ((0x4UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM3 Rising trigger                 */
#define ADC0_EPWM3_PERIOD_TRIGGER           ((0x4UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM3 Period trigger                 */

#define ADC0_EPWM4_FALLING_TRIGGER          ((0x5UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM4 Falling trigger                */
#define ADC0_EPWM4_CENTRAL_TRIGGER          ((0x5UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM4 Counter Central trigger        */
#define ADC0_EPWM4_RISING_TRIGGER           ((0x5UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM4 Rising trigger                 */
#define ADC0_EPWM4_PERIOD_TRIGGER           ((0x5UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM4 Period trigger                 */

#define ADC0_EPWM5_FALLING_TRIGGER          ((0x6UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM5 Falling trigger                */
#define ADC0_EPWM5_CENTRAL_TRIGGER          ((0x6UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM5 Counter Central trigger        */
#define ADC0_EPWM5_RISING_TRIGGER           ((0x6UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM5 Rising trigger                 */
#define ADC0_EPWM5_PERIOD_TRIGGER           ((0x6UL << ADC_TRGSOR_ADC0TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC0PWMTRGSEL_Pos))    /*!< EPWM5 Period trigger                 */

#define ADC0_TIMER0_TRIGGER                 (0x7UL << ADC_TRGSOR_ADC0TRGSOR_Pos)        /*!< Timer0 match trigger */
#define ADC0_TIMER1_TRIGGER                 (0x8UL << ADC_TRGSOR_ADC0TRGSOR_Pos)        /*!< Timer1 match trigger */
#define ADC0_ECAPPHG_TRIGGER                (0x9UL << ADC_TRGSOR_ADC0TRGSOR_Pos)        /*!< ECAP trigger         */

#define ADC0_ADC0IF_TRIGGER                 (0xAUL << ADC_TRGSOR_ADC0TRGSOR_Pos)        /*!< ADC0IF trigger       */
#define ADC0_ADC1IF_TRIGGER                 (0xBUL << ADC_TRGSOR_ADC0TRGSOR_Pos)        /*!< ADC1IF trigger       */

#define ADC1_STADC_RISING_TRIGGER           ((0x0UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1STADCSEL_Pos))     /*!< STADC Rising Edge trigger */
#define ADC1_STADC_FALLING_TRIGGER          ((0x0UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1STADCSEL_Pos))     /*!< STADC Falling Edge trigger */
#define ADC1_STADC_RISING_FALLING_TRIGGER   ((0x0UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1STADCSEL_Pos))     /*!< STADC Rising or Falling Edge trigger */

#define ADC1_EPWM0_FALLING_TRIGGER          ((0x1UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM0 Falling trigger */
#define ADC1_EPWM0_CENTRAL_TRIGGER          ((0x1UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM0 Counter Central trigger */
#define ADC1_EPWM0_RISING_TRIGGER           ((0x1UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM0 Rising trigger */
#define ADC1_EPWM0_PERIOD_TRIGGER           ((0x1UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM0 Period trigger */

#define ADC1_EPWM1_FALLING_TRIGGER          ((0x2UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM1 Falling trigger */
#define ADC1_EPWM1_CENTRAL_TRIGGER          ((0x2UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM1 Counter Central trigger */
#define ADC1_EPWM1_RISING_TRIGGER           ((0x2UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM1 Rising trigger */
#define ADC1_EPWM1_PERIOD_TRIGGER           ((0x2UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM1 Period trigger */

#define ADC1_EPWM2_FALLING_TRIGGER          ((0x3UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM2 Falling trigger */
#define ADC1_EPWM2_CENTRAL_TRIGGER          ((0x3UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM2 Counter Central trigger */
#define ADC1_EPWM2_RISING_TRIGGER           ((0x3UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM2 Rising trigger */
#define ADC1_EPWM2_PERIOD_TRIGGER           ((0x3UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM2 Period trigger */

#define ADC1_EPWM3_FALLING_TRIGGER          ((0x4UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM3 Falling trigger */
#define ADC1_EPWM3_CENTRAL_TRIGGER          ((0x4UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM3 Counter Central trigger */
#define ADC1_EPWM3_RISING_TRIGGER           ((0x4UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM3 Rising trigger */
#define ADC1_EPWM3_PERIOD_TRIGGER           ((0x4UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM3 Period trigger */

#define ADC1_EPWM4_FALLING_TRIGGER          ((0x5UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM4 Falling trigger */
#define ADC1_EPWM4_CENTRAL_TRIGGER          ((0x5UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM4 Counter Central trigger */
#define ADC1_EPWM4_RISING_TRIGGER           ((0x5UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM4 Rising trigger */
#define ADC1_EPWM4_PERIOD_TRIGGER           ((0x5UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM4 Period trigger */

#define ADC1_EPWM5_FALLING_TRIGGER          ((0x6UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x0UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM5 Falling trigger */
#define ADC1_EPWM5_CENTRAL_TRIGGER          ((0x6UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x1UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM5 Counter Central trigger */
#define ADC1_EPWM5_RISING_TRIGGER           ((0x6UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x2UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM5 Rising trigger */
#define ADC1_EPWM5_PERIOD_TRIGGER           ((0x6UL << ADC_TRGSOR_ADC1TRGSOR_Pos) | (0x3UL << ADC_TRGSOR_ADC1PWMTRGSEL_Pos))    /*!< EPWM5 Period trigger */

#define ADC1_TIMER0_TRIGGER                 (0x7UL << ADC_TRGSOR_ADC1TRGSOR_Pos)        /*!< Timer0 match trigger */
#define ADC1_TIMER1_TRIGGER                 (0x8UL << ADC_TRGSOR_ADC1TRGSOR_Pos)        /*!< Timer1 match trigger */
#define ADC1_ECAP_TRIGGER                   (0x9UL << ADC_TRGSOR_ADC1TRGSOR_Pos)        /*!< ECAP trigger EPWM phase change event trigger */

#define ADC1_ADC0IF_TRIGGER                 (0xAUL << ADC_TRGSOR_ADC1TRGSOR_Pos)        /*!< ADC1IF trigger       */
#define ADC1_ADC1IF_TRIGGER                 (0xBUL << ADC_TRGSOR_ADC1TRGSOR_Pos)        /*!< ADC1IF trigger       */

/*----------------------------------------------------------------------------------------------------------*/
/* ADC_TRGSOR Constant Definitions                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
#define ADC_VALSTS_ADC0VALID_CH0            (0)                                         /*!< ADC0 Ch0 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH1            (1)                                         /*!< ADC0 Ch1 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH2            (2)                                         /*!< ADC0 Ch2 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH3            (3)                                         /*!< ADC0 Ch3 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH4            (4)                                         /*!< ADC0 Ch4 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH5            (5)                                         /*!< ADC0 Ch5 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH6            (6)                                         /*!< ADC0 Ch6 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH7            (7)                                         /*!< ADC0 Ch7 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH8            (8)                                         /*!< ADC0 Ch8 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH9            (9)                                         /*!< ADC0 Ch9 valid flag         */
#define ADC_VALSTS_ADC0VALID_CH10           (10)                                        /*!< ADC0 Ch10 valid flag        */
#define ADC_VALSTS_ADC0VALID_CH11           (11)                                        /*!< ADC0 Ch11 valid flag        */
#define ADC_VALSTS_ADC0VALID_CH12           (12)                                        /*!< ADC0 Ch12 valid flag        */
#define ADC_VALSTS_ADC0VALID_CH13           (13)                                        /*!< ADC0 Ch13 valid flag        */
#define ADC_VALSTS_ADC0VALID_CH14           (14)                                        /*!< ADC0 Ch14 valid flag        */
#define ADC_VALSTS_ADC0VALID_CH15           (15)                                        /*!< ADC0 Ch15 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH0            (16)                                        /*!< ADC1 Ch0 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH1            (17)                                        /*!< ADC1 Ch1 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH2            (18)                                        /*!< ADC1 Ch2 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH3            (19)                                        /*!< ADC1 Ch3 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH4            (20)                                        /*!< ADC1 Ch4 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH5            (21)                                        /*!< ADC1 Ch5 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH6            (22)                                        /*!< ADC1 Ch6 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH7            (23)                                        /*!< ADC1 Ch7 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH8            (24)                                        /*!< ADC1 Ch8 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH9            (25)                                        /*!< ADC1 Ch9 valid flag         */
#define ADC_VALSTS_ADC1VALID_CH10           (26)                                        /*!< ADC1 Ch10 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH11           (27)                                        /*!< ADC1 Ch11 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH12           (28)                                        /*!< ADC1 Ch12 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH13           (29)                                        /*!< ADC1 Ch13 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH14           (30)                                        /*!< ADC1 Ch14 valid flag        */
#define ADC_VALSTS_ADC1VALID_CH15           (31)                                        /*!< ADC1 Ch15 valid flag        */


/*@}*/ /* end of group NM1240_ADC_EXPORTED_CONSTANTS */

/** @addtogroup NM1240_ADC_EXPORTED_FUNCTIONS ADC Exported Functions
  @{
*/

/**
  * @brief      Enable the interrupt.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Mask Decides the combination of interrupt enable bits. Each bit corresponds to a interrupt.
  *                     This parameter decides which interrupts will be enabled.
  *                     Valid values are:
  *                     - \ref ADC_CTL_ADC0IEN_Msk
  *                     - \ref ADC_CTL_ADC1IEN_Msk
  * @return     None.
  * @details    The A/D converter generates a conversion end ADCnF (ADC_STATUS[]) upon the end of specific sample module A/D conversion.
  *             If ADCnIEN bit (ADC_CTL[]) is set, the conversion end interrupt request ADCn_INT is generated (n=0~1).
  */
#define ADC_ENABLE_INT(adc, u32Mask)    ((adc)->CTL |= (u32Mask))

/**
  * @brief      Disable the interrupt.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Mask Decides the combination of interrupt enable bits. Each bit corresponds to a interrupt.
  *                     This parameter decides which interrupts will be disabled.
  *                     Valid values are:
  *                     - \ref ADC_CTL_ADC0IEN_Msk
  *                     - \ref ADC_CTL_ADC1IEN_Msk
  * @return     None.
  * @details    Specific sample module A/D ADCn_INT(n=0~1) interrupt function Disabled.
  */
#define ADC_DISABLE_INT(adc, u32Mask)   ((adc)->CTL &= ~(u32Mask))

/**
  * @brief      Start the A/D conversion.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32ModuleMask The combination of sample module. Each bit corresponds to a sample module.
  *                           This parameter decides which sample module will be conversion.
  *                           Valid values are:
  *                           - \ref ADC_CTL_ADC0SWTRG_Msk
  *                           - \ref ADC_CTL_ADC1SWTRG_Msk
  * @return     None.
  * @details    After write ADC_CTL register to start ADC conversion, the ADC_STATUS register will show which SAMPLE will conversion.
  */
#define ADC_START_CONV(adc, u32ModuleMask)    ((adc)->CTL |= (u32ModuleMask))

/**
  * @brief      Get the conversion data of the user-specified sample channel.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32ChannelNum Decides the sample channel number, valid value are:
  *                          - \ref ADC0_ADC0_P0_DAT
  *                          - \ref ADC0_ADC0_P1_DAT
  *                          - \ref ADC0_ADC0_P2_DAT
  *                          - \ref ADC0_ADC0_P3_DAT
  *                          - \ref ADC0_ADC0_P4_DAT
  *                          - \ref ADC0_ADC0_P8_DAT
  *                          - \ref ADC0_BAND_GAP_DAT
  *                          - \ref ADC0_VSS_DAT
  *                          - \ref ADC0_ADC0_P9_DAT
  *                          - \ref ADC0_OP1_O_DAT
  *                          - \ref ADC0_ADC1_P0_DAT
  *                          - \ref ADC0_DAC0_DAT
  *                          - \ref ADC0_DAC1_DAT
  *                          - \ref ADC0_ADC0_P5_DAT
  *                          - \ref ADC0_ADC0_P6_DAT
  *                          - \ref ADC0_ADC0_P7_DAT
  * @return     Return the conversion data of the user-specified sample channel.
  * @details    This macro is used to read ADC_DATn to get conversion data. (n=0~15)
  */
#define ADC0_GET_CONV_DATA(u32ChannelNum)    (*((volatile unsigned int *)(ADC_BASE + 0x80 + (u32ChannelNum << 2))))

/**
  * @brief      Get the conversion data of the user-specified sample channel.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32ChannelNum Decides the sample channel number, valid value are:
  *                          - \ref ADC1_ADC1_P0_DAT
  *                          - \ref ADC1_ADC1_P1_DAT
  *                          - \ref ADC1_ADC1_P2_DAT
  *                          - \ref ADC1_ADC0_P0_DAT
  *                          - \ref ADC1_ADC0_P4_DAT
  *                          - \ref ADC1_ADC1_P8_DAT
  *                          - \ref ADC1_TEMP_SENSOR_DAT
  *                          - \ref ADC1_VSS_DAT
  *                          - \ref ADC1_ADC1_P9_DAT
  *                          - \ref ADC1_OP1_O_DAT
  *                          - \ref ADC1_DAC0_DAT
  *                          - \ref ADC1_ADC1_P3_DAT
  *                          - \ref ADC1_ADC1_P4_DAT
  *                          - \ref ADC1_ADC1_P5_DAT
  *                          - \ref ADC1_ADC1_P6_DAT
  *                          - \ref ADC1_ADC1_P7_DAT
  * @return     Return the conversion data of the user-specified sample channel.
  * @details    This macro is used to read ADC1_DATn to get conversion data. (n=0~15)
  */
#define ADC1_GET_CONV_DATA(u32ChannelNum)    (*((volatile unsigned int *)(ADC_BASE + 0xC0 + (u32ChannelNum << 2))))

/**
  * @brief      Clear the selected interrupt status bits.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Mask The combination of compare interrupt status bits. Each bit corresponds to a compare interrupt status.
  *                     Valid values are:
  *                     - \ref ADC_STATUS_ADC0IF_Msk
  *                     - \ref ADC_STATUS_ADC1IF_Msk

  * @return     None.
  * @details    This macro is used to clear the selected interrupt status bits.
  */
#define ADC_CLR_INT_FLAG(adc, u32Mask)    ((adc)->STATUS = (u32Mask))

/**
  * @brief      Get the current conversion channel
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32Module  The sample module of ADC. Valid values are:
  *                     - \ref STATUS_ADC0CH
  *                     - \ref STATUS_ADC1CH
  * @return     Return the current convert channel.
  * @details    This macro is used to get the user-specified current conversion channel.
  * @note       This filed reflects the current conversion channel when ADC1BUSY = 1.
  *             When ADC1BUSY = 0, it shows the number of the next converted channel.
  */
#define ADC_GET_CURRENT_CONV_CH(adc, u32Module)    ((adc)->STATUS >> u32Module ) & 0xF

/**
  * @brief      Check all sample module A/D result data register valid flags.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @retval     0       None of sample module data register valid flag is set to 1.
  * @retval     1       Any one of sample module data register valid flag is set to 1.
  * @details    The ADCxVALID CHn will keep 1 when data in the sample module is valid. (x=0~1, n=0~15)
  * @note       Since the valid bit will be cleared by hardware after the ADC_DATn register is read,
  *             user MUST call this function BEFORE any other functions that could to read ADC_DATn register.
  *             That includes ADC_GET_CONV_DATA().
  */
#define ADC_IS_DATA_VALID(adc)    ( (adc)->VALSTS == 0 )? 0 : 1

/**
  * @brief      Get the busy state of ADC.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @retval     0       Both A/D converter 0 and 1 are in idle state.
  * @retval     1       A/D converter 0 is in busy state.
  * @retval     2       A/D converter 1 is in busy state.
  * @retval     3       Both A/D converter 0 and 1 are in busy state.
  * @details    This macro is used to read ADC0BUSY/ADC1BUSY bit (ADC_STATUS[3]/ADC_STATUS[11]) to get busy state.
  */
#define ADC_IS_BUSY(adc)    ( \
          (((adc)->STATUS & ADC_STATUS_ADC0BUSY_Msk) >> ADC_STATUS_ADC0BUSY_Pos) |     \
          (((adc)->STATUS & ADC_STATUS_ADC1BUSY_Msk) >> (ADC_STATUS_ADC1BUSY_Pos - 1)) \
        )

/**
  * @brief      Enable the Window Comparator interrupt.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32CMP  Specifies the compare register, valid value are from 0 to 1. This parameter is not used in NM1240.
  * @return     None
  * @details    If the Window Comparator function is enabled and the compare condition matches the setting of ADC_WCMPCTL,
  *             WCMPIF (ADC_STATUS[16]) will be asserted, in the meanwhile,
  *             if WCMPIEN is set to 1, a compare interrupt request is generated.
  */
#define ADC_ENABLE_CMP_INT(adc, u32CMP)    ((adc)->WCMPCTL |= ADC_WCMPCTL_WCMPIEN_Msk)

/**
  * @brief      Disable the Window Comparator interrupt.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @param[in]  u32CMP  Specifies the compare register, valid value are from 0 to 1. This parameter is not used in NM1240.
  * @return     None.
  * @details    This macro is used to disable the Window Comparator interrupt.
  */
#define ADC_DISABLE_CMP_INT(adc, u32CMP)    ((adc)->WCMPCTL &= ~ADC_WCMPCTL_WCMPIEN_Msk)

/**
  * @brief      Set the ADC conversion mode to Independent Mode.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @return     None.
  * @details    This macro is used to set the ADC conversion mode to Independent Mode.
  */
#define ADC_SET_INDEPENDENT_MODE(adc)    ((adc)->CTL = ((adc)->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x0 << ADC_CTL_ADCMODE_Pos))

/**
  * @brief      Set the ADC conversion mode to Continuous Mode.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @return     None.
  * @details    This macro is used to set the ADC conversion mode to Continuous Mode.
  */
#define ADC_SET_Continuous_MODE(adc)    ((adc)->CTL = ((adc)->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x1 << ADC_CTL_ADCMODE_Pos))

/**
  * @brief      Set the ADC conversion mode to Simultaneous Mode.
  * @param[in]  adc    The pointer of the specified ADC module.
  * @return     None.
  * @details    This macro is used to set the ADC conversion mode to Simultaneous Mode.
  */
#define ADC_SET_SIMULTANEOUS_MODE(adc)    ((adc)->CTL = ((adc)->CTL & ~ADC_CTL_ADCMODE_Msk) | (0x2 << ADC_CTL_ADCMODE_Pos))

/*---------------------------------------------------------------------------------------------------------*/
/* Define ADC functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_Open(ADC_T *adc, uint32_t u32InputMode);
void ADC_Close(ADC_T *adc);
void ADC_ConfigChannel(ADC_T *adc, uint32_t u32Channel, uint32_t u32TriggerSrc);
void ADC_SetTriggerDelayTime(ADC_T *adc, uint32_t u32ModuleNum, uint32_t u32TriggerDelayTime);
void ADC_SetSampleCnt(ADC_T *adc, uint32_t u32ModuleNum, uint32_t u32SampleCnt);
uint32_t ADC_Get_Data_Valid_Flag(ADC_T *adc, uint32_t u32Channel);
uint32_t ADC_GET_INT_FLAG(ADC_T *adc, uint32_t u32Msk);

/*@}*/ /* end of group NM1240_ADC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_ADC_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __ADC_H__ */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
