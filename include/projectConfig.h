/*******************************************************************************************************
 * DESCRIPTION: Project config file
 * AUTOR: Fran Guillén
 * BOARD: TTGO LoRa ESP32
*******************************************************************************************************/
#ifndef _projectConfig_h_
#define _projectConfig_h_




/************************************************
 *                PIN CONFIG
 ************************************************/

// ---------- DIGITAL OUTPUTS ----------
#define DO_BUZZER            2

// --------- DIGITAL INPUTS
#define DI_BUTTON            17


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
 *                WIFI CONFIG
 ************************************************/


#define WIFI_SSID       SECRET_WIFI_SSID               // WIFI SSID
#define WIFI_PASSWORD   SECRET_WIFI_PASSWORD           // WIFI password










/************************************************
 *                 OTA CONFIG
 ************************************************/
#define OTA_CHECK_UPDATE_EVERY_MS       5000
#define OTA_SERVER                     "http://192.168.1.32:8000/fota.json"


/************************************************
 *          FREERTOS QUEUE SIZE CONFIG
 ************************************************/

#define QUEUE_ISR_TO_LORA_LENGTH           2
#define QUEUE_ISR_TO_LORA_ITEM_SIZE        sizeof(char[20])





/************************************************
 *                FEED WAGON CONFIG
 ************************************************/
#define VOLTAGE_BATTERY_CHARGING            27
#define VOLTAGE_MOTOR_MOVING_LIMIT          12

#define SECONDS_TO_FEED                     20
#define SECONDS_LOADING_FEED                60


#endif