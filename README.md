# Solar-Tracker

## Description
**Author: Darius McAdam**  
**Date: May 2022**

The goal of the Solar-Tracker project was replicate solar panels that track the sun using an Aduino Nano and a servo. During daylight (from 6:00 AM to 6:00 PM), the arm of the servo will point towards any bright light source. If there is not a light source brighter than a certain theshold, the servo position is based on the time of day. After 6:00 PM the sun is considered to be down, so the servo moves to the 0 degree position to anticipate the rising sun. An LCD connected to the Arduino will always display the servo angle and the clock time.

## Code
**Language:** The project was programmed in C++.  
**Organization:** Three classes were used to control the Solar-Tracker. They controlled the clock, output from the rotary encoder, and the main flow of the program. In the main file, functions were created for major pieces of functionality. Then each function was called in the program's `void loop()`

## Hardware
- Arduino Nano
- Servo
- LCD
- Rotary encoder with push button
- Photoresistors

## Outcome
