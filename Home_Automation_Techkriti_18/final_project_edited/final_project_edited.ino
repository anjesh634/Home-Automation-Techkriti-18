#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL



#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

#include <LiquidCrystal.h>
#include <Servo.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

Servo myservo;
FPS_GT511C3 fps(50, 51);
LiquidCrystal lcd(8,9,4,5,6,7);
int h=16;
int m=0;
int s=0;
float de=0;
int x=0;
int time_now=0;
int led1=0;
int led2=0;
int pled1=0;
int pled2=0;
int lastAction=-1;
float pitch,store,store1,roll;
int i=0,j=0;
int ContHPIR=0;
int ContLPIR=0;

void display_time(){

  lcd.setCursor(1,0);
  lcd.print("TIME: ");

  lcd.setCursor(7,0);
  if(h%12<10)  {
    lcd.print("0");
  }
  lcd.print(h%12);
  lcd.print(":");

  if(m<10){
    lcd.print("0");
  }
  lcd.print(m);

  lcd.setCursor(13,0);
  if(h<12){
    lcd.print("AM");
  }else{
    lcd.print("PM");
  }

}

void upd_time(float d){

  de=de+d;
  s=de;

  if(s>=60){
     s=0;
     de=0;
    m=m+1;
  }
  
  if(m==60){
    m=0;
    h=h+1;
  }

  if(h>24){
    h=0;
  }

}

void display_device_stat(){
  lcd.setCursor(0,1);
  lcd.print("LED1 ");
  if(led1==0){
    lcd.print("OF");
  }else{
    lcd.print("ON");
  }
  lcd.setCursor(9,1);
  lcd.print("LED2 ");
  if(led2==0){
    lcd.print("OF");
  }else{
    lcd.print("ON");
  }

}

void display_last_action(){
  lcd.setCursor(2,0);
  lcd.print("Last Action");
  lcd.setCursor(2,1);
  switch(lastAction){
    case 11: 
      lcd.print("LED 1: ON"); break;
    case 10: 
      lcd.print("LED 1: OF"); break;
    case 21: 
      lcd.print("LED 2: ON"); break;
    case 20: 
      lcd.print("LED 2: OF"); break;
    default:
      lcd.print("NILL"); break;
  }
}


void warnprint(){

      Serial.print("Warning\n");
      lcd.clear();
      lcd.print("Warning!!!");
      delay(2000);
  
  }


void IMU() {
  digitalWrite(49,LOW);
  if(i==0)
  {
    pitch=0;
    i=1;
    }
    else{
      pitch=ypr[1] * 180/M_PI;
      }
  if(j==0)
  {
    roll=0;
    j=1;
    }
    else{
      roll=ypr[2] * 180/M_PI;
      }

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        #ifdef OUTPUT_READABLE_QUATERNION
            // display quaternion values in easy matrix form: w x y z
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            Serial.print("quat\t");
            Serial.print(q.w);
            Serial.print("\t");
            Serial.print(q.x);
            Serial.print("\t");
            Serial.print(q.y);
            Serial.print("\t");
            Serial.println(q.z);
        #endif

        #ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetEuler(euler, &q);
            Serial.print("euler\t");
            Serial.print(euler[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(euler[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
            if(i==1)
            {
              store=pitch-ypr[1] * 180/M_PI;
              if(store>=1 || store<=-1)
              {
                digitalWrite(49,HIGH);
                warnprint();
                }
              }
              digitalWrite(49,LOW);
              if(j==1)
            {
              store1=roll-ypr[2] * 180/M_PI;
              if(store1>=1 || store1<=-1)
              {
                digitalWrite(49,HIGH);
                warnprint();
                }
              }
              digitalWrite(49,LOW);
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
        #endif
    
        #ifdef OUTPUT_TEAPOT
            // display quaternion values in InvenSense Teapot demo format:
            teapotPacket[2] = fifoBuffer[0];
            teapotPacket[3] = fifoBuffer[1];
            teapotPacket[4] = fifoBuffer[4];
            teapotPacket[5] = fifoBuffer[5];
            teapotPacket[6] = fifoBuffer[8];
            teapotPacket[7] = fifoBuffer[9];
            teapotPacket[8] = fifoBuffer[12];
            teapotPacket[9] = fifoBuffer[13];
            Serial.write(teapotPacket, 14);
            teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
        #endif

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}


float fps_call(float d){
  if (fps.IsPressFinger()){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("ID Scanning...");
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();

    if (id <200){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("ID Verified");
      Serial.print("ID Verified");
      lcd.setCursor(4,1);
      lcd.print("Welcome");
      digitalWrite(31,HIGH);// Green led id passed
      for(int angle=0;angle<91;angle++)
      {
        myservo.write(angle);
        delay(1);
        }         
      delay(3000);                
      
      for(int angle=90;angle>=0;angle--)
      {
        myservo.write(angle);
        delay(1);
        }    
        delay(1000);     
      digitalWrite(31,LOW);// Green led id passed    
      delay(2000);
      d=d+6;
   }else{
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("ID Not Found!");
      lcd.setCursor(4,1);
      digitalWrite(32,HIGH);// Red led id failed
      lcd.print("Try Again");
      delay(1000);
      digitalWrite(32,LOW);// Red led id failed
      d=d+1;
   }

//   myservo.write(30);
 }
 return d;
}

void setup(){
  Serial.begin(9600); //set up Arduino's hardware serial UART
  delay(100);
  fps.Open();         //send serial command to initialize fps
  fps.SetLED(true);
  lcd.begin(16,2);
  myservo.attach(30);
  myservo.write(10);
  pinMode(41,INPUT);// read from esp8266
  pinMode(42,INPUT);// read from esp8266
  pinMode(32,OUTPUT);// Red led id failed
  pinMode(31,OUTPUT);// Green led id passed
  pinMode(49, OUTPUT);// IMU sensor warning LED
  pinMode(48, INPUT);// PIR output get into arduino mega digital pin 48 
  pinMode(53, OUTPUT); // if PIR output is HIGH digital pin 53 is HIGH

  // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(9600);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // configure LED for output
    //pinMode(LED_PIN, OUTPUT);
}

void loop(){
  myservo.write(0);

  pled1=digitalRead(41);// read from esp8266
  pled2=digitalRead(42);// read from esp8266

  if(pled1!=led1){
    led1=pled1;
    lastAction = 10+led1;
  }

  if(pled2!=led2){
    led2=pled2;
    lastAction = 20+led2;
  }

  float d=0;
  lcd.clear();
  //Serial.println(s);
  if((int(de)%30)>25){
    display_last_action();
  }else{
    display_time();
    display_device_stat();
  }
  
  pled1=led1;
  pled2=led2;

  delay(100);
  d=d+0.1;
  d = fps_call(d);
  upd_time(d);
  IMU();
//  if(digitalRead(48)==HIGH)
//    {
//      ContHPIR++;
//      Serial.print(ContHPIR);
//    }
////    else{
////      if()
////      }
//    if(ContHPIR==5)
//    {
//          digitalWrite(53,HIGH);
//          lcd.clear();
//          lcd.print("Person Found");
//          Serial.print("PIR\n");
//          delay(500);
//          ContHPIR=0;
//     }
//     else{
//           digitalWrite(53,LOW);
//         }
 }
    

