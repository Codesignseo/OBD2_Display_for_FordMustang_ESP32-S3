# OBD2 Display for Ford Mustang ESP32-S3 🚗💨

![GitHub release](https://img.shields.io/github/release/Codesignseo/OBD2_Display_for_FordMustang_ESP32-S3.svg)
[![Download Releases](https://img.shields.io/badge/Download%20Releases-Here-blue)](https://github.com/Codesignseo/OBD2_Display_for_FordMustang_ESP32-S3/releases)

Welcome to the OBD2 Display for Ford Mustang project! This Arduino-based project provides a sleek interface for displaying OBD2 data on a round LCD display. It is specifically designed for the 2016 Ford Mustang Ecoboost, offering real-time information about the currently selected gear and a simple gear shift indicator.

## Table of Contents

1. [Introduction](#introduction)
2. [Features](#features)
3. [Getting Started](#getting-started)
   - [Hardware Requirements](#hardware-requirements)
   - [Software Requirements](#software-requirements)
   - [Installation Instructions](#installation-instructions)
4. [Usage](#usage)
5. [Wiring Diagram](#wiring-diagram)
6. [Contributing](#contributing)
7. [License](#license)
8. [Contact](#contact)

## Introduction

The OBD2 Display for Ford Mustang project aims to enhance your driving experience by providing essential vehicle data at a glance. With this project, you can monitor your gear selection and RPM in real-time, ensuring you stay informed while on the road.

## Features

- **Real-time Gear Display**: See your current gear selection directly on the LCD.
- **Gear Shift Indicator**: Get a visual cue for when to shift gears.
- **Compact Design**: Fits seamlessly in your vehicle without cluttering the dashboard.
- **Easy Installation**: Simple wiring and setup make it accessible for all skill levels.

## Getting Started

To get started with the OBD2 Display for Ford Mustang project, follow these steps.

### Hardware Requirements

- **ESP32-S3**: The main microcontroller for this project.
- **Round LCD Display (GC9A01)**: For displaying the gear and RPM information.
- **OBD2 Adapter (SN65HVD230)**: To connect to the vehicle's OBD2 port.
- **MOSFET**: For controlling power to the display.
- **Wires and Connectors**: For making connections between components.
- **XIAO ESP32-S3**: A compact version of the ESP32-S3 for space-saving.

### Software Requirements

- **Arduino IDE**: Download and install the latest version of the Arduino IDE from the [official website](https://www.arduino.cc/en/software).
- **ESP32 Board Support**: Install the ESP32 board support in the Arduino IDE.
- **Libraries**: You will need the following libraries:
  - `OBD2 Library`
  - `GC9A01 Library`
  
### Installation Instructions

1. **Clone the Repository**: 
   Open your terminal and run:
   ```
   git clone https://github.com/Codesignseo/OBD2_Display_for_FordMustang_ESP32-S3.git
   ```
2. **Open the Project in Arduino IDE**: Navigate to the cloned folder and open the `.ino` file in the Arduino IDE.
3. **Install Required Libraries**: Go to `Sketch` > `Include Library` > `Manage Libraries`, and search for the required libraries mentioned above. Install them.
4. **Connect Your ESP32-S3**: Use a USB cable to connect your ESP32-S3 to your computer.
5. **Select the Correct Board**: In the Arduino IDE, go to `Tools` > `Board` and select `ESP32-S3`.
6. **Upload the Code**: Click on the upload button in the Arduino IDE to upload the code to your ESP32-S3.
7. **Download the Releases**: For the latest version of the software, visit the [Releases section](https://github.com/Codesignseo/OBD2_Display_for_FordMustang_ESP32-S3/releases) to download and execute the latest build.

## Usage

Once the setup is complete, you can start using the OBD2 Display. The LCD will automatically show the current gear and RPM as you drive. Adjust the display settings as needed through the Arduino IDE.

## Wiring Diagram

Here is a simple wiring diagram to help you connect the components:

```
ESP32-S3       Round LCD (GC9A01)
-----------------------------------
  GND  ------------------  GND
  3.3V ------------------  VCC
  GPIO23  --------------  SCL
  GPIO22  --------------  SDA

ESP32-S3       OBD2 Adapter (SN65HVD230)
------------------------------------------
  GND  ------------------  GND
  TX   ------------------  RX
  RX   ------------------  TX
```

## Contributing

We welcome contributions to this project. If you would like to contribute, please fork the repository and submit a pull request. Make sure to follow the coding standards and include tests for any new features.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contact

For any questions or feedback, please reach out via the GitHub issues page or contact the repository owner directly.

Thank you for your interest in the OBD2 Display for Ford Mustang project! For the latest updates and releases, be sure to check the [Releases section](https://github.com/Codesignseo/OBD2_Display_for_FordMustang_ESP32-S3/releases).