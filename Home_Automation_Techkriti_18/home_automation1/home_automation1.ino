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
      lcd.setCursor(4,1);
      lcd.print("Welcome");
      digitalWrite(12,HIGH);
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
      delay(3000);
      digitalWrite(12,LOW);
      d=d+6;
   }else{
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("ID Not Found!");
      lcd.setCursor(4,1);
      digitalWrite(13,HIGH);
      lcd.print("Try Again");
      delay(1000);
      digitalWrite(13,LOW);
      d=d+1;
   }

//   myservo.write(0);
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
  pinMode(41,INPUT);
  pinMode(42,INPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
}

void loop(){
  myservo.write(0);

  pled1=digitalRead(41);
  pled2=digitalRead(42);

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
  Serial.println(s);
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

}
