#include "Joystick.h"
#include "Keyboard.h"
//#define JOYSTICK_COUNT 2

/*Joystick_ Joystick[JOYSTICK_COUNT] = {
  Joystick_(0x00, JOYSTICK_TYPE_JOYSTICK,  2, 0,  true, true, false, false, false, false, false, false, false, false, false),
  Joystick_(0x01, JOYSTICK_TYPE_JOYSTICK,  2, 0,  true, true, false, false, false, false, false, false, false, false, false)
};*/

Joystick_ Joystick;
//Joystick_ Joystick2;

int pinA = 2;


void setup() {

  // Set Range Values
  /*for(int i = 0; i < 1; i++)
  {
    Joystick[i].setXAxisRange(0, 1023);
    Joystick[i].setYAxisRange(0, 1023);
    Joystick[i].begin(false);
  }*/
  Joystick.setXAxisRange(0, 1023);
  Joystick.setYAxisRange(0, 1023);
  //Joystick2.setXAxisRange(0, 1023);
  //Joystick2.setYAxisRange(0, 1023);
  
  
  Joystick.begin(false);
  //Joystick2.begin(false);
  
  pinMode(A0, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {


  Joystick.setXAxis(analogRead(A0));
  Joystick.setYAxis(analogRead(A1));
  //Joystick2.setXAxis(analogRead(A0));
  //Joystick2.setYAxis(analogRead(A1));
  Joystick.sendState();
  //Joystick2.sendState();
  Serial.println(analogRead(A0));

  if(digitalRead(pinA) == HIGH)
  {
    Serial.println("asd");
  }
 
  digitalWrite(LED_BUILTIN, 1);

  /*for(int i = 0; i < 1; i++)
  {
    Joystick[i].setXAxis(analogRead(A0));
    Joystick[i].setYAxis(analogRead(A1));
    Joystick[i].sendState();
  }*/
  
}