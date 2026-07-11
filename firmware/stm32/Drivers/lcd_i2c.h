#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_
#include "main.h"

// Standard HD44780-over-PCF8574 (I2C) values.
#define SLAVE_ADDRESS_LCD      0x27
#define LCD_CMD_CLEAR_DISPLAY  0x01
#define LCD_CMD_ENTRY_MODE_SET 0x06
#define LCD_CMD_DISPLAY_ON     0x0C
#define LCD_CMD_FUNCTION_SET   0x28

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef lcd_send_cmd(uint8_t cmd);
HAL_StatusTypeDef lcd_send_data(uint8_t data);
void lcd_init(void);
void distance_display(uint32_t *giv_dist, uint32_t *acc_dist);

void lcd_clear(void);
void lcd_put_cursor(int row, int col);
void lcd_send_string(char *str);

#endif /* INC_LCD_I2C_H_ */
