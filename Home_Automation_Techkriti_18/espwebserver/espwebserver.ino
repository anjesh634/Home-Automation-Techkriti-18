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
"<!doctype html>"
"<html>"
"<title>Home Automation WebApp</title>"
"<head>"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">"
"<link rel=\"stylesheet\" href=\"https://code.getmdl.io/1.3.0/material.indigo-blue.min.css\" />"
"<link rel=\"stylesheet\" href=\"https://shashikg.github.io/home-automation/style.css\" />"
"<script defer src=\"https://code.getmdl.io/1.3.0/material.min.js\"></script>"
"<script defer src=\"https://shashikg.github.io/home-automation/myjs.js\"></script>"
"<script defer src=\"https://code.getmdl.io/1.3.0/myjs.js\"></script>"
"</head>"
"<body>"
"<dialog id=\"Logindialog\" class=\"mdl-dialog\">"
"<h4 class=\"mdl-dialog__title\">Sign In</h4>"
"<div class=\"mdl-dialog__content\">"
"<p id=\"loginerror\"></p>"
"<div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\">"
"<input class=\"mdl-textfield__input\" type=\"text\" id=\"loginemail\">"
"<label class=\"mdl-textfield__label\" for=\"sample3\">Email id</label>"
"</div>"
"<div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\">"
"<input class=\"mdl-textfield__input\" type=\"password\" id=\"loginpassword\">"
"<label class=\"mdl-textfield__label\" for=\"sample3\">Password</label>"
"</div>"
"</div>"
"<div class=\"mdl-dialog__actions\">"
"<button id=\"loginbtn\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-button--colored mdl-js-ripple-effect\">Sign In</button>  <div id=\"loginprogress\" class=\"mdl-spinner mdl-js-spinner is-active\"></div>"
"</div>"
"</dialog>"
"<div class=\"login-cover\">"
"<div class=\"mdl-spinner mdl-js-spinner is-active\"></div>"
"</div>"
"<center>"
"<h3>Home Automation</h3>"
"<FORM action=\"/\" method=\"post\">"
"<P>"
"<h5>LED 1</h5>"
"<label class=\"mdl-radio mdl-js-radio mdl-js-ripple-effect\">"
"<INPUT type=\"radio\"  class=\"mdl-radio__button\" name=\"LED\" value=\"11\">"
"<span class=\"mdl-radio__label\"><b>ON</b></span>"
"</label><br>"

"<label class=\"mdl-radio mdl-js-radio mdl-js-ripple-effect\">"
"<INPUT type=\"radio\"  class=\"mdl-radio__button\" name=\"LED\" value=\"10\">"
"<span class=\"mdl-radio__label\"><b>OFF</b></span>"
"</label><br>"

"<h5>LED 2</h5>"
"<label class=\"mdl-radio mdl-js-radio mdl-js-ripple-effect\">"
"<INPUT type=\"radio\"  class=\"mdl-radio__button\" name=\"LED\" value=\"21\">"
"<span class=\"mdl-radio__label\"><b>ON</b></span>"
"</label><br>"

"<label class=\"mdl-radio mdl-js-radio mdl-js-ripple-effect\">"
"<INPUT type=\"radio\"  class=\"mdl-radio__button\" name=\"LED\" value=\"20\">"
"<span class=\"mdl-radio__label\"><b>OFF</b></span>"
"</label><br>"
"<INPUT type=\"submit\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect\" value=\"Send\">"
"</P>"
"</FORM><br>"
"<button id=\"signoutbtn\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-button--colored mdl-js-ripple-effect\">Sign Out<i class=\"material-icons\">account_circle</i>"
"</button>"
"</center>"
"<script src=\"https://www.gstatic.com/firebasejs/4.7.0/firebase.js\"></script>"
"<script>"
"var config = {"
"apiKey: \"AIzaSyCzzrwdI0J-1jzuy6BfkCp71pnUDiWF1lI\","
"authDomain: \"home-auto-34636.firebaseapp.com\","
"databaseURL: \"https://home-auto-34636.firebaseio.com\","
"projectId: \"home-auto-34636\","
"storageBucket: \"home-auto-34636.appspot.com\","
"messagingSenderId: \"913570393666\""
"};"
"firebase.initializeApp(config);"
"</script>"
"<script src=\"https://code.jquery.com/jquery-3.1.0.js\"></script>"
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
  // server.on("/ledon", handleLEDon);
  // server.on("/ledoff", handleLEDoff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
  server.handleClient();
}

