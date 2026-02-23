#include "NM1240.h"
#include "protocol.h"
#include "system_initialize.h"

/*
  =========Protocol description=================================================================
  
  ==============================================================================================
*/

#if defined(__CC_ARM)
// Suppress warning message: extended constant initialiser used
#pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif
VAR_STRUCT_TypeDef const SYS_LIST[] =
{
    // name                                   , command address                               , bit       , min   , max   , signed
    {"MotorA.cmd.u8_start\n"                  , (uint32_t)&MotorA.cmd.u8_start                , B8_Mask   , 0     , 1     , 0}
};

CMD_TypeDef const GROUP0_CMD_LIST[] =
{
    // name                                   , command address                               , bit       , step  , min   , max   , signed
    {"MotorA.cmd.u8_direction\n"              , (uint32_t)&MotorA.cmd.u8_direction            , B8_Mask   , 1     , 0     , 1     , 0},
    {"MotorA.cmd.i16_rotor_speed_target\n"    , (uint32_t)&MotorA.cmd.i16_rotor_speed_target  , B16_Mask  , 500   , 0     , 3000  , 1},
    {"MotorA.cmd.i16_Duty0\n"                 , (uint32_t)&MotorA.cmd.i16_Duty0               , B16_Mask  , 10    , 0     , 100   , 1}
};

CMD_TypeDef const GROUP1_CMD_LIST[] =
{
    // name                                   , command address                               , bit       , step  , min   , max         , signed
    {"PI_Speed.kp_15\n"                       , (uint32_t)&PI_Speed.kp_15                     , B32_Mask  , 2000  , 0     , 0x7FFFFFFF  , 0},
    {"PI_Speed.ki_15\n"                       , (uint32_t)&PI_Speed.ki_15                     , B32_Mask  , 50    , 0     , 0x7FFFFFFF  , 0}
};

CMD_TypeDef const GROUP2_CMD_LIST[] =
{
    // name                                   , command address                               , bit       , step  , min   , max         , signed
    {"PI_Iq.kp_15\n"                          , (uint32_t)&PI_Iq.kp_15                        , B32_Mask  , 50    , 0     , 0x7FFFFFFF  , 0},
    {"PI_Iq.ki_15\n"                          , (uint32_t)&PI_Iq.ki_15                        , B32_Mask  , 10    , 0     , 0x7FFFFFFF  , 0}
};

CMD_TypeDef const GROUP3_CMD_LIST[] =
{
    // name                                   , command address                               , bit       , step  , min   , max         , signed
    {"PI_Id.kp_15\n"                          , (uint32_t)&PI_Id.kp_15                        , B32_Mask  , 50    , 0     , 0x7FFFFFFF  , 0},
    {"PI_Id.ki_15\n"                          , (uint32_t)&PI_Id.ki_15                        , B32_Mask  , 10    , 0     , 0x7FFFFFFF  , 0}
};

VAR_STRUCT_TypeDef const INFO_LIST[] =
{
    // name                                   , information address                           , bit      , min     , max   , signed
    {"MotorA.cmd.i16_rotor_speed_target\n"    , (uint32_t)&MotorA.cmd.i16_rotor_speed_target  , B16_Mask , -5000   , 5000  , 1},
    {"MotorA.info.i16_rotor_speed\n"          , (uint32_t)&MotorA.info.i16_rotor_speed        , B16_Mask , -5000   , 5000  , 1},
    {"MotorA.info.i16_Id\n"                   , (uint32_t)&MotorA.info.i16_Id                 , B16_Mask , -32768  , 32767 , 1},
    {"MotorA.info.i16_Iq\n"                   , (uint32_t)&MotorA.info.i16_Iq                 , B16_Mask , -32768  , 32767 , 1}
};

VAR_STRUCT_TypeDef const MICRO_LIST[] =
{
    // name                                   ,  microcosm address                            , bit       , min     , max   , signed
    {"MotorA.info.i16_Iu\n"                   , (uint32_t)&MotorA.info.i16_Iu                 , B16_Mask  , -32768  , 32767 , 1},
    {"MotorA.info.i16_Iv\n"                   , (uint32_t)&MotorA.info.i16_Iv                 , B16_Mask  , -32768  , 32767 , 1},
    {"MotorA.info.i16_Iw\n"                   , (uint32_t)&MotorA.info.i16_Iw                 , B16_Mask  , -32768  , 32767 , 1},
    {"MotorA.cmd.i16_angle\n"                 , (uint32_t)&MotorA.cmd.i16_angle               , B16_Mask  , 0       , 1024  , 1},
    {"MotorA.info.i16_Id\n"                   , (uint32_t)&MotorA.info.i16_Id                 , B16_Mask  , -32768  , 32767 , 1},
    {"MotorA.info.i16_Iq\n"                   , (uint32_t)&MotorA.info.i16_Iq                 , B16_Mask  , -32768  , 32767 , 1}
};

VAR_STRUCT_TypeDef const MACRO_LIST[] =
{
    // name                                   , macrocosm address                             , bit       , min     , max   , signed
    {"MotorA.cmd.i16_rotor_speed_target\n"    , (uint32_t)&MotorA.cmd.i16_rotor_speed_target  , B16_Mask  , -5000   , 5000  , 1},
    {"MotorA.info.i16_rotor_speed\n"          , (uint32_t)&MotorA.info.i16_rotor_speed        , B16_Mask  , -5000   , 5000  , 1},
    {"MotorA.info.i16_Id\n"                   , (uint32_t)&MotorA.info.i16_Id                 , B16_Mask  , -32768  , 32767 , 1},
    {"MotorA.info.i16_Iq\n"                   , (uint32_t)&MotorA.info.i16_Iq                 , B16_Mask  , -32768  , 32767 , 1}
};

uint16_t TIME_SCALE_CNT = 151;
uint8_t Update_microcosm_fig_req = 0;
uint8_t Update_macrocosm_fig_req = 0;
uint8_t Tx_information_req = 0;
uint8_t Update_microcosm_fig_trigger = 0;
Refresh_req_TypeDef Refresh_group_req = {0, 0};
uint8_t Refresh_sys_req = 0;
uint32_t NVIC_IRQ_TEMP, PWM_CTL_TEMP;

uint8_t TIME_SCALE = 2;

uint8_t RX_data[RX_size] = {0};
uint8_t TX_data[TX_size] = {0};
uint8_t UART_TX_TCNT = 0;
uint8_t UART_RX_CNT = 0;
uint16_t MICRO_TX_CNT;
uint32_t GROUP_CMD_BASE[4] = {GROUP0_CMD_BASE, GROUP1_CMD_BASE, GROUP2_CMD_BASE, GROUP3_CMD_BASE};
uint8_t GROUP_CMD_SIZE[4] = {GROUP0_CMD_SIZE, GROUP1_CMD_SIZE, GROUP2_CMD_SIZE, GROUP3_CMD_SIZE};


