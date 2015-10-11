#include <NokiaLCD.h>
#include "Keypad.h"
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
NokiaLCD NokiaLCD(17,18,19,20,21); // (SCK, MOSI, DC, RST, CS)

FPS_GT511C3 fps(13, 14); //Finger print scanner
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
  5, 4, 3, 2}; //connect to the row pinouts of the keypad
  //Arduino Pin - KeyPad Pin
  //    5       -     5 
  //    4       -     6
  //    3       -     7
  //    2       -     8 
byte colPins[COLS] = {
  9, 8, 7, 6}; //connect to the column pinouts of the keypad
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
int openChannel=15;
int closeChannel=16;
int z=0;

void setup(){
  Serial.begin(9600);
  NokiaLCD.init();   // Init screen.
  NokiaLCD.clear();  // Clear screen.
  NokiaLCD.setCursor(8,0);
  NokiaLCD.print("Introduzca");
  NokiaLCD.setCursor(30,1);
  NokiaLCD.print("PIN:");
  NokiaLCD.setCursor(0,2);
}

void(* Resetea) (void) = 0;

void readKeypad(){
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch(key)
    {
    case '*':
      z=0;
      attempt[6]={0};//Reseteamos el array de intentos
      NokiaLCD.clear();
      NokiaLCD.setCursor(0,0);
      NokiaLCD.print("Limpiando ");
      NokiaLCD.setCursor(0,1);
      NokiaLCD.print("pantalla...");
      delay(1500);
      NokiaLCD.clear();
      NokiaLCD.setCursor(8,0);
      NokiaLCD.print("Introduzca");
      NokiaLCD.setCursor(30,1);
      NokiaLCD.print("PIN:");
      NokiaLCD.setCursor(0,2);
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
      //Serial.print(key);
      NokiaLCD.print("*");
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
    Resetea();
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
  //Serial.print(" * Try again *");
  NokiaLCD.clear();
  NokiaLCD.setCursor(0,0);
  NokiaLCD.print("PIN");
  NokiaLCD.setCursor(0,1);
  NokiaLCD.print("incorrecto");
  delay(2500);
  //Serial.print("  Enter PIN...");
  //Serial.println();
  NokiaLCD.clear();
  NokiaLCD.setCursor(8,0);
  NokiaLCD.print("Introduzca");
  NokiaLCD.setCursor(30,1);
  NokiaLCD.print("PIN:");
  NokiaLCD.setCursor(0,2);
}

void correctPIN() // do this if correct PIN entered
{
  NokiaLCD.clear();
  NokiaLCD.setCursor(0,0);
  NokiaLCD.print("PIN Correcto");
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
  NokiaLCD.clear();
  NokiaLCD.setCursor(0,0);
  NokiaLCD.print("Ponga el ");
  NokiaLCD.setCursor(0,1);
  NokiaLCD.print("dedo en");
  NokiaLCD.setCursor(0,2);
  NokiaLCD.print("el lector");
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
          NokiaLCD.setCursor(5,1);
          NokiaLCD.clear();
          NokiaLCD.setCursor(0,0);
          NokiaLCD.print("Identificador ");
          NokiaLCD.setCursor(0,1);
          //Serial.print(id);
          NokiaLCD.print("inconsistente");
        break;
        }
        //Serial.println(id);
        delay(3500);
        success=true;
      }else{
        NokiaLCD.clear();
        NokiaLCD.setCursor(0,0);
        NokiaLCD.print("Huella no");
        NokiaLCD.setCursor(0,1);
        NokiaLCD.print("encontrada...");
        delay(2500);
        tries++;
        if(tries==5){
          //Serial.println("Please press finger");
          break;
        }else{
        NokiaLCD.clear();
        NokiaLCD.setCursor(0,0);
        NokiaLCD.print("Ponga el dedo ");
        NokiaLCD.setCursor(0,1);
        NokiaLCD.print("en el lector");
        }
      }
    }
    delay(100);
  }while (success!=true);
  fps.Close();
  fps.SetLED(false);
  Resetea();
}

void welcomeHome (int id){
  boolean opened = false;
  switch(id){
    case 1:
      NokiaLCD.clear();
      NokiaLCD.setCursor(0,0);
      if(openDoor){
        NokiaLCD.print("Bienvenido a");
        NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        NokiaLCD.print("casa Alex");
      }else{
        NokiaLCD.print("Hasta pronto");
        NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        NokiaLCD.print("Alex");
      }
      openDoorRelay();
      break;
    case 2:
      NokiaLCD.clear();
      NokiaLCD.setCursor(0,0);
      if(openDoor){
        NokiaLCD.print("Bienvenida a");
        NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        NokiaLCD.print("casa reina mia ");
      }else{
        NokiaLCD.print("Hasta pronto");
        NokiaLCD.setCursor(0,1);
        //Serial.print(id);
        NokiaLCD.print("reina mia ");
      }
      openDoorRelay();
      break;
    default:
      NokiaLCD.clear();
      NokiaLCD.setCursor(0,0);
      NokiaLCD.print("Unespected error.");
      break;
    }
}

void openDoorRelay(){
  if (openDoor){ //only to open the door
    digitalWrite(openChannel, HIGH);
    delay(100);
    digitalWrite(openChannel, LOW);
  }else{
     digitalWrite(closeChannel, HIGH);
    delay(100);
    digitalWrite(closeChannel, LOW);
  }
}

void loop(){
  readKeypad();
}
