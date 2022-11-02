/*******************************************************************************************************
 * Code related to User Interface. It includes:
    - Screen 
    - Buttons
    - Buzzer
*******************************************************************************************************/
#include <Arduino.h>
#include <uiConf.h>
#include <Adafruit_GFX.h>         // Oled library      
#include <Adafruit_SSD1306.h>     // Oled library

extern Adafruit_SSD1306 display;
extern QueueHandle_t xQueueSendToOled;



void buzzerTask(void * parameters)  {
  for(;;) {
    digitalWrite(DO_BUZZER, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(DO_BUZZER, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void oledTask(void * parameters)  {
  oledMessage_t message;
  display.clearDisplay();
  display.display();
  ESP_LOGD(TAG, "Oled Task Created");

  for(;;) {
    xQueueReceive(xQueueSendToOled, &message, portMAX_DELAY);

    //display.ssd1306_command(SSD1306_DISPLAYOFF);
    display.setCursor(message.cursor_t.x, message.cursor_t.y);
    display.setTextSize(message.fontSize);
    display.print(message.message);
    display.display();

    //ESP_LOGD(TAG, "Watermark: %d", uxTaskGetStackHighWaterMark( NULL ));
  }
}