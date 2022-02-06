# -BBC-Microbit-V2-Reading-and-Connecting-Sensor-
Description: To interface with the BBC Microbit V2 in order to display Data from sensors over the Bluetooth Low Energy(BLE) interface as well as the serial port.

Aims: 
1) To display Data from sensors over the BLE interface as well as the serial port. 
2) If the CO2 level goes above a certain threshold (e.g. 600ppm) the LED matrix should light up with some pattern.


Using:  
        BBC-Microbit-V2
        SENSIRION SCD30 sensor module (CO₂, temperature and humidity sensor)
        BBC Microbit LED Matrix Module
        

Scope of project: 
         Measured CO2,  Humidity, temperature, 
         Serial demonstration, BLE Demonstration
         Described I2C wiring, Explain I2C protocol (Logic analyzer)
         Implemented threshold, Use of LED interface
         Use of UUID's, Described GATT permissions.
         
         
         


Step 1: Install zephyr on your computer using the link -> https://docs.zephyrproject.org/latest/getting_started/index.html

Step 2: Read up on zephyr and how to use it by running the sample code avalible at -> https://docs.zephyrproject.org/latest/samples/index.html#samples-and-demos

Step 3: Read the powerpoint slides 

Step 5: Download the file "main.c", "matrix.c", "matrix.h"

Step 6: Download the zip file "ble_co2" and replace the files main.c, matrix.c, matrix.h inside this, with the downloaded ones from step 5. 

Step 7: Build and run the ble_co2 using zephyr (refer to step 2 on how to do this). 




         
 
 
