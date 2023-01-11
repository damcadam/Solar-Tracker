// Author: Darius McAdam
// Program: Simulate solar panels that follow the rising and setting sun with a servo. If
//          a lightsource is not bright enough to follow, base servo position on the time of day.
// Date: May 2022

#include <Servo.h>
#include <LiquidCrystal.h> 
#include "TimeLib.h"        // For the clock
#include "EncoderMonitor.h" // To track the knob position

// Create the servo object to control servo position
Servo myServo;

// LCD Display object for lcd printing
LiquidCrystal lcd(A5, A4, 5, 6, 7, 8);

// Create global variables
int pos = 0;                        // Servo angle
int error = 5;
int lastEncoder = encoderPosition;  // Most recent position since we've entered our .5 second if statement

// Create globals for different pins
const int sens1 = 0;                // A0 (total)      
const int sens2 = 1;                // A1 (midPoint)
const int servoPin = 10;
const int encoderButtonPin = 4;     // Pressed when 0/false

// Create global for storing clock time
time_t currentTime;

// We will have a control of 2 Hz (times/sec)
unsigned long controlTimer;
const int controlVariable = 500;

// For the algorithm that rotates towards a light source
int lightOff = 900;       // Supposed to be similar to the total analog reading of the photoresistors when the lights are off
const int lightTolerance = 25;

// Number of times the encoder button has been pressed since the last .5 second interval
int numButtonPresses = 0;
bool buttonPressed = false;

void setup()
{
    // Set the serial monitor up for debugging
    Serial.begin(9600);

    // Set the servo up with the right arduino pin
    myServo.attach(servoPin);

    // Set A0 and A1 up for input
    pinMode(sens1, INPUT);
    pinMode(sens2, INPUT);

    // Setup the encoder button for input
    // pinMode(encoderButtonPin, INPUT);
    EncoderInitialize();

    // Write our servo in the setup so servo read will correclty give the last written position
    myServo.write(pos);

    // Start the LCD with 16 rows and 2 columns
    lcd.begin(16, 2);

    // Initalize the timer
    controlTimer = millis();

    // Setup the timer for the clock
    // Set the time to May 1st 2022 at 6:00 AM
    setTime(6,0,0,1,5,2022);

    // Calculate the lightoff values as the average of a few resistance readings
    lightOff = AutoEstimateLightOff();
}

void loop()
{
    // Always check when the button pin has been pressed so we can buffer the input
    // buttonPressed makes sure the count does not go up until we release the button and press again
    if (DebounceButton(encoderButtonPin) && !buttonPressed) 
    {
        numButtonPresses++;
        buttonPressed = true;
    }
    // Only let the count go up again when the user depresses the button
    else if (!DebounceButton(encoderButtonPin) && buttonPressed) 
    {
        buttonPressed = false;
    }

    // Check everything every 0.5 seconds
    if (millis() - controlTimer >= controlVariable)
    {
        // Change clock time based on encoder button and rotation
        HandleHours();
        HandleMinutes();

        // Read in the light levels
        int total = analogRead(sens1);
        int midPoint = analogRead(sens2);

        // Print photoresistor readings to debug light tracking
        Serial.println("================");
        Serial.println("Total----->" + (String)analogRead(sens1));
        Serial.println("Midpoint-->" + (String)analogRead(sens2));
        Serial.println("Lightoff-->" + (String)lightOff);

        // There must be a certain amount of light before rotating
        if (total < lightOff)   
        {
            // Track light based on photoresistor readings
            pos = pos + TrackLight(total, midPoint);
        }
        // If we don't detect a light source, base pos on the time of day
        else pos = GetPositionFromTime();

        // Cap position at 0 and 180 degrees
        if (pos > 180) pos = 180;
        if (pos < 0) pos = 0;
        // Move the servo to the desired position
        myServo.write(pos);

        // Print info to the LCD screen
        lcd.clear();
        PrintPosLCD();
        PrintTimeLCD();

        // Update the last encoder position
        lastEncoder = encoderPosition;

        // Increment the timer by .5 seconds
        controlTimer = controlTimer + 500;

        // Reset buffered button presses to 0
        numButtonPresses = 0;
    }
}

// Function: Automatically calucate the analog reading for the light off resistance on program startup
// Returns: Analog reading for the total resistance at the end of the setup
int AutoEstimateLightOff()
{
    int r1, r2, r3;
    r1 = analogRead(sens1);
    delay(100);
    r2 = analogRead(sens1);
    delay(100);
    r3 = analogRead(sens1);
    return (r1 + r2 + r3) / 3.0;
}

