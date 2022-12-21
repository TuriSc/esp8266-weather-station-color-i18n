/* ************** Functions *****************/
void clearscreen() { 
for(int i=0; i<10; i++) {
Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
}

// *****************************************************************************************************************************************************

void connectWifi() {
  WiFi.mode(WIFI_STA);
  if (WiFi.status() == WL_CONNECTED) return;
  //Manual Wifi
  Serial.printf("Connecting to WiFi SSID:  %s", WIFI_SSID.c_str());
  WiFi.disconnect();
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
  int i = 0;
  int counter = 0;
  WiFiConnectLoop = 0;
  while (WiFi.status() != WL_CONNECTED) {
    MyWaitLoop(500);
    if (i > 98) i = 0;
    drawProgress(i,"WiFi: " + String(WIFI_SSID.c_str()));
    i += 10;
    Serial.print(".");
    WiFiConnectLoop += 1;
    counter = MaxWiFiConnectLoop - WiFiConnectLoop;
    gfx.setTextAlignment(TEXT_ALIGN_CENTER);
    if (WiFiConnectLoop > 20) {
    gfx.drawString(120, 270, I18N_WIFI_MANAGER_LAUNCHING);
    gfx.drawString(120, 290, (String)counter);
    gfx.commit();}  
    if (WiFiConnectLoop >= MaxWiFiConnectLoop) {break;}
  }
  if (WiFi.status() != WL_CONNECTED) { 
  Serial.println("RESET - Restarting");
  ResetWeatherStation();} else {
  drawProgress(100,"WiFi: " + String(WIFI_SSID.c_str()));
  Serial.println("connected.");
  Serial.printf("Connected, IP address: %s/%s\n", WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str()); //Get ip and subnet mask
  Serial.printf("Connected, MAC address: %s\n", WiFi.macAddress().c_str());  //Get the local mac address
  }
}

// *****************************************************************************************************************************************************

void loadPropertiesFromLittlefs() {
if (isFSMounted == true)
{
int UWDInt = 0;  
Serial.println("Load Adjustments ...");
  if (LittleFS.exists("/ssid.txt")){
  WIFI_SSID = SetupLoad("ssid.txt");} else {WIFI_SSID = "";}
  WIFI_SSID.trim();
  if (LittleFS.exists("/ssid-passwd.txt")){
  WIFI_PASS = SetupLoad("ssid-passwd.txt");} else {WIFI_PASS = "";}
  WIFI_PASS.trim();  
  if (LittleFS.exists("/owm-apikey.txt")){
  OPEN_WEATHER_MAP_API_KEY = SetupLoad("owm-apikey.txt");} else {OPEN_WEATHER_MAP_API_KEY = "";}
  OPEN_WEATHER_MAP_API_KEY.trim();
  if (LittleFS.exists("/owm-cityid.txt")){
  OPEN_WEATHER_MAP_LOCATION_ID = SetupLoad("owm-cityid.txt");} else {OPEN_WEATHER_MAP_LOCATION_ID = "";} 
  OPEN_WEATHER_MAP_LOCATION_ID.trim();
  if (LittleFS.exists("/owm-uwd.txt")){
  UWD = SetupLoad("owm-uwd.txt");} else { UWD = "";} 
  UWD.trim();
  if (LittleFS.exists("/owm-timezone.txt")){
  TIMEZONE = SetupLoad("owm-timezone.txt");} else {TIMEZONE = "";}
  TIMEZONE.trim();

  // Check Update-Interval
  if (!IsNumeric(UWD)) {UWDInt = 10;} else {UWDInt = UWD.toInt();}
  if (UWDInt > 999) {UWDInt = 10;}
  UPDATE_INTERVAL_SECS = UWDInt * 60; // Set Update Weather data every UWDint
  UWD = (String)UWDInt;
 } 
}

// *****************************************************************************************************************************************************

void savePropertiesToLittlefs() {
if (isFSMounted == true)
{
Serial.println("Save Adjustments ...");
SetupSave("ssid.txt", WIFI_SSID); // SSID WLAN
SetupSave("ssid-passwd.txt", WIFI_PASS); // Password WLAN
if (OPEN_WEATHER_MAP_API_KEY != "" && OPEN_WEATHER_MAP_LOCATION_ID != "") {
SetupSave("owm-apikey.txt", OPEN_WEATHER_MAP_API_KEY);
SetupSave("owm-cityid.txt", OPEN_WEATHER_MAP_LOCATION_ID);} else {
if (LittleFS.exists("/owm-apikey.txt")){LittleFS.remove("/owm-apikey.txt");}
if (LittleFS.exists("/owm-cityid.txt")){LittleFS.remove("/owm-cityid.txt");}} 
SetupSave("owm-uwd.txt", UWD); // OpenWeatherMap update interval
SetupSave("owm-timezone.txt", TIMEZONE);
  }  
}

// *****************************************************************************************************************************************************

void ResetWeatherStation()
{
bool IsFileExist = false;
LittleFS.format();
if (OPEN_WEATHER_MAP_API_KEY != "" && OPEN_WEATHER_MAP_LOCATION_ID != "") {
SetupSave("owm-apikey.txt", OPEN_WEATHER_MAP_API_KEY); 
SetupSave("owm-cityid.txt", OPEN_WEATHER_MAP_LOCATION_ID);}
SetupSave("owm-uwd.txt", UWD);
SetupSave("owm-timezone.txt", TIMEZONE);
MyWaitLoop(500);
Serial.println("Reset - Restarting");
ESP.restart();   
}

// *****************************************************************************************************************************************************

// check a string to see if it is numeric and accept Decimal point and positiv negative value
boolean IsNumeric(String str)
{
if(str.length()<1){return false;}
bool bPoint=false;
if ((str.charAt(0) == '-' || str.charAt(0) == '+') && (str.length()>1)) {  
for(unsigned char i = 1; i < str.length(); i++)
{
if ( !(isDigit(str.charAt(i)) || str.charAt(i) == '.' )|| bPoint) {return false;}
if(str.charAt(i) == '.'){bPoint=true;};
}
} else
{
for(unsigned char i = 0; i < str.length(); i++)
{
if ( !(isDigit(str.charAt(i)) || str.charAt(i) == '.' )|| bPoint) {return false;}
if(str.charAt(i) == '.'){bPoint=true;};
}  
  }
return true;
}

// *****************************************************************************************************************************************************

void initTime() {
  time_t now;
  gfx.fillBuffer(MINI_BLACK);
  Serial.printf("Configuring time for timezone %s\n", TIMEZONE.c_str());
  configTime(TIMEZONE.c_str(), NTP_SERVERS);
  int i = 1;
  WiFiConnectLoop = 0;
  while ((now = time(nullptr)) < NTP_MIN_VALID_EPOCH) {
    if (i > 98) {i = 1;} 
    drawProgress(i, I18N_UPDATING_CLOCK);
    Serial.print(".");
    WiFiConnectLoop += 1;
    if (WiFiConnectLoop >= MaxWiFiConnectLoop) {break;}
    MyWaitLoop(500);
    i++;
  }
  if (WiFiConnectLoop >= MaxWiFiConnectLoop) {ESP.restart();}
  drawProgress(100, I18N_CLOCK_UPDATED);
  Serial.println();

  printf("Local time: %s", asctime(localtime(&now))); // print formated local time, same as ctime(&now)
  printf("UTC time:   %s", asctime(gmtime(&now)));    // print formated GMT/UTC time
}

// *****************************************************************************************************************************************************

void MyWaitLoop(int wlt)
{// Pause Loop
waitloop = 0;
while (waitloop < wlt)
{ 
waitloop += 1;
delay(1);
yield();
}
}

// *****************************************************************************************************************************************************

String SetupLoad(String file_name) {
String result = ""; // init
  File this_file = LittleFS.open(file_name, "r");
  if (!this_file) { // failed to open the file, retrn empty result
    return result;
  }
  while (this_file.available()) {
      result += (char)this_file.read();
  }
  this_file.close();
  MyWaitLoop(10); //wait
  return result;
}

// *****************************************************************************************************************************************************

bool SetupSave(String file_name, String contents) {  
  File this_file = LittleFS.open(file_name, "w");
  if (!this_file) { // failed to open the file, return false
    return false;
  }
  int bytesWritten = this_file.print(contents);
 
  if (bytesWritten == 0) { // write failed
      return false;
  }
  this_file.close();
  MyWaitLoop(10); //wait
  return true;
}

// *****************************************************************************************************************************************************

/*Check and activate when it is time to go to sleep

    parameters: (defined in settings)
      SLEEP_INTERVAL_SECS   time between screen touches in seconds before activating sleep mode
      HARD_SLEEP        true  -> deep sleep requiring interrupt or reset to wake
                  false -> soft sleep turning off backlight wake with screen press

    returns: true when sleep mode is active
*/
bool sleep_mode() {
  static bool sleeping = false; // no need to waste time painting going to sleep screens
  if (SLEEP_INTERVAL_SECS
      && millis() - timerPress >= SLEEP_INTERVAL_SECS * 1000) { // after 2 minutes go to sleep
    if (true == sleeping)
      return sleeping;  // all-ready asleep

    int s = 0;
    do {
      drawProgress(s, I18N_SLEEP_MODE);
      delay(10);
      yield();
    } while (s++ < 100);
    if (s < 100) {                         // early exit abort
      timerPress = millis();               // reset sleep timeout
      //touch stuff was here
    } else {
      sleeping = true;
      if (true == HARD_SLEEP) {
        // go to deepsleep for xx minutes or 0 = permanently
        ESP.deepSleep(0, WAKE_RF_DEFAULT); // 0 delay = permanently to sleep
      } else {
        digitalWrite(TFT_LED, LOW);        // Back light OFF
      }
    }
  } else {                                 // Not time to sleep
    if (sleeping) {                        // Wake up
      digitalWrite(TFT_LED, HIGH);         // Back light ON
      sleeping = false;
    }
  }
  return sleeping;  // used to prevent screen changes on wake-up screen press
} // sleep_mode()

// *****************************************************************************************************************************************************

// Update the internet based information and update screen
void updateData() {
  time_t now = time(nullptr);

  gfx.fillBuffer(MINI_BLACK);

  drawProgress(50, I18N_UPDATING_WEATHER);
  OpenWeatherMapCurrent *currentWeatherClient = new OpenWeatherMapCurrent();
  currentWeatherClient->setMetric(IS_METRIC);
  currentWeatherClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  currentWeatherClient->updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID);
  delete currentWeatherClient;
  currentWeatherClient = nullptr;

  drawProgress(70, I18N_UPDATING_FORECASTS);
  OpenWeatherMapForecast *forecastClient = new OpenWeatherMapForecast();
  forecastClient->setMetric(IS_METRIC);
  forecastClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  uint8_t allowedHours[] = {12, 0};
  forecastClient->setAllowedHours(allowedHours, sizeof(allowedHours));
  forecastClient->updateForecastsById(forecasts, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID, MAX_FORECASTS);
  delete forecastClient;
  forecastClient = nullptr;

  drawProgress(80, I18N_UPDATING_ASTRONOMY);
  // 'now' has to be epoch instant, lat/lng in degrees not radians
  SunMoonCalc *smCalc = new SunMoonCalc(now, currentWeather.lat, currentWeather.lon);
  moonData = smCalc->calculateSunAndMoonData().moon;
  delete smCalc;
  smCalc = nullptr;
  Serial.printf("Free mem: %d\n",  ESP.getFreeHeap());
  LastWeatherUpdateTime = UpdateWeatherTime();
  sensors.requestTemperatures(); 
  temperatureC = sensors.getTempCByIndex(0);
  temperatureF = sensors.getTempFByIndex(0);
  delay(1000);
}

