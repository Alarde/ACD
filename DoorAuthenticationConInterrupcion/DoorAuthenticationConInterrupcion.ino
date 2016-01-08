
// PIN switch with 16-digit numeric keypad
// http://tronixstuff.com/tutorials > chapter 42

//Para la autenticación vía keypad y finger print vamos a tener que hacer las conexiones de la siguiente manera:

//KEY PAD (De derecha a izquierda)
  //Pin 1/2/3/4/5/6/7/8 (Pins de KeyPad)
  //Pin 3/4/5/6/7/8/9/10 (Pins de Arduino)

//Finger Print (No es necesario resistencia)
  //Cable negro: Pin 11 de Arduino
  //Siguiente cable: Pin 12 de Arduino
  //Siguiente cable: Pin GND de Arduino
  //Siguiente cable: Pin 5v de Arduino


  
#include "Keypad.h"
#include <Wire.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <avr/wdt.h>

volatile int estado_actual = LOW;
FPS_GT511C3 fps(11, 12);
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
  6, 5, 4, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  10, 9, 8, 7}; //connect to the column pinouts of the keypad
 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 
char PIN[6]={
  '1','2','3','A','5','6'}; // our secret (!) number
char attempt[6]={ 
  '0','0','0','0','0','0'}; // used for comparison
int z=0;

// Tiempo en que se realizo la última interrupción.
volatile long lastInt;

void setup()
{
  Serial.begin(9600);
  //Al comenza la funcion SETUP lo deshabilitamos, vamos ponesmos esto en la primera linea de la funcion
  //wdt_disable();
  //En la ultima linea de la función setup lo activamos y indicamos el tiempo para reset en este caso 250mSg.
  //wdt_enable(WDTO_8S);
  lastInt=0;
  //Creamos una interrupción que se ejecutará cuando pulsemos '*' '0' '#' ó 'D'
  attachInterrupt(0, eliminarRetorno, CHANGE);
}


void correctPIN() // do this if correct PIN entered
{
  fps.Open();
  fps.SetLED(true);
  checkFinger();
}
 
void incorrectPIN() // do this if incorrect PIN entered
{
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
                    Serial.println("* Try again in few minutes *");
                    break;
                  }else{
                    Serial.println("Please press finger");
                  }
		}
	}
	delay(100);
  }while (success!=true);
  fps.Close();
  fps.SetLED(false);
  Resetea();
}

void eliminarRetorno(){
  Serial.println("Entramos en elimina retorno");
  if((millis()-lastInt)>200) {
    readKeypad();
    Serial.println("Dentro");
    lastInt = millis();
  }
  
  
}

void readKeypad()
{
  Serial.print("PIN Lock ");
  Serial.print("  Enter PIN...");
  Serial.println();
  do{
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
        Serial.print("*");
        break;
      }
    }
  }while(true);
}
 
void loop()
{
  //readKeypad();
  //en la función loop hacemos la llamada a esta función que reinicia el tiempo hasta el reset,
  //wdt_reset();
}
