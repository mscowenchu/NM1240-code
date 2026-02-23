/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 2018/09/26 13:58 $
 * @brief    Software Development Template.
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NM1230.h"


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK_SetCoreClock(FREQ_72MHZ);

    /* Enable USCI0 IP clock */
		CLK_EnableModuleClock(USCI0_MODULE);

		/* USCI-Uart0-GPD5(TX) + GPD6(RX) */
    /* Set GPD multi-function pins for USCI UART0 GPD5(TX) and GPD6(RX) */	
	  SYS->GPD_MFP = SYS->GPD_MFP & ~(SYS_GPD_MFP_PD5MFP_Msk | SYS_GPD_MFP_PD6MFP_Msk) | (SYS_GPD_MFP_PD5_UART0_TXD | SYS_GPD_MFP_PD6_UART0_RXD);	
	
    /* Set GPD5 as output mode and GPD6 as Input mode */
    PD->MODE = PD->MODE & ~(GPIO_MODE_MODE5_Msk | GPIO_MODE_MODE6_Msk) | (GPIO_MODE_OUTPUT << GPIO_MODE_MODE5_Pos);
	
    /* Lock protected registers */
    SYS_LockReg();	
}


int main()
{
    int8_t ch;

    SYS_Init();

    /* Init USCI UART0 to 115200-8n1 for print message */
		UUART_Open(UUART0, 115200);	

    printf("\n\nPDID 0x%08X\n", SYS_ReadPDID());    /* Display PDID */
    printf("CPU @ %dHz\n", SystemCoreClock);        /* Display System Core Clock */

    printf("+-----------------------------------------+\n");
    printf("|            Simple Demo Code             |\n");
    printf("+-----------------------------------------+\n");
	
    printf("Please input any key\n\n");
    do
    {
        printf("Input: ");
        ch = getchar();
        printf("%c\n", ch);
    }
    while(1);
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
