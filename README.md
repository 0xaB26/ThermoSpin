## Temperature-Controlled DC Motor System
### This project demonstrates a temperature-controlled DC motor system using :
- PIC18F452 microcontroller. 
- LM35 temperature sensos.
- DC motor.
- LM016 LCD for displaying temperature and motor speed.

The motor's speed is adjusted based on the temperature measured by the LM35 sensor.

### Project Components
Microcontroller: PIC18F452
Compiler: C18
Simulator : Proteus
Temperature Sensor: LM35
Motor Type: DC Motor
LCD Display: LM016 (16x2)

#### Project Description
This system monitors the ambient temperature using the LM35 temperature sensor. Based on the measured temperature, it controls the speed of a DC motor as follows:

1)-Temperature Range 40°C < T ≤ 150°C:
The DC motor runs at high speed.

2)-Temperature Range 25°C < T ≤ 40°C:
The DC motor runs at medium speed.

3)-Temperature ≤ 25°C:
The DC motor is turned off.

#### NOTE :
This project is for learning and haven't yet implemented in real hardware but it works and give good results in proteus simulator(check vedio).