Temp_T GROUP0_CMD_temp[GROUP0_CMD_SIZE];
Temp_T GROUP1_CMD_temp[GROUP1_CMD_SIZE];
Temp_T GROUP2_CMD_temp[GROUP2_CMD_SIZE];
Temp_T GROUP3_CMD_temp[GROUP3_CMD_SIZE];
Temp_T SYS_temp[SYS_SIZE];

uint32_t GROUP_TEMP_BASE[4] = {GROUP0_TEMP_BASE, GROUP1_TEMP_BASE, GROUP2_TEMP_BASE, GROUP3_TEMP_BASE};
//====================================================

void initial_SYS_name(uint8_t sys_index)
{
    uint8_t name_width_cnt = 0;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    if (sys_index < SYS_SIZE) {
        //function code: initial command name;
        TX_data[0] = Fun_initial_SYS_name;
    }
    else {
        // Transfer a function code of the end
        TX_data[0] = Fun_end;
        TX_data[1] = '\n';
        UART_TX(UART_CH, 2);
        return;
    }
    // Start to read command name to TX_data[] buffer and transfer to PC.
    // Read array of command name until "\n"
    for (name_width_cnt=0;name_width_cnt<50;name_width_cnt++)
    {
        TX_data[name_width_cnt+1] = SYS_LIST[sys_index].name[name_width_cnt];
        if (SYS_LIST[sys_index].name[name_width_cnt] == '\n')
        {
            UART_TX_TCNT = name_width_cnt + 2;  // name width + function code + \n
            break;
        }
    }

    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_CMD_name(uint8_t cmd_index, CMD_TypeDef* CMD_LIST, const uint8_t size)
{
    uint8_t name_width_cnt = 0;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    if (cmd_index < size) {
        //function code: initial command name;
        TX_data[0] = Fun_initial_CMD_name;
    }
    else {
        // Transfer a function code of the end
        TX_data[0] = Fun_end;
        TX_data[1] = '\n';
        UART_TX(UART_CH, 2);
        return;
    }
    // Start to read command name to TX_data[] buffer and transfer to PC.
    // Read array of command name until "\n"
    for (name_width_cnt=0;name_width_cnt<50;name_width_cnt++)
    {
        TX_data[name_width_cnt+1] = CMD_LIST[cmd_index].name[name_width_cnt];
        if (CMD_LIST[cmd_index].name[name_width_cnt] == '\n')
        {
            UART_TX_TCNT = name_width_cnt + 2;  // name width + function code + \n
            break;
        }
    }

    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_INFO_name(uint8_t info_index)
{
    uint8_t name_width_cnt = 0;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    if (info_index < INFO_SIZE) {
        //function code: initial information name;
        TX_data[0] = Fun_initial_INFO_name;
    }
    else {
        // Transfer a function code of the end
        TX_data[0] = Fun_end;
        TX_data[1] = '\n';
        UART_TX(UART_CH, 2);
        return;
    }
    // Start to read information name to TX_data[] buffer and transfer to PC.
    // Read array of information name until "\n"
    for (name_width_cnt=0;name_width_cnt<50;name_width_cnt++)
    {
        TX_data[name_width_cnt+1] = INFO_LIST[info_index].name[name_width_cnt];
        if (INFO_LIST[info_index].name[name_width_cnt] == '\n')
        {
            UART_TX_TCNT = name_width_cnt + 2;  // name width + function code + \n
            break;
        }
    }

    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_MICRO_name(uint8_t micro_index)
{
    uint8_t name_width_cnt = 0;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    if (micro_index < MICRO_SIZE) {
        //function code: initial MICRO name;
        TX_data[0] = Fun_initial_MICRO_name;
    }
    else {
        // Transfer a function code of the end
        TX_data[0] = Fun_end;
        TX_data[1] = '\n';
        UART_TX(UART_CH, 2);
        return;
    }
    // Start to read MICRO name to TX_data[] buffer and transfer to PC.
    // Read array of MICRO name until "\n"
    for (name_width_cnt=0;name_width_cnt<50;name_width_cnt++)
    {
        TX_data[name_width_cnt+1] = MICRO_LIST[micro_index].name[name_width_cnt];
        if (MICRO_LIST[micro_index].name[name_width_cnt] == '\n')
        {
            UART_TX_TCNT = name_width_cnt + 2;  // name width + function code + \n
            break;
        }
    }

    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_MACRO_name(uint8_t macro_index)
{
    uint8_t name_width_cnt = 0;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    if (macro_index < MACRO_SIZE) {
        //function code: initial MACRO name;
        TX_data[0] = Fun_initial_MACRO_name;
    }
    else {
        // Transfer a function code of the end
        TX_data[0] = Fun_end;
        TX_data[1] = '\n';
        UART_TX(UART_CH, 2);
        return;
    }
    // Start to read MACRO name to TX_data[] buffer and transfer to PC.
    // Read array of MACRO name until "\n"
    for (name_width_cnt=0;name_width_cnt<50;name_width_cnt++)
    {
        TX_data[name_width_cnt+1] = MACRO_LIST[macro_index].name[name_width_cnt];
        if (MACRO_LIST[macro_index].name[name_width_cnt] == '\n')
        {
            UART_TX_TCNT = name_width_cnt + 2;  // name width + function code + \n
            break;
        }
    }

    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_SYS(uint8_t sys_index)
{
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_initial_SYS;
    TX_data[1] = SYS_LIST[sys_index].signed_int;
    /* bit */
    TX_data[2] = SYS_LIST[sys_index].bit_mask.Byte.HHByte;
    TX_data[3] = SYS_LIST[sys_index].bit_mask.Byte.HLByte;
    TX_data[4] = SYS_LIST[sys_index].bit_mask.Byte.LHByte;
    TX_data[5] = SYS_LIST[sys_index].bit_mask.Byte.LLByte;
    /* min_value */
    TX_data[6] = SYS_LIST[sys_index].min_value.Byte.HHByte;
    TX_data[7] = SYS_LIST[sys_index].min_value.Byte.HLByte;
    TX_data[8] = SYS_LIST[sys_index].min_value.Byte.LHByte;
    TX_data[9] = SYS_LIST[sys_index].min_value.Byte.LLByte;
    /* max_value */
    TX_data[10] = SYS_LIST[sys_index].max_value.Byte.HHByte;
    TX_data[11] = SYS_LIST[sys_index].max_value.Byte.HLByte;
    TX_data[12] = SYS_LIST[sys_index].max_value.Byte.LHByte;
    TX_data[13] = SYS_LIST[sys_index].max_value.Byte.LLByte;
    /* command_data */
    TX_data[14] = (*(uint8_t*) (SYS_LIST[sys_index].address + 3)) & SYS_LIST[sys_index].bit_mask.Byte.HHByte;
    TX_data[15] = (*(uint8_t*) (SYS_LIST[sys_index].address + 2)) & SYS_LIST[sys_index].bit_mask.Byte.HLByte;
    TX_data[16] = (*(uint8_t*) (SYS_LIST[sys_index].address + 1)) & SYS_LIST[sys_index].bit_mask.Byte.LHByte;
    TX_data[17] = (*(uint8_t*) (SYS_LIST[sys_index].address    )) & SYS_LIST[sys_index].bit_mask.Byte.LLByte;

    UART_TX_TCNT = 18;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_CMD(uint8_t cmd_index, CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp)
{
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_initial_CMD;
    TX_data[1] = CMD_LIST[cmd_index].signed_int;
    /* bit */
    TX_data[2] = CMD_LIST[cmd_index].bit_mask.Byte.HHByte;
    TX_data[3] = CMD_LIST[cmd_index].bit_mask.Byte.HLByte;
    TX_data[4] = CMD_LIST[cmd_index].bit_mask.Byte.LHByte;
    TX_data[5] = CMD_LIST[cmd_index].bit_mask.Byte.LLByte;
    /* step_value */
    TX_data[6] = CMD_LIST[cmd_index].step_value.Byte.HHByte;
    TX_data[7] = CMD_LIST[cmd_index].step_value.Byte.HLByte;
    TX_data[8] = CMD_LIST[cmd_index].step_value.Byte.LHByte;
    TX_data[9] = CMD_LIST[cmd_index].step_value.Byte.LLByte;
    /* min_value */
    TX_data[10] = CMD_LIST[cmd_index].min_value.Byte.HHByte;
    TX_data[11] = CMD_LIST[cmd_index].min_value.Byte.HLByte;
    TX_data[12] = CMD_LIST[cmd_index].min_value.Byte.LHByte;
    TX_data[13] = CMD_LIST[cmd_index].min_value.Byte.LLByte;
    /* max_value */
    TX_data[14] = CMD_LIST[cmd_index].max_value.Byte.HHByte;
    TX_data[15] = CMD_LIST[cmd_index].max_value.Byte.HLByte;
    TX_data[16] = CMD_LIST[cmd_index].max_value.Byte.LHByte;
    TX_data[17] = CMD_LIST[cmd_index].max_value.Byte.LLByte;
    /* command_data */
    TX_data[18] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 3)) & CMD_LIST[cmd_index].bit_mask.Byte.HHByte;
    TX_data[19] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 2)) & CMD_LIST[cmd_index].bit_mask.Byte.HLByte;
    TX_data[20] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 1)) & CMD_LIST[cmd_index].bit_mask.Byte.LHByte;
    TX_data[21] = (*(uint8_t*) (CMD_LIST[cmd_index].address    )) & CMD_LIST[cmd_index].bit_mask.Byte.LLByte;
        
    CMD_temp[cmd_index].changed_flag = 0;
    UART_TX_TCNT = 22;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_INFO(uint8_t info_index)
{
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_initial_INFO;
    TX_data[1] = INFO_LIST[info_index].signed_int;
    /* bit */
    TX_data[2] = INFO_LIST[info_index].bit_mask.Byte.HHByte;
    TX_data[3] = INFO_LIST[info_index].bit_mask.Byte.HLByte;
    TX_data[4] = INFO_LIST[info_index].bit_mask.Byte.LHByte;
    TX_data[5] = INFO_LIST[info_index].bit_mask.Byte.LLByte;
    
    UART_TX_TCNT = 6;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_micro_fig(uint8_t micro_index)
{
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_initial_MICRO;
    TX_data[1] = MICRO_LIST[micro_index].bit_mask.Byte.HHByte;
    TX_data[2] = MICRO_LIST[micro_index].bit_mask.Byte.HLByte;
    TX_data[3] = MICRO_LIST[micro_index].bit_mask.Byte.LHByte;
    TX_data[4] = MICRO_LIST[micro_index].bit_mask.Byte.LLByte;
    /* min_value */
    TX_data[5] = MICRO_LIST[micro_index].min_value.Byte.HHByte;
    TX_data[6] = MICRO_LIST[micro_index].min_value.Byte.HLByte;
    TX_data[7] = MICRO_LIST[micro_index].min_value.Byte.LHByte;
    TX_data[8] = MICRO_LIST[micro_index].min_value.Byte.LLByte;
    /* max_value */
    TX_data[9] = MICRO_LIST[micro_index].max_value.Byte.HHByte;
    TX_data[10] = MICRO_LIST[micro_index].max_value.Byte.HLByte;
    TX_data[11] = MICRO_LIST[micro_index].max_value.Byte.LHByte;
    TX_data[12] = MICRO_LIST[micro_index].max_value.Byte.LLByte;
    /* signed int */
    TX_data[13] = MICRO_LIST[micro_index].signed_int;
    UART_TX_TCNT = 14;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void initial_macro_fig(uint8_t macro_index)
{
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_initial_MACRO;
    TX_data[1] = MACRO_LIST[macro_index].bit_mask.Byte.HHByte;
    TX_data[2] = MACRO_LIST[macro_index].bit_mask.Byte.HLByte;
    TX_data[3] = MACRO_LIST[macro_index].bit_mask.Byte.LHByte;
    TX_data[4] = MACRO_LIST[macro_index].bit_mask.Byte.LLByte;
    /* min_value */
    TX_data[5] = MACRO_LIST[macro_index].min_value.Byte.HHByte;
    TX_data[6] = MACRO_LIST[macro_index].min_value.Byte.HLByte;
    TX_data[7] = MACRO_LIST[macro_index].min_value.Byte.LHByte;
    TX_data[8] = MACRO_LIST[macro_index].min_value.Byte.LLByte;
    /* max_value */
    TX_data[9] = MACRO_LIST[macro_index].max_value.Byte.HHByte;
    TX_data[10] = MACRO_LIST[macro_index].max_value.Byte.HLByte;
    TX_data[11] = MACRO_LIST[macro_index].max_value.Byte.LHByte;
    TX_data[12] = MACRO_LIST[macro_index].max_value.Byte.LLByte;
    /* signed int */
    TX_data[13] = MACRO_LIST[macro_index].signed_int;
    UART_TX_TCNT = 14;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void RX_command(uint8_t rx_data[], CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp)
{
    uint8_t cmd_index = rx_data[1];

    switch(CMD_LIST[cmd_index].bit_mask.LONG)
    {
        case B32_Mask:
            CMD_temp[cmd_index].data.Byte.HHByte = rx_data[3];
            CMD_temp[cmd_index].data.Byte.HLByte = rx_data[4];
        case B16_Mask:
            CMD_temp[cmd_index].data.Byte.LHByte = rx_data[5];
        case B8_Mask:
            CMD_temp[cmd_index].data.Byte.LLByte = rx_data[6];
    }
    CMD_temp[cmd_index].changed_flag = 1;
}

void RX_system_command(uint8_t rx_data[])
{
    uint8_t sys_index;
    
    sys_index = rx_data[1];

    switch(SYS_LIST[sys_index].bit_mask.LONG)
    {
        case B32_Mask:
            SYS_temp[sys_index].data.Byte.HHByte = rx_data[2];
            SYS_temp[sys_index].data.Byte.HLByte = rx_data[3];
        case B16_Mask:
            SYS_temp[sys_index].data.Byte.LHByte = rx_data[4];
        case B8_Mask:
            SYS_temp[sys_index].data.Byte.LLByte = rx_data[5];
    }
    SYS_temp[sys_index].changed_flag = 1;
}
void Command_update()
{
    uint8_t cmd_index, sys_index;
    
    for(cmd_index=0;cmd_index<GROUP0_CMD_SIZE;cmd_index++)
    {
        if (GROUP0_CMD_temp[cmd_index].changed_flag == 1)
        {
            switch(GROUP0_CMD_LIST[cmd_index].bit_mask.LONG)
            {
                case B32_Mask:
                    (*(uint8_t*) (GROUP0_CMD_LIST[cmd_index].address + 3)) = GROUP0_CMD_temp[cmd_index].data.Byte.HHByte;
                    (*(uint8_t*) (GROUP0_CMD_LIST[cmd_index].address + 2)) = GROUP0_CMD_temp[cmd_index].data.Byte.HLByte;
                case B16_Mask:
                    (*(uint8_t*) (GROUP0_CMD_LIST[cmd_index].address + 1)) = GROUP0_CMD_temp[cmd_index].data.Byte.LHByte;
                case B8_Mask:
                    (*(uint8_t*) (GROUP0_CMD_LIST[cmd_index].address))     = GROUP0_CMD_temp[cmd_index].data.Byte.LLByte;
            }
            GROUP0_CMD_temp[cmd_index].changed_flag = 0;
        }
    }
    for(cmd_index=0;cmd_index<GROUP1_CMD_SIZE;cmd_index++)
    {
        if (GROUP1_CMD_temp[cmd_index].changed_flag == 1)
        {
            switch(GROUP1_CMD_LIST[cmd_index].bit_mask.LONG)
            {
                case B32_Mask:
                    (*(uint8_t*) (GROUP1_CMD_LIST[cmd_index].address + 3)) = GROUP1_CMD_temp[cmd_index].data.Byte.HHByte;
                    (*(uint8_t*) (GROUP1_CMD_LIST[cmd_index].address + 2)) = GROUP1_CMD_temp[cmd_index].data.Byte.HLByte;
                case B16_Mask:
                    (*(uint8_t*) (GROUP1_CMD_LIST[cmd_index].address + 1)) = GROUP1_CMD_temp[cmd_index].data.Byte.LHByte;
                case B8_Mask:
                    (*(uint8_t*) (GROUP1_CMD_LIST[cmd_index].address))     = GROUP1_CMD_temp[cmd_index].data.Byte.LLByte;
            }
            GROUP1_CMD_temp[cmd_index].changed_flag = 0;
        }
    }
    for(cmd_index=0;cmd_index<GROUP2_CMD_SIZE;cmd_index++)
    {
        if (GROUP2_CMD_temp[cmd_index].changed_flag == 1)
        {
            switch(GROUP2_CMD_LIST[cmd_index].bit_mask.LONG)
            {
                case B32_Mask:
                    (*(uint8_t*) (GROUP2_CMD_LIST[cmd_index].address + 3)) = GROUP2_CMD_temp[cmd_index].data.Byte.HHByte;
                    (*(uint8_t*) (GROUP2_CMD_LIST[cmd_index].address + 2)) = GROUP2_CMD_temp[cmd_index].data.Byte.HLByte;
                case B16_Mask:
                    (*(uint8_t*) (GROUP2_CMD_LIST[cmd_index].address + 1)) = GROUP2_CMD_temp[cmd_index].data.Byte.LHByte;
                case B8_Mask:
                    (*(uint8_t*) (GROUP2_CMD_LIST[cmd_index].address))     = GROUP2_CMD_temp[cmd_index].data.Byte.LLByte;
            }
            GROUP2_CMD_temp[cmd_index].changed_flag = 0;
        }
    }
    for(cmd_index=0;cmd_index<GROUP3_CMD_SIZE;cmd_index++)
    {
        if (GROUP3_CMD_temp[cmd_index].changed_flag == 1)
        {
            switch(GROUP3_CMD_LIST[cmd_index].bit_mask.LONG)
            {
                case B32_Mask:
                    (*(uint8_t*) (GROUP3_CMD_LIST[cmd_index].address + 3)) = GROUP3_CMD_temp[cmd_index].data.Byte.HHByte;
                    (*(uint8_t*) (GROUP3_CMD_LIST[cmd_index].address + 2)) = GROUP3_CMD_temp[cmd_index].data.Byte.HLByte;
                case B16_Mask:
                    (*(uint8_t*) (GROUP3_CMD_LIST[cmd_index].address + 1)) = GROUP3_CMD_temp[cmd_index].data.Byte.LHByte;
                case B8_Mask:
                    (*(uint8_t*) (GROUP3_CMD_LIST[cmd_index].address))     = GROUP3_CMD_temp[cmd_index].data.Byte.LLByte;
            }
            GROUP3_CMD_temp[cmd_index].changed_flag = 0;
        }
    }
    for(sys_index=0;sys_index<SYS_SIZE;sys_index++)
    {
        if (SYS_temp[sys_index].changed_flag == 1)
        {
            switch(SYS_LIST[sys_index].bit_mask.LONG)
            {
                case B32_Mask:
                    (*(uint8_t*) (SYS_LIST[sys_index].address + 3)) = SYS_temp[sys_index].data.Byte.HHByte;
                    (*(uint8_t*) (SYS_LIST[sys_index].address + 2)) = SYS_temp[sys_index].data.Byte.HLByte;
                case B16_Mask:
                    (*(uint8_t*) (SYS_LIST[sys_index].address + 1)) = SYS_temp[sys_index].data.Byte.LHByte;
                case B8_Mask:
                    (*(uint8_t*) (SYS_LIST[sys_index].address))     = SYS_temp[sys_index].data.Byte.LLByte;
            }
            SYS_temp[sys_index].changed_flag = 0;
        }
    }
}

void Update_microcosm_fig()
{
    uint8_t micro_index;
    
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_update_microcosm_fig;
    for(micro_index=0;micro_index<MICRO_SIZE;micro_index++)
    {
        TX_data[micro_index*2 + 1] = (*(uint8_t*) (MICRO_LIST[micro_index].address + 1)) & MICRO_LIST[micro_index].bit_mask.Byte.LHByte;
        TX_data[micro_index*2 + 2] = (*(uint8_t*) (MICRO_LIST[micro_index].address    )) & MICRO_LIST[micro_index].bit_mask.Byte.LLByte;
    }
    UART_TX_TCNT = MICRO_SIZE * 2 + 1;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void Update_macrocosm_fig()
{
    uint8_t macro_index;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_update_macrocosm_fig;
    for(macro_index=0;macro_index<MACRO_SIZE;macro_index++)
    {
        TX_data[macro_index*2 + 1] = (*(uint8_t*) (MACRO_LIST[macro_index].address + 1)) & MACRO_LIST[macro_index].bit_mask.Byte.LHByte;
        TX_data[macro_index*2 + 2] = (*(uint8_t*) (MACRO_LIST[macro_index].address    )) & MACRO_LIST[macro_index].bit_mask.Byte.LLByte;
    }

    UART_TX_TCNT = MACRO_SIZE * 2 + 1;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void TX_information()
{
    uint8_t info_index;

    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_tx_information;
    for(info_index=0;info_index<INFO_SIZE;info_index++)
    {
        TX_data[info_index*2 + 1] = (*(uint8_t*) (INFO_LIST[info_index].address + 1)) & INFO_LIST[info_index].bit_mask.Byte.LHByte;
        TX_data[info_index*2 + 2] = (*(uint8_t*) (INFO_LIST[info_index].address    )) & INFO_LIST[info_index].bit_mask.Byte.LLByte;
    }

    UART_TX_TCNT = 2 * INFO_SIZE + 1;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void Refresh(CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp, const uint8_t size)
{
    uint8_t cmd_index = 0;
    for (cmd_index = 0;cmd_index<size;cmd_index++)
    {
        while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
        /* command_data */
        TX_data[cmd_index*4 + 0] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 3)) & CMD_LIST[cmd_index].bit_mask.Byte.HHByte;
        TX_data[cmd_index*4 + 1] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 2)) & CMD_LIST[cmd_index].bit_mask.Byte.HLByte;
        TX_data[cmd_index*4 + 2] = (*(uint8_t*) (CMD_LIST[cmd_index].address + 1)) & CMD_LIST[cmd_index].bit_mask.Byte.LHByte;
        TX_data[cmd_index*4 + 3] = (*(uint8_t*) (CMD_LIST[cmd_index].address    )) & CMD_LIST[cmd_index].bit_mask.Byte.LLByte;
        CMD_temp[cmd_index].changed_flag = 0;
    }
    UART_TX_TCNT = size << 2;
    UART_TX(UART_CH, UART_TX_TCNT);
}

void Refresh_sys(void)
{
    uint8_t sys_index = 0;
    for (sys_index = 0;sys_index<SYS_SIZE;sys_index++)
    {
        while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
        /* command_data */
        TX_data[sys_index*4 + 0] = (*(uint8_t*) (SYS_LIST[sys_index].address + 3)) & SYS_LIST[sys_index].bit_mask.Byte.HHByte;
        TX_data[sys_index*4 + 1] = (*(uint8_t*) (SYS_LIST[sys_index].address + 2)) & SYS_LIST[sys_index].bit_mask.Byte.HLByte;
        TX_data[sys_index*4 + 2] = (*(uint8_t*) (SYS_LIST[sys_index].address + 1)) & SYS_LIST[sys_index].bit_mask.Byte.LHByte;
        TX_data[sys_index*4 + 3] = (*(uint8_t*) (SYS_LIST[sys_index].address    )) & SYS_LIST[sys_index].bit_mask.Byte.LLByte;
        SYS_temp[sys_index].changed_flag = 0;
    }
    UART_TX_TCNT = SYS_SIZE << 2;
    UART_TX(UART_CH, UART_TX_TCNT);
}

uint32_t  au32Config[2];
void Initialize_FMC()
{
  /* Enable FMC ISP function */
    /* FMC_Open() */
  SYS_UnlockReg();
  FMC->ISPCTL |=  FMC_ISPCTL_ISPEN_Msk;  // ISP function Enabled.
  FMC->ISPCTL |=  FMC_ISPCTL_CFGUEN_Msk;  // ISP update User Configuration Enabled.

  au32Config[0] = FMC_Read(FMC_CONFIG_BASE);   //Read content of CONFIG 0
  au32Config[1] = FMC_Read(FMC_CONFIG_BASE + 4); //Read content of CONFIG 1

  if((au32Config[0] == 0xFFFFFFFF) && (au32Config[1] == 0xFFFFFFFF)) //Check CONFIG 0 & 1 , if not set , write set value
  {
    FMC_Erase(FMC_CONFIG_BASE);
    FMC_Write(FMC_CONFIG_BASE, 0xFFFFFFFE ); //Write setting to CONFIG 0 ¡A set DEFN = 0 (enable data flash)
//    au32Config[0] = FMC_Read(FMC_CONFIG_BASE);

    FMC_Erase(FMC_CONFIG_BASE + 4);
    FMC_Write(FMC_CONFIG_BASE + 4, DATA_FLASH_BASE);  //Write setting to CONFIG 1 ¡A set Data Flash Base Address
//    au32Config[1] = FMC_Read(FMC_CONFIG_BASE+4);
  }
  
  if((au32Config[0] != 0xFFFFFFFE ) && (au32Config[1] != DATA_FLASH_BASE)) //Check CONFIG 0 & 1 ,if it does not meet the set value , modify the set value
  {
    FMC_Erase(FMC_CONFIG_BASE);
    FMC_Write(FMC_CONFIG_BASE, 0xFFFFFFFE);
//    au32Config[0] = FMC_Read(FMC_CONFIG_BASE); //Write setting to CONFIG 0 ¡A set DEFN = 0 (enable data flash)
    
    FMC_Erase(FMC_CONFIG_BASE + 4);
    FMC_Write(FMC_CONFIG_BASE + 4, DATA_FLASH_BASE);  //Write setting to CONFIG 1 ¡A set Data Flash Base Address
//    au32Config[1] = FMC_Read(FMC_CONFIG_BASE+4);
  }
  
  SYS_LockReg();
}

uint8_t data_flash_is_empty(void)
{
    uint8_t data_flash_empty_flag;
    uint8_t cmd_index, data_flash_empty_amount = 0;

    SYS_UnlockReg();
    FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;
 
    for(cmd_index = 0; cmd_index < GROUP0_CMD_SIZE; cmd_index++)
    {
        if(FMC_Read(DATA_FLASH_BASE + cmd_index * 4) == 0xFFFFFFFF) data_flash_empty_amount++;  
    }
    for(cmd_index = 0; cmd_index < GROUP1_CMD_SIZE; cmd_index++)
    {
        if(FMC_Read(DATA_FLASH_BASE + (cmd_index + 20) * 4) == 0xFFFFFFFF) data_flash_empty_amount++;  
    }
    for(cmd_index = 0; cmd_index < GROUP2_CMD_SIZE; cmd_index++)
    {
        if(FMC_Read(DATA_FLASH_BASE + (cmd_index + 40) * 4) == 0xFFFFFFFF) data_flash_empty_amount++;  
    }
    for(cmd_index = 0; cmd_index < GROUP3_CMD_SIZE; cmd_index++)
    {
        if(FMC_Read(DATA_FLASH_BASE + (cmd_index + 60) * 4) == 0xFFFFFFFF) data_flash_empty_amount++;  
    }

    if(data_flash_empty_amount == (GROUP0_CMD_SIZE + GROUP1_CMD_SIZE + GROUP2_CMD_SIZE+ GROUP3_CMD_SIZE))
        data_flash_empty_flag = 1;
    else 
        data_flash_empty_flag = 0;

    FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
    SYS_LockReg();

    return (data_flash_empty_flag);
}

void read_data_flash_to_CMD_temp()
{
    VAR_T data_flash_temp;
    uint8_t cmd_index;

    SYS_UnlockReg();
    FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;

    for(cmd_index = 0; cmd_index < GROUP0_CMD_SIZE; cmd_index++)
    {
        data_flash_temp.LONG = FMC_Read(DATA_FLASH_BASE + cmd_index * 4);
        switch(GROUP0_CMD_LIST[cmd_index].bit_mask.LONG)
        {
            case B32_Mask:
                GROUP0_CMD_temp[cmd_index].data.Word.HWord = data_flash_temp.Word.HWord;
            case B16_Mask:
                GROUP0_CMD_temp[cmd_index].data.Byte.LHByte = data_flash_temp.Byte.LHByte;
            case B8_Mask:
                GROUP0_CMD_temp[cmd_index].data.Byte.LLByte = data_flash_temp.Byte.LLByte;
        }
        GROUP0_CMD_temp[cmd_index].changed_flag = 1;
    }
    
    for(cmd_index = 0; cmd_index < GROUP1_CMD_SIZE; cmd_index++)
    {
        data_flash_temp.LONG = FMC_Read(DATA_FLASH_BASE + (cmd_index + 20) * 4);
        switch(GROUP1_CMD_LIST[cmd_index].bit_mask.LONG)
        {
            case B32_Mask:
                GROUP1_CMD_temp[cmd_index].data.Word.HWord = data_flash_temp.Word.HWord;
            case B16_Mask:
                GROUP1_CMD_temp[cmd_index].data.Byte.LHByte = data_flash_temp.Byte.LHByte;
            case B8_Mask:
                GROUP1_CMD_temp[cmd_index].data.Byte.LLByte = data_flash_temp.Byte.LLByte;
        }
        GROUP1_CMD_temp[cmd_index].changed_flag = 1;
    }
    for(cmd_index = 0; cmd_index < GROUP2_CMD_SIZE; cmd_index++)
    {
        data_flash_temp.LONG = FMC_Read(DATA_FLASH_BASE + (cmd_index + 40) * 4);
        switch(GROUP2_CMD_LIST[cmd_index].bit_mask.LONG)
        {
            case B32_Mask:
                GROUP2_CMD_temp[cmd_index].data.Word.HWord = data_flash_temp.Word.HWord;
            case B16_Mask:
                GROUP2_CMD_temp[cmd_index].data.Byte.LHByte = data_flash_temp.Byte.LHByte;
            case B8_Mask:
                GROUP2_CMD_temp[cmd_index].data.Byte.LLByte = data_flash_temp.Byte.LLByte;
        }
        GROUP2_CMD_temp[cmd_index].changed_flag = 1;
    }
    for(cmd_index = 0; cmd_index < GROUP3_CMD_SIZE; cmd_index++)
    {
        data_flash_temp.LONG = FMC_Read(DATA_FLASH_BASE + (cmd_index + 60) * 4);
        switch(GROUP1_CMD_LIST[cmd_index].bit_mask.LONG)
        {
            case B32_Mask:
                GROUP3_CMD_temp[cmd_index].data.Word.HWord = data_flash_temp.Word.HWord;
            case B16_Mask:
                GROUP3_CMD_temp[cmd_index].data.Byte.LHByte = data_flash_temp.Byte.LHByte;
            case B8_Mask:
                GROUP3_CMD_temp[cmd_index].data.Byte.LLByte = data_flash_temp.Byte.LLByte;
        }
        GROUP3_CMD_temp[cmd_index].changed_flag = 1;
    }
    FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
    SYS_LockReg();

}

void read_data_flash_to_GUI(uint8_t cmd_group, const uint8_t size)
{
    VAR_T data_flash_temp;
    uint8_t cmd_index;
    uint32_t EPWM_CTL_TEMP;
    EPWM_CTL_TEMP = EPWM->CTL;
    EPWM->CTL &= ~0x3F;
    NVIC_IRQ_TEMP = NVIC->ISER[0];
    #if UART_GDMA_ENABLE == 0
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << USCI_CH_IRQ);
    #elif UART_GDMA_ENABLE == 1
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << GDMA0_IRQn);
    #endif
    SYS_UnlockReg();
    FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;
    
    for(cmd_index = 0; cmd_index < size; cmd_index++)
    {
        TX_data[cmd_index*6] = Fun_read_data_flash_to_GUI;
        TX_data[cmd_index*6 + 1] = cmd_index;
        
        data_flash_temp.LONG = FMC_Read(DATA_FLASH_BASE + (cmd_index + cmd_group*20) * 4);
        TX_data[cmd_index*6 + 2] = data_flash_temp.Byte.HHByte;
        TX_data[cmd_index*6 + 3] = data_flash_temp.Byte.HLByte;
        TX_data[cmd_index*6 + 4] = data_flash_temp.Byte.LHByte;
        TX_data[cmd_index*6 + 5] = data_flash_temp.Byte.LLByte;
    }

    FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
    SYS_LockReg();
    
    UART_TX_TCNT = 6 * size;
    UART_TX(UART_CH, UART_TX_TCNT);
    NVIC->ISER[0] = NVIC_IRQ_TEMP;
    EPWM->CTL = EPWM_CTL_TEMP;
}


void write_data_flash_from_GUI(CMD_TypeDef* CMD_LIST, Temp_T* CMD_temp, uint8_t cmd_group, const uint8_t size)
{
    uint8_t cmd_index;
    uint32_t EPWM_CTL_TEMP;
    EPWM_CTL_TEMP = EPWM->CTL;
    EPWM->CTL &= ~0x3F;
    NVIC_IRQ_TEMP = NVIC->ISER[0];
    #if UART_GDMA_ENABLE == 0
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << USCI_CH_IRQ);
    #elif UART_GDMA_ENABLE == 1
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << GDMA0_IRQn);
    #endif
    SYS_UnlockReg();
    FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;

    //FMC_Erase(DATA_FLASH_BASE);
    for(cmd_index = 0; cmd_index < size; cmd_index++)
    {
        FMC_Write(DATA_FLASH_BASE + (cmd_index + 20*cmd_group) * 4, CMD_temp[cmd_index].data.LONG);
        CMD_temp[cmd_index].changed_flag = 1;
    }

    FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
    SYS_LockReg();
    __enable_irq();
    
    while((UART_CH->BUFSTS & UUART_BUFSTS_TXEMPTY_Msk) == 0);
    TX_data[0] = Fun_write_data_flash_from_GUI;
    UART_TX_TCNT = 1;
    UART_TX(UART_CH, UART_TX_TCNT);
    NVIC->ISER[0] = NVIC_IRQ_TEMP;
    EPWM->CTL = EPWM_CTL_TEMP;
}