// *****************************************************************************************************************************************************

void drawLogo(){

  gfx.setFont(ArialRoundedMTBold_36);
  gfx.setColor(MINI_COLOR_A);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.drawString(120, 10, I18N_LOGO_UPPER);
  gfx.setColor(MINI_COLOR_B);
  gfx.drawString(120, 46, I18N_LOGO_LOWER);

}

// Progress bar helper
void drawProgress(uint8_t percentage, String text) {
  gfx.fillBuffer(MINI_BLACK);
  //gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo); // Disabled here but visible on the about screen
  drawLogo();

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(120, 146, text);
  gfx.drawString(115, 190, I18N_LOADING);
  gfx.setColor(MINI_WHITE);
  gfx.drawRect(10, 168, 240 - 20, 15);
  gfx.setColor(MINI_COLOR_B);
  gfx.fillRect(12, 170, 216 * percentage / 100, 11);
  gfx.commit();
}

// *****************************************************************************************************************************************************

// draws the clock
void drawTime() {
  char time_str[11];
  time_t now = time(nullptr);
  struct tm * timeinfo = localtime(&now);

  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_WHITE);
  String date = WDAY_NAMES[timeinfo->tm_wday] + " " + String(timeinfo->tm_mday) + ". " + MONTH_NAMES[timeinfo->tm_mon] + " " + String(1900 + timeinfo->tm_year);
  gfx.drawString(120, 6, date);

  gfx.setFont(ArialRoundedMTBold_36);

  if (IS_STYLE_12HR) {                            //12:00
    int hour = (timeinfo->tm_hour + 11) % 12 + 1; // take care of noon and midnight
    if (IS_STYLE_HHMM) {
      sprintf(time_str, "%2d:%02d\n", hour, timeinfo->tm_min);                //hh:mm
    } else {
      sprintf(time_str, "%2d:%02d:%02d\n", hour, timeinfo->tm_min, timeinfo->tm_sec); //hh:mm:ss
    }
  } else {                                                                            //24:00
    if (IS_STYLE_HHMM) {
        sprintf(time_str, "%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min); //hh:mm
    } else {
        sprintf(time_str, "%02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); //hh:mm:ss
    }
  }

  gfx.drawString(120, 20, time_str);

  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setFont(ArialMT_Plain_10);
  gfx.setColor(MINI_COLOR_B);
  if (IS_STYLE_12HR) {
    sprintf(time_str, "\n%s", timeinfo->tm_hour >= 12 ? "PM" : "AM");
    gfx.drawString(195, 27, time_str);
  }
}

// *****************************************************************************************************************************************************

// draws the clock
String UpdateWeatherTime() {
  char time_str[11];
  String TimeString;
  time_t now = time(nullptr);
  struct tm * timeinfo = localtime(&now);
  if (IS_STYLE_12HR) { //12:00
    int hour = (timeinfo->tm_hour + 11) % 12 + 1; // take care of noon and midnight
    sprintf(time_str, "%2d:%02d", hour, timeinfo->tm_min); //hh:mm
    if ((timeinfo->tm_hour) >= 12) {TimeString = (String)time_str +" pm";} else {TimeString = (String)time_str + " am";}
  } else
  {sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
  TimeString = (String)time_str;} //hh:mm

    return TimeString;
}

// *****************************************************************************************************************************************************

// draws current weather information
void drawCurrentWeather() {
  gfx.setTransparentColor(MINI_BLACK);
  gfx.drawPalettedBitmapFromPgm(0, 55, getMeteoconIconFromProgmem(currentWeather.icon));

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_COLOR_B);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(220, 65, currentWeather.cityName);

  gfx.setFont(ArialRoundedMTBold_36);
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.drawString(100, 78, String(currentWeather.temp, 1) + (IS_METRIC ? " °C" : " °F"));

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(235, 84, I18N_INTERNAL_TEMPERATURE_SHORT);
  gfx.drawString(235, 98, String((IS_METRIC ? temperatureC : temperatureF), 1) + (IS_METRIC ? " °C" : " °F")); // One decimal place

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setColor(MINI_COLOR_A);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(235, 122, currentWeather.description);
}

