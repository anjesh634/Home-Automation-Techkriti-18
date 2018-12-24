#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Fill in your WiFi router SSID and password
const char* ssid = "wifi";
const char* password = "wifi5233";
MDNSResponder mdns;

ESP8266WebServer server(80);

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
"LED 1<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"10\">On"
"<INPUT type=\"radio\" name=\"LED\" value=\"11\">Off<br>"
"<INPUT type=\"submit\" value=\"Send\"><br>"
"LED 2<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"20\">On"
"<INPUT type=\"radio\" name=\"LED\" value=\"21\">Off<br>"
"<INPUT type=\"submit\" value=\"Send\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LED2 = 2;
const int LED1 = 0;

void handleRoot()
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

  if (LEDvalue == "11") {
    writeLED(true,LED1);
    server.send(200, "text/html", INDEX_HTML);
  }
  else if (LEDvalue == "10") {
    writeLED(false,LED1);
    server.send(200, "text/html", INDEX_HTML);
  }
  else if (LEDvalue == "20") {
    writeLED(false,LED2);
    server.send(200, "text/html", INDEX_HTML);
  }
  else if (LEDvalue == "21") {
    writeLED(true,LED2);
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
// void handleLEDon()
// {
//   writeLED(true,LEDPIN);
//   returnOK();
// }

// /*
//  * Imperative to turn the LED off using a non-browser http client.
//  * For example, using wget.
//  * $ wget http://esp8266webform/ledoff
//  */
// void handleLEDoff()
// {
//   writeLED(false,LEDPIN);
//   returnOK();
// }

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

void writeLED(bool LEDon, int LEDPIN)
{
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon)
    digitalWrite(LEDPIN, 0);
  else
    digitalWrite(LEDPIN, 1);
}

void setup(void)
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  writeLED(false,LED1);
  writeLED(false,LED2);

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

  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
//  server.on("/ledon", handleLEDon);
//  server.on("/ledoff", handleLEDoff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
  server.handleClient();
}
