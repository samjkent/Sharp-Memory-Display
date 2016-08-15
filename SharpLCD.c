/** @file SharpLCD.c
*  @brief Implementation for the LCD driver.
*
*  @author Sam Kent
*/
#include "SharpLCD.h"
#include "font.h"
#include "icons.h"
#include "SEGGER_RTT.h"

// VCOM Timer
uint32_t time_ms = 12; //Time(in milliseconds) between consecutive compare events.
uint32_t time_ticks;
const nrf_drv_timer_t TIMER_VCOM = NRF_DRV_TIMER_INSTANCE(1);

// Initial VCOM value
uint8_t vcom = 0x00;

// Screen Refresh Timer
uint32_t screen_time_ms = 1000; //Time(in milliseconds) between consecutive compare events.
uint32_t screen_time_ticks;
const nrf_drv_timer_t TIMER_SCREEN_REFRESH = NRF_DRV_TIMER_INSTANCE(2);

// SPI
static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

/**
* @brief Initiate LCD
*/
void lcdInit(void){

  SEGGER_RTT_WriteString(0, "LCD Init.. ");

  // Set Up CS Pin for LCD
  nrf_gpio_cfg_output(24);

  // Set Up SPI
  nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG(0);

  config.frequency = NRF_DRV_SPI_FREQ_1M;
  config.mode      = NRF_DRV_SPI_MODE_0;
  config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;

  err_code = nrf_drv_spi_init(&m_spi_master_0, &config, NULL);

  // Set Up Timer To Toggle VCOM
  err_code = nrf_drv_timer_init(&TIMER_VCOM, NULL, toggle_vcom);
  time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_VCOM, time_ms);
  nrf_drv_timer_extended_compare(&TIMER_VCOM, NRF_TIMER_CC_CHANNEL1, time_ticks, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);

  // Set Up Timer To Refresh Screen
  err_code = nrf_drv_timer_init(&TIMER_SCREEN_REFRESH, NULL, refreshDisplay);
  screen_time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_SCREEN_REFRESH, screen_time_ms);
  nrf_drv_timer_extended_compare(&TIMER_SCREEN_REFRESH, NRF_TIMER_CC_CHANNEL2, screen_time_ticks, NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK, true);

  // Enable timers
  // Enable VCOM toggle
  nrf_drv_timer_enable(&TIMER_VCOM);
  // Enable Screen Refresh
  nrf_drv_timer_enable(&TIMER_SCREEN_REFRESH);

  // Send init Commands to LCD
  uint8_t lcdInitCommands[] = {0x60, 0x00};
  nrf_gpio_pin_set(24);
  nrf_drv_spi_transfer(&m_spi_master_0, lcdInitCommands, 2, NULL, 0);
  nrf_gpio_pin_clear(24);

  SEGGER_RTT_WriteString(0, "Done \n");

}

/**
* @brief Toggle VCOM
*/
void toggle_vcom(nrf_timer_event_t event_type, void* p_context){

  nrf_gpio_pin_set(24);

  nrf_drv_spi_transfer(&m_spi_master_0, &vcom, 2, NULL, 0);
  nrf_drv_spi_transfer(&m_spi_master_0, LCD_ZERO, 2, NULL, 0);

  if(vcom == 0x00) vcom = 0x40;
  if(vcom == 0x40) vcom = 0x00;



  nrf_gpio_pin_clear(24);


}

/**
* @brief Refresh Display
*/
void refreshDisplay(nrf_timer_event_t event_type, void* p_context){

  // Chip Select HIGH
  nrf_gpio_pin_set(24);
  // Write Command
  uint8_t lcd_write = LCD_WRITE;
  nrf_drv_spi_transfer(&m_spi_master_0, &lcd_write, 1, NULL, 0);

  // Write 96 Rows
  for(uint8_t row = 0; row < 96; row++){

    // Write Line Number
    uint8_t revRowNum = reverse(row+1);
    nrf_drv_spi_transfer(&m_spi_master_0, &revRowNum, 1, NULL, 0);
    // Write Line Data
    nrf_drv_spi_transfer(&m_spi_master_0, frameBuffer[row], 12, NULL, 0);
    // Write Trailer Byte
    nrf_drv_spi_transfer(&m_spi_master_0, LCD_ZERO, 1, NULL, 0);

  }

  // Write Termination Byte
  nrf_drv_spi_transfer(&m_spi_master_0, LCD_ZERO, 1, NULL, 0);

  // Chip Select LOW
  nrf_gpio_pin_clear(24);

}

