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

// Motor
RouterLiftMotor rlmotor;

// Serial
#define BAUDRATE 9600
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete

void setup()
{
  // Serial
  Serial.begin(BAUDRATE);
  inputString.reserve(200);



  // Motor
  rlmotor.init();

  // Display
  rldisplay.init();
  rldisplay.poweron_screen();


  // Encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(ENCLOWLIMIT, ENCUPPLIMIT, false); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.setAcceleration(500);
  rotaryEncoder.setEncoderValue(ENCINITPOS); // set default to 0


  // Inputs & Outputs
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDRED, OUTPUT);
  digitalWrite(LEDRED, HIGH);

  button_a.attachClick(startRouter);          // link the function to be called on a singleclick event.
  button_a.attachLongPressStop(lockControls); // link the function to be called on a longpress event.

  button_enc.attachClick(moveStepper);
  button_enc.attachLongPressStop(saveZeroPosition);

  rlmotor.set_current_limit(2500);

  Serial.println("Starting Loop");
}

void loop()
{
  serialEvent();
  rlmotor.run();
  rlmotor.evaluate_switches();

  button_a.tick();
  if (!lockedControls)
  {
    button_enc.tick();
    if (mode == MODE_POSITION)
    {
      rldisplay.draw_position(rlmotor.get_position());
      rldisplay.draw_mode("Pos");
      if (rlmotor.get_end_switch_down()) {rldisplay.draw_endswitch("D");}
      if (rlmotor.get_end_switch_up()) {rldisplay.draw_endswitch("U");}
      if (rotaryEncoder.encoderChanged())
      {
        rlmotor.update_position(float(rotaryEncoder.readEncoder()) / 100);
      }
      else
      {
        rotaryEncoder.setEncoderValue(rlmotor.get_position() * 100);
      }
    }
    else if (mode == MODE_SPEED)
    {
      rldisplay.draw_position(rlmotor.get_speed());
      rldisplay.draw_mode("Speed");
      if (rotaryEncoder.encoderChanged())
      {
        rlmotor.set_speed(float(rotaryEncoder.readEncoder()));
      }
    }
    else if (mode == MODE_CURRENT)
    {
      rldisplay.draw_position(rlmotor.get_current());
      rldisplay.draw_mode("mA");
    }
    else if (mode == MODE_USTEP)
    {
      rldisplay.draw_mode("uStep");
    }
  }
  else if (lockedControls)
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
        uint16_t max_current = 0;
        max_current = inputString.toInt();
        Serial.print("Current Limit: ");
        Serial.println(inputString);
        rlmotor.set_current_limit(max_current);
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
  Serial.println("Button A - Short Press");
  digitalWrite(LEDGREEN, !digitalRead(LEDGREEN));
  mode = mode + 1;
  if (mode > 1)
  {
    mode = 0;
  }
  if (mode == MODE_POSITION)
  {
    rotaryEncoder.setBoundaries(ENCLOWLIMIT, ENCUPPLIMIT, false); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    rotaryEncoder.setEncoderValue(rlmotor.get_position() * 100);
  }
  else if (mode == MODE_SPEED)
  {
    rotaryEncoder.setBoundaries(0, ENCSPEEDUPPLIMIT, false); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    rotaryEncoder.setEncoderValue(rlmotor.get_speed());
  }
  Serial.print("Mode: ");
  Serial.println(mode);
}

void lockControls()
{
  Serial.println("Button A - Long Press");
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
  Serial.println("Button Encoder - Short Press");
  rlmotor.update_mot_position();
  rlmotor.enable_motor();
  rldisplay.draw_moving();
  rldisplay.redraw();
}

void saveZeroPosition()
{
  Serial.println("Button Encoder - Long Press");
  rlmotor.save_zero_position();
  rotaryEncoder.setEncoderValue(0);
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

void IRAM_ATTR toggleLED()
{
  digitalWrite(LEDGREEN, !digitalRead(LEDGREEN));
}