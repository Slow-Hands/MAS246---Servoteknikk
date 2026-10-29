#include <LiquidCrystal.h>

int analogPin = A0;  // Initialiserer pin A0
int val = 0;    // Initial verdi for val er satt til 0
int WeightLimit = 680; // Maks vekt.
bool ElevatorState = false;

LiquidCrystal lcd(41, 40, 37, 36, 35, 34);


void setup() {
  Serial.begin(9600); // setup serial
  lcd.begin(16, 2);
  lcd.print("Elevator ready!");
  lcd.clear();
}

void loop() {
  val = analogRead(analogPin); //leser pin A0
  Serial.println(val); //
  delay(200);     //leser hvert 200ms

  if (val < WeightLimit){
     ElevatorState = true;
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(val);
     lcd.setCursor(5, 0);
     lcd.print("kg");
     lcd.setCursor(0, 1);
     lcd.print("OK       ");
  }

  else if (val >= WeightLimit){
    ElevatorState = false;
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(val); 
     lcd.setCursor(5, 0);
     lcd.print("kg");
     lcd.setCursor(0, 1);
     lcd.print("OVERVEKT!");
  }
}
