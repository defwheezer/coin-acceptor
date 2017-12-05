#define COIN_PIN 2 // use one of the arduino interrupt pins (0 = pin 2, 1 = pin 3)
#define ledPin 9  // Add if needed to PWM pin for LED level adj.

//save strings to ROM to save SRAM, failsafe for coins entered
#include <avr/pgmspace.h>
char const coin_0[] PROGMEM = "Received Penny"; // "Received Penny" etc are strings to store - change to suit.
char const coin_1[] PROGMEM = "Received  Nickel";
char const coin_2[] PROGMEM = "Received  Dime";
char const coin_3[] PROGMEM = "Received Quarter";
char const coin_4[] PROGMEM = "Unknown Coin";
// Then set up a table to refer to your strings.
PGM_P const string_table[] PROGMEM = // change "string_table" name to suit
{
  coin_0,
  coin_1,
  coin_2,
  coin_3,
  coin_4, };
  
char buffer[30]; // make sure this is large enough for the largest string it must hold

int totalMoney;

int ledState = LOW; 

void setup() {
  // Debugging output
  Serial.begin(115200);

  pinMode(COIN_PIN, INPUT);
  attachInterrupt(0, coinISR, RISING);  // COIN wire connected to PIN 2;
  Serial.println("Initializations done.");
  Serial.println("Ready to accept coins.");
     
}


// total amount of money collected;
int money = 0;
int prevMoney = 0;
// gets incremented by the ISR;
// gets reset when coin was recognized (after train of pulses ends);
volatile int pulses = 0;
volatile long timeLastPulse = 0;


// executed for every pulse;
void coinISR()
{
  pulses++;
  timeLastPulse = millis();
}

void loop()
{
  //used to check if we have any coins inserted
  long timeFromLastPulse = millis() - timeLastPulse;
  
  //only do if done detecting pulses for a while (usere done entering coins)
  if(timeFromLastPulse > 2000 && money != prevMoney) { 
     Serial.print("Total recieved ");
     Serial.println(money);
     prevMoney = 0;
     money = 0;
  }
  
  if (pulses > 0 && timeFromLastPulse > 200)
  {
    // sequence of pulses stopped; determine the coin type;
    switch(pulses) {
      case '2':
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[0]))); // Necessary casts and dereferencing, just copy.
        Serial.println( buffer );
        money += 1;
      break;
      
      case '4':
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[1]))); // Necessary casts and dereferencing, just copy.
        Serial.println( buffer );
        money += 5;
      break;

      case '6':
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[2]))); // Necessary casts and dereferencing, just copy.
        Serial.println( buffer );
        money += 10;
      break;
      
      case '8':
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[3]))); // Necessary casts and dereferencing, just copy.
        Serial.println( buffer );
        money += 25;
      break;

      default:
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(string_table[4]))); // Necessary casts and dereferencing, just copy.
        Serial.print( buffer );
        Serial.print(" ");
        Serial.print(pulses);
        Serial.println(" pulses");
        ledState = HIGH;
        digitalWrite(ledPin, ledState);
        delay(500);
        ledState = LOW;
        digitalWrite(ledPin, ledState);
        Serial.println("End coin error...");      
      break;
      }

    pulses = 0;
  }
}
