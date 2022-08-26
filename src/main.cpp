
#include <Arduino.h>
#include <TimeLib.h>
#include "pins.h"
// DMX
#include <TeensyDMX.h>

// OLED Display
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "organ.h"

// Inputs
#include <Bounce2.h>

// Pin for enabling or disabling the transmitter.

#define NUM_BUTTONS 3
const int BUTTON_PINS[NUM_BUTTONS] = {SW1_PIN, SW2_PIN, SW3_PIN};
Bounce * buttons = new Bounce[NUM_BUTTONS];

Bounce * PIR = new Bounce;

Organ organ;

bool inputStateEnabled = false;
unsigned long lastInputTime = 0;
unsigned int inputStateTimeout = 5000; // Todo: increase for prodction 

bool builtinBlinkState = false;

// DMX OUTPUT
namespace teensydmx = ::qindesign::teensydmx;
// Create the DMX transmitter on Serial3.
teensydmx::Sender dmxTx{Serial5}; // Tx on Serial5 is pin 20
// The current channel outputting a value.
int channel;
// END DMX OUTPUT

// OLED display
Adafruit_SSD1306 display(OLED_RESET_PIN);
unsigned long lastDisplayUpdate = 0;
unsigned int displayUpdateInterval = 1000;
// END OLED DISPLAY
// TIME


// COLOR
// Max value for 16 bit channels
uint16_t maxBrightness = 65535; // 255*255 65536 values
int rgbw[4] = {0,10000,20000,30000};
int ch[4] = {1,3,5,7};
// END COLOR



time_t getTeensy4Time()
{
  return rtc_get();
}

void displayDigits(int digits){
  // utility function for digital clock display prints leading 0
  if(digits < 10)
    display.print('0');
  display.print(digits);
}

#define NUM_MENU_ITEMS 7
int activeMenuIndex = 0;
// d = date, m = month, y = year, h = hour, m = minute, s = second, t = test
const char menu[NUM_MENU_ITEMS] = {'h', 'm', 's', 'd', 'm', 'y', 't'};

void setup() {
  setSyncProvider(getTeensy4Time); // TODO: new battery 2032 cell when installing on site

  // put your setup code here, to run once:
  //DmxSimple.usePin(3);
  //DmxSimple.maxChannel(32);

  pinMode(IN1_PIN, INPUT_PULLUP);
  //PIR->attach(IN1_PIN, INPUT_PULLUP);       //setup the bounce instance for the current button
  //PIR->interval(40);              // interval in ms

  pinMode(IN2_PIN, INPUT_PULLUP);
  // TODO debounce for activity input (input 1)

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i], INPUT_PULLUP);       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }

  // Set the pin that enables the transmitter; may not be needed
  pinMode(DMX_kTX_PIN, OUTPUT);
  digitalWriteFast(DMX_kTX_PIN, HIGH);

  dmxTx.begin();
    pinMode(LED_BUILTIN, OUTPUT);
    // initialize communication
    Wire.begin();
    Serial.begin(38400);
    // initialize OLED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    // display text
    display.setCursor(0,0);
    display.print("working...");
    display.display();

  
  organ.setup();

}

void loop() {
  // put your main code here, to run repeatedly:

  /*digitalWrite(4, HIGH);
  delay(5000);
  digitalWrite(4, LOW);
  delay(5000);
*/
  organ.update();


for(int i=0; i<4; i++) {
  rgbw[i]++;
  if(rgbw[i] > maxBrightness) {
  }

  float t = millis() / (1000.0+i*500);
  uint16_t value = (maxBrightness*0.5) + (maxBrightness*0.5) * sin( t * 2.0 * PI );

  rgbw[i] = value;
}

// TODO: input circuit
// read button press, knob position

// TODO: acitivity detection
// read PIR input

// TODO: valve control, timing is critical make sure it always has priority

// TODO: make a timer to set DMX update frequency
dmxTx.set16Bit(ch[0], rgbw[0]);
dmxTx.set16Bit(ch[1], rgbw[1]);
dmxTx.set16Bit(ch[2], rgbw[2]);
dmxTx.set16Bit(ch[3], rgbw[3]);

//dmxTx.set16Bit(2, brightness);
//dmxTx.set16Bit(3, brightness);
//dmxTx.set16Bit(4, brightness);


  // disable display until input buttons pressed
  if(!inputStateEnabled){

    display.clearDisplay();
    display.display(); // clear the display only if something on it
    

  } else {

    if(millis() - lastDisplayUpdate > displayUpdateInterval) {
      lastDisplayUpdate = millis();
      char divider = (second() % 2 == 0) ? ':' : ' ';
      display.clearDisplay();
      display.setCursor(0,0);
      displayDigits(hour());
      display.print(divider);
      displayDigits(minute());
      display.print(divider);
      displayDigits(second());

      display.display(); 
    }
  }

  for (int i = 0; i < NUM_BUTTONS; i++)  {
    buttons[i].update();
  }

  if(inputStateEnabled){
    if(millis() - lastInputTime > inputStateTimeout){
      inputStateEnabled = false;
      return;
    } else {

      // Handle button inputs, update lastInputTime if any buttons pressed
      // Menu navigation using 3 buttons to set the current time
      
      // button 1 is menu navigation - hour, minute, second, date (we need date for timezone handling) 
      // button 2 is UP
      // button 3 is DOWN

      if(buttons[0].fell()){
        lastInputTime = millis();

        // shift through active menu item
        activeMenuIndex = (activeMenuIndex + 1) % NUM_MENU_ITEMS;

      }

      if(buttons[1].fell()){
        lastInputTime = millis();

        // UP
        if(menu[activeMenuIndex] == 'h'){
          // hour
        } else if(menu[activeMenuIndex] == 'm'){
          // minute
        } else if(menu[activeMenuIndex] == 's'){
          // second
        } else if(menu[activeMenuIndex] == 'd'){
          // date
        } else if(menu[activeMenuIndex] == 'm'){
          // month
        } else if(menu[activeMenuIndex] == 'y'){
          // year
        } else if(menu[activeMenuIndex] == 't'){
          // test
        }

      }

      if(buttons[2].fell()){
        lastInputTime = millis();

      }

    }
  } else {

    for (int i = 0; i < NUM_BUTTONS; i++)  {
      if ( buttons[i].fell() ) {
        inputStateEnabled = true;
        lastInputTime = millis();

        organ.play(); // TODO: move to proper location in logic
        Serial.println("play organ");
      }
    }

  }
  
  // test buttons
  /*digitalWriteFast(valve_pins[0], buttons[0].read());
  digitalWriteFast(valve_pins[1], buttons[1].read());
  digitalWriteFast(valve_pins[2], buttons[2].read());
*/

  PIR->update();
  if (PIR->fell()) {
      //digitalWriteFast(valve_pins[3], PIR->read());
  }

  //Serial.println(PIR->read());

  //digitalWriteFast(valve_pins[3], digitalReadFast(IN1_PIN));




}


void playSong() {

}