// *****************************************************************************************************************************************************

void drawForecast1(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) {
  drawForecastDetail(x + 10, y + 165, 0);
  drawForecastDetail(x + 95, y + 165, 1);
  drawForecastDetail(x + 180, y + 165, 2);
}

// *****************************************************************************************************************************************************

void drawForecast2(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) {
  drawForecastDetail(x + 10, y + 165, 3);
  drawForecastDetail(x + 95, y + 165, 4);
  drawForecastDetail(x + 180, y + 165, 5);
}

// *****************************************************************************************************************************************************

void drawForecast3(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) {
  drawForecastDetail(x + 10, y + 165, 6);
  drawForecastDetail(x + 95, y + 165, 7);
  drawForecastDetail(x + 180, y + 165, 8);
}

// *****************************************************************************************************************************************************

// helper for the forecast columns
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) {
  gfx.setColor(MINI_COLOR_A);
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  time_t time = forecasts[dayIndex].observationTime;
  struct tm * timeinfo = localtime (&time);
  if (String(timeinfo->tm_hour) == "14") {
    gfx.drawString(x + 25, y - 15, WDAY_NAMES[timeinfo->tm_wday] + " (T)");} else
  {gfx.drawString(x + 25, y - 15, WDAY_NAMES[timeinfo->tm_wday] + " (N)");}
  
  gfx.setColor(MINI_WHITE);
  gfx.drawString(x + 25, y + 4, String(forecasts[dayIndex].temp, 1) + (IS_METRIC ? " °C" : " °F"));

  gfx.drawPalettedBitmapFromPgm(x, y + 20, getMiniMeteoconIconFromProgmem(forecasts[dayIndex].icon));
  gfx.setColor(MINI_COLOR_B);
  gfx.drawString(x + 25, y + 65, String(forecasts[dayIndex].rain, 1) + (IS_METRIC ? " mm" : " in"));
}

