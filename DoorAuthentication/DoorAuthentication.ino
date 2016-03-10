#include <TimerOne.h>
#include <RTClib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include "Keypad.h"
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

//SPI to be tested.

Adafruit_PCD8544 display = Adafruit_PCD8544(26,25,24,22,23); // (SCLK, MOSI, DC, RST, CS)
RTC_DS1307 RTC;
FPS_GT511C3 fps(12, 14); //Finger print scanner
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] =
{
  {
    '1','2','3','A'  }
  ,
  {
    '4','5','6','B'  }
  ,
  {
    '7','8','9','C'  }
  ,
  {
    '*','0','#','D'  }
};
byte rowPins[ROWS] = {
  33, 32, 31, 30}; //connect to the row pinouts of the keypad
  //Arduino Pin - KeyPad Pin
  //    5       -     5 
  //    4       -     6
  //    3       -     7
  //    2       -     8 
byte colPins[COLS] = {
  37, 36, 35, 34}; //connect to the column pinouts of the keypad
  //Arduino Pin - KeyPad Pin
  //     9      -     1
  //     8      -     2
  //     7      -     3
  //     6      -     4 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char PIN2OPEN[6]={'1','1','1','1','1','A'}; // our secret (!) number

char PIN2CLOSE[6]={'1','1','1','1','1','B'}; // our secret (!) number
  
char attempt[6]={'0','0','0','0','0','0'}; // used for comparison

boolean openDoor=false;
const int openChannel=9;
DateTime now;
const int closeChannel=10;
int z=0;
boolean nightClousure=true;

void setup(){
  Serial.begin(9600);
  pinMode(openChannel, OUTPUT);
  pinMode(closeChannel, OUTPUT);
  digitalWrite(openChannel, LOW);
  digitalWrite(closeChannel, LOW);
  /*Programación Reloj*/
  //Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC
  RTC.adjust(DateTime(__DATE__, __TIME__)); // Establece la fecha y hora (Comentar una vez establecida la hora)
  /*Fin Programación Reloj*/
  /*Programación LCD*/
  //NokiaLCD.init();   // Init screen.
  //NokiaLCD.clear();  // Clear screen.
  display.begin();
  display.setContrast(60);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Introduzca ");
  display.println("PIN: ");
  display.display();
  /*Fin Programación LCD*/
  /*Configuracion interrupcion*/
  Timer1.initialize(4000000);         // Dispara cada 4 segundos // 30 min -- 1800000000
  Timer1.attachInterrupt(checkNightClose); // Activa la interrupcion y la asocia a ISR_Blink
  /*Fin Configuracion interrupcion*/
  //NokiaLCD.setCursor(8,0);
  //NokiaLCD.print("Introduzca");
  //NokiaLCD.setCursor(30,1);
  //NokiaLCD.print("PIN:");
  //NokiaLCD.setCursor(0,2);
}

void(* Resetea) (void) = 0;

//Function que chequeara la hora cada 30 minutos para ver si tiene que cerrar la puerta.

void checkNightClose(){
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  if(now.hour() == 23){
    if(now.minute() < 30){
      openDoor=false;
      Serial.println("Son entre las 23:30 y 23:59");
      if(nightClousure!=true){
         doorRelay();
      }
    }
  }else{
    //Reset night clousure 
    nightClousure=false;
  }
}

