#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
//#include "index.h" //Our HTML webpage contents
 
//SSID and Password of your WiFi router
const char* ssid = "wifi";
const char* password = "wifi5233";
 
ESP8266WebServer server(80); //Server on port 80
 
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 char page[]="<html><head><title>Login page</title></head><body><h1 style=\"font-family:Comic Sans Ms;text-align=\"center\";font-size:20pt;color:#00FF00;>ESP8266 Login </h1><form name=\"login\">Username<input type=\"text\" name=\"userid\"/>Password<input type=\"password\" name=\"pswrd\"/><input type=\"button\" onclick=\"check(this.form)\" value=\"Login\"/><input type=\"reset\" value=\"Cancel\"/></form><script language=\"javascript\">function check(form)/*function to check userid & password*/{ /*the following code checkes whether the entered userid and password are matching*/if(form.userid.value == \"esp8266\" && form.pswrd.value == \"mypswrd\"){window.open('target.html')/*opens the target page while Id & password matches*/}else{alert(\"Error Password or Username\")/*displays error message*/}}</script></body></html>"; //Read HTML contents
 //char pages[]=
 "<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>ESP8266 Web Form Demo</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<h1>ESP8266 Web Form Demo</h1>"
"<FORM action=\"/\" method=\"post\">"
"<P>"
"LED<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"1\">On<BR>"
"<INPUT type=\"radio\" name=\"LED\" value=\"0\">Off<BR>"
"<INPUT type=\"submit\" value=\"Send\"> <INPUT type=\"reset\">"
"</P>"
"</FORM>"
"</body>"
"</html>";
 server.send(200,"text/html", page); //Send web page
// server.send(200,"/target.html", pages); //Send web page
}
void handleRoot1() {
  char pages[]="welcome";
  server.send(200,"text/html", pages); //Send web page
// server.send(200,"/target.html", pages); //Send web page
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/target.html",handleRoot1);
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          //Handle client requests
}
