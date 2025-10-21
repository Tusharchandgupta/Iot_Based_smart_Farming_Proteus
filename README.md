🌱 IoT Based Smart Farming (Proteus Simulation)
📘 Overview

IoT Based Smart Farming is a simulation project developed in Proteus that demonstrates how the Internet of Things (IoT) can be integrated into agriculture to automate and optimize farm monitoring.
The system monitors essential environmental parameters like soil moisture, temperature, and humidity, and automatically controls irrigation based on the data — ensuring efficient water use and healthy crop growth.

This project is ideal for students, IoT beginners, and embedded system enthusiasts who want to understand the integration of sensors, microcontrollers, and IoT platforms in smart agriculture.

⚙️ Features

🌾 Soil Moisture Monitoring — Measures real-time soil conditions.

🌡 Temperature and Humidity Detection — Uses sensors to track environment changes.

💧 Automatic Irrigation Control — Turns water pump ON/OFF depending on soil dryness.

☁️ IoT Connectivity (optional) — Sends real-time data to an IoT dashboard or ThingsBoard.

💡 Proteus Simulation — Visual demonstration of working sensors and actuators.

🧠 Components Used
🪛 Hardware (Simulated in Proteus)

Microcontroller: Arduino Uno / NodeMCU (as per setup)

Soil Moisture Sensor

DHT11 / DHT22 Sensor (for temperature & humidity)

Water Pump (DC Motor)

Relay Module

16x2 LCD Display (for local data display)

LED Indicators (for status visualization)

🖥 Software & Tools

Proteus Design Suite (for circuit simulation)

Arduino IDE (for coding the microcontroller)

IoT Platform (optional): ThingsBoard / Blynk / ThingSpeak

🧩 Working Principle

The Soil Moisture Sensor continuously measures the soil's moisture level.

The DHT Sensor monitors the surrounding temperature and humidity.

When soil moisture drops below a threshold, the relay activates the pump, watering the plants.

Data readings are displayed on the LCD screen and optionally sent to an IoT cloud dashboard.

Once the soil is sufficiently moist, the pump automatically turns OFF.

🪜 Steps to Run the Simulation

Open Proteus Design Suite.

Load the provided IoT_Based_Smart_Farming.pdsprj file.

Upload the corresponding Arduino code (.ino) into the virtual Arduino module.

Run the simulation — observe sensor readings, relay activation, and pump control.

(Optional) Connect to an IoT platform to visualize real-time data.
