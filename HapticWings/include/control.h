#include <Arduino.h>

void printCommandInfo(String mode, long *Command)
{
    Serial.println(mode);
    Serial.println("Servo1 angle: " + String(Command[1]) + " Time: " + String(Command[2]));
    Serial.println("Servo2 angle: " + String(Command[3]) + " Time: " + String(Command[4]));
    Serial.println("Stepper1 distance: " + String(Command[5]) + " Acceleration: " + String(Command[6]));
    Serial.println("Stepper2 distance: " + String(Command[7]) + " Acceleration: " + String(Command[8]));
}