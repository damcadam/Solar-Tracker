# Solar-Tracker

## Description
**Author: Darius McAdam**  
**Date: May 2022**  
**Purpose: Final project for Intro to Computer Engineering (ECE 241)**

The goal of the Solar-Tracker project was replicate solar panels that track the sun using an Aduino Nano and a servo. During daylight (from 6:00 AM to 6:00 PM), the arm of the servo will point towards any bright light source. If there is not a light source brighter than a certain theshold, the servo position is based on the time of day. After 6:00 PM the sun is considered to be down, so the servo moves to the 0 degree position to anticipate the rising sun. An LCD connected to the Arduino will always display the servo angle and the clock time. The time on the clock could be adjusted by turning or pressing on a rotary encoder.

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
The project worked well! Bright light sources like the flashlight on my phone were tracked by the servo arm. When a light source was not present, the servo position was calculated using a linear interoplation from the time difference between 6:00 AM and 6:00 PM. The interpolation returned the correct angle for the time of day. I met all the requirements for the project. However, I think some improvements can be made to the Solar-Tracker. Most of the code the `void loop()` executes every 0.5 seconds. When that code runs, the script will increase the hour if the user is pressing on the encoder button. The minutes will decrease or incerase based on the difference between the rotary position before and after the 0.5 second interval. To enhance the Solar-Tracker, I would add code that saves the user's input when the button is pressed. So the user would not need to press the button during one of the 0.5 second intervals. Thw current way the rotary enocder works means the must rotate it a certain minimum angle to register, then a minute is increased or decreased. I would like to have the program