void readKeypad(){
  now = RTC.now();
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch(key)
    {
    case '*':
      z=0;
      attempt[6]={0};//Reseteamos el array de intentos
      display.clearDisplay();   // clears the screen and buffer
      display.println("Limpiando ");
      display.println("pantalla... ");
      display.display();
      display.clearDisplay();
      //NokiaLCD.clear();
      //NokiaLCD.setCursor(0,0);
      //NokiaLCD.print("Limpiando ");
      //NokiaLCD.setCursor(0,1);
      //NokiaLCD.print("pantalla...");
      delay(1500);
      display.println("Introduzca ");
      display.println("PIN: ");
      display.display();
      //NokiaLCD.clear();
      //NokiaLCD.setCursor(8,0);
      //NokiaLCD.print("Introduzca");
      //NokiaLCD.setCursor(30,1);
      //NokiaLCD.print("PIN:");
      //NokiaLCD.setCursor(0,2);
      break;
    case '#':
      z=0;
      delay(100); // for extra debounce
      checkPIN();
      break;
    default:
      attempt[z]=key;
      //NokiaLCD.setCursor(0,2);
      z++;
      Serial.print(key);
      //NokiaLCD.print("*");
      display.print("*");
      display.display();
      break;
    }
  }
}

void checkPIN(){
  int correct=0;
  int correct2open=0;
  int correct2close=0;
  int i;
  Serial.println("Entramos en checkPIN");
  //Serial.println();  
  for ( i = 0;   i < 6 ;  i++ ){
    if (attempt[i]==PIN2OPEN[i])
    {
      correct2open++;
    }
    if (attempt[i]==PIN2CLOSE[i]){
      correct2close++;
    }
  }
  if (correct2open==6){
    openDoor=true;
    correctPIN();
  }else if(correct2close==6){
    openDoor=false;
    correctPIN();
    Serial.println("Se va a cerrar");
  }
  else
  {
    incorrectPIN();
    //Resetea();
    resetScreen();
  }
  attempt[6]={0};//Reseteamos el array de intentos
  //for (int zz=0; zz<6; zz++) 
  //{
    //attempt[zz]='0';
  //}
}

void incorrectPIN() // do this if incorrect PIN entered
{
  attempt[6]={0};//Reseteamos el array de intentos
  display.clearDisplay();
  display.println("PIN incorrecto");
  display.display();
  //Serial.print(" * Try again *");
  //NokiaLCD.clear();
  //NokiaLCD.setCursor(0,0);
  //NokiaLCD.print("PIN");
  //NokiaLCD.setCursor(0,1);
 // NokiaLCD.print("incorrecto");
  delay(2500);
  display.clearDisplay();
  display.println("Introduzca ");
  display.println("PIN: ");
  display.display();
  //Serial.print("  Enter PIN...");
  //Serial.println();
  //NokiaLCD.clear();
  //NokiaLCD.setCursor(8,0);
  //NokiaLCD.print("Introduzca");
  //NokiaLCD.setCursor(30,1);
  //NokiaLCD.print("PIN:");
  //NokiaLCD.setCursor(0,2);
}

void correctPIN() // do this if correct PIN entered
{
  display.clearDisplay();
  display.println("Pin Correcto");
  display.display();
  //NokiaLCD.clear();
  //NokiaLCD.setCursor(0,0);
  //NokiaLCD.print("PIN Correcto");
  //Serial.print("* Correct PIN *");
  delay(500);
  //Serial.print("  Enter PIN...");
  //Serial.println();
  fps.Open();
  fps.SetLED(true);
  checkFinger();
}

