/*******************************************************************************************************
 * DESCRIPTION: MQTT header file
 * AUTOR: Fran Guillén
 * BOARD: TTGO LoRa ESP32
*******************************************************************************************************/

#ifndef _uiConf_h_
#define _uiConf_h_

/************************************************
 *                OLED CONFIG
 ************************************************/
// -------- PIN CONFIG ---------
#define OLED_SDA    4
#define OLED_SCL    15 
#define OLED_RST    16

// --------- SCREEN CONFIG --------
#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64  // OLED display height, in pixels


typedef struct oledMessage 
{
  uint8_t fontSize;
  struct cursor 
  {
    int16_t x;
    int16_t y;
  }cursor_t;
  char message[25];

} oledMessage_t;




/************************************************
 *                BUZZER CONFIG
 ************************************************/
// -------- PIN CONFIG ---------
#define DO_BUZZER            2


/************************************************
 *                BUTTON CONFIG
 ************************************************/
// -------- PIN CONFIG ---------
#define DI_BUTTON            17

#endif