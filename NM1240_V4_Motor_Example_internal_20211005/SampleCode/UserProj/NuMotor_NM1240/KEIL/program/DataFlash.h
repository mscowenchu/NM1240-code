
#ifndef __DataFlash_H__
#define __DataFlash_H__

#include "NM1240.h"







extern void read_data_flash_to_buff(void);
extern void write_data_flash_from_buff(void);
extern void update_variable_from_data_flash_buff(void);
extern void update_data_flash_buff_from_variable(void);

#endif

