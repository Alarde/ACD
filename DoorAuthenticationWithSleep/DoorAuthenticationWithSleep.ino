#include <avr/sleep.h>

//Hay que puentear el pin 2 con el GND, entonces encontrará la interrupción.

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
}

void loop()
{
  delay(5000);
  sleepSetup(); 
}

void sleepSetup(){
  
  sleep_enable();
  attachInterrupt(0,pinInterrupt,LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(13,LOW);
  sleep_cpu();
  Serial.println("Just wake up!");
  digitalWrite(13,HIGH); 
}

void pinInterrupt(){
  
  sleep_disable();
  detachInterrupt(0);
  
}