/**
* @brief Print Line
*/
void printLine(char* charString, int row, bool large){
  for(int i = 0; i < strlen(charString); i++){
      if(large) {
        printLargeChar(charString[i], row, i);
      } else {
      printChar(charString[i], row, i);
      }
  }
}

/**
* @brief Print Large Char
*/
void printLargeChar(char character, int row, int col){
  // First row of character
  int fontRow  = row * 16;
  int characterIndex = character * 16;

  frameBuffer[fontRow + 0][col] = font8x16[characterIndex + 0];
  frameBuffer[fontRow + 1][col] = font8x16[characterIndex + 1];
  frameBuffer[fontRow + 2][col] = font8x16[characterIndex + 2];
  frameBuffer[fontRow + 3][col] = font8x16[characterIndex + 3];
  frameBuffer[fontRow + 4][col] = font8x16[characterIndex + 4];
  frameBuffer[fontRow + 5][col] = font8x16[characterIndex + 5];
  frameBuffer[fontRow + 6][col] = font8x16[characterIndex + 6];
  frameBuffer[fontRow + 7][col] = font8x16[characterIndex + 7];

  frameBuffer[fontRow + 8][col] = font8x16[characterIndex + 8];
  frameBuffer[fontRow + 9][col] = font8x16[characterIndex + 9];
  frameBuffer[fontRow + 10][col] = font8x16[characterIndex + 10];
  frameBuffer[fontRow + 11][col] = font8x16[characterIndex + 11];
  frameBuffer[fontRow + 12][col] = font8x16[characterIndex + 12];
  frameBuffer[fontRow + 13][col] = font8x16[characterIndex + 13];
  frameBuffer[fontRow + 14][col] = font8x16[characterIndex + 14];
  frameBuffer[fontRow + 15][col] = font8x16[characterIndex + 15];

}

/**
* @brief Print Char
*/
void printChar(char character, int row, int col){
  // First row of character
  int fontRow  = row * 8;
  int characterIndex = character * 8;

  frameBuffer[fontRow + 0][col] = font8x8[characterIndex + 0];
  frameBuffer[fontRow + 1][col] = font8x8[characterIndex + 1];
  frameBuffer[fontRow + 2][col] = font8x8[characterIndex + 2];
  frameBuffer[fontRow + 3][col] = font8x8[characterIndex + 3];
  frameBuffer[fontRow + 4][col] = font8x8[characterIndex + 4];
  frameBuffer[fontRow + 5][col] = font8x8[characterIndex + 5];
  frameBuffer[fontRow + 6][col] = font8x8[characterIndex + 6];
  frameBuffer[fontRow + 7][col] = font8x8[characterIndex + 7];

}

/**
* @brief Print Icon 24
*/
void printIcon24(int icon, int row, int col){
  int frameRow = row * 24;

      if(icon == 0){
          for(int y = 0; y < 24; y++){
              for(int x = 0; x < 3; x++){
                  frameBuffer[frameRow + y][col + x] = snapchat24[(y*3)+x];
              }
          }
      } else if(icon == 1){
          for(int y = 0; y < 24; y++){
              for(int x = 0; x < 3; x++){
                  frameBuffer[frameRow + y][col + x] = fbMessenger24[(y*3)+x];
              }
          }
      }


}

/**
* @brief: Reverses Bit Order
*/
uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

/**
* @brief: Highlight row
*/
void highlightRow(int row){
  int frameRow = row * 24;

  for(int y = 0; y < 24; y++){
      for(int x = 0; x < 12; x ++){
          frameBuffer[frameRow + y][x] = ~frameBuffer[frameRow + y][x];
      }
  }

}
