#include "JoyBTCommander.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS/DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////
JoyBTCommander::JoyBTCommander(HardwareSerial& rSerial, unsigned long nTimeoutMillis)
: m_rSerial(rSerial)
, m_nNoCommsTimeout(nTimeoutMillis)
, m_nLastReceivedMillis(0)
, m_pNoCommsTimeoutCallback(nullptr)
, m_bTimeoutReached(true) //Set as true initially so timeout callback does not get called

, m_bReceiving(false)
, m_nDataIndex(0)
, m_nButtonState(B000000)

, m_fJoystickX(0.0f)
, m_fJoystickY(0.0f)
, m_pJoystickCallback(nullptr)
{
	for(byte nButton = 0; nButton < BUTTON_COUNT; nButton++)
	{
		m_abIsMomentary[nButton] = false;
		m_apButtonPressedCallbacks[nButton] = nullptr;
		m_apButtonReleasedCallbacks[nButton] = nullptr;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// METHODS
////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::begin(uint32_t nBaud)
{
	m_rSerial.begin(nBaud);

	while(m_rSerial.available())
		m_rSerial.read(); //empty RX buffer
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::checkReceive(void)
{
	//m_bNewJoystickState = false;		

	while(m_rSerial.available() > 0)
	{
		byte nDataIn = (byte)m_rSerial.read();
		if(!m_bReceiving)
		{
			if(nDataIn == STX)
			{
				m_bReceiving = true;
				m_nDataIndex = 0;
			}
		}
		else
		{
			if(nDataIn > 127 || m_nDataIndex > BUFFER_SIZE)
			{
				m_bReceiving = false; //Communication error
			}
			else if(nDataIn == ETX)
			{
				if(m_nDataIndex == 1)
					decodeButtonState(m_anInBuffer[0]);    // 3 Bytes  ex: < STX "C" ETX >
				else if(m_nDataIndex == 6)
					decodeJoystickState(m_anInBuffer);     // 6 Bytes  ex: < STX "200" "180" ETX >

				m_nLastReceivedMillis = millis();
				m_bTimeoutReached = false;

				m_bReceiving = false;
			}
			else
			{
				m_anInBuffer[m_nDataIndex] = nDataIn;
				m_nDataIndex++;
			}
		}
	}

	unsigned long nCurrentMillis = millis();
	if((nCurrentMillis - m_nLastReceivedMillis > m_nNoCommsTimeout) && !m_bTimeoutReached)
	{
		if(m_pNoCommsTimeoutCallback != nullptr)
			m_pNoCommsTimeoutCallback();

		m_bTimeoutReached = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::sendFields()
{
	sendFields("XXX", "XXX", "XXX");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool JoyBTCommander::buttonState(byte nButton) const
{
	return m_nButtonState & (1 << nButton);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setButtonState(byte nButton, bool bPressed)
{
	if(bPressed)
		handleButtonPress(nButton);
	else
		handleButtonRelease(nButton);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setButtonAsMomentary(byte nButton, bool bIsMomentary)
{
	if(nButton < BUTTON_COUNT)
		m_abIsMomentary[nButton] = bIsMomentary;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
float JoyBTCommander::joystickX(void) const
{
	return m_fJoystickX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
float JoyBTCommander::joystickY(void) const
{
	return m_fJoystickY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setButtonPressedCallback(byte nButton, void(*buttonCallback)())
{
	if(nButton < BUTTON_COUNT)
		m_apButtonPressedCallbacks[nButton] = buttonCallback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setButtonReleasedCallback(byte nButton, void(*buttonCallback)())
{
	if(nButton < BUTTON_COUNT)
		m_apButtonReleasedCallbacks[nButton] = buttonCallback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setTimeoutCallback(void(*timeoutCallback)())
{
	m_pNoCommsTimeoutCallback = timeoutCallback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::setJoystickCallback(void(*joystickCallback)(float, float))
{
	m_pJoystickCallback = joystickCallback;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::decodeButtonState(byte nData)
{
	switch(nData)
	{
		// -----------------  BUTTON #1  -----------------------
	case 'A':
		handleButtonPress(0);
		break;
	case 'B':
		handleButtonRelease(0);
		break;

		// -----------------  BUTTON #2  -----------------------
	case 'C':
		handleButtonPress(1);
		break;
	case 'D':
		handleButtonRelease(1);
		break;

		// -----------------  BUTTON #3  -----------------------
	case 'E':
		handleButtonPress(2);
		break;
	case 'F':
		handleButtonRelease(2);
		break;

		// -----------------  BUTTON #4  -----------------------
	case 'G':
		handleButtonPress(3);
		break;
	case 'H':
		handleButtonRelease(3);
		break;

		// -----------------  BUTTON #5  -----------------------
	case 'I':
		handleButtonPress(4);
		break;
	case 'J':
		handleButtonRelease(4);
		break;

		// -----------------  BUTTON #6  -----------------------
	case 'K':
		handleButtonPress(5);
		break;
	case 'L':
		handleButtonRelease(5);
		break;
	}
	// ---------------------------------------------------------------
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::decodeJoystickState(byte(&anDataIn)[BUFFER_SIZE])
{
	int joyX = (anDataIn[0] - 48) * 100 + (anDataIn[1] - 48) * 10 + (anDataIn[2] - 48);       // obtain the Int from the ASCII representation
	int joyY = (anDataIn[3] - 48) * 100 + (anDataIn[4] - 48) * 10 + (anDataIn[5] - 48);
	joyX = joyX - 200;                                                  // Offset to avoid
	joyY = joyY - 200;                                                  // transmitting negative numbers

	if(joyX < -100 || joyX > 100 || joyY < -100 || joyY > 100)
		return;      // commmunication error

	m_fJoystickX = (float)joyX / 100.0f;
	m_fJoystickY = (float)joyY / 100.0f;

	if(m_pJoystickCallback != nullptr)
		m_pJoystickCallback(m_fJoystickX, m_fJoystickY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::handleButtonPress(byte nButton)
{
	if(!m_abIsMomentary[nButton])	//Only set the button state to pressed if the button is not momentary
		m_nButtonState |= (1 << nButton);        // ON

	if(m_apButtonPressedCallbacks[nButton] != nullptr)
		m_apButtonPressedCallbacks[nButton]();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void JoyBTCommander::handleButtonRelease(byte nButton)
{
	m_nButtonState &= ~(1 << nButton);		// OFF
	if(m_apButtonReleasedCallbacks[nButton] != nullptr)
		m_apButtonReleasedCallbacks[nButton]();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
String JoyBTCommander::buttonStatesToString(void) const
{
	String sState = "";
	for(byte i = 0; i < BUTTON_COUNT; i++)
	{
		if(m_nButtonState & (B100000 >> i))
			sState += "1";
		else
			sState += "0";
	}
	return sState;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////