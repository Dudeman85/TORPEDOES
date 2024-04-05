#include "Joystick.h" // https://github.com/MHeironimus/ArduinoJoystickLibrary

#define JOYSTICK_COUNT 2

//Construct 2 controllers with X and Y axis enabled and 2 buttons
Joystick_ Joystick[JOYSTICK_COUNT] = 
{
  Joystick_(0x03, JOYSTICK_TYPE_JOYSTICK,  3, 0,  true, true, false, false, false, false, false, false, false, false, false),
  Joystick_(0x04, JOYSTICK_TYPE_JOYSTICK,  2, 0,  true, true, false, false, false, false, false, false, false, false, false)
};

void setup() 
{
//Set the joystick ranges and initialize the joysticks
  for (int i = 0; i < JOYSTICK_COUNT; i++)
  {
    Joystick[i].setXAxisRange(0, 1023);
    Joystick[i].setYAxisRange(0, 1023);
    Joystick[i].begin(false);
  }
  
}

void loop() 
{
  //Read the analog input of the potentiometer and set them as the axes of the joysticks
  Joystick[0].setXAxis(analogRead(A0));
  Joystick[0].setYAxis(analogRead(A1));
  Joystick[1].setXAxis(analogRead(A2));
  Joystick[1].setYAxis(analogRead(A3));
  //Send the joystick states after setting the axes
  Joystick[0].sendState();
  Joystick[1].sendState();

  //Analog tests
  //Serial.println(analogRead(A0));
  //Serial.println(analogRead(A1));
  //Serial.println(analogRead(A2));
  //Serial.println(analogRead(A3));
  //Digital tests
  //Serial.println(digitalRead(2));
  //Serial.println(digitalRead(3));
  //Serial.println(digitalRead(4));
  //Serial.println(digitalRead(5));

  //Read digital pin 2 and press the first joystick button
  if(digitalRead(2) == HIGH)
  {
    //Serial.println("1");
    Joystick[0].pressButton(0);
  }
  else
  {
    Joystick[0].releaseButton(0);
  }
  //Read digital pin 3 and press the second joystick button
  if(digitalRead(3) == HIGH)
  {
    //Serial.println("2");
    Joystick[0].pressButton(1);
  }
  else
  {
    Joystick[0].releaseButton(1);
  }
  //Read digital pin 5 and press the pause joystick button
  if(digitalRead(0) == HIGH)
  {
    //Serial.println("0");
    Joystick[0].pressButton(2);
  }
  else
  {
    Joystick[0].releaseButton(2);
  }

  
  //Read digital pin 4 and press the third joystick button
  if(digitalRead(4) == HIGH)
  {
    //Serial.println("3");
    Joystick[1].pressButton(0);
  }
  else
  {
    Joystick[1].releaseButton(0);
  }
  //Read digital pin 5 and press the fourth joystick button
  if(digitalRead(5) == HIGH)
  {
    //Serial.println("4");
    Joystick[1].pressButton(1);
  }
  else
  {
    Joystick[1].releaseButton(1);
  }
}
