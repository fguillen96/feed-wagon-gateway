/*******************************************************************************************************
 * DESCRIPTION: Project config file
 * AUTOR: Fran Guillén
 * BOARD: TTGO LoRa ESP32
*******************************************************************************************************/
#ifndef _loraConf_h_
#define _loraConf_h_

#include <Arduino.h>


/************************************************
 *                PIN CONFIG
 ************************************************/

// ---------- LORA MODULE ---------
#define SCK         5
#define MISO        19
#define MOSI        27
#define SS          18
#define RST         14
#define DIO0        26


// -------- OLED MODULE ---------
#define OLED_SDA    4
#define OLED_SCL    15 
#define OLED_RST    16


/************************************************
 *                DATA CONFIG
 ************************************************/
const uint8_t LORA_MESSAGE_MAX_LENGTH  = 20;
const uint8_t LORA_MESSAGE_NUMER_DATA  = 3;

#define QUEUE_ISR_TO_LORA_LENGTH           2




/************************************************
 *                HEADERS
 ************************************************/

// ---------- ISR ----------
void onReceiveLora_ISR(int);


#endif