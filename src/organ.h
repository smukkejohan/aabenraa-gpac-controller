#ifndef _Organ_h
#define _Organ_h

#include <Arduino.h>
#define SSR1_PIN 4
#define SSR2_PIN 5
#define SSR3_PIN 6
#define SSR4_PIN 7
#define SSR5_PIN 8
#define SSR6_PIN 9
#define SSR_AC_PIN 32

#define MAX_NOTES 20 // Maximum number of notes per pipe, has to be an even number
#define NUM_PIPES 5 // Number of pipes

const int valvePins[NUM_PIPES] = {SSR1_PIN, SSR2_PIN, SSR3_PIN, SSR4_PIN, SSR5_PIN};

class Organ {

    public:
        Organ();

        /*
        Standard version of chorus
        */
        void loadGivePeaceAChanceChorus();

        /*
        Version of chorus with second voice 
        */
        void loadGivePeaceAChanceChorusSecondVoice();

        /*
        Church like version of chorus with second voice 
        */
        void loadGivePeaceAChanceChorusSecondVoiceChurch();

        /* Accepts a twodimensional array of tones with notes in the  order: Bb, A, G, F, C.
        - Each tone is an array of notes, where each note is an array of values: [start, duration, start, duration, ...]. 
        - Every odd value is the starting time in milliseconds.
        - Every even  value is the duration in milliseconds for the note. */ 
        void loadNotes(uint16_t notes[NUM_PIPES][MAX_NOTES]);

        void fanOn();

        void fanOff();
 
        void setup();

        void play(bool autoFan=true);

        void stop();

        void update();

        bool isPlaying();

        uint16_t getCurrentNonBlockingDuration();

        bool getPipeState(int i);

        bool getFanState();

    protected:

        void setValve(uint8_t i, int value);
        uint32_t _startTime;
        bool _isPlaying = false;
        bool _pipeStates[NUM_PIPES];
        bool _fanState = false;
        // {Bb, A, G, F, C}
        uint16_t _notes[NUM_PIPES][MAX_NOTES]; 
        int _endNoteIndexes[NUM_PIPES];
        uint16_t _nextNoteMillis = 0; 
        uint16_t _duration = 0;

};

#endif