// *****************************************************************************************************************************************************

// draw moonphase, sunrise/set and moonrise/set
void drawAstronomy() {
  gfx.setFont(MoonPhases_Regular_36);
  gfx.setColor(MINI_WHITE);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.drawString(120, 275, String(determineMoonIcon()));

  gfx.setColor(MINI_WHITE);
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(120, 250, MOON_PHASES[moonData.phase.index]);

  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(5, 250, SUN_MOON_TEXT[0]);
  gfx.setColor(MINI_WHITE);
  time_t time = currentWeather.sunrise;
  gfx.drawString(5, 276, SUN_MOON_TEXT[1] + ":");
  if (!IS_STYLE_12HR) {
   gfx.drawString(45, 276, getTime(&time));} else
  {gfx.drawString(35, 276, getTime(&time));}
  time = currentWeather.sunset;
  gfx.drawString(5, 291, SUN_MOON_TEXT[2] + ":");
  if (!IS_STYLE_12HR) {
   gfx.drawString(45, 291, getTime(&time));} else
  {gfx.drawString(35, 291, getTime(&time));}

  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(235, 250, SUN_MOON_TEXT[3]);
  gfx.setColor(MINI_WHITE);

  float lunarMonth = 29.53;
  // approximate moon age
  gfx.drawString(190, 276, SUN_MOON_TEXT[3]);
  gfx.drawString(190, 291, SUN_MOON_TEXT[5] + ":");
  gfx.drawString(235, 291, String(moonData.illumination * 100, 0) + "%");
}

