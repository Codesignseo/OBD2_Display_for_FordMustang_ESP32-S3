# OBD2 Display for Ford Mustang using ESP32-S3
 
An Arduino project for a 2016 Ford Mustang Ecoboost that uses OBD2 data and displays the currently selected gear and a simple gear shift indicator on a round LCD display.

NOTE: It's recommended to disconnect the device if the car won't be driven for 4 or 5 days.

Car data like RPM and current gear can be retrieved from the car's high speed CAN bus via the OBD2 port. The ESP32-S3 has a built-in CAN controller, so we connect a SN65HVD230 CAN bus transceiver to the ESP32-S3 which communicates on the high speed bus. Displaying something on an LCD is a blocking operation, i.e. while the LCD is updating, you can't collect the latest car data. Luckily the ESP32-S3 has two cores, therefore we use one core to continuously read CAN frames with car data while updating the display on the other core.

The OBD2 connector has an always-on 12V pin where the device will be powered from. Since it's always on, it will unnecessarily draw power when the car is turned off. One option would be to just always unplug the device when not driving, another might be to add a button to the device to manually switch it on/off. In this project, we simply detect if the car is turned on and if not, we put the device into a lite sleep mode by reducing the processor clock speed to 80Mhz and switching off the display using a mosfet. For some reason switching in and out of actual deep sleep mode drains the car's battery, it's probably waking up some electronic components in the car? When we detect that the car is on, the processor is switched back to 240Hz and the dispay is switched on again.

Below is some power information when the device is connected together with a BAMA ECU tuner logger device, which also draws power and monitors when the car switches on.

| Microcontroller Setup                 | Power         | Notes  | 
| --------------------------------------|---------------|--------| 
| 240MHz with the display on | 100mA @ 5.12V |  |
| 80Mhz with display off     | 30mA @ 5.12V  | Drains car battery 0.01V per hour       | 
| 40Mhz with display off     | Less than 10mA @ 5.12V | Drains car battery 0.08V per hour | 
| Deepsleep with display off            | ~0mA @ 5.12V | Drains car battery 0.18V per hour | 

NOTE: If you intend to experiment and make your own code changes, I recommend enabling the C++ define for DISABLE_POWER_SAVING found in the file OBD2_Display_for_FordMustang_ESP32-S3.ino. If not, the device will go into deep sleep and you might struggle to upload code to the device, since you have to time it just right. When the device is powered on by plugging it into the computer's USB port, you have 5 seconds before it goes into deep sleep. You want to have the device awake while the code is being uploaded. When using the Arduino IDE, I find that if you unplug the device from your computer's USB port, then wait for the message in the Output window that says "Linking everything together...", and then quickly plug the device into your computer's USB port, it gives enough time for the device to stay awake to upload the code.

NOTE: It's fun to tinker with your car, but there is always a chance to mess things up. I won't be liable if for some reason you damage your car.

NOTE: The CAN IDs used in this project specifically work with a 2016 Ford Mustang Ecoboost. The same IDs might not work on other models, you'll have to research what PIDs work with your own car.

Some tips:
 - Diagrams of OBD2 pins are normally shown for the female connector in your car. Don't forget that those pins are in swapped/mirrored positions on the male connector.
 - The OBD2 connector has an "always on" 12V pin. Make sure the wire connecting to that pin on your male connector isn't exposed so that it cannot touch other wires!
 - I tried multiple pins on the ESP32-S3 to connect to the SN65HVD230, but only D4/D5 worked for me. Coincidentally these are also the SDA/SCL pins.
 - For 3D printing, use something like PETG or TPU which is much more resistant to heat than PLA.

Hardware:
 - XIAO ESP32-S3
 - 12V to 5V Voltage regulator
 - SN65HVD230 CAN bus transceiver
 - GC9A01 LCD display
 - N-Channel Mosfet (16A 60V TO-220)
 - 220 Ohm Resistor
 - 2.5mm TRRS Audio Connector (4 pole)
 - OBD2 Connector
 - USB Data Cable
 - HUD Projective Film

