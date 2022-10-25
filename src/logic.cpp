/*******************************************************************************************************
 * Data analysis. The information from LoRa is received by a queue to:
    - Detect errors
    - Convert data to JSON and send to MQTT (another queue is used for that)
*******************************************************************************************************/
#include <Arduino.h>
#include <projectConfig.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>         // Oled library      
#include <Adafruit_SSD1306.h>     // Oled library


extern QueueHandle_t xQueueLoraIsrToTask;
extern QueueHandle_t xQueueSendToMqtt;

enum machine_states {HOME, GOING_FOR_FEED, FEED_LOADING, MOVING, ERROR};
machine_states state  = HOME;

static bool isCharging(int v_battery) {
  return (v_battery >= VOLTAGE_BATTERY_CHARGING);
}

static bool isMoving(int v_motor) {
  return ((v_motor >= VOLTAGE_MOTOR_MOVING_LIMIT) || 
          (v_motor <= - VOLTAGE_MOTOR_MOVING_LIMIT));
}



void dataAnalysisTask (void* pvParameters) {
  
  const uint8_t n_data = 3;
  char lora_buffer[LORA_MESSAGE_MAX_LENGTH];
  

  // Time variables
  time_t time_trip_start;
  time_t now;
  time_t time_feed_loading_start;


  for(;;) 
  {
    BaseType_t data_read = xQueueReceive(xQueueLoraIsrToTask, &lora_buffer, pdMS_TO_TICKS(5000));

    // If data is received properly from LoRa
    if (data_read) {

      // Var from LoRa sender
      float v_battery = atof(strtok(lora_buffer, ","));
      int v_motor = atoi(strtok(NULL, ","));
      bool at_home =  atoi(strtok(NULL, ","));


      // Simplify variables for state machine
      bool charging = isCharging(v_battery);
      bool moving = isMoving(v_motor);

      // Get time
      now = time(nullptr);

      // State machine
      switch (state) 
      {
        case  HOME: 
          if (!moving && !charging)
          {
            state = ERROR;
          }
           
          else if (moving && !charging) 
          {
            state = GOING_FOR_FEED;
            time_trip_start = now;
          }
        break;


        case GOING_FOR_FEED:
          if (!moving && !charging) 
          {
            if (now - time_trip_start >= SECONDS_TO_FEED) 
            {
              state = FEED_LOADING;
              time_feed_loading_start = now;
            }

            else 
            {
              state = ERROR;
            }
          } 

          else if (!moving && charging)
          {
            state = HOME;
          } 

        break;


        case  FEED_LOADING:
          if (moving && !charging) 
          {
            state = MOVING;
          }

          else if (!moving && charging)
          {
            state = HOME;
          }

          else if ((now - time_feed_loading_start) > SECONDS_LOADING_FEED) 
          {
            state = ERROR;
          }

        break;


        case  MOVING: 
        if (!moving && !charging) 
        {
          state = ERROR;
        }

        else if (!moving && charging)
        {
          state = HOME;
        }
        break;

        case  ERROR:
        
        break;
      }

      // Send all data to MQTT 
      //const int capacity = JSON_OBJECT_SIZE(3);
      StaticJsonDocument<100> doc;

      doc["VBat"] = 42;
      doc["VMot"] = 48.748010;
      doc["Reed"] = 2.293491;
      doc["State"] = "liebre";

      char output[MQTT_MESSAGE_BUFFER_LENGTH];
      serializeJsonPretty(doc, output);

      xQueueSend(xQueueSendToMqtt, &output, 0);
    }
  }
}