// *****************************************************************************************************************************************************

// The Moon Phases font has 26 icons for gradiations, 1 full icon, and 1 empty icon: https://www.dafont.com/moon-phases.font
// All of them are an approximation. 
// Depending on date and location they would have to be rotated left or right by a varying degree.
// GOTCHA  I: as we use white to display the moon icon, what is black on that font page (link above) will effectively be rendered white!
// GOTCHA II: illumination in the range {0,1} will with near certainty never be exactly 0 or 1; rounding is, therefore, essential to ever get full/new moon!
char determineMoonIcon() {
  char moonIcon;
  // index in range of 0..14
  char index = round(moonData.illumination * 14);
//  Serial.printf("Moon illumination: %f -> moon icon index: %d\n", moonData.illumination, index);
  if (moonData.phase.index > 4) {
    // waning (4 = full moon)
    moonIcon = currentWeather.lat > 0 ? MOON_ICONS_NORTH_WANING[index] : MOON_ICONS_SOUTH_WANING[index];
  } else {
    // waxing
    moonIcon = currentWeather.lat > 0 ? MOON_ICONS_NORTH_WAXING[index] : MOON_ICONS_SOUTH_WAXING[index];
  }
  return moonIcon;
}

// *****************************************************************************************************************************************************

void drawCurrentWeatherDetail() {
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.fillBuffer(MINI_BLACK);
  // gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo); // Disabled here but visible on the about screen
  drawLogo();

  gfx.setColor(MINI_WHITE);
  gfx.setFont(ArialMT_Plain_16);
  gfx.drawString(120, 125, I18N_LAST_UPDATE);
  gfx.drawString(120, 145, LastWeatherUpdateTime); // Weather Data Update-Time
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_WHITE);

  drawLabelValue(11, I18N_TEMPERATURE, String(currentWeather.temp, 1) + (IS_METRIC ? " °C" : " °F"));
  drawLabelValue(12, I18N_WIND_SPEED, String(currentWeather.windSpeed, 1) + (IS_METRIC ? "m/s" : "mph") );
  drawLabelValue(13, I18N_WIND_DIR, String(currentWeather.windDeg, 1) + "°");
  drawLabelValue(14, I18N_HUMIDITY, String(currentWeather.humidity) + "%");
  drawLabelValue(15, I18N_PRESSURE, String(currentWeather.pressure) + "hPa");
  drawLabelValue(16, I18N_CLOUDS, String(currentWeather.clouds) + "%");
  drawLabelValue(17, I18N_VISIBILITY, String(currentWeather.visibility / 1000) + "km");

}

