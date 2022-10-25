/*******************************************************************************************************
 * Code related to User Interface. It includes:
    - Screen 
    - Buttons
    - Buzzer
*******************************************************************************************************/
#include <Arduino.h>
#include <projectConfig.h>
#include <Adafruit_GFX.h>         // Oled library      
#include <Adafruit_SSD1306.h>     // Oled libraryç

extern Adafruit_SSD1306 display;



void buzzerTask(void * parameters)  {
  for(;;) {
    digitalWrite(DO_BUZZER, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(DO_BUZZER, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void oledTask(void * parameters)  {

  for(;;) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print(25);
    display.setCursor(58, 0);
    display.print(-24);
    display.setCursor(122, 0);
    display.write(127);
    display.setTextSize(3);
    display.setCursor(10, 30);
    display.print("PARADA");
    display.display();


    display.clearDisplay();
    display.setCursor(32, 20);
    display.print("Lora Error");
    display.display();
    Serial.println("LoRa Error");

  }
}