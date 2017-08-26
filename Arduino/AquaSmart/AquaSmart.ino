#include <AButton.h>
#include <AquaSmartGUI.h>

const int MENU_BUTTON = 2;
const int MENU_ITEM_BUTTON = 3;
const int FAN = 4;

AquaSmartGUI gui;
AButton menuButton(MENU_BUTTON, true);
AButton menuItemButton(MENU_ITEM_BUTTON, true);

boolean startShown = false;

unsigned long previousMillis = 0;

#define MENU_ITEMS 3
#define MENU_ITEM_DURATION 1100

const char *menu_items[MENU_ITEMS] = {"TEMPERATURE", "WATER LEVEL", "LIGHT"};
boolean shown_menu_items[MENU_ITEMS] = {false, false, false};
int menu_index = 0;

boolean lastButton = HIGH; 
boolean currentButton = LOW;

// Sensors data
float water_temperature = 26.4;
float water_threshold = 32;
float last_water_temperature = 0.0;
float temperature_delta = 0.5;
boolean temp_is_growing = false;
boolean fanIsOn = false;
int temp_measure_interval = 1000 * 5; // 30 sec interval

int water_level = 35;

boolean light_is_on = false;

void setup() {
  Serial.begin(9600);
  pinMode(MENU_BUTTON, INPUT);
  pinMode(MENU_ITEM_BUTTON, INPUT);
  pinMode(FAN, OUTPUT);

  menuButton.attachClick(menu_click);
  menuItemButton.attachClick(menu_item_click);
}

void loop() {
  menuButton.tick();
  menuItemButton.tick();
  
  if (!startShown) {
    gui.draw_start(startShown);
    delay(100);
  } else {
    show_menu_item(menu_index);
    delay(50);
  }
}

void menu_click() {
  update_menu();
}

void menu_item_click() {
  if (menu_index == 0) {
    fanIsOn = !fanIsOn;
    turnFan(fanIsOn);
  }
}

void turnFan(boolean on) {
  if (on) {
    digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW);
  } 
}

void monitorWaterTemperature() {
  unsigned long currentMillis = millis();
  Serial.print("currentMillis: ");
  Serial.println(currentMillis);
  Serial.print("previousMillis: ");
  Serial.println(previousMillis);
  if ((unsigned long)(currentMillis - previousMillis) >= temp_measure_interval) {
    Serial.println("Cnahging temp...");
    if (water_temperature >= 40) {
      water_temperature = water_temperature - 20;
    } else {
      water_temperature = water_temperature + 5;
    }
    temp_is_growing = water_temperature + temperature_delta >= last_water_temperature;

    if (water_temperature >= water_threshold) {
      Serial.println("Switching fan ON...");
      fanIsOn = true;
      turnFan(true);
    } else {
      Serial.println("Switching fan OFF...");
      fanIsOn = false;
      turnFan(false);
    }
    previousMillis = currentMillis;
  }
}

void show_menu_item(int index) {
  boolean is_shown = shown_menu_items[index];
  if (!is_shown) {
    gui.draw_menu_item(menu_index, menu_items[menu_index]);
    delay(MENU_ITEM_DURATION);
    reset_shown_menu_items();
    shown_menu_items[index] = true;
  }

  monitorWaterTemperature();
  
  if (index == 0) { // Temperature
    gui.draw_temperature(fanIsOn, water_temperature, temp_is_growing, menu_index, MENU_ITEMS);
    last_water_temperature = water_temperature;
  } else if (index == 1) { // Water level
    gui.draw_water_level(water_level, menu_index, MENU_ITEMS);
  } else if (index == 2) { // Light
    gui.draw_light(light_is_on, menu_index, MENU_ITEMS);
  }
}

void reset_shown_menu_items() {
  for (int i = 0; i < MENU_ITEMS; i++) {
    shown_menu_items[i] = false;
  }
}

void update_menu() {
  menu_index = menu_index + 1;
  if (menu_index > MENU_ITEMS - 1) {
    menu_index = 0;
  }
}

boolean debounce(boolean last) {
  boolean current = digitalRead(MENU_BUTTON);
  if (last != current) {
    delay(5);
    current = digitalRead(MENU_BUTTON);
    return current;
  }
}

