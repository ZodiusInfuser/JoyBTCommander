#ifndef _JOYBTCOMMANDER_h
#define _JOYBTCOMMANDER_h

/***** Includes *****/
#include "Arduino.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////////////////////////
class JoyBTCommander
{
	//--------------------------------------------------
	// Constants
	//--------------------------------------------------
private:
	static const byte STX = 0x02;
	static const byte ETX = 0x03;
	static const byte BUFFER_SIZE = 6;
	static const byte BUTTON_COUNT = 6;


	//--------------------------------------------------
	// Constructors/Destructor
	//--------------------------------------------------
public:
	JoyBTCommander(HardwareSerial& rSerial, unsigned long nTimeoutMillis);


	//--------------------------------------------------
	// Methods
	//--------------------------------------------------
public:
	void begin(uint32_t nBaud);
	void checkReceive(void);

	//--------------------------------------------------

	void sendFields();

	template<typename TYPE1>
	void sendFields(TYPE1 dataField1)
	{
		sendFields(dataField1, "XXX", "XXX");
	}

	template<typename TYPE1, typename TYPE2>
	void sendFields(TYPE1 dataField1, TYPE2 dataField2)
	{
		sendFields(dataField1, dataField2, "XXX");
	}

	template<typename TYPE1, typename TYPE2, typename TYPE3>
	void sendFields(TYPE1 dataField1, TYPE2 dataField2, TYPE3 dataField3)
	{
		// Data frame transmitted back from Arduino to Android device:
		// < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >  
		// < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

		m_rSerial.print((char)STX);                                             // Start of Transmission
		m_rSerial.print(buttonStatesToString());  m_rSerial.print((char)0x1);   // buttons status feedback

		m_rSerial.print(dataField1);		m_rSerial.print((char)0x4);		//datafield #1
		m_rSerial.print(dataField2);		m_rSerial.print((char)0x5);		//datafield #2
		m_rSerial.print(dataField3);										//datafield #3

		m_rSerial.print((char)ETX);                                             // End of Transmission
	}

	//--------------------------------------------------

	bool buttonState(byte nButton) const;
	void setButtonState(byte nButton, bool bPressed);
	void setButtonAsMomentary(byte nButton, bool bIsMomentary);

	float joystickX(void) const;
	float joystickY(void) const;

	//--------------------------------------------------

	void setTimeoutCallback(void(*timeoutCallback)());
	void setButtonPressedCallback(byte nButton, void(*buttonCallback)());
	void setButtonReleasedCallback(byte nButton, void(*buttonCallback)());	
	void setJoystickCallback(void(*joystickCallback)(float, float));

	//--------------------------------------------------
private:
	void decodeButtonState(byte nData);
	void decodeJoystickState(byte(&anDataIn)[BUFFER_SIZE]); 
	
	void handleButtonPress(byte nButton);
	void handleButtonRelease(byte nButton);

	String buttonStatesToString(void) const;


	//--------------------------------------------------
	// Variables
	//--------------------------------------------------
private:
	HardwareSerial& m_rSerial;
	unsigned long m_nNoCommsTimeout;
	unsigned long m_nLastReceivedMillis; 
	void(*m_pNoCommsTimeoutCallback)();
	bool m_bTimeoutReached;
	
	bool m_bReceiving;
	byte m_nDataIndex;
	byte m_anInBuffer[BUFFER_SIZE];

	byte m_nButtonState;
	bool m_abIsMomentary[BUTTON_COUNT];
	void(*m_apButtonPressedCallbacks[BUTTON_COUNT])();
	void(*m_apButtonReleasedCallbacks[BUTTON_COUNT])();

	float m_fJoystickX;
	float m_fJoystickY;
	void(*m_pJoystickCallback)(float, float);
};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////