#include <Arduino.h>
#include "motor.h"

void setMicrostep(uint8_t microStep, uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin)
{
    if (microStep == 1)
    {
        digitalWrite(ms1Pin, LOW);
        digitalWrite(ms2Pin, LOW);
        digitalWrite(ms3Pin, LOW);
    }
    else if (microStep == 2)
    {
        digitalWrite(ms1Pin, HIGH);
        digitalWrite(ms2Pin, LOW);
        digitalWrite(ms3Pin, LOW);
    }
    else if (microStep == 4)
    {
        digitalWrite(ms1Pin, LOW);
        digitalWrite(ms2Pin, HIGH);
        digitalWrite(ms3Pin, LOW);
    }
    else if (microStep == 8)
    {
        digitalWrite(ms1Pin, HIGH);
        digitalWrite(ms2Pin, HIGH);
        digitalWrite(ms3Pin, LOW);
    }
    else if (microStep == 16)
    {
        digitalWrite(ms1Pin, HIGH);
        digitalWrite(ms2Pin, HIGH);
        digitalWrite(ms3Pin, HIGH);
    }
}