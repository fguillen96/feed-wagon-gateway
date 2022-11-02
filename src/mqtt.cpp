#include <AsyncMqttClient.h>      // MQTT client library
#include <ArduinoJson.h>
#include <mqttConf.h>

extern AsyncMqttClient mqttClient;
extern QueueHandle_t xQueueSendToMqtt;

static const char* TAG = __FILE__;


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

  // TODO
}



void publishMqttTask(void * parameters)  {
  publishElement_t  publishData;

  for(;;) {
    xQueueReceive(xQueueSendToMqtt, &publishData, portMAX_DELAY);
    
    mqttClient.publish(publishData.TopicBuf, publishData.qos, publishData.retain, publishData.PayloadBuf);
  }
}
