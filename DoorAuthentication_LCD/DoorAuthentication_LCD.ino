

// PIN switch with 16-digit numeric keypad
// http://tronixstuff.com/tutorials > chapter 42
 
#include "Keypad.h"
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>

//****Los pines que usaremos para nuestro display******
//                RS  Enable  D4  D5  D6  D7
LiquidCrystal lcd(21,   20,   19, 18, 17, 16);
FPS_GT511C3 fps(13, 15); //Finger print scanner
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
 
char PIN2OPEN[6]={
  '1','1','1','1','1','A'}; // our secret (!) number

char PIN2CLOSE[6]={
  '1','1','1','1','1','B'}; // our secret (!) number

char attempt[6]={ 
  '0','0','0','0','0','0'}; // used for comparison
int z=0;
boolean openDoor=false;
int openChannel=22;
int closeChannel=25;

void setup()
{
  pinMode(openChannel, OUTPUT);
  pinMode(closeChannel, OUTPUT);
  fps.Close();
  fps.SetLED(false);
  Serial.begin(9600);
  lcd.begin(16, 2); // declaramos el numero de filas y columnas que tendrá nuestro display LCD.(en éste caso, el mío tenía 16 columnas por fila y dos filas)
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Insertar PIN:");
  //fps.UseSerialDebug = true;
  //Al comenza la funcion SETUP lo deshabilitamos, vamos ponesmos esto en la primera linea de la funcion
  //wdt_disable();
  //Serial.print("PIN Lock ");
  //delay(1000);
  //Serial.print("  Enter PIN...");
  //Serial.println();
  //En la ultima linea de la función setup lo activamos y indicamos el tiempo para reset en este caso 250mSg.
  //wdt_enable(WDTO_8S);
}
 
void correctPIN() // do this if correct PIN entered
{
  //Serial.print("* Correct PIN *");
  //delay(1000);
  //Serial.print("  Enter PIN...");
  //Serial.println();
  fps.Open();
  fps.SetLED(true);
  checkFinger();
}
 
void incorrectPIN() // do this if incorrect PIN entered
{
  attempt[6]={0};//Reseteamos el array de intentos
  //Serial.print(" * Try again *");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PIN Incorrecto");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Insertar PIN:");
  //Serial.print("  Enter PIN...");
  //Serial.println();
}
 
void(* Resetea) (void) = 0;

void checkPIN()
{
  int correct=0;
  int correct2open=0;
  int correct2close=0;
  int i;
  //Serial.println();  
  for ( i = 0;   i < 6 ;  i++ )
  {
    if (attempt[i]==PIN2OPEN[i])
    {
      correct2open++;
    }
    if (attempt[i]==PIN2CLOSE[i]){
      correct2close++;
    }
  }
  Serial.println(correct2open);
  Serial.println(correct2close);
  if (correct2open==6){
    openDoor=true;
    correctPIN();
  }else if(correct2close==6){
    openDoor=false;
    correctPIN();
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

void checkFinger(){
  int tries=0;
  boolean success=false;
  //Serial.println("Please press finger");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ponga el dedo en");
  lcd.setCursor(0,1);
  lcd.print("el lector");
  do{
  // Identify fingerprint test
	if (fps.IsPressFinger())
	{
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();
		if (id<200)
		{
			
			//Serial.print("Verified ID:");
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
          lcd.setCursor(5,1);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Identificador ");
          lcd.setCursor(0,1);
          //Serial.print(id);
          lcd.print("inconsistente");
        break;
     }
     
			//Serial.println(id);
     delay(3500);
      success=true;
		}
		else
		{
      lcd.clear();
      lcd.setCursor(0,0);
		  lcd.print("Huella no");
      lcd.setCursor(0,1);
      lcd.print("encontrada...");
      delay(2500);
      tries++;
      if(tries==5){
        //Serial.println("Please press finger");
        break;
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ponga el dedo en");
        lcd.setCursor(0,1);
        lcd.print("el lector");
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
  // definicion del nuevo caracter
      byte love[8] = {
          B00000,
          B00000,
          B11011,
          B11111,
          B11111,
          B01110,
          B00100,
          B00000
      };
      lcd.createChar(0, love);
  switch(id){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      if(openDoor){
        lcd.print("Bienvenido a");
        lcd.setCursor(0,1);
        //Serial.print(id);
        lcd.print("casa Alex");
      }else{
        lcd.print("Hasta pronto");
        lcd.setCursor(0,1);
        //Serial.print(id);
        lcd.print("Alex");
      }
      
      openDoorRelay();
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      if(openDoor){
        lcd.print("Bienvenida a");
        lcd.setCursor(0,1);
        //Serial.print(id);
        lcd.print("casa reina mia ");
        lcd.write(byte(0));
      }else{
        lcd.print("Hasta pronto");
        lcd.setCursor(0,1);
        //Serial.print(id);
        lcd.print("reina mia ");
        lcd.write(byte(0));
      }
      openDoorRelay();
      break;
    default:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Unespected error.");
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

void readKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch(key)
    {
    case '*':
      z=0;
      attempt[6]={0};//Reseteamos el array de intentos
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Limpiando ");
      lcd.setCursor(0,1);
      lcd.print("pantalla...");
      delay(1500);
      lcd.clear();
      lcd.print("Insertar PIN:");
      break;
    case '#':
      z=0;
      delay(100); // for extra debounce
      checkPIN();
      break;
    default:
      attempt[z]=key;
      lcd.setCursor(z,1);
      z++;
      //Serial.print(key);
      lcd.print("*");
      break;
    }
  }
}
 
void loop()
{
  readKeypad();
  //en la función loop hacemos la llamada a esta función que reinicia el tiempo hasta el reset,
  //wdt_reset();
}
