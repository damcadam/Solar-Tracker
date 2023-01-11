# Solar-Tracker
**Author:** Darius McAdam    
**Date:** May 2022  
**Purpose:** Final project for Intro to Computer Engineering (ECE 241)  
**Language:** C++  

## Description
The goal of the Solar-Tracker project was to replicate solar panels that track the sun thoughout the day. The project includes the following hardware.

- Arduino Nano
- Servo
- LCD
- Rotary encoder with push button
- Photoresistors

If there is a bright enough light source on photoresistors, the servo will point towards the light. In the absence of bright light the servo position is based on the time of day. This represents the solar panels pointing to the position the sun would be on a cloudy day. The mintues on the clock can be changed by rotating the encoder, and pressing the enocder will move the hours forward by one. 
