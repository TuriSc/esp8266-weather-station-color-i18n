/*
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com

 With contributions by Mario Ammerschuber - github "ammerscm68" - www.intermario.de
 Internationalization and Italian translation by Turi Scandurra

*/

// Important: see settings.h to configure your settings

#include "i18n_it.h" // Include only the i18n file for your locale 
#include "settings.h"
#include "time.h"
#include "LittleFS.h" 
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <push_button.h>
#include <SunMoonCalc.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>

/***
   Install the following libraries
   - Mini Grafx by Daniel Eichhorn
   - ESP8266 WeatherStation by Daniel Eichhorn
   - Json Streaming Parser by Daniel Eichhorn
   https://github.com/barn53/esp8266-PushButton
   OneWire
   DallasTemperature
 ***/

#include <JsonListener.h>
#include <OpenWeatherMapCurrent.h>
#include <OpenWeatherMapForecast.h>
#include <MiniGrafx.h>
#include <Carousel.h>
#include <ILI9341_SPI.h>

#include "ArialRounded.h"
#include "moonphases.h"
#include "weathericons.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Button.h"

#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_COLOR_A 2
#define MINI_COLOR_B 3

#define MAX_FORECASTS 12

// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE, // 1
                      0x09ff,//2, ORANGE
                      0x003C //3, RED
                     };

// Limited to 4 colors due to memory constraints
int BITS_PER_PIXEL = 2; // 2^2 =  4 colors

ADC_MODE(ADC_VCC);

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);
Carousel carousel(&gfx, 0, 0, 240, 100);

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapForecastData forecasts[MAX_FORECASTS];

SunMoonCalc::Moon moonData;

pb::PushButton button(BUTTON_PIN);

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(SENSOR_PIN);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
// DeviceAddress insideThermometer;

char determineMoonIcon();
String getTime(time_t *timestamp);
const char* getMeteoconIconFromProgmem(String iconText);
const char* getMiniMeteoconIconFromProgmem(String iconText);

FrameCallback frames[] = { drawForecast1, drawForecast2, drawForecast3 };
int frameCount = 3;

// how many different screens do we have?
int screenCount = 6;
long lastDownloadUpdate = millis();

uint8_t screen = 0;
// divide screen into 4 quadrants "< top", "> bottom", " < middle "," > middle "
uint16_t dividerTop, dividerBottom, dividerMiddle;

long timerPress;
void sleep_mode_toggle();
bool sleeping;
char* make12_24(int hour);

bool isFSMounted = false;
int WiFiConnectLoop = 0;
int MaxWiFiConnectLoop = 350;

int waitloop = 0; 
int WSReset = 0;
String UWD = "10";
String LastWeatherUpdateTime = "";

//flag for saving data WiFiManager
bool shouldSaveConfig = false;

long lastDrew = 0;
bool btnClick;

float temperatureC;
float temperatureF;

ESP8266WebServer  server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;

// *****************************************************************************************************************************************************

void setup() {
  Serial.begin(115200);

  clearscreen(); // Serial Monitor Clear

  // ***First Start***  
  Serial.println("Mounting file system...");
  isFSMounted = LittleFS.begin();
  if (!isFSMounted) {
    Serial.println(I18N_FORMATTING_FILE_SYSTEM);
    drawProgress(50, I18N_FORMATTING_FILE_SYSTEM);
    LittleFS.format();
    drawProgress(100, I18N_FORMATTING_DONE);
    MyWaitLoop(10000);
    ESP.restart();
  }

  loadPropertiesFromLittlefs(); // Load Adjustments

  // The LED pin needs to set HIGH
  // Use this pin to save energy
  // Turn on the background LED
  Serial.println(TFT_LED);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);    // HIGH to Turn on;

  gfx.init();
  tft.setRotation(3);
  gfx.setRotation(3); // Edit or remove this line if your screen appears rotated
  gfx.fillBuffer(MINI_BLACK);
  gfx.commit();

  if (WIFI_SSID == "" | WIFI_PASS == "" | OPEN_WEATHER_MAP_API_KEY == "" | OPEN_WEATHER_MAP_LOCATION_ID == "") {
  Serial.println("Starting Access Point ..."); 
  Serial.println("");
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.fillBuffer(MINI_BLACK);
  gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 90, I18N_WEATHER_STATION);
  gfx.drawString(120, 160, I18N_WIFI_MANAGER_LAUNCHED);
  gfx.drawString(120, 180, I18N_IP_ADDRESS "192.168.10.1");
  gfx.commit();
  CaptivePortal();  // Start Captive Portal (Access Point)
  } else
  {  
  // Normal Start  
  Serial.println("Initializing screen...");
  /* Allow user to force a screen re-calibration  */
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.fillBuffer(MINI_BLACK);
  //gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo); // Disabled here but visible on the about screen
  drawLogo();
  gfx.setColor(MINI_WHITE);
  gfx.commit();
  delay(3000);
  yield();

  dividerTop = 0; // Default 64
  dividerBottom = gfx.getHeight() - dividerTop;
  dividerMiddle = gfx.getWidth() / 2;
  Serial.println("dividerTop = "+ (String)dividerTop);
  Serial.println("dividerBottom = "+ (String)dividerBottom);
  Serial.println("dividerMiddle = "+ (String)dividerMiddle);

  connectWifi();

  // #######################################################################################
  // Arduino OTA/DNS
  Serial.println("Arduino OTA/DNS-Server started");
  ArduinoOTA.setHostname(WIFI_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onEnd([]() {Serial.println("\nEnd");});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));});
  ArduinoOTA.onError([](ota_error_t error) {(void)error;ESP.restart();});
  ArduinoOTA.begin();
  // #####################################################################################

  carousel.setFrames(frames, frameCount);
  carousel.disableAllIndicators();

  initTime();

  // update the weather information when Access data correct
  int OWMupdateData = 0;
  updateData();
  while (currentWeather.cityName == "") {
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.fillBuffer(MINI_BLACK);
  //gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo); // Disabled here but visible on the about screen
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 90, I18N_WEATHER_STATION);
  gfx.drawString(120, 150, F(I18N_UNABLE_TO_GET_WEATHER));
  gfx.drawString(120, 200, F(I18N_INCORRECT_LOGIN_DETIALS));
  OWMupdateData += 1; 
  gfx.drawString(120, 260, I18N_ATTEMPT + (String)OWMupdateData);
  gfx.commit();   
  MyWaitLoop(5000);
  updateData(); // Try Next Update
  if (OWMupdateData > 4) {ResetWeatherStation();}}

  timerPress = millis();

