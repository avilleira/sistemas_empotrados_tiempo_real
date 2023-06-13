#include <LiquidCrystal.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <TimerOne.h>
#include "DHT.h"

#define MILISECONDS 1000

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 9;
int ledPins[] = {17, 16};
int ledstate = LOW, PIN_TRIGGER = 8, PIN_ECHO = 7, PIN_JOYX = A0, PIN_JOYY = A1;
int PIN_JOY_BUTTON = 6, SWITCH_PIN = 2, PIN_TEMP = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht(10, DHT11);
long counter;

const int MAX_Y = 1000, MIN_Y = 65;

char *menu[] = { "Cafe Solo 1", "Cafe Cortado 1.10", "Cafe Doble 1.25", "Cafe Premium 1.5", "Chocolate 2.00"};
char *admin[] = { "Ver Temperatura", "Ver Distancia", "Ver contador", "Modificar Precios"};


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
  float pwm;
  bool buttonValue = true;
  int time = random(4,8);

  while(buttonValue == true) {
    Xvalue = analogRead(PIN_JOYX);
    Yvalue = analogRead(PIN_JOYY);
    buttonValue = digitalRead(PIN_JOY_BUTTON);
    
    if (Yvalue < MIN_Y) {
      lcd.clear();
      index_menu++;
      if (index_menu > 4) {
        index_menu = 0;
      }
    }
    else if (Yvalue > MAX_Y) {
      lcd.clear();
      index_menu--;
      if (index_menu < 0) {
        index_menu = 4;
      }
    }
    lcd.setCursor(0,0);
    lcd.print(menu[index_menu]);
    //Reseting watchdog
    wdt_reset();
  }
  lcd.clear();
  lcd.print("Preparando Cafe...");
  pwm = 15;
  previous_time = millis();
  while ((millis() - previous_time) < time*MILISECONDS){
    if (pwm < 255)
      pwm +=0.01;
    analogWrite(ledPins[1], pwm);
  }
  lcd.clear();
  digitalWrite(ledPins[1], LOW);
  lcd.print("RETIRE BEBIDA");
  previous_time = millis();
  while ((millis() - previous_time) < 3*MILISECONDS){}
  lcd.clear();
  service();
}

// Shows counter in the display.
void see_counter() {
  long real_time;

  real_time = (millis() - counter) / MILISECONDS;
  lcd.clear();
  lcd.print(real_time);
}

int admin_menu() {

  int y_value = 0, index_menu = 0, i;
  bool not_pressed = true, use_joystick = true;
  // Turning on the leds:
  for (i = 0; i < 2; i++)
    digitalWrite(ledPins[i], HIGH);
  while (not_pressed == true) {
    y_value = analogRead(PIN_JOYY);
    not_pressed = digitalRead(PIN_JOY_BUTTON);

    if ((y_value < MIN_Y) && (use_joystick == true)) {
      lcd.clear();
      use_joystick = false;
      index_menu++;

      if (index_menu > 3)
        index_menu = 0;
    }
    else if ((y_value > MAX_Y) && (use_joystick == true)) {
      lcd.clear();
      use_joystick = false;
      index_menu--;

      if (index_menu < 0)
        index_menu = 3;
    }
    else
      use_joystick = true;
    // Printing menu pos:
    lcd.setCursor(0,0);
    lcd.print(admin[index_menu]);
    wdt_reset();
  }
  return index_menu;
}

void switch_pressed() {
  Serial.println("ME HAN PULSADO.");
}


void setup() {

  Serial.begin(9600);
  // set up the LCD's number of columns and rows:

  lcd.begin(20, 2);
  lcd.noBlink();
  
  // Deactivating and setting up watchdog:
  wdt_disable();
  wdt_enable(WDTO_8S);

  // Initialize temperature humidity sensor:
  pinMode(PIN_TEMP, INPUT_PULLUP);
  dht.begin();
  lcd.noBlink();

  // Counter:
  counter = millis();

  // Catching interrupts: 
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
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
  lcd.setCursor(0,0);
  
  // print the number of seconds since reset:
  see_counter();
  admin_menu();
  wdt_reset();
}