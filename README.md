# dexcom_wearable
ESP32-based wearable project for aid in blood sugar notifications, created for a friend in need. Based on <a href="https://github.com/gagebenne/pydexcom">pydexcom by Gage Benne</a>, converted to C++ for use on microcontroller units. Anthropic's Claude was used to help convert code between the languages.

## Progress
This project is still a work in progress.

Completed so far is the following:
- Hardware platform choice
- Basic functionality (tested with LED)
- Battery selection and connection to the board

Goals for project completion:
- iOS Companion app (via Bluetooth LE) to set Wi-Fi settings and customize alerts
- Wire a small electronic motor with correct circuitry for alerts
- 3D-printed case that can safely enclose all components, perhaps use the Apple Watch band mechanism(s) or other existing watch bands
- -> Temperature testing to ensure safe usage in different environments
- -> Functionality testing to ensure Wi-Fi and Bluetooth connection holds if obstructed
- -> Battery Life testing

## Project Drive
I have a friend who has both Type 1 Diabetes and hearing loss, and lots of existing commercial solutions rely on audio or have vibration support as an expensive add-on. This project aims to create a cheap DIY solution that has vibration functionality.

## Hardware Design
I chose the XIAO ESP32-C3 platform from <a href="https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/">Seeed Studio.</a> It is a compact ESP32 development board that has built in Wi-Fi 4 (IEEE 802.11 b/g/n) and Bluetooth LE. It also contains circuitry to connect a battery and act as a charger, reducing the amount of additional hardware required.

The battery I am using for initial testing is a generic 500mAh LiPo battery. It is important to note that a minimum of a 370mAh battery is recommended due to the ESP32-C3 having a fixed charge current of 370 mA.

## Usage Notes
To use my code in its current state, create a file named `secrets.h` and define the following secrets:

`SECRET_USER` - Dexcom Share username

`SECRET_PASS` - Dexcom Share password

`WIFI_NAME` - Wifi network to connect to

`WIFI_PASS` - Wifi network password

Eventually these hard-coded values will be replaced with a companion app
