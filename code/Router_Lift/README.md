# Router Lift Code

## Introduction

The actual code is working on a ESP32-WROOM Devkit, with the following PinOut:
![ESP32](https://github.com/jfdzar/router_lift/blob/71ae90a2270c71d60ac2871e5314fc9e2b65e267/doc/wroom-esp32-pinout.jpeg)

The motor controller is a [Pololu A4988](https://www.pololu.com/product/1182):
![A4988](https://a.pololu-files.com/picture/0J10061.1200.jpg?28dcebcb0b7b778f9af83cdb52c0ef8a)

## Pin Mapping
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


## Installation
To be completed 
Mostly self explanatory with PlatformIO
