Introduction

I implemented a Manual Transmission car. With this project comes everything you need in a Manual Transmission. Those parts being: Throttle, Brake, Clutch, Shifter and a Gauge Cluster. 
These parts allow one to experience the highs and lows of what it is like to drive a real life Manual Transmission Car without having to waste gas in THIS ECONOMY!!

User Guide

In order to turn on the “car” the user must press the clutch(far left button) and then simultaneously press the on/off button(joystick switch). Upon completion of this, the car will then be in Neutral. In order to shift up/down one must press the clutch and then move the joystick up/down in order to go up/down in gears(0-6).  The user must be careful though. If the Revolutions Per Minute(RPM) were to drop down too low the user would stall the car therefore needed a restart. Another case of the car stalling would be if the user would shift too early(rpm is too low). However one can needlessly avoid stalling by paying attention to the RPM value on the gauge cluster or listening to the intensity of the fan(represents the RPM; mathematically reduced to range of fan speed). If one were to stall they could simply restart the engine by pressing the clutch and on/off button again.

There are two other values represented on the Gauge Cluster, the gear and Miles Per Hour. These two values are important in their own respective manner as it's always helpful to know which gear you’re in when you’re driving. The MPH is important because you don’t want to speed and get pulled over. 

Hardware Components Used

Nokia 5110 LCD
DC motor + fan
Joystick
Wires
Buttons
Resistors( of various values)



Software Libraries Used

SPI.H : aids me in communicating with my external parts that are not Arduino standard issued.
