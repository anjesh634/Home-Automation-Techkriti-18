#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Fill in your WiFi router SSID and password
const char* ssid = "wifi";
const char* password = "wifi5233";
MDNSResponder mdns;

ESP8266WebServer server(80);
char page[]="<html><head><title>Login page</title></head><body><h1 style=\"font-family:Comic Sans Ms;text-align=\"center\";font-size:20pt;color:#00FF00;>ESP8266 Web Page</h1><form name=\"login\"><p>Username<input type=\"text\" name=\"userid\"/></p><p>Password<input type=\"password\" name=\"pswrd\"/></p><input type=\"button\" onclick=\"check(this.form)\" value=\"Login\"/><input type=\"reset\" value=\"Cancel\"/></form><script language=\"javascript\">function check(form)/*function to check userid & password*/{ /*the following code checkes whether the entered userid and password are matching*/if(form.userid.value == \"esp8266\" && form.pswrd.value == \"esp8266\"){window.open('swtch.html')/*opens the target page while Id & password matches*/}else{alert(\"Error Password or Username\")/*displays error message*/}}</script></body></html>"; //Read HTML contents

const char INDEX_HTML[] =
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
"<INPUT type=\"radio\" name=\"LED\" value=\"1\">Off<BR>"
"<INPUT type=\"radio\" name=\"LED\" value=\"0\">On<BR>"
"<INPUT type=\"submit\" value=\"Send\"> <INPUT type=\"reset\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LEDPIN = 0;

void handleRoot()
{
    server.send(200, "text/html", page);
}
void handleRootswtch()
{
  if (server.hasArg("LED")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
  String LEDvalue;

  if (!server.hasArg("LED")) return returnFail("BAD ARGS");
  LEDvalue = server.arg("LED");
  if (LEDvalue == "1") {
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
  }
  else if (LEDvalue == "0") {
    writeLED(false);
    server.send(200, "text/html", INDEX_HTML);
  }
  else {
    returnFail("Bad LED value");
  }
}

void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

/*
 * Imperative to turn the LED on using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform/ledon
 */
void handleLEDon()
{
  writeLED(true);
  returnOK();
}

/*
 * Imperative to turn the LED off using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform/ledoff
 */
void handleLEDoff()
{
  writeLED(false);
  returnOK();
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void writeLED(bool LEDon)
{
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon)
    digitalWrite(LEDPIN, 0);
  else
    digitalWrite(LEDPIN, 1);
}

void setup(void)
{
  pinMode(LEDPIN, OUTPUT);
  writeLED(false);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(WiFi.localIP());

  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

   server.on("/", handleRoot);
  server.on("/swtch.html", handleRootswtch);
  server.on("/swtch.html/ledon", handleLEDon);
  server.on("/swtch.html/ledoff", handleLEDoff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
  server.handleClient();
   Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(WiFi.localIP());
}
