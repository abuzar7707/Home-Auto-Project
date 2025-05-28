# ESP32 Smart Home Automation System

## 📝 Description
This project implements a smart home automation system using an ESP32 microcontroller. It provides web-based control of various home devices including lights, fans, doors, windows, and a clothes hanger, with additional sensor integration for rain and motion detection.

**⚠️ Note:** This repository is still under active development. Some features may be incomplete or subject to change.

## 🛠️ Features
- **Web-based Control Panel**
  - Control 6 LED lights
  - Fan speed regulation
  - Garage and main door control (servo-actuated)
  - Window open/close functionality
  - Clothes hanger extension/retraction
- **Sensor Integration**
  - Rain detection (digital + analog)
  - Motion sensing (IR sensor)
- **Special Effects**
  - Piano light sequence effect
- **Responsive Web Interface**
  - Works on mobile and desktop
  - Real-time status updates

## 📦 Hardware Requirements
- ESP32 Development Board
- Servo Motors (5x)
- L298N Motor Driver (for fan)
- LEDs (6x)
- IR Motion Sensor
- Rain Sensor Module
- Jumper Wires
- Breadboard
- 5V Power Supply

## 🔌 Pin Connections
| Component       | ESP32 Pin |
|-----------------|-----------|
| LED 1-6         | 2,4,5,12,15,16 |
| Fan IN1         | 18        |
| Fan IN2         | 19        |
| Fan ENA (PWM)   | 21        |
| Garage Door     | 22        |
| Main Door       | 23        |
| Window 1        | 14        |
| Window 2        | 25        |
| Clothes Hanger  | 26        |
| IR Sensor       | 27        |
| Rain Sensor (D) | 13        |
| Rain Sensor (A) | 34        |
| Piano LEDs      | 32,33,35  |

## 🚀 Installation
1. Clone this repository
   ```bash
   git clone https://github.com/yourusername/esp32-smart-home.git

Open the project in Arduino IDE

Install required libraries:

WiFi.h

WebServer.h

ArduinoJson.h

ESP32Servo.h

Modify WiFi credentials in the sketch:

cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
Upload to your ESP32

🌐 Accessing the Control Panel
After uploading, check Serial Monitor for IP address

Connect any device to the same network

Open a browser and navigate to http://[ESP_IP_ADDRESS]

🛠️ Current Development Status
✅ Completed Features
Basic device control (LEDs, fan, doors)

Web interface framework

Sensor integration

🚧 In Progress
Enhanced UI/UX improvements

Advanced automation rules

Mobile app integration

📅 Planned Features

Energy monitoring features

Voice control integration

Scheduled automation

Multi-user access control

Data logging and analytics

🤝 Contributing

Contributions are welcome! Since this project is under active development, please:

Fork the repository

Create your feature branch (git checkout -b feature/AmazingFeature)

Commit your changes (git commit -m 'Add some AmazingFeature')

Push to the branch (git push origin feature/AmazingFeature)

Open a Pull Request

📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

📧 Contact
For questions or suggestions, please contact:
M Abuzar Subhan - [abuzarm7707@gmail.com]
Project Link: https://github.com/abuzar7707/Home-Auto-Project