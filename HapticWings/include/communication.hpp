#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <ArduinoJson.h>

// 500为舵机中间位置，时间目前设置为500-1000ms均可； 步进电机的距离数值，其加速度以在前面调整过
// Example {"Servo": {"Angle": 500, "Time": 1000}, "Stepper": {"Distance": 500, "Acceleration": true}}

String buff = "";

const size_t capacity = JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(2); // 修改为适当的 JSON 容量，以适应舵机和步进电机的信息
StaticJsonDocument<capacity> jb;

long *SerialReceive()
{
  static long Command[5]; // 数组用于存储舵机的角度和时间，以及步进电机的移动距离和加速度标志位
  if (Serial.available())
  {
    char inChar;
    while (inChar != '\n')
    {
      while (!Serial.available())
        ;
      inChar = (char)Serial.read();
      if (inChar == '\n')
      {
        Serial.println(buff);
        DeserializationError err = deserializeJson(jb, buff);
        if (err)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(err.c_str());
        }

        // 解析舵机的控制信息
        JsonObject servoData = jb["Servo"];
        if (!servoData.isNull())
        {
          Command[0] = 0;
          Command[1] = servoData["Angle"];
          Command[2] = servoData["Time"];
        }

        // 解析步进电机的控制信息
        JsonObject stepperData = jb["Stepper"];
        if (!stepperData.isNull())
        {
          Command[3] = stepperData["Distance"];
          Command[4] = stepperData["Acceleration"];
        }

        buff = "";
      }
      else
        buff += inChar;
    }
  }
  else
  {
    for (int i = 0; i < 5; i++)
      Command[i] = 99;
    return Command;
  }
  return Command;
}

#endif