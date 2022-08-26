

// Each note has an array
// every odd array value is the starting time in milliseconds for the note
// every even array value is the duration in milliseconds for the note
#include <Arduino.h>
//#include<vector>

//using std::vector;

/*struct PipeNotes {

    public:
        int pin;
        int *notes;

}*/

#define MAX_NOTES 20 // Maximum number of notes per pipe, has to be an even number
#define NUM_PIPES 5 // Number of pipes

class Organ {
    public:

        Organ() {
                // initialize pipeStates to false
                for(int i = 0; i < NUM_PIPES; ++i) {
                    pipeStates[i] = false;
                }

                // initlize numNotes to MAX_NOTES
                for(int i = 0; i < NUM_PIPES; ++i) {
                    numNotes[i] = MAX_NOTES;
                }


        }

        int Bb2[MAX_NOTES] = {5557,521};   
        int A2[MAX_NOTES]  = {0,521,6078,521};
        int G2[MAX_NOTES]  = {521,521,1389,1042,6599,347};
        int F2[MAX_NOTES]  = {1042,347,6947,2779};
        int C2[MAX_NOTES]  = {2431,290,2766,2097,5187,370};

        long int startTime = 0;

        // TODO: load song function
        /*void loadSong(int song[][MAX_NOTES]) {
            for (int i = 0; i < NUM_PIPES; i++) {
                for (int j = 0; j < MAX_NOTES; j++) {
                    notes[i][j] = song[i][j];
                }
            }
        }*/

        void fanOn() {
            digitalWrite(fanPin, HIGH);
        }

        void fanOff() {
            digitalWrite(fanPin, LOW);
        }

        void setup() {

            pinMode(fanPin, OUTPUT);
            digitalWrite(fanPin, LOW); 

            for(int i = 0; i < NUM_PIPES; ++i) {
                pinMode(pins[i], OUTPUT);
                digitalWriteFast(pins[i], LOW);
            }


            for(int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex) {
                for(int i = 1; i < MAX_NOTES; i++) {
                        if(notes[pipeIndex][i] == 0) {
                            numNotes[pipeIndex] = i;
                            break;
                        }
                }
            }


            for(int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex) {
                int endMillis = notes[pipeIndex][numNotes[pipeIndex]-2] + notes[pipeIndex][numNotes[pipeIndex]-1];
                if(duration < endMillis) {
                    duration = endMillis;
                }
            }


        }

        void play() {
            isPlaying = true;
            startTime = millis();
        }

        void stop() {
            isPlaying = false;
            // Close all valves
            for (int i = 0; i < NUM_PIPES; i++) {
                digitalWriteFast(pins[i], LOW);
                pipeStates[i] = false;
            }
        }

        void update() { 
            if(!isPlaying) return;

            long int t = millis();
            int delta = t - startTime;

            if(delta > duration) {
                stop();
                return;
            }

            for(int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex) {

                for(int i = numNotes[pipeIndex]-2; i >= 0; i-=2) {
                    if(delta > notes[pipeIndex][i]) {
                        if(delta < notes[pipeIndex][i] + notes[pipeIndex][i+1]) {
                            setValve(pipeIndex, true);
                            break;
                        } else {
                            setValve(pipeIndex, false);
                        }
                        break;
                    }
                }
            }
        }

        bool isPlaying() {
            return isPlaying;
        }

    private:
        bool isPlaying = false;
        bool pipeStates[NUM_PIPES];
        int pins[NUM_PIPES] = {SSR1_PIN, SSR2_PIN, SSR3_PIN, SSR4_PIN, SSR5_PIN};
        int fanPin = SSR_AC_PIN;

        int *notes[NUM_PIPES] = {Bb2, A2, G2, F2, C2};
        int numNotes[NUM_PIPES];

        int duration = 0;

        void setValve(int i, int value) {

            if(pipeStates[i] != value) {
                digitalWriteFast(pins[i], value);
                pipeStates[i] = value;
            } 
        }
      
};

/*
class GivePeaceAChanceChorus : public OrganNotes {
  public:
    int Bb2[2] = {5557,521};   
    int A2[4] = {0,521,6078,521};
    int G2[6] = {521,521,1389,1042,6599,347};
    int F2[4] = {1042,347,6947,2779};
    int C2[6] = {2431,290,2766,2097,5187,370};
};


class GivePeaceAChanceChorusV2 {
  public:
    int Bb2[2] = {5557,521};   
    int A2[4] = {0,521,6078,521};
    int G2[8] = {521,521,1389,1042,3473,695,5557,1389};
    int F2[4] = {1042,347,6947,2779};
    int C2[8] = {1389,1346,2766,2097,5187,370,6947,2779};
};

class GivePeaceAChanceChorusV3 {
  public:
    int Bb2[10] = {3126,174,3473,174,4515,174,5557,521,8336,695};   
    int A2[10] = {0,521,6078,521,7641,695,9031,347,9725,2779};
    int G2[16] = {521,521,1389,1042,3126,174,3473,174,4515,174,4863,347,5557,1389,9378,347};
    int F2[10] = {0,174,347,174,1042,347,2084,174,6947,2779};
    int C2[14] = {695,174,1389,174,1737,174,2431,290,2779,2084,5187,370,6947,5557};
};
*/

/*
Var1 (bare melodi)
Bb2 = [5557,521]
A2 = [0,521,6078,521]
G2 = [521,521,1389,1042,6599,347]
F2 = [1042,347,6947,2779]
C2 = [2431,290,2766,2097,5187,370]
Pause = [9725,1389]

Var2 (Med andenstemme)

Bb2 = [5557,521]
A2 = [0,521,6078,521]
G2 = [521,521,1389,1042,3473,695,5557,1389]
F2 = [0,1389,6947,2779]
C2 = [1389,1346,2766,2097,5187,370,6947,2779]

Pause = [9725,1389]

Var3 (Lidt kirkeagtig og skal helst slutte. Altså ikke spilles inden en af de andre)
Bb2 = [3126,174,3473,174,4515,174,5557,521,8336,695]
A2 = [0,521,6078,521,7641,695,9031,347,9725,2779]
G2 = [521,521,1389,1042,3126,174,3473,174,4515,174,4863,347,5557,1389,9378,347]
F2 = [0,174,347,174,1042,347,2084,174,6947,2779]
C2 = [695,174,1389,174,1737,174,2431,290,2779,2084,5187,370,6947,5557]
*/
