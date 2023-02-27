# Arduino IDE Setup

The following libraries are required to be installed with the [Arduino IDE 2.x](https://www.arduino.cc/en/software). All but the WavTrigger and MillisDelay library can be found within the Arduino Library Manager with the app. Go to `Sketch -> Include Library -> Manage Libraries...` to access the Library Manager. Search for the libraries by name and install the latest version available.

- **FastLED** by Daniel Garcia
- **ezButton** by ArduinoGetStarted.com
- **Ramp** by Sylvain Garnavault
- **AltSoftSerial** by Paul Stoffregen
- **millisDelay** `See Below`
- **WavTrigger** `See Below`

### MillisDelay

The MillisDelay library must be downloaded from the project GitHub page. Download the code as a zip use the `Sketch -> Add .ZIP Library` option to import the downloaded file.
[https://github.com/ansonhe97/millisDelay](https://github.com/ansonhe97/millisDelay)

No further configuration is needed for this library.

### WavTrigger

The WavTrigger library must be downloaded from the project GitHub page. Download the code as a zip use the `Sketch -> Add .ZIP Library` option to import the downloaded file.
[https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library](https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library)

Documented on both the Proton Pack and Neutron Wand code at the top of each file, is instructions on modifying the wavTrigger.h for each application. The wavTrigger.h file can be located in your `Arduino/Libraries/<wav trigger folder>` (MacOS) or `C:\Arduino\Libraries` (Windows).

![WavTrigger Serial Class for Arduino Mega](images/wt_serial_mega.png)

**For compiling code on the Arduino Nano:**

You need to enable `__WT_USE_ALTSOFTSERIAL__` by uncommenting this line. Be sure to comment out any other serial class option previously in use. Note that you will need to confirm this value if you switch between compiling for the Arduino Mega.

![WavTrigger Serial Class for Arduino Nano](images/wt_serial_nano.png)