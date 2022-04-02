/*********************************************************************
 * Router Lift for Ultimate Workbench Project
 * @author jfdzar
 * @date 02.2022
 * @details To be completed
 *********************************************************************/
#include <Arduino.h>
#include "RouterLiftDisplay.h"
#include "RouterLiftMotor.h"
#include "interfaces.h"

// Display
RouterLiftDisplay rldisplay;

// Serial
#define BAUDRATE 9600
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete

// Motor
RouterLiftMotor rlmotor;

void setup()
{
  // Serial
  Serial.begin(BAUDRATE);
  inputString.reserve(200);

  // Motor
  rlmotor.init();

  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 100 * 100, false); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  // rotaryEncoder.setAcceleration(50);
  rotaryEncoder.setEncoderValue(ENCINITPOS); // set default to 0

  // Inputs & Outputs
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDRED, OUTPUT);

  button_a.attachClick(startRouter);          // link the function to be called on a singleclick event.
  button_a.attachLongPressStop(lockControls); // link the function to be called on a longpress event.

  button_enc.attachClick(moveStepper);
  button_enc.attachDuringLongPress(saveZeroPosition);

  // Display
  rldisplay.init();
  rldisplay.poweron_screen();

  Serial.println("Starting Loop");
}

void loop()
{
  serialEvent();
  rlmotor.run();

  button_a.tick();
  if (!lockedControls)
  {
    button_enc.tick();
    rldisplay.draw_position(rlmotor.get_position());
    if (rotaryEncoder.encoderChanged())
    {
      rlmotor.update_position(float(rotaryEncoder.readEncoder()) / 100);
    }
  }
  if (lockedControls)
  {

    rldisplay.draw_locked();
  }

  if (!rlmotor.is_moving())
  {
    rlmotor.disable_motor();
    rldisplay.redraw();
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
        long speed = 0;
        speed = inputString.toInt();
        Serial.print("Speed: ");
        Serial.println(inputString);
        rlmotor.set_speed(speed);
      }
      else
      {
        long acceleration = 0;
        acceleration = inputString.toInt();
        Serial.print("Acceleration: ");
        Serial.println(inputString);
        rlmotor.set_acceleration(acceleration);
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
  rlmotor.update_mot_position();
  rlmotor.enable_motor();
  rldisplay.draw_moving();
  rldisplay.redraw();
}

void saveZeroPosition()
{
  rlmotor.save_zero_position();
  rotaryEncoder.setEncoderValue(0);
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}