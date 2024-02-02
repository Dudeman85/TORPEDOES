#include "Joystick.h"
#include "Keyboard.h"

#define JOYSTICK_COUNT 2

Joystick_ Joystick[JOYSTICK_COUNT] = {
  Joystick_(0x03, JOYSTICK_TYPE_JOYSTICK,  0, 0,  true, true, false, false, false, false, false, false, false, false, false),
  Joystick_(0x04, JOYSTICK_TYPE_JOYSTICK,  0, 0,  true, true, false, false, false, false, false, false, false, false, false)
};

//Joystick_ Joystick;
//Joystick_ Joystick2;

int pinA = 2;
int nappi1 = analogRead(A2);

void setup() {

  for (int i = 0; i < JOYSTICK_COUNT; i++)
  {
    Joystick[i].setXAxisRange(0, 1023);
    Joystick[i].setYAxisRange(0, 1023);
    Joystick[i].begin(false);
  }

  // Set Range Values
  /*for(int i = 0; i < 1; i++)
  {
    Joystick[i].setXAxisRange(0, 1023);
    Joystick[i].setYAxisRange(0, 1023);
    Joystick[i].begin(false);
  }*/
  //Joystick.setXAxisRange(0, 1023);
  //Joystick.setYAxisRange(0, 1023);
  //Joystick[1].setXAxisRange(0, 1023);
  //Joystick[2].setYAxisRange(0, 1023);
  //Joystick2.setXAxisRange(0, 1023);
  //Joystick2.setYAxisRange(0, 1023);
  
  //Joystick[1].begin(false);
  //Joystick[2].begin(false);
  //Joystick2.begin(false);
  
  pinMode(A0, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  digitalWrite(LED_BUILTIN, 1);
  /*for (int i = 0; i < JOYSTICK_COUNT; i++)
  {
    Joystick[i].setXAxis(analogRead(A0));
    Joystick[i].setYAxis(analogRead(A1));
  }*/
  Joystick[0].setXAxis(analogRead(A0));
  Joystick[0].setYAxis(analogRead(A1));
  Joystick[1].setXAxis(analogRead(A2));
  Joystick[1].setYAxis(analogRead(A3));
  /*Joystick[1].setXAxis(analogRead(A0));
  Joystick[1].setYAxis(analogRead(A1));
  Joystick[2].setXAxis(analogRead(A3));
  Joystick[2].setYAxis(analogRead(A4));
  Joystick[1].sendState();
  Joystick[2].sendState();*/
  Joystick[0].sendState();
  Joystick[1].sendState();
  Serial.println(analogRead(A0));
  Serial.println(analogRead(A1));
  Serial.println(analogRead(A2));
  Serial.println(analogRead(A3));
  //delay(10);
  //Serial.println(digitalRead(2));
  //Serial.println(digitalRead(3));
  //Serial.println(digitalRead(4));
  //Serial.println(digitalRead(5));

  if(digitalRead(2) == HIGH)
  {
    //Serial.println("1");
  }
 if(digitalRead(3) == HIGH)
  {
    //Serial.println("2");
  }
  if(digitalRead(4) == HIGH)
  {
    //Serial.println("3");
  }
  if(digitalRead(5) == HIGH)
  {
    //Serial.println("4");
  }


  

  /*for(int i = 0; i < 1; i++)
  {
    Joystick[i].setXAxis(analogRead(A0));
    Joystick[i].setYAxis(analogRead(A1));
    Joystick[i].sendState();
  }*/
  
}