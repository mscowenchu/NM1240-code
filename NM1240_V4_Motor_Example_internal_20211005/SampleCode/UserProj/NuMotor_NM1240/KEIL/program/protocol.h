
#include "NM1240.h"
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define UART_USE            2   //Select UART channel
#define UART_GDMA_ENABLE    1   //Enable GDMA

#define UART_TX_PORT        PD  //UART TX PORT could be PA, PB, PC, PD, PE or PF.
#define UART_TX_PIN         7   //UART TX PIN could be 0~7.

#define UART_RX_PORT        PE  //UART RX PORT could be PA, PB, PC, PD, PE or PF.
#define UART_RX_PIN         0   //UART RX PIN could be 0~7.

#if UART_USE == 0
  #define UART_CH  UUART0
  #define USCI_CH_IRQ USCI0_IRQn
  #define USCI_IRQHandler USCI0_IRQHandler
#elif UART_USE == 1
  #define UART_CH UUART1
  #define USCI_CH_IRQ USCI1_IRQn
  #define USCI_IRQHandler USCI1_IRQHandler
#elif UART_USE == 2
  #define UART_CH  UUART2
  #define USCI_CH_IRQ USCI2_IRQn
  #define USCI_IRQHandler USCI2_IRQHandler
#endif

#define RX_size 7     //PC command length
#define TX_size 100

typedef union{
    uint32_t LONG;
    struct{
        uint16_t LWord;
        uint16_t HWord;
    }Word;
    struct{
        uint8_t LLByte;
        uint8_t LHByte;
        uint8_t HLByte;
        uint8_t HHByte;
    }Byte;
}MASK_T;

typedef union{
    int32_t LONG;
    struct{
        int16_t LWord;
        int16_t HWord;
    }Word;
    struct{
        int8_t LLByte;
        int8_t LHByte;
        int8_t HLByte;
        int8_t HHByte;
    }Byte;
}VAR_T;

//--- Data structure for register test
typedef struct
{
    uint8_t  name[50];   // Register name
    uint32_t address;
    MASK_T   bit_mask;
    VAR_T step_value;
    VAR_T min_value;
    VAR_T max_value;
    uint8_t signed_int;
}CMD_TypeDef;

typedef struct
{
    uint8_t  name[50];   // Register name
    uint32_t address;    // Register address
    MASK_T   bit_mask;   // 8, 16, 32 bit
    VAR_T min_value;     // min value (int32)
    VAR_T max_value;     // max value (int32)
    uint8_t signed_int;  // 0:unsigned, 1:signed
}VAR_STRUCT_TypeDef;

typedef struct
{
VAR_T data;
uint8_t changed_flag;
}Temp_T;

typedef struct
{
  uint8_t flag;
  uint8_t cmd_group;
} Refresh_req_TypeDef;

#define SYS_SIZE sizeof(SYS_LIST)/72
#define GROUP0_CMD_SIZE sizeof(GROUP0_CMD_LIST)/76
#define GROUP1_CMD_SIZE sizeof(GROUP1_CMD_LIST)/76
#define GROUP2_CMD_SIZE sizeof(GROUP2_CMD_LIST)/76
#define GROUP3_CMD_SIZE sizeof(GROUP3_CMD_LIST)/76
#define INFO_SIZE sizeof(INFO_LIST)/72
#define MICRO_SIZE sizeof(MICRO_LIST)/72
#define MACRO_SIZE sizeof(MACRO_LIST)/72

#define B8_Mask 0x000000FF
#define B16_Mask 0x0000FFFF
#define B32_Mask 0xFFFFFFFF

#define Fun_uart_check 0x70
#define Fun_reset_mcu 0x88

#define Fun_initial_SYS_name 0x50
#define Fun_initial_CMD_name 0x51
#define Fun_initial_INFO_name 0x52
#define Fun_initial_MICRO_name 0x53
#define Fun_initial_MACRO_name 0x54
#define Fun_initial_end 0x55

#define Fun_initial_SYS 0x60
#define Fun_initial_CMD 0x61
#define Fun_initial_INFO 0x62
#define Fun_initial_MICRO 0x63
#define Fun_initial_MACRO 0x64

