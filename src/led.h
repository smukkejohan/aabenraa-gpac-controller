
#ifndef _LED_h
#define _LED_h

#include <Arduino.h>
#include <TeensyDMX.h>

#define DMX_kTX_PIN 23
#define NUM_LED_SEGMENTS 8

const int numChannels = NUM_LED_SEGMENTS*4;
const float maxBrightness = 65535; // 255*255 65536 values

/*
const float BRIGHT[4] = {0.93*maxBrightness, 0.7*maxBrightness, 0.96 * maxBrightness, 0.5*maxBrightness};

const float DARK[4] = {0.82*maxBrightness, 0.52*maxBrightness, 0.82 * maxBrightness, 0*maxBrightness};
*/

const float BRIGHT[4] = {0.94*maxBrightness, 0.65*maxBrightness, 0/*0.23*maxBrightness*/, 0}; // orange

const float DARK[4] = {0.82*maxBrightness, 0.52*maxBrightness, 0/*0.1 * maxBrightness*/, 0};


class LED {
    public:
        LED();

        void setup();

        void update();


        void purpleColorLoop();

        void dmxSet(int index, uint16_t value);

        void getSegmentColor(int segment, uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *w);
        
        void setSegmentColor(int segment, uint16_t r, uint16_t g, uint16_t b, uint16_t w);

        static float getFadeMod(int offsetIndex=1);

    protected:
        ::qindesign::teensydmx::Sender dmxTx{Serial5};

        uint8_t ch[numChannels];
        uint16_t val[numChannels];
        //int channel;
};

#endif