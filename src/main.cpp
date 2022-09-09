
#include <Arduino.h>
#include <TimeLib.h>
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "organ.h"
#include "led.h"
#include <Bounce2.h>

#define OLED_RESET_PIN 3
#define IN1_PIN 39
#define IN2_PIN 38
#define SW1_PIN 33
#define SW2_PIN 34
#define SW3_PIN 35

#define NUM_BUTTONS 3
const int BUTTON_PINS[NUM_BUTTONS] = {SW1_PIN, SW2_PIN, SW3_PIN};
Bounce *buttons = new Bounce[NUM_BUTTONS];
Bounce *PIR = new Bounce;
Organ organ;
LED led;

#define NUM_MENU_ITEMS 6
int activeMenuIndex = 0;
// d = date, m = month, y = year, h = hour, m = minute, s = second, t = test
const char menu[NUM_MENU_ITEMS] = {'D', 'M', 'Y', 'h', 'm', 's'};

bool inputStateEnabled = false;
uint32_t lastInputTime = 0;
uint16_t inputStateTimeout = 8000;

// OLED display
Adafruit_SSD1306 display(OLED_RESET_PIN); 
uint32_t lastDisplayUpdate = 0;
uint16_t displayUpdateInterval = 300;
// END OLED DISPLAY
// TIME

uint32_t lastPIRActivityTime = 0;


bool displayClear = false;

uint32_t activationDelay = 1000*30*60; // 30 minutes
uint16_t fanPressureDelay = 10000;

// 0: idle, 1: armed, 2: active
int state = 0;
uint32_t stateChangeTime = 0;

time_t getTeensy4Time()
{
  return rtc_get();
}

uint8_t nightStartHour = 20;
uint8_t nightEndHour = 8;
bool isNight() {
  return hour() >= nightStartHour || hour() < nightEndHour;
}

int getLEDSegment(int pipeIndex) {
    return 5-pipeIndex;
}

void displayDigits(int digits, bool activeInput=false)
{
  // utility function for digital clock display prints leading 0
  if(activeInput) {
    display.setTextColor(BLACK, WHITE);
  }

  if (digits < 10) {
    display.print('0');
  }
  display.print(digits);

  display.setTextColor(WHITE, BLACK);
}

void displayDateTime()
{
  char divider = (second() % 2 == 0) ? ':' : ' ';
  display.clearDisplay();
  display.setCursor(0, 0);
  displayDigits(day(), menu[activeMenuIndex] == 'D' && inputStateEnabled);
  display.print('-');
  displayDigits(month(), menu[activeMenuIndex] == 'M' && inputStateEnabled);
  display.print('-');
  displayDigits(year(), menu[activeMenuIndex] == 'Y' && inputStateEnabled);
  display.print(' ');
  displayDigits(hour(), menu[activeMenuIndex] == 'h' && inputStateEnabled);
  display.print(divider);
  displayDigits(minute(), menu[activeMenuIndex] == 'm' && inputStateEnabled);
  display.print(divider);
  displayDigits(second(), menu[activeMenuIndex] == 's' && inputStateEnabled);
  display.display();

}

const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