// Get all information of your LittleFS
    if (isFSMounted == true)
    {
    Serial.println(F("Little Filesystem Init - done."));
    FSInfo fs_info;
    LittleFS.info(fs_info);
    Serial.println("File sistem info.");
    Serial.print("Total space:      ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");
    Serial.print("Total space used: ");
    Serial.print(fs_info.usedBytes);
    Serial.println("byte");
    Serial.print("Block size:       ");
    Serial.print(fs_info.blockSize);
    Serial.println("byte");
    Serial.print("Page size:        ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");
    Serial.print("Max open files:   ");
    Serial.println(fs_info.maxOpenFiles);
    Serial.print("Max path length:  ");
    Serial.println(fs_info.maxPathLength);
    Serial.println();
    // Open dir folder
    Dir dir = LittleFS.openDir("/");
    // Cycle all the content
    while (dir.next()) {
        // get filename
        Serial.print(dir.fileName());
        Serial.print(" - ");
        // If element have a size display It else write 0
        if(dir.fileSize()) {
            File f = dir.openFile("r");
            Serial.println(f.size());
            f.close();
        }else{
            Serial.println("0");
        }   
    }
   Serial.println("_______________________________");
   Serial.println("");
   }
   else
     {
     Serial.println("Little Filesystem Init - Error");      
     }

  Serial.println("Start Weather station"); 
  }

  sensors.begin();
  // sensors.getAddress(insideThermometer, 0);
  // sensors.setResolution(9); // One decimal place
}

// *****************************************************************************************************************************************************

void loop() {
if (WIFI_SSID == "" | WIFI_PASS == "" | OPEN_WEATHER_MAP_API_KEY == "" | OPEN_WEATHER_MAP_LOCATION_ID == "") {
  server.handleClient();
  dnsServer.processNextRequest();
  } else
{  
   WiFi.mode(WIFI_STA);
  if (WiFi.status() == WL_CONNECTED) 
  {
  ArduinoOTA.handle();
  gfx.fillBuffer(MINI_BLACK);

  switch (button.getEvent()) {
    case pb::PushButton::Event::SHORT_PRESS:
    case pb::PushButton::Event::LONG_PRESS:
        if (!sleeping) screen = (screen + 1) % 4;
        timerPress = millis();
        wake_up_from_sleep();
        break;
    case pb::PushButton::Event::LONG_HOLD:
        break;
    case pb::PushButton::Event::DOUBLE_PRESS:
        if (!sleeping) screen = 4;
        break;
    case pb::PushButton::Event::TRIPLE_PRESS:
        if (!sleeping) screen = 5;
        break;
    case pb::PushButton::Event::NOT_READY:
        //Serial.println("D7 PushButton::Event::NOT_READY");
        break;
    case pb::PushButton::Event::INVALID:
        Serial.println("D7 PushButton::Event::INVALID");
        break;
    default:
        //Serial.println("D7 PushButton::Event::NONE");
        break;
    }

  sleep_mode_task();

  if (!sleeping) {
    if (screen == 0) {
      drawTime();

      drawWifiQuality();
      int remainingTimeBudget = carousel.update();
      if (remainingTimeBudget > 0) {
        // You can do some work here
        // Don't do stuff if you are below your
        // time budget.
        delay(remainingTimeBudget);
      }
      drawCurrentWeather();
      drawAstronomy();
    } else if (screen == 1) {
      drawCurrentWeatherDetail();
    } else if (screen == 2) {
      drawForecastTable(0);
    } else if (screen == 3) {
      drawForecastTable(4);
    } else if (screen == 4) {
      drawAbout();
    } else if (screen == 5) {
      ResetWeatherStationScreen();  
    }
    if(screen != 5) {
      WSReset = 30; // Reset the countdown timer
    }
    gfx.commit();

    // Check if we should update weather information
    if (millis() - lastDownloadUpdate > 1000 * UPDATE_INTERVAL_SECS) {
      updateData();
      lastDownloadUpdate = millis();
    }
  }
  } else {connectWifi();}
}
}
// *****************************************************************************************************************************************************
// *****************************************************************************************************************************************************
