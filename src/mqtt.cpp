#include <AsyncMqttClient.h>      // MQTT client library
#include <ArduinoJson.h>
#include <projectConfig.h>

extern AsyncMqttClient mqttClient;
extern QueueHandle_t xQueueSendToMqtt;

const char* TAG = "mqtt";


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  ESP_LOGD(TAG, "MQTT Message: ");
  for (unsigned int i = 0; i < len; i++) {
    ESP_LOGD(payload[i]);
  }
  ESP_LOGD(TAG, "\n * Topic: %s\n * Length: %d\n", topic, len);

  // ---------- DEVICE CONFIG TOPIC --------
  if (strcmp(topic, TOPIC_SUB_CONFIG) == false) {
    ;
    // TODO
  }
}



void onMqttConnect(bool sessionPresent) {
  ESP_LOGD(TAG, "Connected to MQTT");
}


void publishMqttTask(void * parameters)  {
  //publishElement  publishElement_t;

  for(;;) {
    //BaseType_t data_read = xQueueReceive(xQueueSendToMqtt, &lora_buffer, pdMS_TO_TICKS(5000));
    digitalWrite(DO_BUZZER, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(DO_BUZZER, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
