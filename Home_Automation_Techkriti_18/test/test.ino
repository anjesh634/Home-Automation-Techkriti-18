#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Fill in your WiFi router SSID and password
const char* ssid = "ESP8266";
const char* password = "esp8266";
//MDNSResponder mdns;

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
"<FORM action=\"/\" method=\"get\">"
"<P>"
"LED<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"0\">On<BR>"
"<INPUT type=\"radio\" name=\"LED\" value=\"1\">Off<BR>"
"</P>"
"<P>"
"LED1<br>"
"<INPUT type=\"radio\" name=\"LED1\" value=\"0\">On<BR>"
"<INPUT type=\"radio\" name=\"LED1\" value=\"1\">Off<BR>"
"<INPUT type=\"submit\" value=\"Send\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LEDPIN = 2;
const int LED1PIN = 2;


void handleRoot()
{
  if (server.hasArg("LED") || server.hasArg("LED1")) {
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


void writeLED(bool LEDon)
{
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon)
    digitalWrite(LEDPIN, 0);
  else
    digitalWrite(LEDPIN, 1);
}

void writeLED1(bool LEDon)
{
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon)
    digitalWrite(LED1PIN, 0);
  else
    digitalWrite(LED1PIN, 1);
}


void handleSubmit()
{
  String LEDvalue;
  String LED1value;
  if (!server.hasArg("LED")) return returnFail("BAD ARGS");
  {
    LEDvalue = server.arg("LED");
    LED1value = server.arg("LED1");
  }
  //if (LEDvalue == "1" || LEDvalue == "0") {
    if(LED1value == "1")
    {writeLED1(true);
    server.send(200, "text/html", INDEX_HTML);}
    if(LED1value == "0")
    {writeLED1(false);
    server.send(200, "text/html", INDEX_HTML);}
    if(LEDvalue == "1")
    {writeLED(true);
    server.send(200, "text/html", INDEX_HTML);}
    if(LEDvalue == "0")
    {writeLED(false);
    server.send(200, "text/html", INDEX_HTML);}
    server.send(200, "text/html", INDEX_HTML);
  //}
//  else {
//    server.send(200, "text/html", INDEX_HTML);
//  }
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


void setup(void)
{
  pinMode(LEDPIN, OUTPUT);
  writeLED(false);

  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security
 
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
  server.handleClient();
}


