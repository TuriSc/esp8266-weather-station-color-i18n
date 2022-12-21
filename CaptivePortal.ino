/* ***************** Captive Portal *********************/
IPAddress cpIP(192, 168, 10, 1);  // Captive Portal IP-Address - outside of the router's own DHCP range

String ProgramName = WIFI_HOSTNAME;

// *****************************************************************************************************************************************************

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

// *****************************************************************************************************************************************************

void CaptivePortal(){
// Start Captive Portal (Access Point)
WiFi.mode(WIFI_AP);
WiFi.softAPConfig(cpIP, cpIP, IPAddress(255, 255, 255, 0));   //Captive Portal IP-Address
WiFi.softAP(ProgramName, "");
dnsServer.start(DNS_PORT, "*", cpIP);
server.onNotFound(handlePortal);
server.on("/", handlePortal);
server.begin();
}

// *****************************************************************************************************************************************************

void handlePortal() {
String HTMLString; 
String ssid; // for Sort SSID's
int loops = 0; // for Sort SSID's
if (ProgramName == "") {ProgramName = "Hostname";} 
if (server.method() == HTTP_POST) {
    WIFI_SSID = server.arg("ssid"); // Wifi SSID
    WIFI_PASS = server.arg("password"); // Wifi SSID Password
    TZ_LOCATION = server.arg("timezone");
    TIMEZONE = getTzInfo(TZ_LOCATION);
    OPEN_WEATHER_MAP_API_KEY = server.arg("apikey"); // OpenWeatherMap - API-Key
    OPEN_WEATHER_MAP_LOCATION_ID = server.arg("cityid"); // OpenWeatherMap - City ID
    UWD = server.arg("uwd"); // OpenWeatherMap - Update-Interval in Minutes

    if (WIFI_SSID != "" && WIFI_PASS != "" && OPEN_WEATHER_MAP_API_KEY != "" && OPEN_WEATHER_MAP_LOCATION_ID != "") { 
    Serial.println("SSID: "+WIFI_SSID); // Wifi SSID
    Serial.println("Password: "+WIFI_PASS); // Wifi SSID Password

    if (isFSMounted == true){
    savePropertiesToLittlefs();
    
    Serial.println(I18N_SETTINGS_SAVED);
    HTMLString = "<!doctype html><html lang='"I18N_LOCALE"'>";
    HTMLString += "<head><meta charset='utf-8'>";
    HTMLString += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    HTMLString += "<title>"I18N_WIFI_MANAGER"</title>";
    HTMLString += "<style>";
    HTMLString += "*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:";
    HTMLString += "'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;";
    HTMLString += "font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:";
    HTMLString += "block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid";
    HTMLString += "transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;";
    HTMLString += "line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}";
    HTMLString += "h5 { font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
    HTMLString += "h6 { font-size: 18px; text-align:center; margin-top: 0px; margin-bottom: 15px;}"; 
    HTMLString += "</style>";
    HTMLString += "</head>";
    HTMLString += "<body><main class='form-signin'>";
    HTMLString += "<h1>"I18N_WIFI_MANAGER"</h1>";
    HTMLString += "<h5>("+ProgramName+")</h5>";
    HTMLString += "<br/>";
    HTMLString += "<h6>"I18N_SETTINGS_SAVED"<br />"I18N_RESTARTING"</h6>";
    HTMLString += "</main>";
    HTMLString += "</body>";
    HTMLString += "</html>";
    server.send(200, "text/html", HTMLString); // Captive Portal 
    // Reset
    Serial.println(I18N_RESTARTING);
    MyWaitLoop(3000);
    ESP.restart();  
    }
     else
     {
     Serial.println(I18N_SETTINGS_NOT_SAVED);
    HTMLString = "<!doctype html><html lang='"I18N_LOCALE"'>";
    HTMLString += "<head><meta charset='utf-8'>";
    HTMLString += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    HTMLString += "<title>"I18N_WIFI_MANAGER"</title>";
    HTMLString += "<style>";
    HTMLString += "*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:";
    HTMLString += "'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;";
    HTMLString += "font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:";
    HTMLString += "block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid";
    HTMLString += "transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;";
    HTMLString += "line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}";    
    HTMLString += "h5 { font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
    HTMLString += "h6 { font-size: 18px; text-align:center; margin-top: 0px; margin-bottom: 15px;}"; 
    HTMLString += "</style>";
    HTMLString += "</head>";
    HTMLString += "<body><main class='form-signin'>";
    HTMLString += "<h1>"I18N_WIFI_MANAGER"</h1>";
    HTMLString += "<h5>("+ProgramName+")</h5>";
    HTMLString += "<br/>";
    HTMLString += "<h6>"I18N_SETTINGS_NOT_SAVED"<br />"I18N_RESTARTING"</h6>";
    HTMLString += "</main>";
    HTMLString += "</body>";
    HTMLString += "</html>";
    server.send(200, "text/html", HTMLString); // Captive Portal 
     // Reset
     Serial.println(I18N_RESTARTING);
     MyWaitLoop(3000);
     ESP.restart();  
    } 
    } else
       {
    // Settings incomplete   
    HTMLString =  "<!doctype html><html lang='"I18N_LOCALE"'>";
    HTMLString += "<head><meta charset='utf-8'>";
    HTMLString += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    HTMLString += "<title>"I18N_WIFI_MANAGER"</title>";
    HTMLString += "<style>";
    HTMLString += "*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:";
    HTMLString += "'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;";
    HTMLString += "font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:";
    HTMLString += "block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid";
    HTMLString += "transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;";
    HTMLString += "line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}";
    HTMLString += "h5 { font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
    HTMLString += "h6 { font-size: 18px; text-align:center; margin-top: 0px; margin-bottom: 15px;}"; 
    HTMLString += "</style>";
    HTMLString += "</head>";
    HTMLString += "<body><main class='form-signin'>";
    HTMLString += "<h1>"I18N_WIFI_MANAGER"</h1>";
    HTMLString += "<h5>("+ProgramName+")</h5>";
    HTMLString += "<br/>";
    HTMLString += "<h6>"I18N_SETTINGS_INCOMPLETE"<br />"I18N_SETTINGS_NOT_SAVED"</h6>";
    HTMLString += "<br/>";
    HTMLString += "<a href =\"/\"><button class=\"button\">"I18N_BACK"</button></a>";
    HTMLString += "</main>";
    HTMLString += "</body>";
    HTMLString += "</html>";}
    server.send(200, "text/html", HTMLString);
  } else {
    // Captive Portal
    int n = WiFi.scanNetworks(false, false); //WiFi.scanNetworks(async, show_hidden)
    HTMLString =  "<!doctype html><html lang='"I18N_LOCALE"'>";
    HTMLString += "<head><meta charset='utf-8'>";
    HTMLString += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    HTMLString += "<title>"I18N_WIFI_MANAGER"</title>";
    HTMLString += "<style>";
    HTMLString += "*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:";
    HTMLString += "'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;";
    HTMLString += "font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:";
    HTMLString += "block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid";
    HTMLString += "transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;";
    HTMLString += "line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}";
    HTMLString += "h5 { font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
    HTMLString += "h6 { font-size: 18px; margin-left: 110px; margin-top: 15px; margin-bottom: 5px; color: #0245b0;}";
    HTMLString += "h7 { font-size: 20px; font-weight: bold; margin-left: 110px; margin-top: 0px; margin-bottom: 5px; color: #06942c;}";
    HTMLString += "</style>";
    HTMLString += "</head>";
    HTMLString += "<body><main class='form-signin'>";
    HTMLString += "<form action='/' method='post'>";
    HTMLString += "<h1>"I18N_WIFI_MANAGER"</h1>";
    HTMLString += "<h5>("+ProgramName+")</h5>";
    HTMLString += "<br/>";
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
            //int temp = indices[j];
            //indices[j] = indices[i];
            //indices[i] = temp;
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
    if (i == 0) {
    HTMLString += "<h7 onclick='SetSSID"+(String)i+"()' id='fssid"+(String)i+"'>" + WiFi.SSID(indices[i]) + "</h7>";} else {
    HTMLString += "<h6 onclick='SetSSID"+(String)i+"()' id='fssid"+(String)i+"'>" + WiFi.SSID(indices[i]) + "</h6>";}
    HTMLString += "<script>";
    HTMLString += "function SetSSID"+(String)i+"() {document.getElementById('ssid').value = document.getElementById('fssid"+(String)i+"').innerHTML;}";
    HTMLString += "</script>";}
    } else {
    HTMLString += "<br/>";  
    HTMLString += "<br/>"; 
    HTMLString += "<h6>"I18N_NO_NETWORK_FOUND"</h6>";
    HTMLString += "<br/>";}
    HTMLString += "<br/>";
    HTMLString += "<div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid' id='ssid' value='' id='CPSSID'></div>";
    HTMLString += "<div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password' id='password' value=''></div>";
    HTMLString += "<br/>";
    
    HTMLString += "<div class='form-floating'><br/><label>"I18N_TIMEZONE"</label><input type='text' class='form-control' name='timezone' id='timezone' value='"+TZ_LOCATION+"'></div>";

    // ******************************************* Parameter ************************************************************************************************************************************
    HTMLString += "<div class='form-floating'><br/><label>OpenWeatherMap - API-Key</label><input type='text' class='form-control' name='apikey' id='apikey' value='"+OPEN_WEATHER_MAP_API_KEY+"'></div>";
    HTMLString += "<div class='form-floating'><br/><label>OpenWeatherMap - City-ID</label><input type='text' class='form-control' name='cityid' id='cityid' value='"+OPEN_WEATHER_MAP_LOCATION_ID+"'></div>";
    HTMLString += "<div class='form-floating'><br/><label>OpenWeatherMap - "I18N_OWM_UPDATES_IN_MINS"</label><input type='text' class='form-control' name='uwd' id='uwd' value='"+UWD+"'></div>";
    // ******************************************* Parameter ************************************************************************************************************************************
    HTMLString += "<br/><br/>";
    HTMLString += "<button type='submit'>"I18N_SAVE"</button>";
    HTMLString += "</form>";
    HTMLString += "<br/><br/>";
    HTMLString += "<form action='/' method='get'>";
    HTMLString += "<button type='submit'>"I18N_REFRESH"</button>";
    HTMLString += "</form>";
    HTMLString += "</main>";
    HTMLString += "</body>";
    HTMLString += "</html>";
    server.send(200, "text/html", HTMLString); // Captive Portal 
  }
}

