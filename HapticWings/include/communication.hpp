#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <ArduinoJson.h>

// 第一个mode用于判断是否是归位还是要运行，0为归位，1为运行；
// 第二个第三个用于判断舵机的角度和运行所需的时间，这里假设我们使用的串行舵机
// 第四个第五个用于判断步进电机的移动距离和是否需要加速
// Example 到最远端1 {"Mode": "Run", "Servos": [{"Angle": 875, "Time": 700}, {"Angle": 875, "Time": 700}], "Steppers": [{"Distance": -500, "Acceleration": false}, {"Distance": -700, "Acceleration": false}]}
// 到最远端口2 {"Mode": "Run", "Servos": [{"Angle": 125, "Time": 700}, {"Angle": 125, "Time": 1000}], "Steppers": [{"Distance": -500, "Acceleration": false}, {"Distance": -500, "Acceleration": false}]}
// 初始化 {"Mode": "Run", "Servos": [{"Angle": 500, "Time": 1000}, {"Angle": 500, "Time": 1000}], "Steppers": [{"Distance": 0, "Acceleration": false}, {"Distance": 0, "Acceleration": false}]}


String buff = "";

// const size_t capacity = JSON_OBJECT_SIZE(3) + 2*JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(2); // 修改为适当的 JSON 容量，以适应舵机和步进电机的信息
const size_t capacity = 512;
StaticJsonDocument<capacity> jb;

long *SerialReceive()
{
  static long Command[9];
  if (Serial.available())
  {
    String buff;
    char inChar = '\0'; // 初始化字符
    while (inChar != '\n')
    {
      while (!Serial.available())
        ; // 等待数据
      inChar = (char)Serial.read();
      if (inChar == '\n')
      {
        Serial.println(buff);
        StaticJsonDocument<200> jb; // 调整大小根据实际需求
        DeserializationError err = deserializeJson(jb, buff);
        if (err)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(err.c_str());
        }

        String mode = jb["Mode"];
        if (mode == "Init")
        {
          Command[0] = 0;
        }
        else if (mode == "Run")
        {
          Command[0] = 1;
        }

        JsonArray servoDataArray = jb["Servos"];
        for (int i = 0; i < 2; i++)
        {
          JsonObject servoData = servoDataArray[i];
          if (!servoData.isNull())
          {
            Command[1 + i * 2] = servoData["Angle"];
            Command[2 + i * 2] = servoData["Time"];
          }
        }

        JsonArray stepperDataArray = jb["Steppers"];
        for (int i = 0; i < 2; i++)
        {
          JsonObject stepperData = stepperDataArray[i];
          if (!stepperData.isNull())
          {
            Command[5 + i * 2] = stepperData["Distance"];
            Command[6 + i * 2] = stepperData["Acceleration"];
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