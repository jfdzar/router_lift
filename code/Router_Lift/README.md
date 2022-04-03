# Router Lift Code

The actual code is working on a ESP32-WROOM Devkit, with the following PinOut:
![ESP32](https://github.com/jfdzar/router_lift/blob/71ae90a2270c71d60ac2871e5314fc9e2b65e267/doc/wroom-esp32-pinout.jpeg)

the motor controller is a [Pololu A4988](https://www.pololu.com/product/1182).

Following Pins are used at the moment:

For the Encoder
* ENCAPIN 32
* ENCBPIN 35
* ENCBUTTON 34

For Buttons and LEDs
* BUTTONAPIN 27
* LEDGREEN 4
* LEDRED 23

For the Motor
* STEPPIN 12
* DIRPIN 13
* ENABLEPIN 2 (Using the same as the Built-In LED)
* MS1PIN 33
* MS2PIN 25
* MS3PIN 26

For the display - I2C Pins are being used
* SDA D21
* SCK D22
