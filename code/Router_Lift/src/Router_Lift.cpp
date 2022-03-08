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
#include <DabbleESP32.h>

void serialEvent();
void IRAM_ATTR readEncoderISR();

// Bluetooth
#define CUSTOM_SETTINGS
#define INCLUDE_DABBLEINPUTS_MODULE

// Stepper
#define STEPPIN 12
#define DIRPIN 13
#define MS1PIN 33
#define MS2PIN 25
#define MS3PIN 26
#define MOTORINTERFACETYPE 1

#define MICROSTEP 1
#define MOTORSTEPREV 200
#define SPINDLEMMREV 4
#define MOTPOSPOSFACTOR (MOTORSTEPREV * MICROSTEP / SPINDLEMMREV)

#define INITMAXSPEED 4000 // in step/s
#define MAXACCEL 45000    // in step/s^2
#define MINPULSEWIDTH 20  // in uS

AccelStepper stepper = AccelStepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);

int64_t mot_position = 0;
float position = 0;
int64_t speed = 2000;
unsigned long lastTimePressed = 0;

// Display
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Encoder
uint8_t encAPin = 32;
uint8_t encBPin = 35;
uint8_t encButton = 34;
volatile uint8_t encPosition = 0;

#define ROTARY_ENCODER_STEPS 1
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(encAPin, encBPin, encButton, -1, ROTARY_ENCODER_STEPS);

// Serial
#define BAUDRATE 9600
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete

void setup()
{
  // Serial
  Serial.begin(BAUDRATE);
  inputString.reserve(200);

  // Bluetooth
  Dabble.begin("RouterLift");

  // Motor
  stepper.setMaxSpeed(INITMAXSPEED);
  stepper.setAcceleration(MAXACCEL);
  stepper.setMinPulseWidth(MINPULSEWIDTH);
  pinMode(MS1PIN, OUTPUT);
  pinMode(MS2PIN, OUTPUT);
  pinMode(MS3PIN, OUTPUT);
  setMicrostep(1, MS1PIN, MS2PIN, MS3PIN);

  // Display
  //  Show image buffer on the display hardware.
  //  Since the buffer is intialized with an Adafruit splashscreen
  //  internally, this will display the splashscreen.
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast(0);           // dim display
  display.display();
  delay(500);

  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  // rotaryEncoder.setBoundaries(88 * 10, 104 * 10, true); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  // rotaryEncoder.setAcceleration(50);
  rotaryEncoder.setEncoderValue(0); // set default to 0

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
  /*
  Dabble.processInput();
  position = Inputs.getPot1Value();
  rotaryEncoder.setEncoderValue(position);
  mot_position = position;
  */

  stepper.moveTo(mot_position);
  while (stepper.currentPosition() != mot_position)
  {
    stepper.run();
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(32, 32);
  display.println(position);
  display.display();

  if (rotaryEncoder.encoderChanged())
  {
    Serial.println("Encoder Changed");
    Serial.println(rotaryEncoder.readEncoder());
    position = float(rotaryEncoder.readEncoder()) / 100;
    mot_position = round(position * MOTPOSPOSFACTOR);
  }
  if (rotaryEncoder.isEncoderButtonClicked())
  {
    Serial.println("Button Clicked");
    position = 0;
    rotaryEncoder.setEncoderValue(position);
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
      Serial.println(inputString);
      speed = inputString.toFloat();
      inputString = "";
    }
  }
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}