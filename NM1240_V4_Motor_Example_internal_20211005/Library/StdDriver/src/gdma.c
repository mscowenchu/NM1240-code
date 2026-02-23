/**************************************************************************//**
 * @file     gdma.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/06/08 16:27 $
 * @brief    NM1240 GDMA driver source file
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NM1240.h"

/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_GDMA_Driver BPWM Driver
  @{
*/


/** @addtogroup NM1240_GDMA_EXPORTED_FUNCTIONS BPWM Exported Functions
  @{
*/

/**
  * @brief      Open and set GDMA function
  * @param[in]  gdma        The pointer of the specified GDMA module
  * @param[in]  u32ChannelNum The GDMA channel Mask. Valid values are between 0~1.
  * @param[in]  u32ModeMask GDMA mode select. Valid values are:
  *                         - \ref GDMA_SOFTWARE_MODE
  *                         - \ref GDMA_USCI_MODE
  * @param[in]  u32TransferWidth Transfer width select. Valid values are:
  *                         - \ref GDMA_TWS_8BITS
  *                         - \ref GDMA_TWS_16BITS
  *                         - \ref GDMA_TWS_32BITS
  * @param[in]  u32BurstMask Burst mode enable. Valid values are:
  *                         - \ref GDMA_BURST_DISABLE
  *                         - \ref GDMA_BURST_ENABLE
	* @param[in]  u32AddressMask Address direction and fixed or not. Valid values are: (use '|' for multiple selections)
  *                         - \ref Destination_ADDRESS_FIXED
  *                         - \ref Destination_ADDRESS_INC
  *                         - \ref Destination_ADDRESS_DEC
  *                         - \ref SOURCE_ADDRESS_FIXED
  *                         - \ref SOURCE_ADDRESS_INC
  *                         - \ref SOURCE_ADDRESS_DEC
  * @return     None
  * @details    This function use to open and set GDMA function.
  */
void GDMA_OPEN(GDMA_T* gdma, uint32_t u32ModeMask, uint32_t u32TransferWidth, uint32_t u32BurstMask, uint32_t u32AddressMask)
{
    /* Enable GDMA controller clock */
    CLK->AHBCLK |= CLK_AHBCLK_GDMACKEN_Msk;

    /* Select transfer width */
    if (u32BurstMask == GDMA_BURST_ENABLE)
      gdma->CTL = (gdma->CTL & ~(GDMA_CTL_TWS_Msk)) | (GDMA_TWS_32BITS << GDMA_CTL_TWS_Pos) | GDMA_CTL_BME_Msk;
    else
      gdma->CTL = (gdma->CTL & ~(GDMA_CTL_TWS_Msk | GDMA_CTL_BME_Msk)) | ((u32TransferWidth & 0x3) << GDMA_CTL_TWS_Pos);
		
    /* Enable GDMA and set mode */
    gdma->CTL = (gdma->CTL & ~(GDMA_CTL_GDMAMS_Msk)) | ((u32ModeMask & 0x3) << GDMA_CTL_GDMAMS_Pos);
			
    /* Set address direction or fixed */
    gdma->CTL = (gdma->CTL & ~(0xF << GDMA_CTL_DADIR_Pos)) | (u32AddressMask << GDMA_CTL_DADIR_Pos);
}


/*@}*/ /* end of group NM1240_GDMA_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_GDMA_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