Arduino libraries used:
 - ESP32-TWAI-CAN: https://github.com/handmade0octopus/ESP32-TWAI-CAN
 - ArduinoGFX: https://github.com/moononournation/Arduino_GFX

If you are unfamiliar with Arduino, it’s pretty simple and there is a multitude of small tutorials on the internet. At a high level, you will need to:
-	Install the Arduino IDE - https://www.arduino.cc/en/software
-	Configure Arduino IDE for ESP32 - https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/
-	Install the two libraries mentioned above - https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/
-	Try out the most simple ESP32-S3 app to switch on its onboard LED - https://youtu.be/JlnV3U3Rx7k?si=xF3rMG3LD-cYAaQe

--------------------------------

Example: Device is placed on dashboard near windshield. By applying a projective film on the windshield, the LCD projects onto the windshield as a transparent holographic effect. You can also simply face the LCD display directly towards the driver if the holographic effect isn't bright enough. If so, comment out the C++ define MIRROR_TEXT_FOR_HOLOGRAPHIC_REFLECTION in the file OBD2_Display_for_FordMustang_ESP32-S3.ino, which is responsible for mirroring the fonts to be used as a reflection.

![Demo](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Demo_240p.gif?raw=true)

![Demo](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/DashboardMount.jpg?raw=true)
--------------------------------

A small design, coming in at 73mm x 48mm x 12mm

![Design1](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design1.jpg?raw=true)

--------------------------------

OBD2 wires attatch at the back of the device using a convenient audio connector. A TRRS 4-pole connector has 4 wires, which is exactly what we need, two for power and two for CAN bus communication.

![Design2](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design2.jpg?raw=true)

--------------------------------

Here's what's inside!

![Design3](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design3.jpg?raw=true)

--------------------------------

![Design4](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design4.jpg?raw=true)

--------------------------------

I highly recommend using more expensive thin 28 AWG flexible silicone wires, since the space is limited between the components. It would of course be easier to use a custom PCB.

![Design5](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design5.jpg?raw=true)

--------------------------------

You will need about 1.5m of wiring to reach from the car's OBD2 connector up to the windshield of the car. Only four OBD2 wires are required to reach the device, two for power and two for the high speed CAN bus communication. I used three different cables for this. First, an OBD2 splitter so that another device, e.g. a tuner device, can be attached while this device is used. Then I soldered a long USB data cable to the OBD2 connector cable. Make sure that you specifically buy a data cable and not a charging cable, since a charging cable won’t have extra data wires. From there, I then soldered a short piece of TRRS 4-pole audio connector cable to reach the device.

![Design6](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Design6.jpg?raw=true)

--------------------------------

This isn't required, but I wanted to make the footprint of the device smaller, so I cut off the wire connector on the SN65HVD230

![CutSmaller](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/CutSmaller.jpg?raw=true)

--------------------------------

Each component fits neatly in the base of the 3D print, but it's useful to use a smudge of silicone sealant to keep them in place. The lid fits on top of the base, and that will simply slide into the dashboard mount. I used a small piece of double-sided tape to stick the mount onto the dashboard. For 3D printing, I used PETG which is more durable than PLA.

![3DPrints](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/3DPrints.jpg?raw=true)

--------------------------------

![Components](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/Components.jpg?raw=true)

--------------------------------

NOTE: I opened up two different OBD2 cables and both had the same color coded thin wires, e.g. the green wire is connected to pin 6. But, I highly recommend to check if your wires are color coded the same way using a multimeter. This video might be helpful: https://youtu.be/-5xS5c7USjE?si=0HzdZcqJ7PLiXYwY

![OBD2_Pins](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/OBD2_Pins.jpg?raw=true)

--------------------------------

![PowerDiagram](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/PowerDiagram.jpg?raw=true)

--------------------------------

![SN65HVD230](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/SN65HVD230.jpg?raw=true)

--------------------------------

![GC9A01](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/GC9A01.jpg?raw=true)

--------------------------------

![FullWireDiagram](https://github.com/ClaudeMarais/OBD2_Display_FordMustang_ESP32-S3/blob/main/Images/FullWireDiagram.jpg?raw=true)

--------------------------------