# #SelfieShirt Controller

## What?
This is a subproject of the [#SelfieShirt](https://github.com/syzygy-ffm/selfieshirt). If you landed here by accident you should first checkout the short intro movie on the [accompanying website](http://syzygy.de/selfieshirt).

The controller is the "wearable" part of the project - it connects to a [iOS application](https://github.com/syzygy-ffm/selfieshirt-ios) and is responsible for playing the flash effect.

![Overview](https://raw.githubusercontent.com/syzygy-ffm/selfieshirt/master/Content/Controller-HowItWorks.jpg)

## Overview
A [bluetooth low energy (ble)](http://en.wikipedia.org/wiki/Bluetooth_low_energy) shield is used to allow wireless communication between the controller and the iPhone. The controller periodically polls the ble for any new data. When data has arrived it loops through all received bytes and checks if they represent a valid command identifier and then executes them in order. 
 
![Overview](https://raw.githubusercontent.com/syzygy-ffm/selfieshirt/master/Content/Controller-Overview.jpg)

## What do i needed?
 - 1 x [Xadow Main Board](http://www.seeedstudio.com/depot/Xadow-Main-Board-p-1524.html?cPath=84_120)
 - 1 x [Xadow Breakout](http://www.seeedstudio.com/depot/Xadow-Breakout-p-1519.html?cPath=84_120)
 - 1 x [Xadow BLE Slave](http://www.seeedstudio.com/depot/Xadow-BLE-Slave-p-1546.html?cPath=84_120)

In most cases it is a good idea to [search for a local distributor](http://www.seeedstudio.com/depot/index.php?main_page=distributors) because seeedstudio (the manufacturer) needs some time to ship the stuff from china to your country. Especially the customs may need some time to clear your order.

## Assembly
Just [download our how-to](http://syzygy.de/selfieshirt) and follow the instructions. 

## Installing
First you need to install the [arduino ide](http://arduino.cc/en/Main/Software), then download the [xadow](https://github.com/reeedstudio/xadow) and [adafruit neopixel](https://github.com/adafruit/Adafruit_NeoPixel) libraries and [install them](http://arduino.cc/en/Guide/Libraries) into the arduino libraries folder.

## Developing
The code is well commented so you should be able to tailor it to your needs. If you have never worked with arduino boards go check out the [arduino website](http://arduino.cc/) - they have tutorials and demo code that will help learn the basics.
A good start might be to play with the effect parameters found in `executeCommands()`.

## Testing
While working on your own stuff you should activate debugging output by uncommenting this line :

	#define DEBUG 1 
	
This enables debugging output that you can see via the arduino serial monitor.

### Serial monitor
If you comment the USE_BLE line

	//#define USE_BLE 1

you will be able to send commands to the controller via the serial monitor instead of the ble. Just make sure that the monitor will not send any line endings with your commands (see the selectbox in the lower right corner of the monitor).

![SerialMonitor](https://raw.githubusercontent.com/syzygy-ffm/selfieshirt/master/Content/Controller-SerialMonitor.jpg)

### BLE Utility
When you are ready to test your stuff via the ble you can install a tool named [BLE Utility](https://itunes.apple.com/de/app/ble-utility/id606210918?mt=8) on your iPhone and try to connect to the arduino to send it commands. Just remember to uncomment the USE_BLE define.

	#define USE_BLE 1

Make sure the BLE's blue led is blinking - if that's not the case reset the controller. After some time the BLE Utility should see a "Xadow BLE Slave".

#### Sending a command
Look for a device named Xadow BLE Slave or Seeed_BLE. Next select the Unknown service and then search for a characteristic that has the property "Write". After having selected the characteristic you should be able to send commands in hexadecimal format via the input field. 

![BLE](https://raw.githubusercontent.com/syzygy-ffm/selfieshirt/master/Content/Controller-BLE Utility.jpg)

Just try to input 46 and tap OK. You should see the led do a flash animation. If this is not the case activate DEBUG mode as described above and open the serial monitor to see whats happening.

## Share it!
Build your own Shirt. Create, imitate, improvise, play around. And don’t forget to share: #SelfieShirt  
If you have any questions see [syzygy.de/selfieshirt](http://syzygy.de/selfieshirt) for more details and contact information.
