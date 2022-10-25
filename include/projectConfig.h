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
#include <secrets.h>

#define WIFI_SSID       SECRET_WIFI_SSID               // WIFI SSID
#define WIFI_PASSWORD   SECRET_WIFI_PASSWORD           // WIFI password


/************************************************
 *                OLED CONFIG
 ************************************************/
#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64  // OLED display height, in pixels




/************************************************
 *                 MQTT CONFIG
 ************************************************/
#define MQTT_HOST           SECRET_MQTT_HOST             // MQTT server IP
#define MQTT_PORT           1883                         // MQTT port
#define MQTT_ID             "FWAGON"                     // MQTT id
#define MQTT_USERNAME       ""                           // MQTT server username
#define MQTT_PASSWORD       ""                           // MQTT server password

// ---------- PUBLISH STRUCT ---------
#define MQTT_TOPIC_BUFFER_LENGTH        50
#define MQTT_MESSAGE_BUFFER_LENGTH      100

// ---------- MQTT TOPICS  ----------


const char* TOPIC_PUB_DEVICE_INFO  = "pedrosanchez/fwagon/device/info";
const char* TOPIC_PUB_WAGON_PARAM =  "pedrosanchez/fwagon/status/param";
const char* TOPIC_PUB_WAGON_TRIP =   "pedrosanchez/fwagon/status/trip";


// ---------- MQTT TOPICS (PUB) ----------

#define TOPIC_SUB                       "pedrosanchez/fwagon/device/config/#"
#define TOPIC_SUB_CONFIG                "pedrosanchez/fwagon/device/config"
#define TOPIC_PUB_DEV_STAT              "pedrosanchez/fwagon/device/info"

// --------- WILL MESSAGE ---------
#define WILL_MESSAGE PROGMEM  R"EOF({"status": "disconnected"})EOF"



/************************************************
 *                 OTA CONFIG
 ************************************************/
#define OTA_CHECK_UPDATE_EVERY_MS       5000
#define OTA_SERVER                     "http://192.168.1.32:8000/fota.json";


/************************************************
 *          FREERTOS QUEUE SIZE CONFIG
 ************************************************/

#define QUEUE_ISR_TO_LORA_LENGTH           2
#define QUEUE_ISR_TO_LORA_ITEM_SIZE        sizeof(char[20])


/************************************************
 *                LORA DATA CONFIG
 ************************************************/
#define LORA_MESSAGE_MAX_LENGTH         12



/************************************************
 *                FEED WAGON CONFIG
 ************************************************/
#define VOLTAGE_BATTERY_CHARGING            27
#define VOLTAGE_MOTOR_MOVING_LIMIT          12

#define SECONDS_TO_FEED                     20
#define SECONDS_LOADING_FEED                60

#endif