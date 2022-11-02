/*******************************************************************************************************
 * DESCRIPTION: Feed Wagon Gateway. Analyze LoRa data and send it to MQTT server
 * AUTOR: Fran Guillén
 * BOARD: TTGO LoRa ESP32
*******************************************************************************************************/

#include <Arduino.h>              // =)
#include <WiFiManager.h>          // Manage WiFi connection and credentials input
#include <esp32fota.h>            // Over The Air updates over Internet
#include <Adafruit_GFX.h>         // Oled library      
#include <Adafruit_SSD1306.h>     // Oled library
#include <AsyncMqttClient.h>      // MQTT client library
#include <Lora.h>                 // Manage LoRa connection
#include <time.h>                 // NTP time
#include <LoraConf.h>
#include <mqttconf.h>
#include <uiConf.h>
#include <mainConf.h>


// Debug
static const char* TAG = __FILE__;




/************************************************
 *                PROTOTYPES START
 ************************************************/

// --------- TASK HEADERS ---------
void otaUpdateTask(void * );
void dataAnalysisTask(void *);
void buzzerTask(void *);
void oledTask(void *);

// ---------- CALLBACK MQTT HEADERS --------
void onMqttMessage(char* , char*, AsyncMqttClientMessageProperties, size_t, size_t, size_t);
void onMqttConnect(bool);


/************************************************
 *                PROTOTYPES END
 ************************************************/



/************************************************
 *                GLOBAL START
 ************************************************/
// -------- WIFI MANAGER ---------
WiFiManager wm;

// -------- MQTT  ---------
AsyncMqttClient mqttClient;

// -------- TASK HANDLERS --------
TaskHandle_t xHandleOtaUpdate = NULL;
TaskHandle_t xHandleManageConnection = NULL;
TaskHandle_t xHandleBuzzer = NULL;
TaskHandle_t xLoraReceive = NULL;
TaskHandle_t xHandleOled = NULL;


// ---------- QUEUE HANDLERS ---------
QueueHandle_t xQueueLoraIsrToTask;
QueueHandle_t xQueueSendToMqtt;
QueueHandle_t xQueueSendToOled;

// -------- OTA UPDATE ----------
esp32FOTA esp32FOTA("esp32-fota-http", 1);

// -------- OLED --------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST, 100000UL, 100000UL);



/************************************************
 *                  GLOBAL END
 *************************************************/


/************************************************
 *                  TASKS START
 *************************************************/


void otaUpdate(void * parameters)  {
  for(;;) {

    bool updatedNeeded = esp32FOTA.execHTTPcheck();
    if (updatedNeeded) 
    {
      esp32FOTA.execOTA();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void manageConnection(void * parameters)  {
  for(;;) {
    if (WiFi.status() == WL_DISCONNECTED) {
      ESP_LOGE(TAG, "WiFi connection lost. Reconnecting ...");
      WiFi.begin();
    }
    else if(!mqttClient.connected() && WiFi.status() == WL_CONNECTED && WiFi.localIP() != IPAddress(0, 0, 0, 0)) {
      ESP_LOGI(TAG, "Connecting MQTT ...");
      mqttClient.connect();
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}





/************************************************
 *                  TASKS END
 *************************************************/



/************************************************
 *                  MAIN (SETUP + LOOP)
 *************************************************/
void setup() {
  // --------- DEBUG ---------
  Serial.begin(115200);


  // --------- IO CONFIG --------
  pinMode(DO_BUZZER, OUTPUT);         // Buzzer
  pinMode(DI_BUTTON, INPUT_PULLUP);   // Button


  // --------- OLED CONFIG --------
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3C for 128x32
     ESP_LOGE(TAG, "SSD1306 allocation failed");
  }
  display.setTextColor(WHITE, BLACK);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(32,32);
  display.print("@fguillen96");
  display.display();



  // --------- WIFI MANAGER ---------
  //WiFi.mode(WIFI_STA);
  //wm.setTimeout(60);
  //wm.autoConnect("LoRa Gateway");

  ESP_LOGI(TAG, "Connecting to WiFi");
  WiFi.begin("Tortuga", "mwNEGBaAmmsQvA9J496S");
 /*  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  } */

  ESP_LOGI(TAG, "WiFi connected");

  // --------- NTP SERVER CONFIG ---------
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now2 = time(nullptr);
  while (now2 < 1635448489) {
    delay(500);
    now2 = time(nullptr);
  }
  ESP_LOGI(TAG, "Time OK: %d", now2);


  // --------- MQTT CLIENT ---------
  mqttClient.setClientId(MQTT_ID);
  mqttClient.setWill(TOPIC_PUB_DEVICE_INFO, 1, true, WILL_MESSAGE);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setCleanSession(true);

  // --------- OTA UPDATE --------
  esp32FOTA.checkURL = OTA_SERVER;


  // --------- QUEUES ---------
  xQueueLoraIsrToTask = xQueueCreate(QUEUE_ISR_TO_LORA_LENGTH, sizeof(char[LORA_MESSAGE_MAX_LENGTH]));
  xQueueSendToMqtt =  xQueueCreate(5, sizeof(publishElement_t));
  xQueueSendToOled =  xQueueCreate(3, sizeof(oledMessage_t));



  // ---------- TASKS  ----------
  //xTaskCreate(otaUpdate, "OTA", 3000, NULL, 5, &xHandleOtaUpdate);
  //xTaskCreate(manageConnection, "Keep WiFi Alive", 2000, NULL, 1, &xHandleManageConnection);
  //xTaskCreate(buzzerTask, "BuzzerAlarm", 1000, NULL, 1, &xHandleBuzzer);
  xTaskCreatePinnedToCore(oledTask, "Oled Task", 3000, NULL, 1, &xHandleOled, 1);
  xTaskCreate(dataAnalysisTask, "Lora Gateway", 3500, NULL, 2, &xLoraReceive);


  // --------- LORA CONFIG  --------
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(868E6)) ESP_LOGE(TAG, "LoRa module error");
  LoRa.onReceive(onReceiveLora_ISR);
  LoRa.receive();

}

void loop() {
  vTaskDelete(NULL);    // Delete loop task because it is not used
}