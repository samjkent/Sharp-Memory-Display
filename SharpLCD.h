/** @file SharpLCD.h
 *  @brief Function prototypes for the LCD driver.
 *
 *  @author Sam Kent
 */

#ifndef _SHARP_LCD_H
#define _SHARP_LCD_H

#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_config.h"
#include "nrf_drv_timer.h"

// SPI Commands
#define LCD_WRITE 0x80
#define LCD_ZERO 0x00

// Chip Select
#define LCD_CHIP_SELECT 24

// Set up buffer to store pixel data
uint8_t frameBuffer[96][12];

// Hold Error Code
uint32_t err_code;

/** @brief Initiates Screen On SPI Port 0
 *
 *  @param None
 *  @return None
 */
void lcdInit( void );

/** @brief Prints Char
 *
 *  @param Character to print, Row to print on, Column to print on
 *  @return None
 */
void printChar(char character, int row, int col);

/** @brief Prints Large Char
 *
 *  @param Char to print, Row to print on, Double size text
 *  @return None
 */

void printLargeChar(char character, int row, int col);

/** @brief Prints String
 *
 *  @param String to print, Row to print on, Double size text
 *  @return None
 */

void printLine(char* charString, int row, bool large);

/** @brief Prints icon from icons.h
 *
 *  @param ID of icon to print, Row, Column
 *  @return None
 */
void printIcon24(int icon, int row, int col);

/** @brief Highlights (Inverts) the specified row
 *
 *  @param Row to invert
 *  @return None
 */
void highlightRow(int row);

/** @brief Toggles VCOM, necessary for LCD
 *
 *  @param Timer, Context (Used as handler for timer timeout)
 *  @return None
 */
void toggle_vcom(nrf_timer_event_t event_type, void* p_context);

/** @brief Updates the display
 *
 *  @param Timer, Context (Used as handler for timer timeout)
 *  @return None
 */
void refreshDisplay(nrf_timer_event_t event_type, void* p_context);

/** @brief Reverse bit order
 *
 *  @param uint8_t to reverse
 *  @return None
 */
uint8_t reverse(uint8_t b);


#endif /* _SHARP_LCD_H */
