/**************************************************************************//**
 * @file     gdma.h
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/06/05 17:34 $
 * @brief    NM1240 GDMA driver header file
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __GDMA_H__
#define __GDMA_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_GDMA_Driver GDMA Driver
  @{
*/

/** @addtogroup NM1240_GDMA_EXPORTED_CONSTANTS GDMA Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* GDMA I2C master event definitions                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define GDMA_SOFTWARE_MODE                   (0x0UL)    /*!< GDMA SOFTWARE MODE          */
#define GDMA_USCI_MODE                       (0x1UL)    /*!< GDMA USCI MODE              */
#define GDMA_TWS_8BITS                       (0x0UL)    /*!< GDMA Transfer width 8 bits  */
#define GDMA_TWS_16BITS                      (0x1UL)    /*!< GDMA Transfer width 16 bits */
#define GDMA_TWS_32BITS                      (0x2UL)    /*!< GDMA Transfer width 32 bits */
#define GDMA_BURST_DISABLE                   (0x0UL)    /*!< GDMA Transfer width 32 bits */
#define GDMA_BURST_ENABLE                    (0x1UL)    /*!< GDMA Transfer width 32 bits */
#define Destination_ADDRESS_FIXED            (0x4UL)    /*!< GDMA Destination address fixed */
#define Destination_ADDRESS_INC              (0x0UL)    /*!< GDMA Destination address increment direction */
#define Destination_ADDRESS_DEC              (0x1UL)    /*!< GDMA Destination address dectement direction */
#define SOURCE_ADDRESS_FIXED                 (0x8UL)    /*!< GDMA Source address fixed */
#define SOURCE_ADDRESS_INC                   (0x0UL)    /*!< GDMA Destination address increment direction */
#define SOURCE_ADDRESS_DEC                   (0x2UL)    /*!< GDMA Destination address decrement direction */ 

/*@}*/ /* end of group NM1240_GDMA_EXPORTED_CONSTANTS */


/** @addtogroup NM1240_GDMA_EXPORTED_FUNCTIONS GDMA Exported Functions
  @{
*/

/**
 * @brief       This macro is used to get GDMA transfer complete interrupt flag status
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      Current GDMA transfer complete interrupt flag status (1: transfer complete)
 * @details     This macro will return GDMA transfer complete interrupt flag status.
 * \hideinitializer
 */
#define GDMA_GET_TRANSFER_COMPLETE_STATUS(gdma)   ((gdma->CTL & GDMA_CTL_TCIF_Msk) >> GDMA_CTL_TCIF_Pos)

/**
 * @brief       This macro is used to clear GDMA transfer complete interrupt flag
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      None
 * @details     This macro will clear GDMA transfer complete interrupt flag.
 * \hideinitializer
 */
#define GDMA_CLR_TRANSFER_COMPLETE_FLAG(gdma)   (gdma->CTL &= ~(GDMA_CTL_TCIF_Msk))

/**
 * @brief       This macro is used to get GDMA transfer error status
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      Current GDMA transfer error status (1: transfer complete)
 * @details     This macro will return GDMA transfer error status.
 * \hideinitializer
 */
#define GDMA_GET_TRANSFER_ERROR_STATUS(gdma)   ((gdma->CTL & GDMA_CTL_GDMAERR_Msk) >> GDMA_CTL_GDMAERR_Pos)

/**
  * @brief      This macro is used to start the transfer of GDMA by software trigger
  * @param[in]  gdma             The pointer of the specified GDMA module
  * @return     None
  * @details    This macro will start the transfer of GDMA by software trigger.
  * \hideinitializer
  */
#define GDMA_SOFTWARE_TRG(gdma)   (gdma->CTL |= GDMA_CTL_SOFTTRG_Msk)

/**
 * @brief       This macro set GDMA enable
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      None
 * @details     This macro will enable GDMA.
 */
#define GDMA_ENABLE(gdma)   (gdma->CTL |= GDMA_CTL_GDMAEN_Msk)

/**
 * @brief       This macro set GDMA enable
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      None
 * @details     This macro will enable GDMA.
 */
#define GDMA_DISABLE(gdma)   (gdma->CTL &= ~GDMA_CTL_GDMAEN_Msk)

/**
 * @brief       This macro set the GDMA source base address. 
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @param[in]   u32SrcAddr       The GDMA destination base address. Valid values are between 0~0xFFFFFFFF;
 * @return      None
 * @details     This macro is used to set the GDMA source base address.
 * \hideinitializer
 */
#define GDMA_SET_SRC_BASE_ADDR(gdma, u32SrcAddr)   (gdma->SRCB = (uint32_t)u32SrcAddr)

/**
 * @brief       This macro set the GDMA destination base address. 
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @param[in]   u32DstAddr       The GDMA destination base address. Valid values are between 0~0xFFFFFFFF;
 * @return      None
 * @details     This macro is used to set the GDMA destination base address.
 * \hideinitializer
 */
#define GDMA_SET_DST_BASE_ADDR(gdma, u32DstAddr)   (gdma->DSTB = (uint32_t)u32DstAddr)

/**
 * @brief       This macro set the GDMA transfer count. 
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @param[in]   u32TransferCnt   The GDMA transfer count. Valid values are between 0~0x1FFF;
 * @return      None
 * @details     This macro is used to set the GDMA transfer count.
 * \hideinitializer
 */
#define GDMA_SET_TRANSFER_COUNT(gdma, u32TransferCnt)   (gdma->TCNT = u32TransferCnt)

/**
 * @brief       This macro get the current value of GDMA source address
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      Current value of GDMA source address
 * @details     This macro is used to get the current value of GDMA source address
 * \hideinitializer
 */
#define GDMA_GET_SRC_ADDR(gdma)   (gdma->CSRC)

/**
 * @brief       This macro get the current value of GDMA destination address
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      Current value of GDMA destination address
 * @details     This macro is used to get the current value of GDMA destination address
 * \hideinitializer
 */
#define GDMA_GET_DST_ADDR(gdma)   (gdma->CDST)

/**
 * @brief       This macro get the current value of GDMA transfer count
 * @param[in]   gdma             The pointer of the specified GDMA module
 * @return      Current value of GDMA transfer count
 * @details     This macro is used to get the current value of GDMA transfer count
 * \hideinitializer
 */
#define GDMA_GET_CURRENT_TRANSFER_COUNT(gdma)   (gdma->CTCNT)

/**
 *    @brief        This macro enables the interrupt of GDMA module
 *
 *    @param[in]    gdma            The pointer of the specified GDMA module.
 *
 *    @return       None
 *
 *    @details      The macro is used to enable GDMA interrupt events
 */
#define GDMA_EnableINT(gdma)   (gdma->CTL |= GDMA_CTL_GIEN_Msk)

/**
 *    @brief        This macro disables the interrupt of GDMA module
 *
 *    @param[in]    gdma            The pointer of the specified GDMA module.
 *
 *    @return       None
 *
 *    @details      The macro is used to disable GDMA interrupt events
 */
#define GDMA_DisableINT(gdma)   (gdma->CTL &= ~GDMA_CTL_GIEN_Msk)

void GDMA_OPEN(GDMA_T* gdma, uint32_t u32ModeMask, uint32_t u32TransferWidth, uint32_t u32BurstMask, uint32_t u32AddressMask);


/*@}*/ /* end of group NM1240_GDMA_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_GDMA_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

#ifdef __cplusplus
}
#endif

#endif 

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