void erase_data_flash()
{
    int32_t FMC_Erase_status;
    uint32_t EPWM_CTL_TEMP;
    EPWM_CTL_TEMP = EPWM->CTL;
    EPWM->CTL &= ~0x3F;
    NVIC_IRQ_TEMP = NVIC->ISER[0];
    #if UART_GDMA_ENABLE == 0
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << USCI_CH_IRQ);
    #elif UART_GDMA_ENABLE == 1
        NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << GDMA0_IRQn);
    #endif
    SYS_UnlockReg();
    FMC->ISPCTL |= FMC_ISPCTL_APUEN_Msk;

    FMC_Erase_status = FMC_Erase(DATA_FLASH_BASE);
    
    FMC->ISPCTL &= ~FMC_ISPCTL_APUEN_Msk;
    SYS_LockReg();
    __enable_irq();
    if (FMC_Erase_status == 0) {
        TX_data[0] = Fun_erase_data_flash;
        UART_TX_TCNT = 1;
        UART_TX(UART_CH, UART_TX_TCNT);
        //UUART_WRITE(UART_CH, Fun_erase_data_flash);
    }
    NVIC->ISER[0] = NVIC_IRQ_TEMP;
    EPWM->CTL = EPWM_CTL_TEMP;
}

void Load_from_data_flash()
{
    if (data_flash_is_empty() == 0) {
        read_data_flash_to_CMD_temp();
    }
}

