#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <ArduinoJson.h>

// 第一个mode用于判断是否是归位还是要运行，0为归位，1为运行；
// 第二个第三个用于判断舵机的角度和运行所需的时间，这里假设我们使用的串行舵机
// 第四个第五个用于判断步进电机的移动距离和是否需要加速
// Example {"Mode": "Run", "Servos": [{"Angle": 500, "Time": 1000}, {"Angle": 500, "Time": 1000}], "Steppers": [{"Distance": 500, "Acceleration": true}, {"Distance": 500, "Acceleration": true}]}

String buff = "";

// const size_t capacity = JSON_OBJECT_SIZE(3) + 2*JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(2); // 修改为适当的 JSON 容量，以适应舵机和步进电机的信息
const size_t capacity = 1024;
StaticJsonDocument<capacity> jb;

long *SerialReceive()
{
  static long Command[9]; // 数组用于存储模式，两个舵机的角度和时间，以及两个步进电机的移动距离和加速度标志位
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

        // 解析模式
        String mode = jb["Mode"];
        if (mode == "Init") {
          Command[0] = 0;
        } else if (mode == "Run") {
          Command[0] = 1;
        }

        // 解析舵机的控制信息
        JsonArray servoDataArray = jb["Servos"];
        for (int i = 0; i < 2; i++) {
          JsonObject servoData = servoDataArray[i];
          if (!servoData.isNull())
          {
            Command[1 + i*2] = servoData["Angle"];
            Command[2 + i*2] = servoData["Time"];
          }
        }

        // 解析步进电机的控制信息
        JsonArray stepperDataArray = jb["Steppers"];
        for (int i = 0; i < 2; i++) {
          JsonObject stepperData = stepperDataArray[i];
          if (!stepperData.isNull())
          {
            Command[5 + i*2] = stepperData["Distance"];
            Command[6 + i*2] = stepperData["Acceleration"];
          }
        }

        buff = "";
      }
      else
        buff += inChar;
    }
  }
  else
  {
    for (int i = 0; i < 9; i++)
      Command[i] = 99;
    return Command;
  }
  return Command;
}

#endif