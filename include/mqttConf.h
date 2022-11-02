/*******************************************************************************************************
 * DESCRIPTION: MQTT header file
 * AUTOR: Fran Guillén
 * BOARD: TTGO LoRa ESP32
*******************************************************************************************************/

#ifndef _mqttConf_h_
#define _mqttConf_h_

#include <Arduino.h>
#include <secrets.h>


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
#define MQTT_MESSAGE_BUFFER_LENGTH      150


// ---------- MQTT TOPICS (PUB)  ----------
const char* const TOPIC_PUB_DEVICE_INFO   =   "ps/fwagon/device/info";
const char* const TOPIC_PUB_WAGON_PARAM   =   "ps/fwagon/param";
const char* const TOPIC_PUB_WAGON_STATUS  =   "ps/fwagon/status";


// ---------- MQTT TOPICS (SUB) ----------
const char* const TOPIC_SUB_CONFIG    =    "ps/fwagon/device/config";
const char* const TOPIC_SUB_TO_LORA   =    "ps/fwagon/tolora";


// --------- WILL MESSAGE ---------
#define WILL_MESSAGE PROGMEM  R"EOF({"status": "disconnected"})EOF"


// --------- STRUCT MESSAGE --------
typedef struct publishElement
{
    char PayloadBuf[MQTT_TOPIC_BUFFER_LENGTH];
    char TopicBuf[MQTT_MESSAGE_BUFFER_LENGTH];
    uint8_t qos;
    bool retain;
} publishElement_t;

#endif