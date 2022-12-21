/**The MIT License (MIT)
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifndef SETTINGS_H
#define SETTINGS_H
#include "TZinfo.h"

// ************** Setup *************************************************************************
// **********************************************************************************************

#define OTA_PASSWORD "fDXuid2378sd7623eYa9S8d" // Change this to a random value

String WIFI_SSID = ""; // set from WiFiManager (AccessPoint)
String WIFI_PASS = ""; // set from WiFiManager (AccessPoint)
String TZ_LOCATION = "Europe/Rome"; // can be changed from WiFiManager (AccessPoint)
#define WIFI_HOSTNAME I18N_WEATHER_STATION

int UPDATE_INTERVAL_SECS = 20 * 60; // Update every 10 minutes - default
const int SLEEP_INTERVAL_SECS = 60;  // Going to sleep after idle times, set 0 for insomnia
const boolean HARD_SLEEP = false;   // true go into deepSleep false = turn Back light off

// OpenWeatherMap Settings
// Sign up here to get an API key: https://docs.thingpulse.com/how-tos/openweathermap-key/
String OPEN_WEATHER_MAP_API_KEY = ""; // set from WiFiManager (AccessPoint)
/*
Go to https://openweathermap.org/find?q= and search for a location. Go through the
result set and select the entry closest to the actual location you want to display 
data for. It'll be a URL like https://openweathermap.org/city/2657896. The number
at the end is what you assign to the constant below.
 */
String OPEN_WEATHER_MAP_LOCATION_ID = ""; // set from WiFiManager (AccessPoint)

/*
Arabic -> ar, Bulgarian -> bg, Catalan -> ca, Czech -> cz, German -> de, Greek -> el,
English -> en, Persian (Farsi) -> fa, Finnish -> fi, French -> fr, Galician -> gl,
Croatian -> hr, Hungarian -> hu, Italian -> it, Japanese -> ja, Korean -> kr,
Latvian -> la, Lithuanian -> lt, Macedonian -> mk, Dutch -> nl, Polish -> pl,
Portuguese -> pt, Romanian -> ro, Russian -> ru, Swedish -> se, Slovak -> sk,
Slovenian -> sl, Spanish -> es, Turkish -> tr, Ukrainian -> ua, Vietnamese -> vi,
Chinese Simplified -> zh_cn, Chinese Traditional -> zh_tw.
*/
const String OPEN_WEATHER_MAP_LANGUAGE = I18N_LOCALE;

const String SUN_MOON_TEXT[] = {I18N_SUN, I18N_SUNRISE, I18N_SUNSET, I18N_MOON, I18N_PHASE, I18N_ILLUMINATED};
const String WDAY_NAMES[] = {I18N_SUNDAY, I18N_MONDAY, I18N_TUESDAY, I18N_WEDNESDAY, I18N_THURSDAY, I18N_FRIDAY, I18N_SATURDAY};
const String MONTH_NAMES[] = {I18N_JAN, I18N_FEB, I18N_MAR, I18N_APR, I18N_MAY, I18N_JUN, I18N_JUL, I18N_AUG, I18N_SEP, I18N_OCT, I18N_NOV, I18N_DEC};
const String MOON_PHASES[] = {I18N_MOON_NEW, I18N_MOON_WAXING_CRESCENT, I18N_MOON_FIRST_QUARTER, I18N_MOON_WAXING_GIBBOUS, I18N_MOON_FULL, I18N_MOON_WANING_GIBBOUS, I18N_MOON_LAST_QUARTER, I18N_MOON_WANING_CRESCENT};

// pick one from TZinfo.h
// String TIMEZONE = getTzInfo(TZ_LOCATION);
String TIMEZONE;  

// values in metric or imperial system?
bool IS_METRIC = true;

// Change for 12 Hour/ 24 hour style clock
bool IS_STYLE_12HR = false;

// Change for HH:MM/ HH:MM:SS format clock
bool IS_STYLE_HHMM = true; // true => HH:MM

// ************** Setup - End *******************************************************************
// **********************************************************************************************

// change for different NTP (time servers)
#define NTP_SERVERS "pool.ntp.org"
// #define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

// Locations on the northern hemisphere (latitude > 0) and those on the southern hemisphere need 
// an inverted set of moon phase icons/characters.
// fully illuminated -> full moon -> char 48
// zero illumination -> new moon -> char 64
const char MOON_ICONS_NORTH_WANING[] = {64, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 48};
const char MOON_ICONS_NORTH_WAXING[] = {64, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 48};
const char MOON_ICONS_SOUTH_WANING[] = {64, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 48};
const char MOON_ICONS_SOUTH_WAXING[] = {64, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 48};

// August 1st, 2018
// #define NTP_MIN_VALID_EPOCH 1533081600
// October Third, 2022
#define NTP_MIN_VALID_EPOCH 1664802185

// Pins for the ILI9341
#define TFT_DC    2     // D4
#define TFT_CS    4 // D2
#define TFT_LED 15 //D8
//

#define BUTTON_PIN 5 //D1 
// GPIO where the DS18B20 is connected to
#define SENSOR_PIN 0 // D3 

//#define UNUSED_PIN 16 //D0

/***************************
 * End Settings
 **************************/
#endif
