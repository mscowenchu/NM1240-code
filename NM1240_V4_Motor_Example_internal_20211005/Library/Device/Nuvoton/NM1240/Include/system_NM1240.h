/**************************************************************************//**
 * @file     system_NM1240.h
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/07/07 14:38 $
 * @brief    NM1240 Series CMSIS System Header File
 *
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#ifndef __SYSTEM_NM1240_H
#define __SYSTEM_NM1240_H

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Macro Definition                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/* Using UART1 or UART2 */
//#define DEBUG_PORT   UUART1
#define DEBUG_PORT   UUART2

/*----------------------------------------------------------------------------
  Define SYSCLK
 *----------------------------------------------------------------------------*/
#define __EXT       (24000000UL)    /*!< External Crystal Clock Frequency: up to 24MHz. */
#define __LIRC      (   10000UL)    /*!< Internal 10K RC Oscillator Frequency */

extern uint32_t __HIRC;             /*!< HIRC can be 48MHz or 60MHz */
extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock) */
extern uint32_t CyclesPerUs;        /*!< Cycles per micro second             */

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialize GPIO directions and values
 */
extern void SystemInit(void);


/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from CPU registers.
 */
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus

}
#endif

#endif
/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
