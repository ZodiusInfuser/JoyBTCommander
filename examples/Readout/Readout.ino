#include <JoyBTCommander.h>

#define    ledPin       13

JoyBTCommander btController(Serial1, 1000);

long previousMillis = 0;
long sendInterval = 500;
bool ledState = false;

//Data to send back to the Android device
int intField = 0;
float floatField = 50.0f;
String textField = "Hello";



////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(void)
{
  Serial.begin(57600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  btController.setButtonPressedCallback(0, Button0Pressed);
  btController.setButtonReleasedCallback(0, Button0Released);

  btController.setButtonPressedCallback(1, Button1Pressed);
  btController.setButtonReleasedCallback(1, Button1Released);

  btController.setButtonAsMomentary(2, true);
  btController.setButtonPressedCallback(2, LEDToggle);  

  btController.setJoystickCallback(JoystickUpdated);
  btController.setTimeoutCallback(ControllerLost);

  btController.setButtonState(0, true); //Start with button 0 pressed

  btController.begin(57600);  
}
                       

   
////////////////////////////////////////////////////////////////////////////////////////////////////
// LOOP
////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  btController.checkReceive();

  long currentMillis = millis();
  if(currentMillis - previousMillis > sendInterval)
  {
    previousMillis = currentMillis;

    intField++;
    
    floatField -= 0.5f;
    if(floatField < - 50)
      floatField = 50;      

    //Send data back to smartphone
    btController.sendFields(intField, floatField, textField);
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////
void Button0Pressed(void)
{
  Serial.println("Button 0 Pressed");
  digitalWrite(ledPin, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Button0Released(void)
{
  Serial.println("Button 0 Released");
  digitalWrite(ledPin, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Button1Pressed(void)
{
  Serial.println("Button 1 Pressed");
  digitalWrite(ledPin, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Button1Released(void)
{
  Serial.println("Button 1 Released");
  digitalWrite(ledPin, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void LEDToggle(void)
{
  Serial.println("LED Toggled");
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoystickUpdated(float joyX, float joyY)
{
  Serial.print("Joystick position:  ");
  Serial.print(joyX);  
  Serial.print(", ");  
  Serial.println(joyY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ControllerLost(void)
{
  Serial.println("Controller Input Lost");
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////