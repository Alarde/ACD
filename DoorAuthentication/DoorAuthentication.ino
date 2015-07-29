
// PIN switch with 16-digit numeric keypad
// http://tronixstuff.com/tutorials > chapter 42
 
#include "Keypad.h"
#include <Wire.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <avr/wdt.h>

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
  Serial.begin(9600);
  fps.UseSerialDebug = true;
  //Al comenza la funcion SETUP lo deshabilitamos, vamos ponesmos esto en la primera linea de la funcion
  //wdt_disable();
  Serial.print("PIN Lock ");
  delay(1000);
  Serial.print("  Enter PIN...");
  Serial.println();
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
  Serial.print(" * Try again *");
  delay(1000);
  Serial.print("  Enter PIN...");
  Serial.println();
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
 
  for (int zz=0; zz<6; zz++) 
  {
    attempt[zz]='0';
  }
}

void checkFinger(){
  int tries=0;
  boolean success=false;
  Serial.println("Please press finger");
  do{
        // Identify fingerprint test
	if (fps.IsPressFinger())
	{
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();
		if (id <200)
		{
			Serial.print("Verified ID:");
			Serial.println(id);
                        success=true;
		}
		else
		{
		  Serial.println("Finger not found");
                   tries++;
                  if(tries==5){
                    Serial.println("Please press finger");
                    break;
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
      break;
    case '#':
      z=0;
      delay(100); // for extra debounce
      checkPIN();
      break;
    default:
      attempt[z]=key;
      z++;
      Serial.print(key);
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
