#include <LiquidCrystal.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <TimerOne.h>
#include "DHT.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
int ledPins[] = {9, 1};
int ledstate = LOW, PIN_TRIGGER = 8, PIN_ECHO = 7, PIN_JOYX = A1, PIN_JOYY = A0;
int PIN_JOY_BUTTON = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht(10, DHT11);

char *menu[] = { "Cafe Solo 1", "Cafe Cortado 1.10", "Cafe Doble 1.25", "Cafe Premium 1.5", "Chocolate 2.00"};


void start() {
  int i = 0;
  unsigned long previous_time;
  lcd.print("CARGANDO...");
  previous_time = millis();
  digitalWrite(ledPins[0], !ledstate);
  ledstate = !ledstate;
  while (i<5) {
    if ((millis()-previous_time) >= 1000) {
      digitalWrite(ledPins[0], !ledstate);
      ledstate = !ledstate;
      previous_time = millis();
      i++;
    }
  }
}

long ping () {
  long duration, distance_cm;
  //Generamos un pulso limpio primero haciendo una espera de 4 segundos
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(4);
  //Se lanza el pulso.
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);

  duration = pulseIn(PIN_ECHO,HIGH);

  distance_cm = duration * 0.01715;
  return distance_cm;
}

void show_temp_hum() {
  float hum, temp;
  long previous_time;
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.println(temp);

  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("Hum ");
  lcd.print(hum);
  lcd.print("%");

  
  previous_time = millis();

  while ((millis() - previous_time) < 5000){}
  lcd.setCursor(0,1);
  lcd.clear();

}

void service_menu() { 
  int Xvalue = 0, Yvalue = 0, index_menu = 0, previous_time;
  bool buttonValue = false;
  int time = random(4,8);

  lcd.print(menu[index_menu]);

  while(buttonValue == false) {
    Xvalue = analogRead(PIN_JOYX);
    delay(100);
    Yvalue = analogRead(PIN_JOYY);
    //buttonValue = digitalRead(PIN_JOY_BUTTON);
    

    if (Yvalue < 7) {
      lcd.clear();
      index_menu++;
      if (index_menu > 4) {
        index_menu = 0;
      }
    }
    else if (Yvalue > 990) {
      lcd.clear();
      index_menu--;
      if (index_menu < 0) {
        index_menu = 4;
      }
    }
    lcd.setCursor(0,0);
    lcd.print(menu[index_menu]);
  }
  Serial.print("HH");
  lcd.clear();
  lcd.print("Preparando Cafe...");
  previous_time = millis();
  while ((millis() - previous_time) < time){}
  lcd.clear();
  lcd.print("RETIRE BEBIDA");
  previous_time = millis();
  while ((millis() - previous_time) < 3){}
  lcd.clear();
  Serial.print("TT");
}


void setup() {

  Serial.begin(9600);
  // set up the LCD's number of columns and rows:

  lcd.begin(20, 2);
  
  //Initialize temperature humidity sensor:
  pinMode(10, INPUT_PULLUP);
  dht.begin();
  lcd.noBlink();
  //set Joystick:
  pinMode(PIN_JOY_BUTTON, INPUT_PULLUP);
  //Setting LED's mode:
  for (int index = 0; index < 4; index++){
    pinMode(ledPins[index], OUTPUT);
  }

  //Setting up Distance sensor:
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  start();
  lcd.clear();
}

void service() {
  if (ping() < 100) {
    lcd.clear();
    show_temp_hum();
    service_menu();
  }
  else
    lcd.print("ESPERANDO CLIENTE");
}

void loop() {

  //Se que por ahora solo tengo el servicio, pero para la reentrega lo tendré listo.
  lcd.setCursor(0,0);
  // print the number of seconds since reset:
  service();

}