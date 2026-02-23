/**************************************************************************//**
 * @file     usci_i2c.h
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 2020/07/24 14:24 $
 * @brief    NM1240 USCI I2C(UI2C) driver header file
 *
 * @note
 * Copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#ifndef __USCI_I2C_H__
#define __USCI_I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup NM1240_Device_Driver NM1240 Device Driver
  @{
*/

/** @addtogroup NM1240_USCI_I2C_Driver UI2C Driver (USCI I2C Mode Driver)
  @{
*/

/** @addtogroup NM1240_USCI_I2C_EXPORTED_CONSTANTS UI2C Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/* USCI_I2C master event definitions                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
enum UI2C_MASTER_EVENT { 
                             MASTER_SEND_ADDRESS = 10,    /*!< Master send address to Slave */
                             MASTER_SEND_H_WR_ADDRESS,    /*!< Master send High address to Slave */
                             MASTER_SEND_H_RD_ADDRESS,    /*!< Master send address to Slave (Read ADDR) */
                             MASTER_SEND_L_ADDRESS,       /*!< Master send Low address to Slave */
                             MASTER_SEND_DATA,            /*!< Master Send Data to Slave */
                             MASTER_SEND_REPEAT_START,    /*!< Master send repeat start to Slave */
                             MASTER_READ_DATA,            /*!< Master Get Data from Slave */
                             MASTER_STOP,                 /*!< Master send stop to Slave */
                             MASTER_SEND_START            /*!< Master send start to Slave */
                           };

/*---------------------------------------------------------------------------------------------------------*/
/* USCI_I2C slave event definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
enum UI2C_SLAVE_EVENT {
                            SLAVE_ADDRESS_ACK = 100,      /*!< Slave send address ACK */
                            SLAVE_H_WR_ADDRESS_ACK,       /*!< Slave send High address ACK */
                            SLAVE_L_WR_ADDRESS_ACK,       /*!< Slave send Low address ACK */
                            SLAVE_GET_DATA,               /*!< Slave Get Data from Master (Write CMD) */
                            SLAVE_SEND_DATA,              /*!< Slave Send Data to Master (Read CMD) */
                            SLAVE_H_RD_ADDRESS_ACK,       /*!< Slave send High address ACK */
                            SLAVE_L_RD_ADDRESS_ACK        /*!< Slave send Low address ACK */
                          };

/*---------------------------------------------------------------------------------------------------------*/
/*  USCI_CTL constant definitions.                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define UI2C_CTL_PTRG              0x20UL    /*!< USCI_CTL setting for I2C control bits. It would set PTRG bit */
#define UI2C_CTL_STA               0x08UL    /*!< USCI_CTL setting for I2C control bits. It would set STA bit */
#define UI2C_CTL_STO               0x04UL    /*!< USCI_CTL setting for I2C control bits. It would set STO bit */
#define UI2C_CTL_AA                0x02UL    /*!< USCI_CTL setting for I2C control bits. It would set AA bit  */

/*---------------------------------------------------------------------------------------------------------*/
/*  USCI_I2C GCMode constant definitions.                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define UI2C_GCMODE_ENABLE         1    /*!< Enable  USCI_I2C GC Mode */
#define UI2C_GCMODE_DISABLE        0    /*!< Disable USCI_I2C GC Mode */

/*---------------------------------------------------------------------------------------------------------*/
/*  USCI_I2C Wakeup Mode constant definitions.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define UI2C_DATA_TOGGLE_WK        (0x0 << UI2C_WKCTL_WKADDREN_Pos)    /*!< Wakeup according data toggle */
#define UI2C_ADDR_MATCH_WK         (0x1 << UI2C_WKCTL_WKADDREN_Pos)    /*!< Wakeup according address match */

/*---------------------------------------------------------------------------------------------------------*/
/* USCI_I2C interrupt mask definitions                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define UI2C_TO_INT_MASK           (0x001)    /*!< Time-out interrupt mask */
#define UI2C_STAR_INT_MASK         (0x002)    /*!< Start condition received interrupt mask */
#define UI2C_STOR_INT_MASK         (0x004)    /*!< Stop condition received interrupt mask */
#define UI2C_NACK_INT_MASK         (0x008)    /*!< Non-acknowledge interrupt mask */
#define UI2C_ARBLO_INT_MASK        (0x010)    /*!< Arbitration lost interrupt mask */
#define UI2C_ERR_INT_MASK          (0x020)    /*!< Error interrupt mask */
#define UI2C_ACK_INT_MASK          (0x040)    /*!< Acknowledge interrupt mask */

