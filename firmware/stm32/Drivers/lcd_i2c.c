#include "lcd_i2c.h"

// Function to send a command to the LCD
HAL_StatusTypeDef lcd_send_cmd(uint8_t cmd)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    data_u = (cmd & 0xF0);        // Upper nibble
    data_l = ((cmd << 4) & 0xF0); // Lower nibble

    data_t[0] = data_u | 0x0C; // Enable high, RS low
    data_t[1] = data_u | 0x08; // Enable low, RS low
    data_t[2] = data_l | 0x0C; // Enable high, RS low
    data_t[3] = data_l | 0x08; // Enable low, RS low

    return HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD << 1, data_t, 4, HAL_MAX_DELAY);
}

// Function to send data to the LCD
HAL_StatusTypeDef lcd_send_data(uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    data_u = (data & 0xF0);        // Upper nibble
    data_l = ((data << 4) & 0xF0); // Lower nibble

    data_t[0] = data_u | 0x0D; // Enable high, RS high
    data_t[1] = data_u | 0x09; // Enable low, RS high
    data_t[2] = data_l | 0x0D; // Enable high, RS high
    data_t[3] = data_l | 0x09; // Enable low, RS high

    return HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD << 1, data_t, 4, HAL_MAX_DELAY);
}

void lcd_init(void)
{
    // 4-bit initialization sequence
    HAL_Delay(50); lcd_send_cmd(0x30); // Wait >40ms
    HAL_Delay(5);  lcd_send_cmd(0x30); // Wait >4.1ms
    HAL_Delay(1);  lcd_send_cmd(0x30); // Wait >100us
    HAL_Delay(10); lcd_send_cmd(0x20); // Switch to 4-bit mode

    // Display configuration
    lcd_send_cmd(LCD_CMD_FUNCTION_SET);   // 2-line, 5x8 font
    lcd_send_cmd(LCD_CMD_DISPLAY_ON);     // Enable display
    lcd_send_cmd(LCD_CMD_CLEAR_DISPLAY);  // Clear screen
    lcd_send_cmd(LCD_CMD_ENTRY_MODE_SET); // Auto-increment cursor
}

void distance_display(uint32_t *giv_dist, uint32_t *acc_dist)
{
    lcd_clear();

    // First line - Given (measured) distance
    lcd_put_cursor(0, 0); lcd_send_string("G.D.:");
    lcd_put_cursor(0, 6); lcd_send_string((char *)giv_dist);
    lcd_put_cursor(0, 10); lcd_send_string("cm");

    // Second line - Accepted (target) distance
    lcd_put_cursor(1, 0); lcd_send_string("A.D.:");
    lcd_put_cursor(1, 10); lcd_send_string("cm");

    char buffer[12];
    itoa(*acc_dist, buffer, 10);
    lcd_put_cursor(1, 6); lcd_send_string(" "); // Clear previous value
    lcd_put_cursor(1, 6); lcd_send_string(buffer);

    HAL_Delay(150); // Controlled update rate
}
