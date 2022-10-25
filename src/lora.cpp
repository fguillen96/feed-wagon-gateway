
/*******************************************************************************************************
 * LoRa Management. Interrupt when LoRa receive. Send data to queue.
*******************************************************************************************************/

#include <LoRa.h>
#include <projectConfig.h>

// Debug
static const char* TAG = "Lora";


extern QueueHandle_t  xQueueLoraIsrToTask;


void IRAM_ATTR onReceiveLora_ISR(int packetSize) {
  BaseType_t xHigherPriorityTaskWoken;

  char buffer[LORA_MESSAGE_MAX_LENGTH];
  LoRa.readBytes(buffer, packetSize);

  buffer[packetSize] = '\0';

  xQueueSendFromISR(xQueueLoraIsrToTask, &buffer, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}