/*---------------------------------------------------------------------------------------------------------*/
/* USCI_I2C Status mask definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define UI2C_ONBUSY_MASK           (0x001)    /*!< On bus busy status mask */
#define UI2C_SLASEL_MASK           (0x002)    /*!< Slave select status mask */
#define UI2C_SLAREAD_MASK          (0x004)    /*!< Slave read request status mask */
#define UI2C_WKAKDONE_MASK         (0x008)    /*!< Wakeup address frame acknowledge bit done mask */
#define UI2C_WRSTSWK_MASK          (0x010)    /*!< Read/Write status bit in address wakeup frame mask */
#define UI2C_BUSHANG_MASK          (0x020)    /*!< Bus hang-up status mask */

/*@}*/ /* end of group NM1240_USCI_I2C_EXPORTED_CONSTANTS */


/** @addtogroup NM1240_USCI_I2C_EXPORTED_FUNCTIONS UI2C Exported Functions
  @{
*/
 
/**
 *    @brief        This macro sets the USCI_I2C protocol control register at one time
 *  
 *    @param[in]    ui2c      The pointer of the specified USCI_I2C module.
 *    @param[in]    u8Ctrl    Set the register value of USCI_I2C control register.
 *                  - \ref UI2C_CTL_PTRG
 *                  - \ref UI2C_CTL_STA
 *                  - \ref UI2C_CTL_STO
 *                  - \ref UI2C_CTL_AA
 *    @return       None
 *
 *    @details      Set UI2C_PROTCTL register to control USCI_I2C bus conditions of START, STOP, SI, ACK.
 */
#define UI2C_SET_CONTROL_REG(ui2c, u8Ctrl) ((ui2c)->PROTCTL = ((ui2c)->PROTCTL & ~0x2E) | u8Ctrl)

/**
 *    @brief        This macro only set START bit to protocol control register of USCI_I2C module.
 *
 *    @param[in]    ui2c      The pointer of the specified USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      Set the USCI_I2C bus START condition in UI2C_PROTCTL register.
 */
#define UI2C_START(ui2c) ((ui2c)->PROTCTL = ((ui2c)->PROTCTL & ~UI2C_PROTCTL_PTRG_Msk) | UI2C_PROTCTL_STA_Msk)

/**
 *    @brief        This macro only set STOP bit to the control register of USCI_I2C module
 *
 *    @param[in]    ui2c      The pointer of the specified USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      Set the USCI_I2C bus STOP condition in UI2C_PROTCTL register.
 */
#define UI2C_STOP(ui2c) ((ui2c)->PROTCTL = ((ui2c)->PROTCTL & ~0x2E) | (UI2C_PROTCTL_PTRG_Msk | UI2C_PROTCTL_STO_Msk))

/**
 *    @brief        This macro returns the data stored in data register of USCI_I2C module
 *
 *    @param[in]    ui2c      The pointer of the specified USCI_I2C module.
 *
 *    @return       Data
 *
 *    @details      Read a byte data value of UI2C_RXDAT register from USCI_I2C bus
 */
#define UI2C_GET_DATA(ui2c) ((ui2c)->RXDAT)

/**
 *    @brief        This macro writes the data to data register of USCI_I2C module
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *    @param[in]    u8Data   The data which will be written to data register of USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      Write a byte data value of UI2C_TXDAT register, then sends address or data to USCI I2C bus 
 */
#define UI2C_SET_DATA(ui2c, u8Data) ((ui2c)->TXDAT = u8Data)

/**
 *    @brief        This macro returns time-out flag
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @retval       0        USCI_I2C bus time-out is not happened
 *    @retval       1        USCI_I2C bus time-out is happened
 *
 *    @details      USCI_I2C bus occurs time-out event, the time-out flag will be set. If not occurs time-out event, this bit is cleared.
 */
#define UI2C_GET_TIMEOUT_FLAG(ui2c) (((ui2c)->PROTSTS & UI2C_PROTSTS_TOIF_Msk) == UI2C_PROTSTS_TOIF_Msk ? 1:0)

/**
 *    @brief        This macro returns wakeup flag
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @retval       0        Chip is not wakeup from power-down mode
 *    @retval       1        Chip is wakeup from power-down mode
 *
 *    @details      USCI_I2C controller wakeup flag will be set when USCI_I2C bus occurs wakeup from deep-sleep.
 */