#define Fun_update_microcosm_fig 0x30
#define Fun_update_macrocosm_fig 0x31
#define Fun_tx_information 0x32
#define Fun_update_time_scale 0x33
#define Fun_rx_command 0x34
#define Fun_rx_sys_command 0x35
#define Fun_refresh_command 0x36
#define Fun_refresh_sys 0x37

#define Fun_read_data_flash_to_GUI 0x40
#define Fun_write_data_flash_from_GUI 0x41
#define Fun_erase_data_flash 0x42

#define Fun_end 0xCC

//DataFlash
#define DATA_FLASH_SIZE		512 	//unit byte
#define DATA_FLASH_BASE		(FMC_APROM_END - DATA_FLASH_SIZE)

#define GROUP0_CMD_BASE (uint32_t)&GROUP0_CMD_LIST[0].name[0]
#define GROUP1_CMD_BASE (uint32_t)&GROUP1_CMD_LIST[0].name[0]
#define GROUP2_CMD_BASE (uint32_t)&GROUP2_CMD_LIST[0].name[0]
#define GROUP3_CMD_BASE (uint32_t)&GROUP3_CMD_LIST[0].name[0]

#define GROUP0_TEMP_BASE (uint32_t)&GROUP0_CMD_temp[0].data
#define GROUP1_TEMP_BASE (uint32_t)&GROUP1_CMD_temp[0].data
#define GROUP2_TEMP_BASE (uint32_t)&GROUP2_CMD_temp[0].data
#define GROUP3_TEMP_BASE (uint32_t)&GROUP3_CMD_temp[0].data

#if UART_GDMA_ENABLE == 0
    #define UART_TX(uart, tx_tcnt) {\
        uint16_t tx_cnt = 0;\
        __disable_irq();\
        UUART_CLR_PROT_INT_FLAG(uart, UUART_PROTSTS_TXENDIF_Msk);\
        uart->TXDAT = TX_data[tx_cnt++];\
        while (tx_cnt<tx_tcnt) {\
            if (uart->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) {\
              UUART_CLR_PROT_INT_FLAG(uart, UUART_PROTSTS_TXENDIF_Msk);\
              uart->TXDAT = TX_data[tx_cnt++];\
            }\
        }\
        while ((uart->PROTSTS & UUART_PROTSTS_TXENDIF_Msk) == 0 );\
        __enable_irq();\
    }
#elif UART_GDMA_ENABLE == 1
    #define UART_TX(uart, tx_tcnt) {\
        GDMA1->TCNT = tx_tcnt;\
        GDMA_ENABLE(GDMA1);\
    }
#endif

void initial_SYS_name(uint8_t sys_index);
void initial_CMD_name(uint8_t cmd_index, CMD_TypeDef* CMD_LIST, const uint8_t size);
void initial_INFO_name(uint8_t info_index);
void initial_MICRO_name(uint8_t micro_index);
void initial_MACRO_name(uint8_t macro_index);

void initial_SYS(uint8_t sys_index);
void initial_CMD(uint8_t cmd_index, CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp);
void initial_INFO(uint8_t info_index);
void initial_micro_fig(uint8_t micro_index);
void initial_macro_fig(uint8_t macro_index);

void Update_microcosm_fig(void);
void Update_macrocosm_fig(void);
void TX_information(void);
void Update_time_scale(uint8_t rx_data[]);
void Command_update(void);
void RX_system_command(uint8_t rx_data[]);
void RX_command(uint8_t rx_data[], CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp);
void Refresh(CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp, const uint8_t size);
void Refresh_sys(void);

void TX_data_to_PC(int16_t rotor_speed, uint16_t min_cmd_speed_rpm);
void Trigger_update_micro_fig(void);
void Request_decode(uint8_t rx_data[]);

void Initialize_FMC(void);
uint8_t data_flash_is_empty(void);
void read_data_flash_to_CMD_temp(void);
void read_data_flash_to_GUI(uint8_t cmd_group, const uint8_t size);
void write_data_flash_from_GUI(CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp, uint8_t cmd_group, const uint8_t size);
void erase_data_flash(void);
void Load_from_data_flash(void);
void Initialize_USCI_UART(void);
#endif
