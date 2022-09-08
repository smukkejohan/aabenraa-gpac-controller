
#include "led.h"


        LED::LED() {
            int _ch = 1;
            for(int i=0; i<numChannels; i++) {
                ch[i] = _ch;
                _ch += 2;
                val[i] = 0;
            }
        }

        void LED::setup() {
            // Set the pin that enables the transmitter; may not be needed
            pinMode(DMX_kTX_PIN, OUTPUT);
            digitalWriteFast(DMX_kTX_PIN, HIGH);

            dmxTx.begin();

            //dmxSet(0, maxBrightness);

            //purpleColorLoop();

            //setSegmentColor(3, maxBrightness,0,0,0);
        }

        void LED::update() {
            /**/
        }

        void LED::purpleColorLoop() {

            // Brick wall
            for(int i=1; i<=NUM_LED_SEGMENTS; i++) {
            
            }
            // Middle wall 

            // Organ wall

            for(int i=1; i<=NUM_LED_SEGMENTS; i++) {
                
                setSegmentColor(i, 0.93*maxBrightness, 0.7*maxBrightness, 0.96 * maxBrightness, 0.5*maxBrightness);

                // TODO: 

                // higher intensity in segment 1 to 5

                // middle wall segment (8) medium deep purple/violet

                // segment 6 and 7 more white on darker background

                //setSegmentColor
                /*float t = millis() / (1000.0+i*500);
                uint16_t value = (maxBrightness*0.5) + (maxBrightness*0.5) * sin( t * 2.0 * PI );

                val[i] = value;
                dmxTx.set16Bit(ch[i], val[i]);*/
            } 
        }

        float LED::getFadeMod(int offsetIndex) {
            float t = millis() / (20000.0+offsetIndex*500);
            return 0.5 + 0.5 * sin( t * 2.0 * PI );
        }  

        void LED::dmxSet(int ch16, uint16_t value) {
            val[ch16] = value;
            dmxTx.set16Bit(ch[ch16], val[ch16]);
        }

        void LED::getSegmentColor(int segment, uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *w) {
            int c = segment*4;
            *r = val[c-4];
            *g = val[c-3];
            *b = val[c-2];
            *w = val[c-1];
        }
        
        // Segments are 1 indexed
        void LED::setSegmentColor(int s, uint16_t r, uint16_t g, uint16_t b, uint16_t w) {

            int c = (s)*4;

            /*Serial.print("setSegment s: ");
            Serial.println(s);

            Serial.print("start channel: ");
            Serial.println(ch[c-4]);

            Serial.print("end channel s: ");
            Serial.println(ch[c-1]);
            
            Serial.print(" chc-4: ");
            Serial.print(ch[c-4]);
            Serial.print(" chc-3: ");
            Serial.print(ch[c-3]);
            Serial.print(" chc-2: ");
            Serial.print(ch[c-2]);
            Serial.print(" chc-1: ");
            Serial.println(ch[c-1]);*/

            //TODO: find minimum value LED will display - set 0 if under. 

            dmxSet(c-4, r);
            dmxSet(c-3, g);
            dmxSet(c-2, b); 
            dmxSet(c-1, w);
        }