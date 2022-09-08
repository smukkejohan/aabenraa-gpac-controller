#include "organ.h"

Organ::Organ()
{

    _startTime = 0;
    _isPlaying = false;
    _fanState = false;
    _nextNoteMillis = 0; 
    _duration = 0;

    // initialize _pipeStates to false
    for (int i = 0; i < NUM_PIPES; ++i)
    {
        _pipeStates[i] = 0;
    }
    // initialize _endNoteIndexes to MAX_NOTES
    for (int i = 0; i < NUM_PIPES; ++i)
    {
        _endNoteIndexes[i] = MAX_NOTES - 1;
    }
}

void Organ::loadGivePeaceAChanceChorus()
{
    uint16_t notes[][MAX_NOTES] = {{5557, 521}, {0, 521, 6078, 521}, {521, 521, 1389, 1042, 6599, 347}, {1042, 347, 6947, 2779}, {2431, 290, 2766, 2097, 5187, 370}};

    loadNotes(notes);
}

void Organ::loadGivePeaceAChanceChorusSecondVoice()
{
    uint16_t notes[][MAX_NOTES] = {{5557, 521}, {0, 521, 6078, 521}, {521, 521, 1389, 1042, 3473, 695, 5557, 1389}, {1042, 347, 6947, 2779}, {1389, 1346, 2766, 2097, 5187, 370, 6947, 2779}};

    loadNotes(notes);
}

void Organ::loadGivePeaceAChanceChorusSecondVoiceChurch()
{
    uint16_t notes[][MAX_NOTES] = {
        {3126, 174, 3473, 174, 4515, 174, 5557, 521, 8336, 695},
        {0, 521, 6078, 521, 7641, 695, 9031, 347, 9725, 2779},
        {521, 521, 1389, 1042, 3126, 174, 3473, 174, 4515, 174, 4863, 347, 5557, 1389, 9378, 347},
        {0, 174, 347, 174, 1042, 347, 2084, 174, 6947, 2779},
        {695, 174, 1389, 174, 1737, 174, 2431, 290, 2779, 2084, 5187, 370, 6947, 5557}};

    loadNotes(notes);
}

void Organ::loadNotes(uint16_t notes[NUM_PIPES][MAX_NOTES])
{
    Serial.println("organ loadNotes() called");
    for (int p = 0; p < NUM_PIPES; ++p)
    {
        for (int i = 0; i < MAX_NOTES; ++i)
        {
            _notes[p][i] = notes[p][i];
        }
    }

    for (int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex)
    {
        for (int i = 1; i < MAX_NOTES; i++)
        {
            if (_notes[pipeIndex][i] == 0)
            {
                _endNoteIndexes[pipeIndex] = i - 1;
                break;
            }
        }
    }

    _duration = 0;
    for (int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex)
    {
        uint16_t endMillis = _notes[pipeIndex][_endNoteIndexes[pipeIndex] - 1] + _notes[pipeIndex][_endNoteIndexes[pipeIndex]];
        if (_duration < endMillis)
        {
            _duration = endMillis;
        }
    }

    Serial.print("organ loadNotes() duration: ");
    Serial.println(_duration);
}

void Organ::fanOn()
{
    _fanState = true;
    Serial.print("organ fanOn called _fanState: ");
    Serial.println(_fanState);
    digitalWriteFast(SSR_AC_PIN, HIGH);
}

void Organ::fanOff()
{
    _fanState = false;
    Serial.print("organ fanOff called _fanState: ");
    Serial.println(_fanState);
    digitalWriteFast(SSR_AC_PIN, LOW);
}

void Organ::setup()
{
    pinMode(SSR_AC_PIN, OUTPUT);
    digitalWriteFast(SSR_AC_PIN, HIGH);

    for (int i = 0; i < NUM_PIPES; ++i)
    {
        pinMode(valvePins[i], OUTPUT);
        digitalWriteFast(valvePins[i], 0);
    }
}

void Organ::play(bool autoFan)
{
    _isPlaying = true;
    _startTime = millis();
    if (!_fanState && autoFan)
    {
        fanOn();
    }

    Serial.print("organ play called");
    Serial.print("_startTime: ");
    Serial.println(_startTime);
}

void Organ::stop()
{
    _isPlaying = false;
    // Close all valves
    Serial.println("organ stop() called");

    for (int i = 0; i < NUM_PIPES; i++)
    {
        digitalWriteFast(valvePins[i], 0);
        _pipeStates[i] = 0;
    }
}

void Organ::update()
{
    if (!_isPlaying) {
        return;
    }

    uint32_t t = millis();
    uint16_t delta = t - _startTime;

    if (delta > _duration)
    {
        Serial.print("delta above duration, delta: ");
        Serial.println(delta);
        Serial.print(" t: ");
        Serial.println(t);

        stop();
        return;
    }

    uint16_t next = _duration;

    for (int pipeIndex = 0; pipeIndex < NUM_PIPES; ++pipeIndex)
    {
        int end = _endNoteIndexes[pipeIndex] - 1;

        for (int i = end; i >= 0; i -= 2)
        {
            if (delta > _notes[pipeIndex][i])
            {
                uint16_t nextCloseAt = _notes[pipeIndex][i] + _notes[pipeIndex][i + 1];
                if (delta < nextCloseAt)
                {
                    setValve(pipeIndex, 1);
                    if (nextCloseAt < next)
                    {
                        next = nextCloseAt;
                    }
                    break;
                }
                else
                {
                    setValve(pipeIndex, 0);
                    if (i + 2 <= end)
                    {
                        uint16_t nextOpenAt = _notes[pipeIndex][i + 2];
                        if (nextOpenAt < next)
                        {
                            next = nextOpenAt;
                        }
                    }
                }
                break;
            }
        }
    }

    _nextNoteMillis = _startTime + next;

    // Block the caller thread and loop update if next update is within critical timeframe
    // TODO: needs testing
    /*if(getCurrentNonBlockingDuration() < 10) {
        update();
    }*/
}

bool Organ::isPlaying()
{
    return _isPlaying;
}

uint16_t Organ::getCurrentNonBlockingDuration()
{
    return _nextNoteMillis - millis();
}

bool Organ::getPipeState(int i)
{
    return _pipeStates[i];
}

bool Organ::getFanState()
{
    return _fanState;
}

// private:

void Organ::setValve(uint8_t i, int value)
{

    if (_pipeStates[i] != value)
    {
        digitalWriteFast(valvePins[i], value);
        _pipeStates[i] = value;
    }
}