#define UI2C_GET_WAKEUP_FLAG(ui2c) (((ui2c)->WKSTS & UI2C_WKSTS_WKF_Msk) == UI2C_WKSTS_WKF_Msk ? 1:0)

/**
 *    @brief        This macro is used to clear USCI_I2C wakeup flag
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      If USCI_I2C wakeup flag is set, use this macro to clear it.
 */
#define UI2C_CLR_WAKEUP_FLAG(ui2c)  ((ui2c)->WKSTS = UI2C_WKSTS_WKF_Msk)

/**
 *    @brief        This macro disables the USCI_I2C 10-bit address mode
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      The UI2C_I2C is 7-bit address mode, when disable USCI_I2C 10-bit address match function.
 */
#define UI2C_DISABLE_10BIT_ADDR_MODE(ui2c)  ((ui2c)->PROTCTL &= ~(UI2C_PROTCTL_ADDR10EN_Msk))

/**
 *    @brief        This macro enables the 10-bit address mode
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @return       None
 *
 *    @details      To enable USCI_I2C 10-bit address match function.
 */
#define UI2C_ENABLE_10BIT_ADDR_MODE(ui2c)  ((ui2c)->PROTCTL |= UI2C_PROTCTL_ADDR10EN_Msk)

/**
 *    @brief        This macro gets USCI_I2C protocol interrupt flag or bus status
 *
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *
 *    @return       A word data of USCI_I2C_PROTSTS register
 *
 *    @details      Read a word data of USCI_I2C PROTSTS register to get USCI_I2C bus Interrupt flags or status.
 */
#define UI2C_GET_PROT_STATUS(ui2c)    ((ui2c)->PROTSTS)

/**
  * @brief  Get USCI_I2C status.
  * @param[in]  ui2c The pointer of the specified USCI_I2C module.
  * @param[in]  u32Mask The combination of all related sources.
  *         Each bit corresponds to a source.
  *         This parameter decides which status will be read. It is combination of:
  *           - \ref UI2C_PROTSTS_ONBUSY_Msk
  *           - \ref UI2C_PROTSTS_SLASEL_Msk
  *           - \ref UI2C_PROTSTS_SLAREAD_Msk
  *           - \ref UI2C_PROTSTS_WKAKDONE_Msk
  *           - \ref UI2C_PROTSTS_WRSTSWK_Msk
  *           - \ref UI2C_PROTSTS_BUSHANG_Msk
  * @return Flags of selected sources.
  */
#define UI2C_GetStatus(ui2c, u32Mask)    (ui2c->PROTSTS & u32Mask)

/**
 *    @brief        This macro clears specified protocol interrupt flag
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *    @param[in]    u32IntTypeFlag Interrupt Type Flag, should be
 *                                  - \ref UI2C_PROTSTS_ACKIF_Msk
 *                                  - \ref UI2C_PROTSTS_ERRIF_Msk
 *                                  - \ref UI2C_PROTSTS_ARBLOIF_Msk
 *                                  - \ref UI2C_PROTSTS_NACKIF_Msk
 *                                  - \ref UI2C_PROTSTS_STORIF_Msk
 *                                  - \ref UI2C_PROTSTS_STARIF_Msk
 *                                  - \ref UI2C_PROTSTS_TOIF_Msk
 *    @return None
 *
 *    @details      To clear interrupt flag when USCI_I2C occurs interrupt and set interrupt flag.
 */
#define UI2C_CLR_PROT_INT_FLAG(ui2c,u32IntTypeFlag)    ((ui2c)->PROTSTS = u32IntTypeFlag)

/**
 *    @brief        This macro enables specified protocol interrupt
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *    @param[in]    u32IntSel Interrupt Type, should be
 *                                  - \ref UI2C_PROTIEN_ACKIEN_Msk
 *                                  - \ref UI2C_PROTIEN_ERRIEN_Msk
 *                                  - \ref UI2C_PROTIEN_ARBLOIEN_Msk
 *                                  - \ref UI2C_PROTIEN_NACKIEN_Msk
 *                                  - \ref UI2C_PROTIEN_STORIEN_Msk
 *                                  - \ref UI2C_PROTIEN_STARIEN_Msk
 *                                  - \ref UI2C_PROTIEN_TOIEN_Msk
 *    @return None
 *
 *    @details      Set specified USCI_I2C protocol interrupt bits to enable interrupt function.
 */
