/**************************************************************************//**
 * @file     op.h
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/06/03 17:32 $
 * @brief    NM1240 OP driver header file
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __OP_H__
#define __OP_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_OP_Driver OP Driver
  @{
*/

/** @addtogroup NM1240_OP_EXPORTED_CONSTANTS OP Exported Constants
  @{
*/

/** @addtogroup NM1240_OP_EXPORTED_FUNCTIONS OP Exported Functions
  @{
*/

/**
  * @brief      Enable OP
  * @param[in]  op     The pointer of the specified OP module
  * @param[in]  u32Ch is OP channel. Including :
  *             - \ref OP_CTL_OP1EN_Msk
  * @return     None
  * @details    This macro enable OP
  */
#define OP_ENABLE(op, u32Ch)     (op->CTL |= u32Ch)

/**
  * @brief      Disable OP
  * @param[in]  op     The pointer of the specified OP module
  * @param[in]  u32Ch is OP channel. Including :
  *             - \ref OP_CTL_OP1EN_Msk
  * @return     None
  * @details    This macro disable OP
  */
#define OP_DISABLE(op, u32Ch)    (op->CTL &= ~u32Ch)

/*@}*/ /* end of group NM1240_OP_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_OP_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __OP_H__ */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