void checkFinger(){
  int tries=0;
  boolean success=false;
  //Serial.println("Please press finger");
  display.clearDisplay();
  display.println("Ponga el ");
  display.println("dedo en ");
  display.println("el lector");
  display.display();
  //NokiaLCD.clear();
  //NokiaLCD.setCursor(0,0);
 // NokiaLCD.print("Ponga el ");
 // NokiaLCD.setCursor(0,1);
  //NokiaLCD.print("dedo en");
 /// NokiaLCD.setCursor(0,2);
 // NokiaLCD.print("el lector");
  do{
    if (fps.IsPressFinger()){
      fps.CaptureFinger(false);
      int id = fps.Identify1_N();
      if (id<200){
        switch(id){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
          welcomeHome(1);
          break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
          welcomeHome(2);
          break;
        default:
        display.clearDisplay();
        display.println("Identificador ");
        display.println(id);
        display.println("inconsistente");
        display.display();
          //NokiaLCD.setCursor(5,1);
          //NokiaLCD.clear();
         // NokiaLCD.setCursor(0,0);
         // NokiaLCD.print("Identificador ");
         // NokiaLCD.setCursor(0,1);
          //Serial.print(id);
         // NokiaLCD.print("inconsistente");
        break;
        }
        //Serial.println(id);
        delay(3500);
        success=true;
      }else{
        display.clearDisplay();
        display.println("Huella no ");
        display.println("encontrada...");
        display.display();
       // NokiaLCD.clear();
        //NokiaLCD.setCursor(0,0);
        //NokiaLCD.print("Huella no");
       // NokiaLCD.setCursor(0,1);
        //NokiaLCD.print("encontrada...");
        delay(2500);
        tries++;
        if(tries==5){
          //Serial.println("Please press finger");
          break;
        }else{
          display.clearDisplay();
          display.println("Ponga el ");
          display.println("dedo en ");
          display.println("el lector");
          display.display();
        //NokiaLCD.clear();
       // NokiaLCD.setCursor(0,0);
       // NokiaLCD.print("Ponga el ");
       /// NokiaLCD.setCursor(0,1);
        //NokiaLCD.print("dedo en");
        //NokiaLCD.setCursor(0,2);
        //NokiaLCD.print("el lector");
        }
      }
    }
    delay(100);
  }while (success!=true);
  fps.Close();
  fps.SetLED(false);
  //Resetea();
  resetScreen();
}

void resetScreen(){

  char attempt[6]={'0','0','0','0','0','0'};
  display.clearDisplay();
  display.println("Introduzca ");
  display.println("PIN: ");
  display.display();
  //NokiaLCD.clear();
  //NokiaLCD.setCursor(8,0);
  //NokiaLCD.print("Introduzca");
  //NokiaLCD.setCursor(30,1);
  //NokiaLCD.print("PIN:");
  //NokiaLCD.setCursor(0,2);
  readKeypad();
}

void welcomeHome (int id){
  boolean opened = false;
  switch(id){
    case 1:
      display.clearDisplay();
     // NokiaLCD.clear();
      //NokiaLCD.setCursor(0,0);
      if(openDoor){
        display.println("Bienvenido a ");
        display.println("casa Alex.");
        display.display();
        //NokiaLCD.print("Bienvenido a");
        //NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        //NokiaLCD.print("casa Alex");
      }else{
        display.println("Hasta pronto");
        display.println("Alex.");
        display.display();
        //NokiaLCD.print("Hasta pronto");
        //NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        //NokiaLCD.print("Alex");
      }
      doorRelay();
      break;
    case 2:
      //NokiaLCD.clear();
      //NokiaLCD.setCursor(0,0);
      if(openDoor){
        display.println("Bienvenida a ");
        display.println("casa reina mía.");
        display.display();
        //NokiaLCD.print("Bienvenida a");
        //NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        //NokiaLCD.print("casa reina mia ");
      }else{
        display.println("Hasta pronto ");
        display.println("reina mía.");
        display.display();
        //NokiaLCD.print("Hasta pronto");
        //NokiaLCD.setCursor(0,1);
        //Serial.print(id);
       // NokiaLCD.print("reina mia ");
      }
      doorRelay();
      break;
    default:
      display.clearDisplay();
      //NokiaLCD.clear();
      //NokiaLCD.setCursor(0,0);
      display.println("Error inesperado.");
      display.display();
      //NokiaLCD.print("Unespected error.");
      break;
    }
}

void doorRelay(){
  
  Serial.println("Entramos en doorRelay");
  if (openDoor){ //only to open the door
    Serial.println("Abrimos puerta");
    digitalWrite(openChannel, HIGH);
    delay(2000);
    digitalWrite(openChannel, LOW);
  }else{
    Serial.println("Cerramos puerta");
    digitalWrite(closeChannel, HIGH);
    delay(2000);
    digitalWrite(closeChannel, LOW);
  }
}

void loop(){
  readKeypad();
}
