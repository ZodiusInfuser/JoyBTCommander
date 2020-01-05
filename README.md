# JoyBTCommander
An Arduino library for interfacing with the [Joystick Bluetooth Commander](https://play.google.com/store/apps/details?id=org.projectproto.btjoystick) app on Android . I was using the app to control several Arduino robots I've build, but found it tedious to repeatedly adapt the [example sketch](https://forum.arduino.cc/index.php?topic=173246.msg1766646#msg1766646) example sketch provided by the app's creator to each situation. Looking around I could not see an existing library for this, so decided to create my own.

The library is based on callbacks, allowing you to register functions with the JoyBTCommander for button press, button release, and joystick updates. I also added a communication timeout callback for when if the app is closed / stops transmitting. Any registered callbacks are called as part of checkReceive().

As with the original sketch, up to 3 data fields can be sent back to the app for display. This is achieved by calling sendFields, which is a templated function accepting any serially printable data type that the app supports (ints, floats and strings were tested).

Note, the library currently only supports HardwareSerial classes. I may change this in the future, but most of my microcontrollers have multiple UARTS so this has not been an issue for me so far.
