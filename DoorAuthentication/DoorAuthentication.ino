
// PIN switch with 16-digit numeric keypad
// http://tronixstuff.com/tutorials > chapter 42
 
#include "Keypad.h"
#include <Wire.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <avr/wdt.h>
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
  // 5 - 5
  // 4 - 6
  // 3 - 7
  // 2 - 8 
byte colPins[COLS] = {
  9, 8, 7, 6}; //connect to the column pinouts of the keypad
  //Arduino Pin - KeyPad Pin
  // 9 - 1
  // 8 - 2
  // 7 - 3
  // 6 - 4 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 
char PIN[6]={
  '1','2','3','A','5','6'}; // our secret (!) number
char attempt[6]={ 
  '0','0','0','0','0','0'}; // used for comparison
int z=0;

void setup()
{
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
  lcd.print("* Try again *");
  delay(3000);
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
  int i;
  Serial.println();  
  for ( i = 0;   i < 6 ;  i++ )
  {
    if (attempt[i]==PIN[i])
    {
      correct++;
    }
  }
  if (correct==6)
  {
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
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Bienvenido a");
      lcd.setCursor(0,1);
      Serial.print(id);
      lcd.print("casa ");
			//Serial.print("Verified ID:");
      switch(id){
        case 0:
          lcd.setCursor(5,1);
          lcd.print("Alejandro");
          //Abriremos la puerta mediante 
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
      Serial.print(key);
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