#define UI2C_ENABLE_PROT_INT(ui2c, u32IntSel)    ((ui2c)->PROTIEN |= (u32IntSel))

/**
 *    @brief        This macro disables specified protocol interrupt
 *    @param[in]    ui2c     The pointer of the specified USCI_I2C module.
 *    @param[in]    u32IntSel Interrupt Type, should be
 *                                  - \ref UI2C_PROTIEN_ACKIEN_Msk
 *                                  - \ref UI2C_PROTIEN_ERRIEN_Msk
 *                                  - \ref UI2C_PROTIEN_ARBLOIEN_Msk  
 *                                  - \ref UI2C_PROTIEN_NACKIEN_Msk
 *                                  - \ref UI2C_PROTIEN_STORIEN_Msk
 *                                  - \ref UI2C_PROTIEN_STARIEN_Msk
 *                                  - \ref UI2C_PROTIEN_TOIEN_Msk
 *    @return None
 *
 *    @details      Clear specified USCI_I2C protocol interrupt bits to disable interrupt function.
 */
#define UI2C_DISABLE_PROT_INT(ui2c, u32IntSel)    ((ui2c)->PROTIEN &= ~ (u32IntSel))

/**
  * @brief        This macro enables UI2C DMA Control.
  * @param[in]    ui2c     The pointer of the specified USCI_I2C module.
  * @param[in]    u32CtrlMask DMA control, should be (use '|' for multiple selections)
  *                                  - \ref UI2C_DMACTL_DMARST_Msk
  *                                  - \ref UI2C_DMACTL_TXDMAEN_Msk
  *                                  - \ref UI2C_DMACTL_RXDMAEN_Msk
  *                                  - \ref UI2C_DMACTL_DMAEN_Msk
  *                                  - \ref UI2C_DMACTL_NACKEN_Msk
  *                                  - \ref UI2C_DMACTL_STOPEN_Msk
  * @return None
  */
#define UI2C_ENABLE_DMA_CTL(ui2c, u32CtrlMask)    ((ui2c)->DMACTL |= (u32CtrlMask))

/**
  * @brief        This macro disables UI2C DMA Control.
  * @param[in]    ui2c     The pointer of the specified USCI_I2C module.
  * @param[in]    u32CtrlMask DMA control, should be (use '|' for multiple selections)
  *                                  - \ref UI2C_DMACTL_DMARST_Msk
  *                                  - \ref UI2C_DMACTL_TXDMAEN_Msk
  *                                  - \ref UI2C_DMACTL_RXDMAEN_Msk
  *                                  - \ref UI2C_DMACTL_DMAEN_Msk
  *                                  - \ref UI2C_DMACTL_NACKEN_Msk
  *                                  - \ref UI2C_DMACTL_STOPEN_Msk
  * @return None
  */
#define UI2C_DISABLE_DMA_CTL(ui2c, u32CtrlMask)    ((ui2c)->DMACTL &= ~(u32CtrlMask))

/**
 *    @brief        Get Tx empty
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0   Tx buffer is not empty
 *    @retval       >=1 Tx buffer is empty
 *
 *    @details      This macro get Transmitter buffer empty register value.
 */
#define UI2C_GET_TX_EMPTY(ui2c)    ((ui2c)->BUFSTS & UI2C_BUFSTS_TXEMPTY_Msk)

/**
 *    @brief        Get Rx empty
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0   Rx buffer is not empty
 *    @retval       >=1 Rx buffer is empty
 *
 *    @details      This macro get Receiver buffer empty register value.
 */
#define UI2C_GET_RX_EMPTY(ui2c)    ((ui2c)->BUFSTS & UI2C_BUFSTS_RXEMPTY_Msk)

/**
 *    @brief        Check specified usci_ui2c port transmission is over.
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0 Tx transmission is not over
 *    @retval       1 Tx transmission is over
 *
 *    @details      This macro return Transmitter Empty Flag register bit value. \n
 *                  It indicates if specified usci_i2c port transmission is over nor not.
 */
#define UI2C_IS_TX_EMPTY(ui2c)    (((ui2c)->BUFSTS & UI2C_BUFSTS_TXEMPTY_Msk) >> UI2C_BUFSTS_TXEMPTY_Pos)


/**
 *    @brief        Check specified usci_i2c port receiver is empty.
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0 Rx receiver is not empty
 *    @retval       1 Rx receiver is empty
 *
 *    @details      This macro return Receive Empty Flag register bit value. \n
 *                  It indicates if specified usci_i2c port receiver is empty nor not.
 */