/* Handle 3 button menu navigation. Allows to to set the date and time
- button 1: input navigation
- button 2: increase value
- button 3: decrease value
Update lastInputTime if any buttons pressed */
void inputState() {


      if (buttons[0].fell())
      {
        lastInputTime = millis();
        // Shift through active menu item
        activeMenuIndex = (activeMenuIndex + 1) % NUM_MENU_ITEMS;
      } else {

         bool upPressed = buttons[1].fell();
         bool downPressed = buttons[2].fell();
         bool upOrDownPressed = upPressed || downPressed;

        if(upOrDownPressed) {
          lastInputTime = millis();
        }

        tmElements_t tm;
        breakTime(now(), tm);

        if (menu[activeMenuIndex] == 'D')
        {
          if(upPressed) {
            if(tm.Day < monthDays[tm.Month-1]) {
              tm.Day++;
            }
          } else if(downPressed) {
            if(tm.Day > 0) {
              tm.Day--;
            }
          }
          
        }
        else if (menu[activeMenuIndex] == 'M')
        {
          // month
          if(upPressed) {
            if(tm.Month < 12) {
              tm.Month++;
            }
          } else if(downPressed) {
            if(tm.Month > 0) {
              tm.Month--;
            }
          } 

          if(upOrDownPressed) {
            if(tm.Day > monthDays[tm.Month-1]) {
              tm.Day = monthDays[tm.Month-1];
            }
          }

        }
        else if (menu[activeMenuIndex] == 'Y')
        {
          // year
          if(upPressed) {
            tm.Year++;
          } else if(downPressed) {
            if(tm.Year > 0) {
              tm.Year--;
            }
          }
        }
        else if (menu[activeMenuIndex] == 'h')
        {
          // hour
          if(upPressed) {
            if(tm.Hour < 24) {
              tm.Hour++;
            }
          } else if(downPressed) {
            if(tm.Hour > 0) {
              tm.Hour--;
            }
          }
        }
        else if (menu[activeMenuIndex] == 'm')
        {
          // minute
          if(upPressed) {
            if(tm.Minute < 60) {
              tm.Minute++;
            }
          } else if(downPressed) {
            if(tm.Minute > 0) {
              tm.Minute--;
            }
          }
        }
        else if (menu[activeMenuIndex] == 's')
        {
          // second
          if(upPressed) {
            if(tm.Second < 60) {
              tm.Second++;
            }
          } else if(downPressed) {
            if(tm.Minute > 0) {
              tm.Second--;
            }
          }
        }

        if(upOrDownPressed) {
          setTime(makeTime(tm));
          rtc_set(now());
        }
    }
}


void setup()
{
  setSyncProvider(getTeensy4Time); // TODO: new battery 2032 cell when installing on site
  
  //led.setup();

  //pinMode(IN1_PIN, INPUT_PULLUP);
  PIR->attach(IN1_PIN, INPUT_PULLUP);       //setup the bounce instance for the current button
  PIR->interval(40);              // interval in ms

  // IN2 is unused
  //pinMode(IN2_PIN, INPUT_PULLUP);

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].attach(BUTTON_PINS[i], INPUT_PULLUP); // setup the bounce instance for the current button
    buttons[i].interval(25);                         // interval in ms
  }

  pinMode(LED_BUILTIN, OUTPUT);
  // initialize communication
  Wire.begin();
  Serial.begin(38400);
  delay(500);
  // initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // display text
  display.setCursor(0, 0);
  display.print("Starting up...");
  display.display();

  delay(500);

  organ.setup();
  organ.loadGivePeaceAChanceChorus();
  //organ.loadGivePeaceAChanceChorusSecondVoice();
  //organ.loadGivePeaceAChanceChorusSecondVoiceChurch();
  led.setup();

}

bool lastStateForced = false;
void enterState(int newState, bool force=false) {
  state = newState;
  stateChangeTime = millis();

  lastStateForced = force;

  if(state == 0) {
    // idle
    Serial.println("Enter state 0 (idle): Wait.");
    organ.fanOff(); 

  } else if(state == 1) {
    // armed
    Serial.println("Enter state 1 (armed): Turn fan on.");
    if(isNight() && !force ) {
      organ.fanOff();
    } else {
      organ.fanOn();
    }

  } else if(state == 2) {
    // active
    Serial.println("Enter state 2 (active): Play give peace a chance.");

    int songSelect = random(100);
    if(songSelect < 20) {
      organ.loadGivePeaceAChanceChorus();
    } else if(songSelect < 80) {
      organ.loadGivePeaceAChanceChorusSecondVoice();
    } else {
      organ.loadGivePeaceAChanceChorusSecondVoiceChurch();
    }

    if(isNight() && !force) {
      organ.fanOff();
      organ.play(false);
    } else {
      organ.play(true);
    }

  }
}

uint16_t color[4];

