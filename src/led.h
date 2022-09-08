
#ifndef _LED_h
#define _LED_h

#include <Arduino.h>
#include <TeensyDMX.h>

#define DMX_kTX_PIN 23
#define NUM_LED_SEGMENTS 8

const int numChannels = NUM_LED_SEGMENTS*4;
const uint16_t maxBrightness = 65535; // 255*255 65536 values

class LED {
    public:
        LED();

        void setup();

        void update();

        void purpleColorLoop();

        void dmxSet(int index, uint16_t value);

        void getSegmentColor(int segment, uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *w);
        
        void setSegmentColor(int segment, uint16_t r, uint16_t g, uint16_t b, uint16_t w);


    protected:
        ::qindesign::teensydmx::Sender dmxTx{Serial5};

        uint8_t ch[numChannels];
        uint16_t val[numChannels];
        //int channel;
};

#endif