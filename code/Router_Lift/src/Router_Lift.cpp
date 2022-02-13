/*********************************************************************
 * Router Lift for Ultimate Workbench Project
 * @author jfdzar
 * @date 02.2022
 * @details To be completed
*********************************************************************/
#include <Arduino.h>
#include "display.h"
#include <AccelStepper.h>

// Stepper
uint8_t stepPin = 12;
uint8_t dirPin = 13;
uint8_t motorInterfaceType = 1;
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// Display
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Serial
String inputString = "";     // a String to hold incoming data
bool stringComplete = false; // whether the string is complete

void setup()
{
  Serial.begin(9600);
  inputString.reserve(200);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(10000);
  stepper.setMinPulseWidth(20);

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast(0);           // dim display
  display.display();
  delay(2000);

  // text display tests
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(32, 0);
  display.println("Hola");
  //display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.println(0.0);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop()
{

  float number = 0;
  Serial.println("Starting Loop");

  display.println(number);
  display.display();

  //for (int i = 0; i < 100; i++) {
  while (1)
  {

    Serial.println("Printing Number...");
    display.clearDisplay();
    display.setTextSize(3);
    number = number + 1.1;
    display.setCursor(32, 32);
    display.println(number);
    display.display();

    // Set the speed in steps per second:
    stepper.setSpeed(400);

    int position = stepper.currentPosition();
    // Step the motor with a constant speed as set by setSpeed():
    stepper.moveTo(position + 400);
    while (stepper.isRunning())
    {
      stepper.run();
    }
    delay(1000);
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
      stringComplete = true;
    }
  }
}