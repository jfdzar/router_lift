/*********************************************************************
 * Router Lift for Ultimate Workbench Project
 * @author jfdzar
 * @date 02.2022
 * @details To be completed
 *********************************************************************/
#include <Arduino.h>
#include "display.h"
#include "motor.h"
#include <AccelStepper.h>
#include <AiEsp32RotaryEncoder.h>
#include <OneButton.h>

void serialEvent();
void IRAM_ATTR readEncoderISR();

void startRouter();
void lockControls();
void moveStepper();
void saveZeroPosition();

// Stepper
#define STEPPIN 12
#define DIRPIN 13
#define MS1PIN 33
#define MS2PIN 25
#define MS3PIN 26
#define MOTORINTERFACETYPE 1

#define MICROSTEP 4
#define MOTORSTEPREV 200
#define SPINDLEMMREV 3
#define MOTPOSPOSFACTOR (MOTORSTEPREV * MICROSTEP / SPINDLEMMREV)

#define INITSPEED 3200   // in step/s
#define MAXACCEL 45000   // in step/s^2
#define MINPULSEWIDTH 20 // in uS

// Encoder
#define ROTARY_ENCODER_STEPS 1
#define ENCAPIN 32
#define ENCBPIN 35
#define ENCBUTTON 34
#define ENCINITPOS 0

// Buttons and LEDs
#define BUTTONAPIN 27
#define LEDGREEN 4
#define LEDRED 23

// Serial
#define BAUDRATE 9600

// Motor
AccelStepper stepper = AccelStepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);
int64_t mot_position = 0;
float position = 0;
long speed = 6000;
long acceleration = 45000;

// Display
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ENCAPIN, ENCBPIN, ENCBUTTON, -1, ROTARY_ENCODER_STEPS);

// Inputs & Outputs
OneButton button_a(BUTTONAPIN, true, true);
OneButton button_enc(ENCBUTTON, false, false);

bool lockedControls = false;

// Serial
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete

unsigned long lastTime = 0;

void setup()
{
  // Serial
  Serial.begin(BAUDRATE);
  inputString.reserve(200);

  // Motor
  stepper.setMaxSpeed(INITSPEED);
  stepper.setSpeed(INITSPEED);
  stepper.setAcceleration(MAXACCEL);
  stepper.setMinPulseWidth(MINPULSEWIDTH);
  pinMode(MS1PIN, OUTPUT);
  pinMode(MS2PIN, OUTPUT);
  pinMode(MS3PIN, OUTPUT);
  setMicrostep(MICROSTEP, MS1PIN, MS2PIN, MS3PIN);

  // Display
  //  Show image buffer on the display hardware.
  //  Since the buffer is intialized with an Adafruit splashscreen
  //  internally, this will display the splashscreen.
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast(0);           // dim display
  display.display();
  delay(250);

  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 100 * 100, false); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  // rotaryEncoder.setAcceleration(50);
  rotaryEncoder.setEncoderValue(ENCINITPOS); // set default to 0

  // Inputs & Outputs
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDRED, OUTPUT);

  button_a.attachClick(startRouter);          // link the function to be called on a singleclick event.
  button_a.attachLongPressStop(lockControls); // link the function to be called on a longpress event.

  button_enc.attachClick(moveStepper);
  button_enc.attachDuringLongPress(saveZeroPosition);

  // text display tests
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 10);
  display.println("Automatic");
  display.println("Router Lift");
  // display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.display();
  delay(1000);
  display.clearDisplay();
  Serial.println("Starting Loop");
}

void loop()
{

  serialEvent();
  stepper.moveTo(mot_position);
  stepper.run();
  button_a.tick();
  if (!lockedControls)
  {
    button_enc.tick();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(32, 32);
    display.println(position);
    if (rotaryEncoder.encoderChanged())
    {
      stepper.run();
      position = float(rotaryEncoder.readEncoder()) / 100;
      stepper.run();
    }
  }
  if (lockedControls)
  {
    display.setTextSize(1);
    display.setCursor(60, 0);
    display.println("Locked");
  }

  if (stepper.currentPosition() == mot_position)
  {
    display.display();
  }
}

void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {

      if (!lockedControls)
      {
        speed = inputString.toInt();
        Serial.print("Speed: ");
        Serial.println(inputString);
        stepper.setSpeed(speed);
        stepper.setMaxSpeed(speed);
      }
      else
      {
        acceleration = inputString.toInt();
        Serial.print("Acceleration: ");
        Serial.println(inputString);
        stepper.setAcceleration(acceleration);
      }

      inputString = "";
    }
  }
}

void startRouter()
{
  digitalWrite(LEDGREEN, !digitalRead(LEDGREEN));
}

void lockControls()
{
  digitalWrite(LEDRED, !digitalRead(LEDRED));
  lockedControls = !lockedControls;

  if (lockedControls)
  {
    rotaryEncoder.disable();
  }
  else
  {
    rotaryEncoder.enable();
  }
}

void moveStepper()
{
  mot_position = round(position * MOTPOSPOSFACTOR);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("*");
  display.display();
}

void saveZeroPosition()
{
  position = 0;
  rotaryEncoder.setEncoderValue(position);
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}