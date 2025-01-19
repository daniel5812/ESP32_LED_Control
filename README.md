# ESP32 LED Control via HTTP Server

## Description
This project demonstrates how to use an ESP32 as a WiFi-enabled HTTP server to control an LED. The system includes:
- A web-based chat interface.
- Real-time LED control based on user commands.
- Integration with an NTP server to display timestamps.

## Features
- **WiFi Connectivity:** The ESP32 connects to a WiFi network and hosts an HTTP server.
- **Real-Time LED Control:** Users can send commands to turn the LED ON, OFF, or trigger blinking patterns.
- **Chat Interface:** A simple web interface to send messages and control the LED.
- **Timestamping:** Messages include the exact time they were sent, synchronized with an NTP server.

## Project Demo
### LED in Action
![LED in Action](images/led_action.gif)

## Hardware Requirements
- ESP32 Development Board
- 1 LED (connected to GPIO 22)
- Resistor (220Ω recommended)
- Breadboard and Jumper Wires
- WiFi Network

## Setup Instructions
1. **Connect the Hardware:**
   - Use the diagram below to connect the LED to the ESP32.

2. **Upload the Code:**
- Open the Arduino IDE.
- Update the `ssid` and `password` variables with your WiFi credentials.
- Upload the code to the ESP32.

3. **Access the Web Interface:**
- Open the Serial Monitor to get the ESP32's IP address.
- Enter the IP address into a browser.

4. **Send Commands:**
- Use the web interface to control the LED:
  - `ON`: Turns the LED on.
  - `OFF`: Turns the LED off.
  - `BLINK`: Makes the LED blink.

## Future Enhancements
- Add support for RGB LEDs.
- Implement user authentication.
- Save messages to non-volatile memory.

## Author
- **Name:** Daniel Dahan
- **LinkedIn:** [Daniel Dahan](https://www.linkedin.com/in/daniel-dahan-73a960285/)
- **GitHub:** [Daniel5812](https://github.com/daniel5812)