void Update_time_scale(uint8_t rx_data[])
{
    VAR_T time_scale_cnt_temp;

    time_scale_cnt_temp.Byte.HHByte = rx_data[1];
    time_scale_cnt_temp.Byte.HLByte = rx_data[2];
    time_scale_cnt_temp.Byte.LHByte = rx_data[3];
    time_scale_cnt_temp.Byte.LLByte = rx_data[4];
    
    TIME_SCALE_CNT = time_scale_cnt_temp.LONG;
    Update_microcosm_fig_req = 0;
    Update_microcosm_fig_trigger = 0;
    MICRO_TX_CNT = 0;
}
void TX_data_to_PC(int16_t rotor_speed, uint16_t min_cmd_speed_rpm)
{
    rotor_speed = ABS(rotor_speed);

    /* `If rotor speed is lower than min speed, update micocosm fig trigger = 1 */
    if ((rotor_speed>=0)&&(rotor_speed < min_cmd_speed_rpm)&&(Update_microcosm_fig_req==1)) {
            Update_microcosm_fig_trigger = 1;
        }
    if (Update_microcosm_fig_trigger == 1){
        
        Update_microcosm_fig();
        MICRO_TX_CNT ++;
        if (MICRO_TX_CNT == TIME_SCALE_CNT)
        {
            Update_microcosm_fig_req = 0;
            Update_microcosm_fig_trigger = 0;
            MICRO_TX_CNT = 0;
        }
    }
    else if (Update_macrocosm_fig_req == 1) {
        Update_macrocosm_fig_req = 0;
        Update_macrocosm_fig();
    }
    else if (Tx_information_req == 1) {
        Tx_information_req = 0;
        TX_information();
    }
    else if (Refresh_group_req.flag == 1) {
        Refresh_group_req.flag = 0;
        Refresh((CMD_TypeDef *)GROUP_CMD_BASE[Refresh_group_req.cmd_group], (Temp_T *)GROUP_TEMP_BASE[Refresh_group_req.cmd_group], GROUP_CMD_SIZE[Refresh_group_req.cmd_group]);
    }
    else if (Refresh_sys_req == 1) {
        Refresh_sys_req = 0;
        Refresh_sys();
    }
}

