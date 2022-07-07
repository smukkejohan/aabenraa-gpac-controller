
#include <Arduino.h>
#include <TimeLib.h>

// DMX
#include <TeensyDMX.h>

// OLED Display
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#include <DmxSimple.h>
//#include <FastLED.h>
bool builtinBlinkState = false;

// DMX OUTPUT
namespace teensydmx = ::qindesign::teensydmx;
// Pin for enabling or disabling the transmitter.
// This may not be needed for your hardware.
constexpr uint8_t kTXPin = 23;
// Create the DMX transmitter on Serial3.
teensydmx::Sender dmxTx{Serial5}; // Tx on Serial5 is pin 20
// The current channel outputting a value.
int channel;
// END DMX OUTPUT

// OLED display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
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


// Valves TODO
//int valve_pins[4] = {1,2,3,4,5,6};
//#define NUM_VALVES 6
//#define FAN_PIN 7



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


void setup() {
  setSyncProvider(getTeensy4Time);

  // put your setup code here, to run once:
  //DmxSimple.usePin(3);
  //DmxSimple.maxChannel(32);


  pinMode(4, OUTPUT);

  // Set the pin that enables the transmitter; may not be needed
  pinMode(kTXPin, OUTPUT);
  digitalWriteFast(kTXPin, HIGH);

    // Set up the test packet, including the start code
  /*for (int i = 0; i < teensydmx::kMaxDMXPacketSize; i++) {
    dmxTx.set(i, 0x55);
  }*/

    // Set appropriate BREAK and MAB times:
  // Bit time=9us, format=8E1: BREAK=90us, MAB=9us
  // Note that the MAB time might be larger than expected;
  // if too large, try the timer method below
  //dmxTx.setBreakSerialParams(1000000/9, SERIAL_8E1);

  // Optionally, we can try setting the times directly,
  // but they'll likely be larger by a few microseconds
  // dmxTx.setBreakTime(88);  // Should be in the range 88-120us
  // dmxTx.setMABTime(8);     // Should be in the range 8-16us
  // dmxTx.setBreakUseTimerNotSerial(true);

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


}

void loop() {
  // put your main code here, to run repeatedly:

  /*digitalWrite(4, HIGH);
  delay(5000);
  digitalWrite(4, LOW);
  delay(5000);
*/

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


  // TODO: make a timer to set display update frequency
  // disable display until input knob pressed

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
