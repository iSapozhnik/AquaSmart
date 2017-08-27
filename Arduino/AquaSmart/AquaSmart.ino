#include <AButton.h>
#include <AquaSmartGUI.h>

const int MENU_BUTTON = 2;
const int MENU_ITEM_BUTTON = 3;
const int FAN = 4;
const int LIGHT = 5;

AquaSmartGUI gui;
AButton menuButton(MENU_BUTTON, true);
AButton menuItemButton(MENU_ITEM_BUTTON, true);

boolean startShown = false;

unsigned long previousMillis = 0;

#define MENU_ITEMS 4
#define MENU_ITEM_DURATION 800

const char *menu_items[MENU_ITEMS] = {"TEMPERATURE", "WATER LEVEL", "LIGHT", "AERATION"};
boolean shown_menu_items[MENU_ITEMS] = {false, false, false, false};
int menu_index = 0;

enum FanMode { 
  Auto, 
  ManualOff,
  ManualOn,
  End
};

boolean lastButton = HIGH; 
boolean currentButton = LOW;

// Sensors data
float water_temperature = 26.4;
float water_threshold = 32;
float last_water_temperature = 0.0;
float temperature_delta = 0.5;
boolean temp_is_growing = false;
boolean fan_is_on = false;
FanMode fan_mode = Auto;
int temp_measure_interval = 1000 * 5; // 30 sec interval

int water_level = 35;

boolean light_is_on = false;

void setup() {
  Serial.begin(9600);
  pinMode(MENU_BUTTON, INPUT);
  pinMode(MENU_ITEM_BUTTON, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(LIGHT, OUTPUT);

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
    int current_value = (int)fan_mode + 1;
    if (current_value == (int)End) {
      fan_mode = Auto;
    } else {
      fan_mode = (FanMode)current_value;
    }
    
    switch (fan_mode) {
    case Auto:
      fan_is_on = water_temperature >= water_threshold;
      break;
    case ManualOn:
      fan_is_on = true;
      break;
    case ManualOff:
      fan_is_on = false;
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
    turnFan(fan_is_on);
  } else if (menu_index == 1) {
    
  } else if (menu_index == 2) {
    light_is_on = !light_is_on;
    turnLight(light_is_on);
  }
}

void turnFan(boolean on) {
  if (on) {
    digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW);
  } 
}

void turnLight(boolean on) {
  if (on) {
    digitalWrite(LIGHT, HIGH);
  } else {
    digitalWrite(LIGHT, LOW);
  } 
}

void monitorWaterTemperature() {
  unsigned long currentMillis = millis();
  if ((unsigned long)(currentMillis - previousMillis) >= temp_measure_interval) {
    if (water_temperature >= 40) {
      water_temperature = water_temperature - 20;
    } else {
      water_temperature = water_temperature + 5;
    }
    temp_is_growing = water_temperature + temperature_delta >= last_water_temperature;
    
    if (fan_mode == Auto) {
      if (water_temperature >= water_threshold) {
        fan_is_on = true;
        turnFan(true);
      } else {
        fan_is_on = false;
        turnFan(false);
      }
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
    gui.draw_temperature(fan_is_on, fan_mode, water_temperature, temp_is_growing, menu_index, MENU_ITEMS);
    last_water_temperature = water_temperature;
  } else if (index == 1) { // Water level
    gui.draw_water_level(water_level, menu_index, MENU_ITEMS);
  } else if (index == 2) { // Light
    gui.draw_light(light_is_on, menu_index, MENU_ITEMS);
  } else if (index == 3) { // aeration
    gui.draw_aeration(false, menu_index, MENU_ITEMS);
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

