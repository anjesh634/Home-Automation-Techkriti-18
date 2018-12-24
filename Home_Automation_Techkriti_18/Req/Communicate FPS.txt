/*------------------------------------------------------------------------------
  Program:      FPS2processing

  Description:  Relays data between GT-511C3 fingerprint scanner and processing
                application via the serial port at 9600 baud.
  
  Hardware:     - Arduino Uno
                - GT-511C3 Optical Fingerprint Scanner Module
                - Interfacing resitors
 
  References:   GT-511C3 datasheet v1.1, www.adh-tech.com.tw
  
  Date:         3 July 2015
 
  Author:       W.A. Smith, http://startingelectronics.org
------------------------------------------------------------------------------*/
#include <SoftwareSerial.h>

// need a serial port to communicate with the GT-511C3
SoftwareSerial gtSerial(8, 7); // Arduino RX (GT TX), Arduino TX (GT RX)
// the Arduino TX pin needs a voltage divider, see wiring diagram at:
// http://startingelectronics.com/articles/GT-511C3-fingerprint-scanner-hardware/

void setup() {
  Serial.begin(9600);    // serial connection to processing app.
  gtSerial.begin(9600);  // for communicating with the GT-511C3
}

byte rx_byte = 0;        // stores received byte

void loop() {
  if (Serial.available()) {
    // get byte from processing app. and send to FPS
    rx_byte = Serial.read();
    gtSerial.write(rx_byte);
  }
  // check for a byte from the GT-511C3
  if (gtSerial.available()) {
    // get a byte from the FPS and send it to the processing app.
    rx_byte = gtSerial.read();
    Serial.write(rx_byte);
  }
}