// *****************************************************************************************************************************************************

void drawLabelValue(uint8_t line, String label, String value) {
  const uint8_t labelX = 15;
  const uint8_t valueX = 130; // Default: 150
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(labelX, 30 + line * 15, label);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(valueX, 30 + line * 15, value);
}

// *****************************************************************************************************************************************************

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

// *****************************************************************************************************************************************************

void drawWifiQuality() {
  int8_t quality = getWifiQuality();
  gfx.setColor(MINI_WHITE);
  gfx.setFont(ArialMT_Plain_10);
  gfx.setTextAlignment(TEXT_ALIGN_RIGHT);
  gfx.drawString(228, 9, String(quality) + "%");
  for (int8_t i = 0; i < 4; i++) {
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        gfx.setPixel(230 + 2 * i, 18 - j);
      }
    }
  }
}

// *****************************************************************************************************************************************************

void drawForecastTable(uint8_t start) {
  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 2, I18N_FORECASTS);
  uint16_t y = 0;

  String degreeSign = " °F";
  if (IS_METRIC) {
    degreeSign = " °C";
  }
  int firstColumnLabelX = 50;
  int firstColumnValueX = 70;
  int secondColumnLabelX = 130;
  int secondColumnValueX = 170;
  for (uint8_t i = start; i < start + 4; i++) {
    gfx.setTextAlignment(TEXT_ALIGN_LEFT);
    y = 45 + (i - start) * 75;
    if (y > 320) {
      break;
    }
    gfx.setColor(MINI_WHITE);
    gfx.setTextAlignment(TEXT_ALIGN_CENTER);
    time_t time = forecasts[i].observationTime;
    struct tm * timeinfo = localtime (&time);
    if (String(timeinfo->tm_hour) == "14") {
    gfx.drawString(120, y - 15, WDAY_NAMES[timeinfo->tm_wday] + " ("+I18N_DAY+")");} else
    {gfx.drawString(120, y - 15, WDAY_NAMES[timeinfo->tm_wday] + " ("+I18N_NIGHT+")");}

    gfx.drawPalettedBitmapFromPgm(0, 0 + y, getMiniMeteoconIconFromProgmem(forecasts[i].icon));
    gfx.setTextAlignment(TEXT_ALIGN_LEFT);
    gfx.setColor(MINI_COLOR_A);
    gfx.setFont(ArialRoundedMTBold_14);
    gfx.setTextAlignment(TEXT_ALIGN_LEFT);

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(firstColumnLabelX, y+1, I18N_TEMPERATURE_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(firstColumnValueX, y+1, "  " + String(forecasts[i].temp, 0) + degreeSign);

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(firstColumnLabelX, y + 16, I18N_HUMIDITY_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(firstColumnValueX, y + 16, "   " + String(forecasts[i].humidity) + " %");

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(firstColumnLabelX, y + 31, I18N_RAIN_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(firstColumnValueX, y + 31, "  " + String(forecasts[i].rain, 0) + (IS_METRIC ? " mm" : " in"));

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(secondColumnLabelX+1, y+1, I18N_PRESSURE_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(secondColumnValueX, y+1, String(forecasts[i].pressure, 0) + " hPa");

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(secondColumnLabelX+1, y + 16, I18N_WIND_SPEED_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(secondColumnValueX, y + 16, String(forecasts[i].windSpeed*3.6, 0) + (IS_METRIC ? " km/h" : " mph") );

    gfx.setColor(MINI_COLOR_B);
    gfx.drawString(secondColumnLabelX+1, y + 31, I18N_WIND_DIR_SHORT);
    gfx.setColor(MINI_WHITE);
    gfx.drawString(secondColumnValueX, y + 31, String(forecasts[i].windDeg, 0) + " °");
  }
}

// *****************************************************************************************************************************************************

void drawAbout() {
  gfx.clear();
  gfx.fillBuffer(MINI_BLACK);
  gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_16);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 90, I18N_WEATHER_STATION);

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  drawLabelValue(6, "Heap Mem:", String(ESP.getFreeHeap() / 1024) + " kb");
  drawLabelValue(7, "Flash Mem:", String(ESP.getFlashChipRealSize() / 1024 / 1024) + " MB");
  drawLabelValue(8, "WiFi Strength:", String(WiFi.RSSI()) + " dB");
  drawLabelValue(9, "Chip ID:", String(ESP.getChipId()));
  drawLabelValue(10, "VCC: ", String(ESP.getVcc() / 1024.0) + " V");
  drawLabelValue(11, "CPU Freq.: ", String(ESP.getCpuFreqMHz()) + " MHz");
  char time_str[15];
  const uint32_t millis_in_day = 1000 * 60 * 60 * 24;
  const uint32_t millis_in_hour = 1000 * 60 * 60;
  const uint32_t millis_in_minute = 1000 * 60;
  uint8_t days = millis() / (millis_in_day);
  uint8_t hours = (millis() - (days * millis_in_day)) / millis_in_hour;
  uint8_t minutes = (millis() - (days * millis_in_day) - (hours * millis_in_hour)) / millis_in_minute;
  sprintf(time_str, "%2dT%2dST%2dM", days, hours, minutes);
  drawLabelValue(12, "Uptime: ", time_str);
  drawLabelValue(13, "IP address: ", WiFi.localIP().toString());
  drawLabelValue(14, "WiFi SSID: ", String(WiFi.SSID()));
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_COLOR_A);
  gfx.drawString(15, 280, "Last Reset: ");
  gfx.setColor(MINI_WHITE);
  gfx.drawStringMaxWidth(16, 295, 240 - 2 * 15, ESP.getResetInfo());
}

// *****************************************************************************************************************************************************

void calibrationCallback(int16_t x, int16_t y) {
  gfx.setColor(1);
  gfx.fillCircle(x, y, 10);
}

// *****************************************************************************************************************************************************

String getTime(time_t *timestamp) {
  struct tm *timeInfo = localtime(timestamp);

  char buf[9];  // "12:34 pm\0"
  char ampm[3];
  ampm[0]='\0'; //Ready for 24hr clock
  uint8_t hour = timeInfo->tm_hour;

  if (IS_STYLE_12HR) {
    if (hour > 12) {
      hour = hour - 12;
      sprintf(ampm, " pm");
    } else {
      sprintf(ampm, " am");
    }
    sprintf(buf, "%2d:%02d %s", hour, timeInfo->tm_min, ampm);
  } else {
    sprintf(buf, "%02d:%02d %s", hour, timeInfo->tm_min, ampm);
  }
  return String(buf);
}

// *****************************************************************************************************************************************************

/*
 *  Convert hour from 24 hr time to 12 hr time.
 *
 *  @return cString with 2 digit hour + am or pm 
 */
char* make12_24(int hour){
  static char hr[6];
  if(hour > 12){
    sprintf(hr, "%2d pm", (hour -12) );
    //sprintf(buf, "%02d:%02d %s", hour, timeInfo->tm_min, ampm);
  } else {
    sprintf(hr, "%2d am", hour);
  }
  return hr;
}

void ResetWeatherStationScreen(){ 
gfx.clear();
  gfx.fillBuffer(MINI_BLACK);
  gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setFont(ArialMT_Plain_16);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 90, I18N_WEATHER_STATION);
gfx.drawString(120, 160, I18N_WEATHER_STATION_RESET);  
gfx.drawString(120, 180, I18N_CLICK_TO_CANCEL);
gfx.drawString(120, 270, I18N_WIFI_MANAGER_LAUNCHING);
if (WSReset > 0 && screen == 5){
Serial.print(".");
MyWaitLoop(500); // wait
WSReset -= 1;
gfx.drawString(120, 290, (String)WSReset);} 
if (WSReset <= 0 && screen == 5) {Serial.print("Resetting weather station"); ResetWeatherStation();}  
}
