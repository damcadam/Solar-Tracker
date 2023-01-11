// Author: Darius McAdam
// Program: Hold functions to setup the hardware for the encoder and 
//          run ISRs when encoder positions change

// Position of the encoder. Will change in the interrupt service routines
volatile int encoderPosition;

// ISR for A
void EncoderMonitorA()
{
  //input A and input B are equal // input A and input B are pins 2 & 3 respectively.
  if (digitalRead(2) == digitalRead(3)) encoderPosition++;
  else encoderPosition--;
}

// ISR for B
void EncoderMonitorB()
{
  //input A and input B are equal // input A and input B are pins 2 & 3 respectively.
  if (digitalRead(2) == digitalRead(3)) encoderPosition--;
  else encoderPosition++;
}

void EncoderInitialize()
{
  // Set pins 2 and 3 to input
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  // Attach the ISR to the pins, choose the ISR function, and set the waveform condition
  attachInterrupt(digitalPinToInterrupt(2), EncoderMonitorA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), EncoderMonitorB, CHANGE);
}