void Trigger_update_micro_fig()
{
    if (Update_microcosm_fig_req == 1)
        Update_microcosm_fig_trigger = 1;
}

void Request_decode(uint8_t rx_data[])
{
    if ((rx_data[0] == Fun_update_microcosm_fig) && (Update_microcosm_fig_trigger == 0)) {
        Update_microcosm_fig_req = 1;
    }
    else if (rx_data[0] == Fun_update_macrocosm_fig) {
        Update_macrocosm_fig_req = 1;
    }
    else if (rx_data[0] == Fun_tx_information) {
        Tx_information_req = 1;
    }
    else if (rx_data[0] == Fun_update_time_scale) {
        Update_time_scale(rx_data);
    }
    else if (rx_data[0] == Fun_rx_command) {
        RX_command(rx_data, (CMD_TypeDef *)GROUP_CMD_BASE[rx_data[2]], (Temp_T *)GROUP_TEMP_BASE[rx_data[2]]);
    }
    else if (rx_data[0] == Fun_rx_sys_command) {
        RX_system_command(rx_data);
    }
    else if (rx_data[0] == Fun_refresh_command) {
        Refresh_group_req.flag = 1;
        Refresh_group_req.cmd_group = rx_data[1];
    }
    else if (rx_data[0] == Fun_refresh_sys) {
        Refresh_sys_req = 1;
    }
    else if (rx_data[0] == Fun_read_data_flash_to_GUI) {
        read_data_flash_to_GUI(rx_data[1], GROUP_CMD_SIZE[rx_data[1]]);
    }
    else if (rx_data[0] == Fun_write_data_flash_from_GUI) {
        write_data_flash_from_GUI((CMD_TypeDef *)GROUP_CMD_BASE[rx_data[1]], (Temp_T *)GROUP_TEMP_BASE[rx_data[1]], rx_data[1], GROUP_CMD_SIZE[rx_data[1]]);
    }
    else if (rx_data[0] == Fun_erase_data_flash) {
        erase_data_flash();
    }
    else if (rx_data[0] == Fun_uart_check) {
        PWM_CTL_TEMP = EPWM->CTL;
        EPWM->CTL &= ~0x3F;
        NVIC_IRQ_TEMP = NVIC->ISER[0];
        #if UART_GDMA_ENABLE == 0
            NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << USCI_CH_IRQ);
        #elif UART_GDMA_ENABLE == 1
            NVIC->ICER[0] = 0xFFFFFFFF & ~(1UL << GDMA0_IRQn);
        #endif
        TX_data[0] = Fun_uart_check;
        UART_TX_TCNT = 2;
        UART_TX(UART_CH, UART_TX_TCNT);
    }
    else if (rx_data[0] == Fun_initial_SYS_name) {
        initial_SYS_name(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_CMD_name) {
        initial_CMD_name(rx_data[1], (CMD_TypeDef *)GROUP_CMD_BASE[rx_data[2]], GROUP_CMD_SIZE[rx_data[2]]);
    }
    else if (rx_data[0] == Fun_initial_INFO_name) {
        initial_INFO_name(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_MICRO_name) {
        initial_MICRO_name(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_MACRO_name) {
        initial_MACRO_name(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_SYS) {
        initial_SYS(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_CMD) {
        initial_CMD(rx_data[1], (CMD_TypeDef *)GROUP_CMD_BASE[rx_data[2]], (Temp_T *)GROUP_TEMP_BASE[rx_data[2]]);
    }
    else if (rx_data[0] == Fun_initial_INFO) {
        initial_INFO(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_MICRO) {
        initial_micro_fig(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_MACRO) {
        initial_macro_fig(rx_data[1]);
    }
    else if (rx_data[0] == Fun_initial_end) {
        TX_data[0] = Fun_initial_end;
        UART_TX_TCNT = 1;
        UART_TX(UART_CH, UART_TX_TCNT);
        NVIC->ISER[0] = NVIC_IRQ_TEMP;
        EPWM->CTL = PWM_CTL_TEMP;
    }
    else if (rx_data[0] == Fun_reset_mcu) {
        SYS_UnlockReg();
        SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
        SYS_LockReg();
    }
}

void UART_TX_MFP(GPIO_T *port, uint8_t pin)
{
    uint32_t port_offset = ((uint32_t)&port->MODE - GPIO_BASE) >> 4;
    uint32_t field_len = 4;
    *(uint32_t *) (GCR_BASE + 0x30 + port_offset) &= ~(0xF << (pin*field_len));
    *(uint32_t *) (GCR_BASE + 0x30 + port_offset) |= (0xB << (pin*field_len));
    field_len = 2;
    port->MODE = (port->MODE & ~(0x3 << (pin*field_len))) | (GPIO_MODE_OUTPUT << (pin*field_len));
}

void UART_RX_MFP(GPIO_T *port, uint32_t pin)
{
    uint32_t port_offset = ((uint32_t)&port->MODE - GPIO_BASE) >> 4;
    uint32_t field_len = 4;
    *(uint32_t *) (GCR_BASE + 0x30 + port_offset) &= ~(0xF << (pin*field_len));
    *(uint32_t *) (GCR_BASE + 0x30 + port_offset) |= (0xB << (pin*field_len));
    field_len = 2;
    port->MODE = (port->MODE & ~(0x3 << (pin*field_len))) | (GPIO_MODE_INPUT << (pin*field_len));
}

void Initialize_USCI_UART(void)
{
    /* Enable USCI1 IP clock */
    CLK->APBCLK = CLK->APBCLK | (1<<(24+UART_USE));

    /* SYS_ResetModule(SYS_IPRST1_USCI1RST_Msk); */
    SYS->IPRST1 |= (1<<(24+UART_USE));
    SYS->IPRST1 &= ~(1<<(24+UART_USE));

    /* Init USCI UART1 to 4000000-8n1 for print message */
    UUART_Open(UART_CH, 4000000);

    // Set multi_function pins and GPIO mode
    UART_TX_MFP(UART_TX_PORT, UART_TX_PIN);
    UART_RX_MFP(UART_RX_PORT, UART_RX_PIN);

    #if UART_GDMA_ENABLE == 1
        Initialize_GDMA();
    #endif
}



#if UART_GDMA_ENABLE == 0
    void USCI_IRQHandler(void)
    {
        uint32_t u32IntSts = UART_CH->PROTSTS;

        if(u32IntSts & UUART_PROTSTS_RXENDIF_Msk) {
            UUART_CLR_PROT_INT_FLAG(UART_CH, UUART_PROTSTS_RXENDIF_Msk);
            RX_data[UART_RX_CNT++] = UUART_READ(UART_CH);
            if (UART_RX_CNT == (RX_size)) {
                UART_RX_CNT = 0;
                Request_decode(RX_data);
            }
        }
    }

#elif UART_GDMA_ENABLE == 1
    void GDMA0_IRQHandler (void)
    {
        /* Clear TCIF */
        GDMA0->CTL &= ~GDMA_CTL_TCIF_Msk;
        GDMA_ENABLE(GDMA0);
        Request_decode(RX_data);
        
    }

    void GDMA1_IRQHandler (void)
    {
        /* Clear TCIF */
        GDMA1->CTL &= ~GDMA_CTL_TCIF_Msk;
        //UUART2->DMACTL = UUART_DMACTL_DMAEN_Msk | UUART_DMACTL_RXDMAEN_Msk;
    }

    void Initialize_GDMA(void)
    {
        /* Enable GDMA controller clock */
        CLK->AHBCLK |= CLK_AHBCLK_GDMACKEN_Msk;

        /* Reset GDMA */
        SYS_UnlockReg();    /* Unlock protected registers */
        SYS->IPRST0 |= SYS_IPRST0_GDMARST_Msk;
        SYS_LockReg();      /* Lock protected registers */

        /* Select transfer width */
        GDMA0->CTL = (GDMA0->CTL & ~(GDMA_CTL_TWS_Msk | GDMA_CTL_BME_Msk)) | (GDMA_TWS_8BITS << GDMA_CTL_TWS_Pos);
        GDMA1->CTL = (GDMA1->CTL & ~(GDMA_CTL_TWS_Msk | GDMA_CTL_BME_Msk)) | (GDMA_TWS_8BITS << GDMA_CTL_TWS_Pos);
        
        /* Enable GDMA and set mode */
        GDMA0->CTL = (GDMA0->CTL & ~(GDMA_CTL_GDMAMS_Msk)) | (GDMA_USCI_MODE << GDMA_CTL_GDMAMS_Pos);
        GDMA1->CTL = (GDMA1->CTL & ~(GDMA_CTL_GDMAMS_Msk)) | (GDMA_USCI_MODE << GDMA_CTL_GDMAMS_Pos);
          
        /* Set address direction or fixed */
        GDMA0->CTL = (GDMA1->CTL & ~(0xF << GDMA_CTL_DADIR_Pos)) |
                     ((SOURCE_ADDRESS_FIXED | Destination_ADDRESS_INC) << GDMA_CTL_DADIR_Pos);
        GDMA1->CTL = (GDMA1->CTL & ~(0xF << GDMA_CTL_DADIR_Pos)) |
                     ((SOURCE_ADDRESS_INC | Destination_ADDRESS_FIXED) << GDMA_CTL_DADIR_Pos);

        /* Set transfer count */
        GDMA_SET_TRANSFER_COUNT(GDMA0, RX_size);
        
        /* Set destination base address */
        GDMA0->DSTB = (uint32_t)&RX_data[0];
        GDMA1->DSTB = (uint32_t)&UART_CH->TXDAT;
        
        /* Set source base address */
        GDMA0->SRCB = (uint32_t)&UART_CH->RXDAT;
        GDMA1->SRCB = (uint32_t)&TX_data[0];

        /* Reset UART DMA CTL */
        UART_CH->DMACTL |= UUART_DMACTL_DMARST_Msk;

        /* Set UART DMA CTL */
        UART_CH->DMACTL |= UUART_DMACTL_DMAEN_Msk | UUART_DMACTL_RXDMAEN_Msk | UUART_DMACTL_TXDMAEN_Msk;

        /* Enable GDMA0 External Interrupt */
        

        /* Enable GDMA0 INT */
        GDMA0->CTL |= GDMA_CTL_GIEN_Msk;

        /* Enable GDMA0 to start receiving command*/
        GDMA_ENABLE(GDMA0);
    }
#endif
