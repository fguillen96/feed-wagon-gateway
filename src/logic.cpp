/*******************************************************************************************************
 * Data analysis. The information from LoRa is received by a queue to:
    - Detect errors
    - Convert data to JSON and send to MQTT (another queue is used for that)
*******************************************************************************************************/
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>         // Oled library      
#include <Adafruit_SSD1306.h>     // Oled library
#include <mqttConf.h>
#include <loraConf.h>
#include <uiConf.h>

// Debug
static const char* TAG = __FILE__;


extern QueueHandle_t xQueueLoraIsrToTask;
extern QueueHandle_t xQueueSendToMqtt;
extern QueueHandle_t xQueueSendToOled;

// --------- STATE MACHINE --------
// States enum
enum machine_states {
  HOME, 
  MOVING_F, 
  MOVING_B, 
  STOP,
  E_WAGON,
  E_LORA
  };

static machine_states state  = HOME;
static machine_states state_to_print  = HOME;
static machine_states last_state = HOME;

const char states_string[6][7] = 
{
  "CASA",
  ">>>>>>",
  "<<<<<<",
  " STOP ",
  "ALARMA",
  "E-LORA"
};


const unsigned int max_ms_stopped = 60000;
const int v_battery_when_charging = 27;
const int v_motor_moving_limit = 12;


static bool isCharging(int v_battery) {
  return (v_battery >= v_battery_when_charging);
}

static int8_t isMoving(int v_motor) {
  int8_t direction = 0;
  
  if (v_motor >= v_motor_moving_limit) direction = 1;
  else if (v_motor <= - v_motor_moving_limit) direction = -1;

  return direction;
}



void dataAnalysisTask (void* pvParameters) {
  // LORA data
  char lora_buffer[LORA_MESSAGE_MAX_LENGTH];

  // Time variables
  time_t time_trip_start;
  time_t now;

  unsigned int millis_at_stop = 0;





  for(;;) 
  {

    BaseType_t data_read = xQueueReceive(xQueueLoraIsrToTask, &lora_buffer, pdMS_TO_TICKS(5000));



    // If data is received properly from LoRa
    if (data_read) {

      float v_battery = 0;
      int v_motor = 0;
      bool at_home = 0;

      uint8_t colon_count = 0;
      for (int i = 0; i<strlen(lora_buffer); i++)
      {
        if (lora_buffer[i] == ',')
          colon_count ++;
      }

      if (colon_count != 2) 
        ESP_LOGE(TAG, "String Error: %d", colon_count);
      else
      {
      // Var from LoRa sender
        v_battery = atof(strtok(lora_buffer, ","));
        v_motor = atoi(strtok(NULL, ","));
        at_home =  atoi(strtok(NULL, ","));
      }
      
      // Simplify variables for state machine
      bool charging = isCharging(v_battery);
      int8_t moving = isMoving(v_motor);

      // Get time
      now = time(nullptr);


      // Logic to detect errors

      switch (state)
      {
        case HOME:
        if (!moving && !charging) 
        {
          state = STOP;
          millis_at_stop = millis();
        }

        else if (moving && !charging) 
        {
          time_trip_start = now;
          state = (moving > 0) ? MOVING_F : MOVING_B;
        }
    
        break;

        case MOVING_F:
        case MOVING_B:
          if (!moving && !charging) 
          {
            state = STOP;
            millis_at_stop = millis();
          }

          else if (!moving && charging)
          {
            state = HOME;
          }

        break;

        case STOP:
          if (!moving && charging)
          {
            state = HOME;
          }
          else if (moving && !charging)
          {
            state = (moving > 0) ? MOVING_F : MOVING_B;
          }
          else if ((millis() - millis_at_stop) > max_ms_stopped)
          {
            state = E_WAGON;
          }

        break;

        
        case E_WAGON:

          if (!moving && charging)
          {
            state = HOME;
          }
          else if (moving && !charging)
          {
             state = (moving > 0) ? MOVING_F : MOVING_B;
          }

        break;
      }

       // Send all data to MQTT 
      StaticJsonDocument<150> mqtt_param_doc;

      mqtt_param_doc["VBat"] = v_battery;
      mqtt_param_doc["VMot"] = v_motor;
      mqtt_param_doc["Reed"] = at_home;
      mqtt_param_doc["ts"] = now;

      char output[MQTT_MESSAGE_BUFFER_LENGTH];
      serializeJsonPretty(mqtt_param_doc, output);

      publishElement_t dataToMqtt;
      strcpy(dataToMqtt.PayloadBuf, output);
      strcpy(dataToMqtt.TopicBuf, TOPIC_PUB_WAGON_PARAM);
      dataToMqtt.qos = 0;
      dataToMqtt.retain = 0;

      xQueueSend(xQueueSendToMqtt, &dataToMqtt, 0);


      // Send to OLED
      char home = (at_home) ? 127 : ' ';

      oledMessage_t dataToOled;
      dataToOled.fontSize = 1;
      dataToOled.cursor_t.x = 0;
      dataToOled.cursor_t.y = 0;
      sprintf(dataToOled.message, "%4.1f      %3d       %c", v_battery, v_motor, home);

      xQueueSend(xQueueSendToOled, &dataToOled, 0);


      state_to_print = state;
    }

    else
    {
      ESP_LOGE(TAG, "LoRa Error");
      state_to_print = E_LORA;

      // Send do Oled
      oledMessage_t dataToOled;
      dataToOled.fontSize = 1;
      dataToOled.cursor_t.x = 0;
      dataToOled.cursor_t.y = 0;
      strcpy(dataToOled.message, "                   ");

      xQueueSend(xQueueSendToOled, &dataToOled, 0);
    }

    if (last_state != state_to_print) 
    {
      
      last_state = state_to_print;

       // Send to MQTT
      StaticJsonDocument<100> mqtt_status_doc;
      mqtt_status_doc["LastTrip"] = time_trip_start;
      mqtt_status_doc["Status"] = states_string[state_to_print];

      char output[MQTT_MESSAGE_BUFFER_LENGTH];
      serializeJsonPretty(mqtt_status_doc, output);

      publishElement_t dataToMqtt;
      strcpy(dataToMqtt.PayloadBuf, output);
      strcpy(dataToMqtt.TopicBuf, TOPIC_PUB_WAGON_STATUS);
      dataToMqtt.qos = 1;
      dataToMqtt.retain = 1;

      xQueueSend(xQueueSendToMqtt, &output, 0);


      // Send do Oled
      oledMessage_t dataToOled;
      dataToOled.fontSize = 3;
      dataToOled.cursor_t.x = 10;
      dataToOled.cursor_t.y = 30;
      strcpy(dataToOled.message, states_string[state_to_print]);

      xQueueSend(xQueueSendToOled, &dataToOled, 0);
    }

    //ESP_LOGD(TAG, "Watermark: %d", uxTaskGetStackHighWaterMark( NULL ));
  }
} 


