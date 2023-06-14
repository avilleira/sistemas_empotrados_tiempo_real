#include <LiquidCrystal.h>
#include <Arduino.h>
#include <avr/wdt.h>
#include <TimerOne.h>
#include "DHT.h"

#define MILISECONDS 1000
#define DEBOUNCE_T 20

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 9; // LCD pins
const int MAX_Y = 1000, MIN_Y = 100;
const int PIN_TRIGGER = 8, PIN_ECHO = 7, PIN_JOYX = A0, PIN_JOYY = A1, PIN_JOY_BUTTON = 6, SWITCH_PIN = 2, PIN_TEMP = 10;

const int led_pins[] = {17, 16};
const char *menu[] = { "Cafe Solo ", "Cafe Cortado ", "Cafe Doble ", "Cafe Premium ", "Chocolate "};
const char *admin[] = { "Ver Temperatura", "Ver Distancia", "Ver contador", "Modificar Precios"};

volatile bool interrupt_flag = false;
bool admin_sts = false;
int led_state = LOW; 
double prices[] = {1.00, 1.10, 1.25, 1.50, 2.00};
long counter;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht(10, DHT11);

void start() {
  int i = 0;
  unsigned long previous_time;
  lcd.print("CARGANDO...");
  previous_time = millis();
  digitalWrite(led_pins[0], !led_state);
  led_state = !led_state;
  while (i<5) {
    if ((millis()-previous_time) >= 1000) {
      digitalWrite(led_pins[0], !led_state);
      led_state = !led_state;
      previous_time = millis();
      i++;
    }
  }
}

void software_reset() {
  wdt_enable(WDTO_15MS);
  while (true);
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

  lcd.print(String("Temp: ") + temp + String(" C"));
  lcd.setCursor(0,1);
  lcd.print(String("Hum ") + hum + String("%"));

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
    if (interrupt_flag == true)
      break;
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
    lcd.setCursor(0,1);
    lcd.print(prices[index_menu]);
    lcd.setCursor(0,0);
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
    analogWrite(led_pins[1], pwm);
  }
  wdt_reset();
  lcd.clear();
  digitalWrite(led_pins[1], LOW);
  lcd.print("RETIRE BEBIDA");
  previous_time = millis();
  while ((millis() - previous_time) < 3*MILISECONDS){}
  lcd.clear();
  service_menu();
}

// Shows counter in the display.
void see_counter() {
  long real_time;

  real_time = (millis() - counter) / MILISECONDS;
  lcd.clear();
  lcd.print(real_time);
}

void show_distance() {
  long distance, previous_time;
  previous_time = millis();

  while((millis() - previous_time) < 3000)
  {
    distance = ping();
    lcd.clear();
    lcd.print(distance + String("cm"));
  }
}

int admin_menu() {

  int y_value = 0, index_menu = 0, i;
  bool not_pressed = true, use_joystick = true;
  // Turning on the leds:
  for (i = 0; i < 2; i++)
    digitalWrite(led_pins[i], HIGH);
  while (not_pressed == true) {
    if (interrupt_flag == true)
      break;
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
  lcd.clear();
  return index_menu;
}

// In order to avoid the millis() problem, we need to make the callback this way.
void switch_callback() {
  interrupt_flag = true;
}

long switch_pressed() {
  long time, current_time;
  int switch_state;

  time = millis();
  switch_state = digitalRead(SWITCH_PIN);
  Serial.println("HOLA QUE TAl");
  while (millis() - time < DEBOUNCE_T){
  }

  while (switch_state == 0) {
    switch_state = digitalRead(SWITCH_PIN);
    Serial.println(millis());
  }
  interrupt_flag = false;
  return millis() - time;
}


void setup() {

  Serial.begin(9600);
  // Set up the LCD
  lcd.begin(20, 2);
  lcd.noBlink();
  // Deactivating and setting up watchdog:
  wdt_disable();
  wdt_enable(WDTO_8S);
  // Initialize temperature humidity sensor:
  pinMode(PIN_TEMP, INPUT_PULLUP);
  dht.begin();
  // Counter:
  counter = millis();
  // Catching interrupts: 
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_callback, FALLING);
  // Set joystick button:
  pinMode(PIN_JOY_BUTTON, INPUT_PULLUP);
  // Setting LED's mode:
  for (int index = 0; index < 4; index++){
    pinMode(led_pins[index], OUTPUT);
  }
  // Setting up distance sensor:
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

  int position;
  long time_pressed;
  lcd.setCursor(0,0);
  if (interrupt_flag == true) {
    time_pressed = switch_pressed();
    if (time_pressed > 2000 && time_pressed < 3000)
      software_reset();
    else if (time_pressed )
  }
  // print the number of seconds since reset:
  //see_counter();
  
  //service_menu();

  /*position = admin_menu();
  
  if (position == 0) {
    show_temp_hum();
  }
  else if (position == 1) {
    show_distance();
  }*/
  wdt_reset();
}