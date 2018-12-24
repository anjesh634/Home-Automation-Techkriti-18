#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
//SSID and Password to your ESP Access Point
const char* ssid = "ESPWebServer";
const char* password = "esp8266";
 
ESP8266WebServer server(80); //Server on port 80
 
//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
 char page[]="<html><head><title>Login page</title></head><body><h1 style=\"font-family:Comic Sans Ms;text-align=\"center\";font-size:20pt;color:#00FF00;>ESP8266 Web Page</h1><form name=\"login\"><p>Username<input type=\"text\" name=\"userid\"/></p><p>Password<input type=\"password\" name=\"pswrd\"/></p><input type=\"button\" onclick=\"check(this.form)\" value=\"Login\"/><input type=\"reset\" value=\"Cancel\"/></form><script language=\"javascript\">function check(form)/*function to check userid & password*/{ /*the following code checkes whether the entered userid and password are matching*/if(form.userid.value == \"esp8266\" && form.pswrd.value == \"esp8266\"){window.open('target.html')/*opens the target page while Id & password matches*/}else{alert(\"Error Password or Username\")/*displays error message*/}}</script></body></html>"; //Read HTML contents
 server.send(200, "text/html", page); //Send web page
}
 
//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security
 
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
 
  server.on("/", handleRoot);      //Which routine to handle at root location
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  server.handleClient();          //Handle client requests
}
