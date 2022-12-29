/* ***************** Captive Portal *********************/
IPAddress cpIP(192, 168, 10, 1);  // Captive Portal IP-Address - outside of the router's own DHCP range

String ProgramName = APSSID;

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void CaptivePortal(){
  // Start Captive Portal (Access Point)
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(cpIP, cpIP, IPAddress(255, 255, 255, 0));   //Captive Portal IP-Address
  WiFi.softAP(ProgramName, "");
  dnsServer.start(DNS_PORT, "*", cpIP);
  server.onNotFound(handlePortal);
  server.on("/", handlePortal);
  server.begin();
  Serial.println("Captive portal started");
}

String getHtmlHead(){
return F("<!doctype html><html lang='") + String(I18N_LOCALE) + F("'>\
<head><meta charset='utf-8'>\
<meta name='viewport' content='width=device-width, initial-scale=1'>\
<title>") + String(I18N_WIFI_MANAGER) + F("</title>\
<style>\
*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:\
'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;\
font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:\
block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid\
transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;\
line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}\
h1,p{text-align: center}\
h5{ font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}\
h6{ font-size: 18px; text-align:center; margin-top: 0px; margin-bottom: 15px;}\
</style></head>");
}

void handlePortal() {
  String HTMLString;
  HTMLString += getHtmlHead();

  Serial.println("Handling new http request");
  String ssid; // for Sort SSID's
  int loops = 0; // for Sort SSID's
  if (ProgramName == "") {ProgramName = "Hostname";} 
  if (server.method() == HTTP_POST) {
    WIFI_SSID = server.arg("ssid"); // Wifi SSID
    WIFI_PASS = server.arg("password"); // Wifi SSID Password
    TZ_LOCATION = server.arg("timezone");
    TIMEZONE = getTzInfo(TZ_LOCATION);
    IS_METRIC = String(server.arg("metric")).toInt();
    OPEN_WEATHER_MAP_API_KEY = server.arg("apikey"); // OpenWeatherMap - API-Key
    OPEN_WEATHER_MAP_LOCATION_ID = server.arg("cityid"); // OpenWeatherMap - City ID
    UWD = server.arg("uwd"); // OpenWeatherMap - Update-Interval in Minutes

    if (WIFI_SSID != "" && WIFI_PASS != "" && OPEN_WEATHER_MAP_API_KEY != "" && OPEN_WEATHER_MAP_LOCATION_ID != "") { 
    Serial.println("SSID: "+WIFI_SSID); // Wifi SSID
    Serial.println("Password: "+WIFI_PASS); // Wifi SSID Password

    if (isFSMounted == true){
    savePropertiesToLittlefs();
    
    Serial.println(I18N_SETTINGS_SAVED);
        HTMLString += F("<body><main class='form-signin'>\
<h1>") + String(I18N_WIFI_MANAGER) + F("</h1>\
<h5>(") + String(ProgramName) + F(")</h5>\
<br/>\
<h6>") + I18N_SETTINGS_SAVED + F("<br />") + I18N_RESTARTING + F("</h6>\
</main>\
</body>\
</html>");
    server.send(200, "text/html", HTMLString); // Captive Portal 
    // Reset
    Serial.println(I18N_RESTARTING);
    MyWaitLoop(3000);
    ESP.restart();  
    }
     else
     {
     Serial.println(I18N_SETTINGS_NOT_SAVED);
    HTMLString += F("<body><main class='form-signin'>\
<h1>") + String(I18N_WIFI_MANAGER) + F("</h1>\
<h5>(") + String(ProgramName) + F(")</h5>\
<br/>\
<h6>") + String(I18N_SETTINGS_NOT_SAVED) + F("<br />") + String(I18N_RESTARTING) + F("</h6>\
</main>\
</body>\
</html>");
    server.send(200, "text/html", HTMLString); // Captive Portal 
     // Reset
     Serial.println(I18N_RESTARTING);
     MyWaitLoop(3000);
     ESP.restart();  
    } 
    } else
       {
    // Settings incomplete
    HTMLString +=  F("<body><main class='form-signin'>\
<h1>") + String(I18N_WIFI_MANAGER) + F("</h1>\
<h5>(") + String(ProgramName) + F(")</h5>\
<br/>\
<h6>") + String(I18N_SETTINGS_INCOMPLETE) + F("<br />") + String(I18N_SETTINGS_NOT_SAVED) + F("</h6>\
<br/>\
<a href =\"/\"><button class=\"button\">") + String(I18N_BACK) + F("</button></a>\
</main>\
</body>\
</html>");}
    server.send(200, "text/html", HTMLString);
  } else {
    // Captive Portal
    Serial.println("Serving captive portal settings page");
    int n = WiFi.scanNetworks(false, false); //WiFi.scanNetworks(async, show_hidden)
HTMLString +=  F("<body><main class='form-signin'>\
<form action='/' method='post'>\
<h1>") + String(I18N_WIFI_MANAGER) + F("</h1>\
<h5>(") + String(ProgramName) + F(")</h5>\
<br/>");
    if (n > 0) {
      // WLAN's sort by RSSI
      int indices[n];
      int skip[n];
      int o = n;
      for (int i = 0; i < n; i++) {
      indices[i] = i;}
      for (int i = 0; i < n; i++) {
          for (int j = i + 1; j < n; j++) {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
              loops++;
              std::swap(indices[i], indices[j]);
              std::swap(skip[i], skip[j]);}}}
        for (int i = 0; i < n; i++) {
          if(indices[i] == -1){
            --o;
            continue;}
          ssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) {
            loops++;
            if (ssid == WiFi.SSID(indices[j])) {
              indices[j] = -1;}}}
      for (int i = 0; i < n; ++i){
      // Print SSID 
          HTMLString += F("<h6 onclick='SetSSID(") + String(i) + F(")' id='fssid") + String(i) + F("'>") + WiFi.SSID(indices[i]) + F("</h6>");
      }
    } else {
      HTMLString += F("<br/><br/><h6>") + String(I18N_NO_NETWORK_FOUND) + F("</h6><br/>");
    }
      HTMLString += F("<br/>\
<div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid' id='ssid' value='' id='CPSSID'></div>\
<div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password' id='password' value=''></div>\
<div class='form-floating'><br/><label>") + String(I18N_TIMEZONE) + F("</label><input type='text' class='form-control' name='timezone' id='timezone' value='") + String(TZ_LOCATION) + F("'></div>\
<div class='form-floating'><br/><label>") + String(I18N_IS_METRIC) + F("</label><input type='checkbox' class='form-control' name='metric' id='metric' value='1' checked></div>\
<div class='form-floating'><br/><label>OpenWeatherMap - API-Key</label><input type='text' class='form-control' name='apikey' id='apikey' value='") + String(OPEN_WEATHER_MAP_API_KEY) + F("'></div>\
<div class='form-floating'><br/><label>OpenWeatherMap - City-ID</label><input type='text' class='form-control' name='cityid' id='cityid' value='") + String(OPEN_WEATHER_MAP_LOCATION_ID) + F("'></div>\
<div class='form-floating'><br/><label>OpenWeatherMap - ") + String(I18N_OWM_UPDATES_IN_MINS) + F("</label><input type='text' class='form-control' name='uwd' id='uwd' value='") + String(UWD) + F("'></div>\
<br/><br/><button type='submit'>") + String(I18N_SAVE) + F("</button>\
</form>\
<br/><br/>\
<form action='/' method='get'>\
<button type='submit'>") + String(I18N_REFRESH) + F("</button>\
</form>\
</main>\
<script>function SetSSID(n) {document.getElementById('ssid').value = document.getElementById('fssid' + n).innerHTML;}</script>\
</body>\
</html>");
      server.send(200, "text/html", HTMLString); // Captive Portal
      Serial.println("Sent configuration page");
  }
}

// *****************************************************************************************************************************************************

void WebSiteNotFound() {
  Serial.println("Serving 404 page");
  String HTMLString;
  HTMLString += getHtmlHead();
  HTMLString += F("<body><main class='form-signin'>\
<h1>") + String(I18N_WIFI_MANAGER) + F("</h1>\
<h5>(") + String(ProgramName) + F(")</h5>\
<br/>\
<h6>") + String(I18N_WEBSITE_NOT_FOUND) + F("</h6>\
<br/><br/>\
<a href =\"/\"><button class=\"button\">") + String(I18N_BACK) + F("</button></a>\
</main>\
</body>\
</html>");
server.send(200, "text/html", HTMLString);   
} 