#include <AccelStepper.h>
#include <OneButton.h>

// Stepper
#define STEPPIN 12
#define DIRPIN 13
#define ENABLEPIN 2 // Same as built in LEDPIN
#define MS1PIN 33
#define MS2PIN 25
#define MS3PIN 26
#define MOTORINTERFACETYPE 1

#define ENDSTOPUP 18
#define ENDSTOPDOWN 19

#define MICROSTEP 4
#define MOTORSTEPREV 200
#define SPINDLEMMREV 3
#define MOTPOSPOSFACTOR (MOTORSTEPREV * MICROSTEP / SPINDLEMMREV)

#define INITSPEED 2000   // in step/s
#define MAXACCEL 45000   // in step/s^2
#define MINPULSEWIDTH 20 // in uS

class RouterLiftMotor
{
public:
    RouterLiftMotor()
    {
        _mot_position = 0;
    }

    void init()
    {
        _mot_position = 0;
        _stepper.setMaxSpeed(INITSPEED);
        _stepper.setSpeed(INITSPEED);
        _stepper.setAcceleration(MAXACCEL);
        _stepper.setMinPulseWidth(MINPULSEWIDTH);
        pinMode(MS1PIN, OUTPUT);
        pinMode(MS2PIN, OUTPUT);
        pinMode(MS3PIN, OUTPUT);
        pinMode(ENABLEPIN, OUTPUT);
        pinMode(ENDSTOPDOWN, PULLDOWN);
        pinMode(ENDSTOPUP, PULLDOWN);
        setMicrostep(MICROSTEP);
    }

    void check_end_stops()
    {
        _endstopdown = digitalRead(ENDSTOPDOWN);
        _endstopup = digitalRead(ENDSTOPUP);

        if (_endstopdown || _endstopup)
        {
            _mot_position = _stepper.currentPosition();
            _stepper.moveTo(_mot_position);
            update_position(_mot_position / MOTPOSPOSFACTOR);
        }
    }

    void enable_motor()
    {
        digitalWrite(ENABLEPIN, LOW);
    }

    void disable_motor()
    {
        digitalWrite(ENABLEPIN, HIGH);
    }

    void setMicrostep(uint8_t microStep)
    {
        _setMicrostep(MICROSTEP, MS1PIN, MS2PIN, MS3PIN);
    }

    float get_position()
    {
        return _position;
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
    }

    void set_speed(long speed)
    {
        _speed = speed;
        _stepper.setSpeed(_speed);
        _stepper.setMaxSpeed(_speed);
    }

    void set_acceleration(long acceleration)
    {
        _acceleration = acceleration;
        _stepper.setAcceleration(_acceleration);
    }

    void run()
    {
        _stepper.run();
        check_end_stops();
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

    int64_t _mot_position = 0;
    float _position = 0;
    long _speed = INITSPEED;
    long _acceleration = MAXACCEL;

    bool _endstopdown = false;
    bool _endstopup = false;

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