// Function: The encouder button state is bouncing. This causes multiple button presses to register
//           even after the code for button buffering is there
// Returns: The correct button state after the mechancial contacts quit bouncing
bool DebounceButton(bool buttonState)
{
    bool currentState = !digitalRead(encoderButtonPin);
    if(buttonState != currentState)
    {
        unsigned long nowTime = millis();
        while (millis() - nowTime < 10) {}
        currentState = !digitalRead(encoderButtonPin);
    }
    return currentState;
}

// Function: Get the servo position based on time. We use this if the light threshold is too low
// Returns: 0 if before 6 AM or after 6 PM. Otherwise calculate an angle based on the time between 6 AM and 6 PM
int GetPositionFromTime()
{
    // Get time from clock
    int myHour = hour();
    int myMintute = minute();
    bool inPM = isPM();

    // Check if the time is after 6 PM or before 6 AM
    // Hours represented from 0 to 24
    if (myHour >= 18 && inPM && myMintute > 0 || myHour > 18 && inPM || myHour < 6 && !inPM)  return 0;
    
    // Calulate the position from 0 to 180 between the range of 6 AM to 6 PM
    // 12 hour range
    else
    {
        // Do a linear interpolation based on the number of minutes in a 12-hour range
        // to find what our position should be
        int minMinutes = 0;
        int minAngle = 0;
        int maxMinutes = 12*60;
        int maxAngle = 180;

        int currentMin = (hour()*60 + minute()) - 6*60;

        // Calculate position
        return minAngle + (currentMin - minAngle)*((maxAngle - minAngle)/(float)(maxMinutes - minMinutes));
    } 
}

// Function: Increase the hours on the clock every time the ecoder button is pressed
// Parameter (isInputBuffered): true if the user pressed the button in the .5 seconds before we run most of the code
void HandleHours()
{
    setTime(hour() + numButtonPresses, minute(), second(), day(), month(), year());
}

// Function: Adjust the minutes based on how much the encoder has rotated since the last 0.5 seconds
void HandleMinutes()
{
    // Change clock minutes based on the amount the user rotated the encoder
    // Every 4 position changes will equal a minute
    int newMinute = 0;
    bool timeChange = false;

    if (encoderPosition - lastEncoder != 0)
    {
        // The user is trying to increase the time
        newMinute = minute() + (encoderPosition - lastEncoder) / 4;
        timeChange = true;
    }

    // Update the time if we changed it
    int newHour;
    if (timeChange)
    {
        // Change time values so we can go to earlier hours with the minutes
        // TimeLib handles going forward automatically
        if (newMinute < 0)
        {
            newMinute = 59;
            newHour = hour() - 1;
        }
        else
        {
            newHour = hour();
        }
        
        setTime(newHour, newMinute, second(), day(), month(), year());
    }
}

// Function: Print the last written servo position to the LCD
void PrintPosLCD()
{
    // Print the position after writing to the servo
    lcd.setCursor(0, 0);
    lcd.print("Position = " + (String)pos);
}

// Function: Print the current clock time to the LCD
void PrintTimeLCD()
{
    // Set cursor to the second row
    lcd.setCursor(0, 1);

    // Get Hour:Min AM/PM
    String amPM, myHour, myMinute;

    // Use time library functions to pull the current times, then format times correctly
    myMinute = (String)minute();
    if (isPM())
    {
        amPM = "PM";
        myHour = (String)(hour() - 12);
    }
    else
    {
        amPM = "AM";
        myHour = (String)(hour());
    }

    // Format the single digit hour and minutes with a 0 up front
    if (hour() < 10) myHour = "0" + myHour;
    if (minute() < 10) myMinute = "0" + myMinute;

    // Change the hour to 12 if the hour is 00
    if (myHour == "00" || myHour == "0") myHour = "12";       

    // Print the time!
    lcd.print(myHour + ":" + myMinute + " " + amPM);
}

// Function: Rotate the servo based on light readings
// Returns: 3 or -3, this increments the servo based on light readings
int TrackLight(int total, int midPoint)
{
    // Resistance decreases with more light
    // V = IR so voltage also decreases with more light

    // If the total and the tolerance are different enough we check more things
    if (abs(total - midPoint) > lightTolerance)
    {
        // If the total is larger than double the midpoint increase the position
        if (total > 2 * midPoint) return 3;
        // If the total is less than double the midpoint decrease the position
        else return -3;
    }
}