// *****************************************************************************************************************************************************

void WebSiteNotFound() {
String HTMLString;   
HTMLString =  "<!doctype html><html lang='"I18N_LOCALE"'>";
HTMLString += "<head><meta charset='utf-8'>";
HTMLString += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
HTMLString += "<title>"I18N_WIFI_MANAGER"</title>";
HTMLString += "<style>";
HTMLString += "*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:";
HTMLString += "'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;";
HTMLString += "font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:";
HTMLString += "block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid";
HTMLString += "transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;";
HTMLString += "line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}";
HTMLString += "h5 { font-size: 24px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
HTMLString += "h6 { font-size: 20px; text-align:center; margin-top: 0px; margin-bottom: 10px;}"; 
HTMLString += "</style>";
HTMLString += "</head>";
HTMLString += "<body><main class='form-signin'>";
HTMLString += "<h1>"I18N_WIFI_MANAGER"</h1>";
HTMLString += "<h5>("+ProgramName+")</h5>";
HTMLString += "<br/>";
HTMLString += "<h6>"I18N_WEBSITE_NOT_FOUND"</h6>";
HTMLString += "<br/><br/>";
HTMLString += "<a href =\"/\"><button class=\"button\">"I18N_BACK"</button></a>";
HTMLString += "</main>";
HTMLString += "</body>";
HTMLString += "</html>";    
server.send(200, "text/html", HTMLString);   
} 

// *****************************************************************************************************************************************************
// *****************************************************************************************************************************************************
