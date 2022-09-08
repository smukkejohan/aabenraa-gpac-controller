
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
uint16_t inputStateTimeout = 20000;

// OLED display
Adafruit_SSD1306 display(OLED_RESET_PIN);
uint32_t lastDisplayUpdate = 0;
uint16_t displayUpdateInterval = 300;
// END OLED DISPLAY
// TIME

uint32_t lastPIRActivityTime = 0;

uint8_t nightStartHour = 20;
uint8_t nightEndHour = 8;

bool displayClear = false;

uint16_t activationDelay = 5000; // TODO: increase for prodction 1 hour
uint16_t fanPressureDelay = 30000;

// 0: idle, 1: armed, 2: active
int state = 0;
uint32_t stateChangeTime = 0;

time_t getTeensy4Time()
{
  return rtc_get();
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

        lastInputTime = millis();

        tmElements_t tm;
        breakTime(now(), tm);

        if (menu[activeMenuIndex] == 'D')
        {
          if(upPressed) {
            tm.Day++;
          } else if(downPressed) {
            tm.Day--;
          }
          
        }
        else if (menu[activeMenuIndex] == 'M')
        {
          // month
          if(upPressed) {
            tm.Month++;
          } else if(downPressed) {
            tm.Month--;
          }

        }
        else if (menu[activeMenuIndex] == 'Y')
        {
          // year
          if(upPressed) {
            tm.Year++;
          } else if(downPressed) {
            tm.Year--;
          }
        }
        else if (menu[activeMenuIndex] == 'h')
        {
          // hour
          if(upPressed) {
            tm.Hour++;
          } else if(downPressed) {
            tm.Hour--;
          }
        }
        else if (menu[activeMenuIndex] == 'm')
        {
          // minute
          if(upPressed) {
            tm.Minute++;
          } else if(downPressed) {
            tm.Minute--;
          }
        }
        else if (menu[activeMenuIndex] == 's')
        {
          // second
          if(upPressed) {
            tm.Second++;
          } else if(downPressed) {
            tm.Second--;
          }
        }

        if(upOrDownPressed) {
          setTime(makeTime(tm));
        }

    }

}



void setup()
{
  setSyncProvider(getTeensy4Time); // TODO: new battery 2032 cell when installing on site
  
  //led.setup();

  pinMode(IN1_PIN, INPUT_PULLUP);
  // PIR->attach(IN1_PIN, INPUT_PULLUP);       //setup the bounce instance for the current button
  // PIR->interval(40);              // interval in ms

  pinMode(IN2_PIN, INPUT_PULLUP);
  // TODO debounce for activity input (input 1)

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

  led.setup();

}

void loop()
{
  
  //led.purpleColorLoop();
  organ.update();
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
      organ.play();
      Serial.println("test play");
    }
    else if (buttons[2].fell())
    {
      Serial.println(organ.getFanState());
      
      if(organ.getFanState() == 1) {
        organ.fanOff();
      } else {
        organ.fanOn();
      }
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

      // TODO: do not turn on at night

      if (state == 0)
      {

        if (lastPIRActivityTime - stateChangeTime > activationDelay)
        {
          Serial.println("enter state 1, turn fan on");

          organ.fanOn();
          state = 1;
          stateChangeTime = millis();
        }
      }
      else if (state == 1)
      {

        if (lastPIRActivityTime - stateChangeTime > fanPressureDelay)
        {
          Serial.println("enter state 2, play organ");

          organ.play();
          state = 2;
          stateChangeTime = millis();
        }
      }
      // digitalWriteFast(valve_pins[3], PIR->read());
    }
  }
  else if (state == 2)
  {
    if (!organ.isPlaying())
    {
      // Exit state when done playing
      organ.fanOff(); // TODO: do not turn fan off if we want to loop music
      state = 0;
      stateChangeTime = millis();
    }
    else
    {
      // TODO: set light to music and state
      for (int i = 0; i < NUM_PIPES; ++i)
      {
        if (organ.getPipeState(i))
        {
          // add intensity hint to color of led
          //led.setSegmentColor(i, 0.93*maxBrightness, 0.7*maxBrightness, 0.96 * maxBrightness, maxBrightness);
        } else {
          //led.setSegmentColor(i, 0.93*maxBrightness, 0.7*maxBrightness, 0.96 * maxBrightness, 0.5*maxBrightness);
        }
      }
    }
  }
}
