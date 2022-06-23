#include <AccelStepper.h>
#include <OneButton.h>
#include <SPI.h>
#include <HighPowerStepperDriver.h>

// Stepper
#define STEPPIN 16
#define DIRPIN 17
#define ENABLEPIN 26
#define SLEEPPIN 27
#define MS1PIN 25
#define MS2PIN 33
#define MS3PIN 32
#define SCK 18
#define MISO 19
#define MOSI 23
#define CSPIN 5
#define MOTORINTERFACETYPE 1

#define ENDSTOPUP 35
#define ENDSTOPDOWN 34
//#define ENDSTOPDOWN 39 test

#define MICROSTEP 4
#define MICROSTEPTI HPSDStepMode::MicroStep4
#define MOTORSTEPREV 200
#define SPINDLEMMREV 3
#define MOTPOSPOSFACTOR (MOTORSTEPREV * MICROSTEP / SPINDLEMMREV)

#define INITSPEED 5000  // in step/s
#define MAXACCEL 45000  // in step/s^2
#define MINPULSEWIDTH 2 // in uS

#define CURRENTLIMIT 2500 // Current limit in mA

class RouterLiftMotor
{
public:
    RouterLiftMotor()
    {
        _mot_position = 0;
    }

    void init()
    {
        SPI.begin(SCK, MISO, MOSI, CSPIN);
        _sd.setChipSelectPin(CSPIN);
        delay(1);

        // Reset the driver to its default settings and clear latched status
        // conditions.
        _sd.resetSettings();
        _sd.clearStatus();
        // Select auto mixed decay.  TI's DRV8711 documentation recommends this mode
        // for most applications, and we find that it usually works well.
        _sd.setDecayMode(HPSDDecayMode::AutoMixed);
        // Set the current limit. You should change the number here to an appropriate
        // value for your particular system.
        _sd.setCurrentMilliamps36v4(CURRENTLIMIT);
        // Set the number of microsteps that correspond to one full step.
        _sd.setStepMode(MICROSTEPTI);
        // Enable the motor outputs.
        _sd.enableDriver();
        _sd.setDirection(0);

        _mot_position = 0;
        _stepper.setMaxSpeed(INITSPEED);
        _stepper.setSpeed(INITSPEED);
        _stepper.setAcceleration(MAXACCEL);
        _stepper.setMinPulseWidth(MINPULSEWIDTH);
        pinMode(MS1PIN, OUTPUT);
        pinMode(MS2PIN, OUTPUT);
        pinMode(MS3PIN, OUTPUT);
        pinMode(ENABLEPIN, OUTPUT);
        pinMode(SLEEPPIN, OUTPUT);
        digitalWrite(SLEEPPIN, HIGH);
        pinMode(ENDSTOPDOWN, PULLUP);
        pinMode(ENDSTOPUP, PULLUP);
        setMicrostep(MICROSTEP);
    }

    void check_end_stops()
    {
        _endstopdown = digitalRead(ENDSTOPDOWN);
        _endstopup = digitalRead(ENDSTOPUP);

        if (_endstopdown)
        {
            if (_mot_position > _stepper.currentPosition())
            {
                _mot_position = _stepper.currentPosition();
                _stepper.moveTo(_mot_position);
                _position = _mot_position / MOTPOSPOSFACTOR;
                _endstopevent = true;
            }
        }
        if (_endstopup)
        {
            if (_mot_position < _stepper.currentPosition())
            {
                _mot_position = _stepper.currentPosition();
                _stepper.moveTo(_mot_position);
                _position = _mot_position / MOTPOSPOSFACTOR;
                _endstopevent = true;
            }
        }
    }

    bool get_endstopevent()
    {
        return _endstopevent;
    }

    void reset_endstopevent()
    {
        _endstopevent = false;
    }

    void enable_motor()
    {
        digitalWrite(ENABLEPIN, LOW);
        _sd.enableDriver();
    }

    void disable_motor()
    {
        digitalWrite(ENABLEPIN, HIGH);
        _sd.disableDriver();
    }

    void setMicrostep(uint8_t microStep)
    {
        _setMicrostep(MICROSTEP, MS1PIN, MS2PIN, MS3PIN);
    }

    float get_position()
    {
        return _position;
    }

    long get_speed()
    {
        return _speed;
    }

    uint16_t get_current()
    {
        return _max_current;
    }

    float get_mot_position()
    {
        return _mot_position;
    }

    void update_position(float position)
    {
        _position = position;
    }

    void update_mot_position()
    {
        _mot_position = round(_position * MOTPOSPOSFACTOR);
        _stepper.moveTo(_mot_position);
    }

    void save_zero_position()
    {
        _position = 0;
        _mot_position = 0;
        _stepper.setCurrentPosition(0);
    }

    void set_speed(long speed)
    {
        _speed = speed;
        _stepper.setSpeed(_speed);
        _stepper.setMaxSpeed(_speed);
    }

    void set_current_limit(uint16_t max_current)
    {
        _max_current = max_current;
        _sd.setCurrentMilliamps36v4(max_current);
    }

    void set_acceleration(long acceleration)
    {
        _acceleration = acceleration;
        _stepper.setAcceleration(_acceleration);
    }

    void run()
    {
        if (_mot_position == _stepper.currentPosition())
        {
            _sd.disableDriver();
        }
        else
        {
            unsigned long moveTime = millis();
            _sd.enableDriver();
            _stepper.runToPosition();
            Serial.print("Move Time: ");
            moveTime = millis() - moveTime;
            Serial.println(moveTime);
            Serial.print("Faults: ");
            uint8_t faults = _sd.readFaults();
            _sd.clearFaults();
            Serial.println(faults, HEX);
        }

        //_stepper.run();
        // check_end_stops();
    }

    bool is_moving()
    {
        if (_stepper.currentPosition() == _mot_position)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool get_endstopdown()
    {
        return _endstopdown;
    }

    bool get_endstopup()
    {
        return _endstopup;
    }

private:
    // Motor
    AccelStepper _stepper = AccelStepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);
    HighPowerStepperDriver _sd;

    int64_t _mot_position = 0;
    float _position = 0;
    long _speed = INITSPEED;
    long _acceleration = MAXACCEL;
    uint16_t _max_current = CURRENTLIMIT;

    bool _endstopdown = false;
    bool _endstopup = false;
    bool _endstopevent = false;

    void _setMicrostep(uint8_t microStep, uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin)
    {
        disable_motor();
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
        enable_motor();
    }
};