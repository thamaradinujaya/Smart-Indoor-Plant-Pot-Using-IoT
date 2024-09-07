Smart Indoor Plant Pot
Project Overview
This project aims to solve common issues in indoor gardening by creating a Smart Indoor Plant Pot that automates watering, light control, and humidity regulation using IoT technology. The system integrates several sensors and components, including soil moisture, light, and humidity sensors, to provide an optimized environment for plant growth.
Features
•	Automated Watering System: Uses soil moisture sensors to monitor and regulate water levels based on the plant's needs.
•	Automated Lighting System: Blue and red LED lights simulate natural sunlight, promoting photosynthesis and healthy growth.
•	Automated Humidity Control: Regulates humidity to ensure plants have the ideal air moisture content for optimal growth.
•	Blynk App Integration: Real-time monitoring and control via a mobile app for remote plant care.
Components
•	ESP32 WROOM DA Module
•	Soil Moisture Sensor
•	DHT11 Temperature and Humidity Sensor
•	5V Relay Module
•	0.96-inch OLED Display
•	Ultrasonic Mist Maker
•	DS1302 RTC Real-Time Clock Module
•	LDR (Light Dependent Resistor)
•	5V DC Motor
System Flow
1.	Watering: The soil moisture sensor activates the water pump if moisture levels drop below a set threshold.
2.	Lighting: The light sensor triggers the LEDs based on ambient light intensity.
3.	Humidity Control: The DHT11 sensor activates the mist maker when the humidity level falls below a certain point.
4.	Monitoring: Sensor data is displayed on the OLED and can be monitored through the Blynk app.
Future Work
•	Incorporate machine learning algorithms for more intelligent water and light control based on plant needs.
•	Use image processing to detect plant diseases.

