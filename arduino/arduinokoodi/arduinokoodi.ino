#include <Joystick.h>
#include <Keyboard.h>
#include <BetterJoystick.h>

int pinA = 14;                                       //Declaring variables for the pins
int pinB = 15;
int pinC = 16;
int pinD = 17;
int MaxVal = 1023;

Joystick_ Joystick;

// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
const bool testAutoSendMode = true;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
Serial.begin(9600);

  // Set Range Values
  Joystick.setXAxisRange(-127, 127);
  Joystick.setYAxisRange(-127, 127);
  Joystick.setZAxisRange(-127, 127);
  Joystick.setRxAxisRange(0, 360);
  Joystick.setRyAxisRange(360, 0);
  Joystick.setRzAxisRange(0, 720);
  Joystick.setThrottleRange(0, 255);
  Joystick.setRudderRange(255, 0);
  
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }

}

void testXYAxis(unsigned int currentStep)
{
  int xAxis;
  int yAxis;
  
  if (currentStep < 256)
  {
    xAxis = currentStep - 127;
    yAxis = -127;
    Joystick.setXAxis(xAxis);
    Joystick.setYAxis(yAxis);
  } 
  else if (currentStep < 512)
  {
    yAxis = currentStep - 256 - 127;
    Joystick.setYAxis(yAxis);
  }
  else if (currentStep < 768)
  {
    xAxis = 128 - (currentStep - 512);
    Joystick.setXAxis(xAxis);
  }
  else if (currentStep < 1024)
  {
    yAxis = 128 - (currentStep - 768);
    Joystick.setYAxis(yAxis);
  }
  else if (currentStep < 1024 + 128)
  {
    xAxis = currentStep - 1024 - 127;
    Joystick.setXAxis(xAxis);
    Joystick.setYAxis(xAxis);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue0 = analogRead(A0);
  int sensorValue1 = analogRead(A1);
  int sensorValue2 = analogRead(A2);
  int sensorValue3 = analogRead(A3);
  // print out the value you read:
  /*Serial.println("potentio1");
  Serial.println(sensorValue0);
  Serial.println("potentio2");
  Serial.println(sensorValue1);*/
  //Serial.println("nappi1");
  //Serial.println(sensorValue2);
  //Serial.println("nappi2");
  //Serial.println(sensorValue3);
  delay(200);  // delay in between reads for stability

  //potentio mittari 1 LOW ja HIGH signaalit
  if (digitalRead(pinA) == LOW)                     
  {
    //Serial.println("Potentio1 LOW");                            
  }
  if (digitalRead(pinA) == HIGH)                     
  {
    //Serial.println("Potentio1 HIGH");                            
  }
  //potentio mittari 2 LOW ja HIGH signaalit
  if (digitalRead(pinB) == LOW)                     
  {
    //Serial.println("Potentio2 LOW");                            
  }
  if (digitalRead(pinB) == HIGH)                     
  {
    //Serial.println("Potentio2 HIGH");                            
  }
  //nappi1 paino signaali
  if (digitalRead(pinC) == HIGH)                     
  {
    //Serial.println("Nappi1 painettu");                            
  }
  //nappi2 paino signaali
  if (digitalRead(pinD) == HIGH)                     
  {
    //Serial.println("Nappi2 painettu");                            
  }
  if (sensorValue2 >= MaxVal)
  {
    //Serial.println("asd");
    Keyboard.write('w');
  }

  
  if (millis() >= gNextTime)
  {
    if (gCurrentStep < (37 + 256 + 1024 + 128))
    {
      gNextTime = millis() + gcAnalogDelta;
      testXYAxis(gCurrentStep - (37 + 256));
    }
    
    if (testAutoSendMode == false)
    {
      Joystick.sendState();
    }
    
    gCurrentStep++;
    if (gCurrentStep == (37 + 256 + 1024 + 128 + 510 + 28 + 360))
    {
      gNextTime = millis() + gcCycleDelta;
      gCurrentStep = 0;
    }
  }
}