void loop()
{
  organ.update();

    for(int i=0; i<4; i++) {
      float colorValue = map(LED::getFadeMod(1), 0, 1, DARK[i], BRIGHT[i]);
      color[i] = colorValue;
    }

  /*for(int i=1; i<= NUM_LED_SEGMENTS; i++) {
    led.setSegmentColor(i, 0, 0, 0, 0);
  }*/

  for (int i = 0; i < NUM_PIPES; ++i) {

        int ledSegment = getLEDSegment(i);
        if (organ.getPipeState(i))
        {
          // LED for active PIPE
          led.setSegmentColor(ledSegment, color[0], color[1], color[2], 0.8*maxBrightness);

        } else {
          // LED for inactive PIPE 
          if(organ.isPlaying()) {

            led.setSegmentColor(ledSegment, color[0], color[1], color[2], 0.1*maxBrightness);
          } else {
            float modValue = map(LED::getFadeMod(i), 0, 1, 0.5, 1);

            led.setSegmentColor(ledSegment, color[0], color[1]*modValue, color[2], 0);
          }

        }
  }


  for(int i=6; i<=7; i++) {
      float modValue = map(LED::getFadeMod(i), 0, 1, 0.5, 0.8);

    if(organ.isPlaying()) {
      // Brick wall
      led.setSegmentColor(i, color[0]*0.8, color[1]*modValue, color[2], 0);

    } else {

      // Brick wall
      led.setSegmentColor(i, color[0]*0.8, color[1]*modValue, color[2], 0);
    }
  }

  float modValue = map(LED::getFadeMod(8), 0, 1, 0.5, 0.9);
  // Middle wall
  if(organ.isPlaying()) {
    //float modValue = map(LED::getFadeMod(8), 0, 1, 0.5, 1);
    led.setSegmentColor(8, color[0]*0.9, color[1]*modValue, color[2], 0);
  } else {
    //float modValue = map(LED::getFadeMod(8), 0, 1, 0.5, 1);
    led.setSegmentColor(8, color[0]*0.9, color[1]*modValue, color[2], 0);
  }


  //led.update();

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].update();
  }

  // clear display untill button 1 is pressed
  if (!inputStateEnabled)
  {
    if(displayClear == false){
      display.clearDisplay();
      display.display();
      displayClear = true;
    }

    if (buttons[0].fell())
    {
      inputStateEnabled = true;
      lastInputTime = millis();
    }
    else if (buttons[1].fell())
    {
      Serial.println("test armed");
      if(state == 0) {
        enterState(1, true);
      } else {
        enterState(0, true);

      }
    }
    else if (buttons[2].fell())
    {
      Serial.println("test play");
      if(state != 1) {
        enterState(1, true);
        delay(fanPressureDelay);
      }
      delay(2000);
      enterState(2, true); 
    }
  }
  else
  {
    displayClear = false;

    if (millis() - lastInputTime > inputStateTimeout)
    {
      inputStateEnabled = false;
    } else {
      inputState();
    }

    if (state != 2)
    {
      if (millis() - lastDisplayUpdate > displayUpdateInterval)
      {
        lastDisplayUpdate = millis();
        displayDateTime();
      }
    }
  }

  if (state == 0 || state == 1)
  {

    // set idle colors
    //led.purpleColorLoop();

    // set armed colors

    PIR->update();

    if (PIR->fell())
    {
      Serial.println("PIR acivity detected");
      lastPIRActivityTime = millis();

      if (state == 0)
      {
        if (lastPIRActivityTime - stateChangeTime > activationDelay)
        {
          enterState(1);
        }
      }
      else if (state == 1)
      {

        if (lastPIRActivityTime - stateChangeTime > fanPressureDelay)
        {
          enterState(2, lastStateForced);
        }
      }
    }

  }
  else if (state == 2)
  {
    if (!organ.isPlaying())
    {
      if(lastStateForced) {
        enterState(1, true);
      } else {
        enterState(0);
      }
      // Exit state when done playing
    }
  }
}
