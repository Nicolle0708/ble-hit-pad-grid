# Ble-hit-pad-grid
Bluetooth-based hit detection system for an Iron Palm training table, with an ESP32 sensor array and a browser app for live visualization.

**Features**
- 10 pressure sensors connected to an ESP32
- Sensor calibration and baseline correction
- Smoothed analog readings for more stable detection
- Peak capture during each hit window
- Strongest impact zone detection
- BLE data transmission from ESP32 to app
- Browser-based app for live hit grid display

**Project Structure**
- index.html — web app interface for connecting to the ESP32 and displaying sensor data
- ESP32_Sensor_Code/Pressure_array_ble.ino — main ESP32 BLE firmware
- ESP32_Sensor_Code/Pressure_array_test.ino — sensor testing and calibration code
- Images/ — project photos, wiring images, and app screenshots

**How It Works**
1. The ESP32 reads values from the pressure sensors
2. The system calibrates idle sensor values and subtracts the baseline
3. Readings are smoothed and peak values are captured over a short time window
4. The ESP32 sends sensor data over Bluetooth Low Energy
5. The web app connects to the ESP32 and displays the hit data as a live grid

**Future Improvements**
- Expand to a 5x5 sensor matrix for higher spatial resolution
- Improve mobile support and ensure compatibility with iOS devices
- Implement hit speed estimation
- Integrate dedicated force sensors for more accurate impact measurement
- Optimize pressure sensor sensitivity using calibration, filtering, and threshold tuning
- Improve system accuracy and consistency across all sensing zones
- Develop a native mobile application (iOS/Android)

**Author**
Nicolle Filion Delgado 
Electrical Engineering Student — University of Ottawa  