#define UI2C_IS_RX_EMPTY(ui2c)    (((ui2c)->BUFSTS & UI2C_BUFSTS_RXEMPTY_Msk) >> UI2C_BUFSTS_RXEMPTY_Pos)


/**
 *    @brief        Wait specified usci_i2c port transmission is over
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @return       None
 *
 *    @details      This macro wait specified usci_i2c port transmission is over.
 */
#define UI2C_WAIT_TX_EMPTY(ui2c)    while(!((((ui2c)->BUFSTS) & UI2C_BUFSTS_TXEMPTY_Msk) >> UI2C_BUFSTS_TXEMPTY_Pos)) {;}


/**
 *    @brief        Check TX buffer is full or not
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       1 TX buffer is full
 *    @retval       0 TX buffer is not full
 *
 *    @details      This macro check TX buffer is full or not.
 */
#define UI2C_IS_TX_FULL(ui2c)    (((ui2c)->BUFSTS & UI2C_BUFSTS_TXFULL_Msk)>>UI2C_BUFSTS_TXFULL_Pos)


/**
 *    @brief        Check RX buffer is full or not
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       1 RX buffer is full
 *    @retval       0 RX buffer is not full
 *
 *    @details      This macro check RX buffer is full or not.
 */
#define UI2C_IS_RX_FULL(ui2c)    (((ui2c)->BUFSTS & UI2C_BUFSTS_RXFULL_Msk)>>UI2C_BUFSTS_RXFULL_Pos)


/**
 *    @brief        Get Tx full register value
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0   Tx buffer is not full.
 *    @retval       >=1 Tx buffer is full.
 *
 *    @details      This macro get Tx full register value.
 */
#define UI2C_GET_TX_FULL(ui2c)    ((ui2c)->BUFSTS & UI2C_BUFSTS_TXFULL_Msk)


/**
 *    @brief        Get Rx full register value
 *
 *    @param[in]    ui2c    The pointer of the specified USCI_I2C module
 *
 *    @retval       0   Rx buffer is not full.
 *    @retval       >=1 Rx buffer is full.
 *
 *    @details      This macro get Rx full register value.
 */
#define UI2C_GET_RX_FULL(ui2c)    ((ui2c)->BUFSTS & UI2C_BUFSTS_RXFULL_Msk)

uint32_t UI2C_Open(UI2C_T *ui2c, uint32_t u32BusClock);
void UI2C_Close(UI2C_T *ui2c);
void UI2C_ClearTimeoutFlag(UI2C_T *ui2c);
void UI2C_Trigger(UI2C_T *ui2c, uint8_t u8Start, uint8_t u8Stop, uint8_t u8Ptrg, uint8_t u8Ack);
void UI2C_DisableInt(UI2C_T *ui2c, uint32_t u32Mask);
void UI2C_EnableInt(UI2C_T *ui2c, uint32_t u32Mask);
uint32_t UI2C_GetBusClockFreq(UI2C_T *ui2c);
uint32_t UI2C_SetBusClockFreq(UI2C_T *ui2c, uint32_t u32BusClock);
uint32_t UI2C_GetIntFlag(UI2C_T *ui2c, uint32_t u32Mask);
void UI2C_ClearIntFlag(UI2C_T* ui2c , uint32_t u32Mask);
uint32_t UI2C_GetData(UI2C_T *ui2c);
void UI2C_SetData(UI2C_T *ui2c, uint8_t u8Data);
void UI2C_SetSlaveAddr(UI2C_T *ui2c, uint8_t u8SlaveNo, uint16_t u16SlaveAddr, uint8_t u8GCMode);
void UI2C_SetSlaveAddrMask(UI2C_T *ui2c, uint8_t u8SlaveNo, uint16_t u16SlaveAddrMask);
void UI2C_EnableTimeout(UI2C_T *ui2c, uint32_t u32TimeoutCnt);
void UI2C_DisableTimeout(UI2C_T *ui2c);
void UI2C_EnableWakeup(UI2C_T *ui2c, uint8_t u8WakeupMode);
void UI2C_DisableWakeup(UI2C_T *ui2c);

/*@}*/ /* end of group NM1240_USCI_I2C_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NM1240_USCI_I2C_Driver */

/*@}*/ /* end of group NM1240_Device_Driver */

#ifdef __cplusplus
}
#endif

#endif

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/
