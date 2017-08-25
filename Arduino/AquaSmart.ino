#include <AquaSmartGUI.h>

AquaSmartGUI gui;

int fan = 0;
boolean fanIsOn = false;
boolean startShown = false;

unsigned long previousMillis=0;

#define MENU_ITEMS 3
#define MENU_ITEM_DURATION 1100

const char *menu_items[MENU_ITEMS] = {"TEMPERATURE", "WATER LEVEL", "LIGHT"};
boolean shown_menu_items[MENU_ITEMS] = {false, false, false};
int menu_index = 0;

const int MENU_BUTTON = 2;
const int MENU_ITEM_BUTTON = 3;

boolean lastButton = HIGH; 
boolean currentButton = LOW;

// Sensors data
float water_temperature = 26.4;
float last_water_temperature = 28.5;
float temperature_delta = 0.5;
int temp_measure_interval = 1000 * 5; // 30 sec interval

void setup() {
  Serial.begin(9600);
  pinMode(MENU_BUTTON, INPUT);
  pinMode(MENU_ITEM_BUTTON, INPUT);
}

void printSomething() {
  Serial.println("Timer");
}

void loop() {
  if (!startShown) {
    gui.draw_start(startShown);
    delay(100);
  } else {

    currentButton = debounce(lastButton);
    if (lastButton == LOW && currentButton == HIGH) {
      update_menu();
      fanIsOn = !fanIsOn;
    }
    lastButton = currentButton;

    show_menu_item(menu_index);
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
//
//  Serial.print("Time:");
//  Serial.println(time);
//  Serial.print("temp_measure_interval:");
//  Serial.println(temp_measure_interval);

    unsigned long currentMillis = millis();
    if ((unsigned long)(currentMillis - previousMillis) >= temp_measure_interval) {
      if (water_temperature >= 40) {
        water_temperature = water_temperature - 20;
      } else {
      
        water_temperature = water_temperature + 5;
      }

    gui.draw_temperature(fanIsOn, water_temperature, last_water_temperature, temperature_delta, fan, menu_index, MENU_ITEMS);
    last_water_temperature = water_temperature;
      
      // Use the snapshot to set track time until next event
      previousMillis = currentMillis;
   }
  
  update_fan();
  delay(50);
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

//TODO: this should go inside GUI
void update_fan() {
  fan++;
  if (fan == 4) {
    fan = 0;
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

