#include <AccelStepper.h>
#include "include.h"
#include "SerialServo.h"
#include <communication.hpp>
#include "control.h"
#include "Arduino.h"
//#include <SoftwareSerial.h>

#define DIR_PIN_LEFT 2 // 定义步进和方向引脚
#define STEP_PIN_LEFT 4

#define DIR_PIN_RIGHT 6
#define STEP_PIN_RIGHT 5

#define NUMBER_OF_CIRCURES 200

int rxPin = 15;
int txPin = 14;

/*以电机输出轴对人脸方向，
步进电机顺时针为正，
舵机顺时针为正*/

const int switchPin = 8;                                                     // 选择的数字IO口
AccelStepper stepperLeft(AccelStepper::DRIVER, STEP_PIN_LEFT, DIR_PIN_LEFT); // 初始化步进电机
AccelStepper stepperRight(AccelStepper::DRIVER, STEP_PIN_RIGHT, DIR_PIN_RIGHT);

bool isAccelerationMode = true; // True 是加速模式，False 是匀速模式
bool isInitializtion = false;   // 是否初始化

int switchState = 1;
int circle = 3; // 旋转圈数
bool allowMove = false;
bool shouldServoMove = true;
bool shouldServoMove1 = false;
bool shouldServoMove2 = false;
int i = 1000;

bool stepperLeftIsArrived = false;
bool stepperRightIsArrived = false;

void setup()
{
  pinMode(switchPin, INPUT_PULLUP); // 设置数字口为输入模式，启用内部上拉电阻
  Serial.begin(115200);             // 初始化串口通信
  //mySerial.begin(19200);             // 初始化串口通信
  Serial3.begin(115200);
  stepperLeft.stop();
  stepperRight.stop();

  /*当前似乎1200和350已经是极限了，但该模型使用的导轨和滑块比较陈旧，
  因此实际上会更高一些*/

  stepperLeft.setCurrentPosition(0); // 设置当前位置为0
  // stepperLeft.setMaxSpeed(1200);     // 设置最大速度
  // stepperLeft.setAcceleration(350);  // 设置加速度

  // stepperRight.setCurrentPosition(0);
  // stepperRight.setMaxSpeed(1200);
  // stepperRight.setAcceleration(350);

  stepperLeft.setCurrentPosition(0); // 设置当前位置为0
  stepperLeft.setMaxSpeed(2000);     // 设置最大速度
  i = 0;
}

void loop()
{
   switchState = digitalRead(switchPin); // 读取开关状态

  long *Command = SerialReceive();

  // 用于测试communication.hpp中的SerialReceive()函数是否正常工作
  if (Command[0] == 0)
  {
    //printCommandInfo("TestMode: Init", Command);
  }
  else if (Command[0] == 1)
  {
    //printCommandInfo("RunMode: ", Command);
  }
  

  if (Command[0] == 0 || Command[0] == 1)
  {
    allowMove = true;
    shouldServoMove1 = true;
    shouldServoMove2 = true;
    if (Command[0] == 0)
    {
      stepperLeft.moveTo(0);
      stepperLeft.setSpeed(1000);
      stepperRight.moveTo(0);
      stepperRight.setSpeed(1000);
    }
    else if (Command[0] == 1)
    {
      stepperLeft.moveTo(Command[5]);
      stepperLeft.setSpeed(800);
      stepperRight.moveTo(Command[7]);
      stepperRight.setSpeed(800);
    }
  }

// NewFunction();

  if (allowMove)
  {
    if (stepperLeft.runSpeedToPosition())
      stepperLeftIsArrived = true;
    if (stepperRight.runSpeedToPosition())
      stepperRightIsArrived = true;

    if (shouldServoMove1)
    {
      LobotSerialServoMove(Serial3, ID_ALL, Command[1], Command[2]);
      Serial.println("Send suscessful");
      shouldServoMove1 = false;
    }

    // if (shouldServoMove2)
    // {
    //   LobotSerialServoMove(Serial, ID_ALL, Command[3], Command[4]);
    //   shouldServoMove2 = false;
    // }
  }
  else
  {
    // Serial.println("Switch is OFF");
  }

  // if (switchState == LOW)
  // {
  //   //  Serial.println("Switch is ON");
  //   allowMove = true;

  //   // stepperLeft.moveTo(Command[3]); // 按圈数移动指定的步数

  //   stepperLeft.moveTo(700);
  //   stepperLeft.setSpeed(1000);

  //   shouldServoMove = true;
  // }

  //Command[0] = 99;
  // 补充逻辑，如果舵机和步进电机都到了目标位置，将allowMove置为false
  // delay(500); // 简单的延时，防止信息打印过快
}

// void NewFunction()
// {
//   if(!switchState){
//     allowMove = true;
//     shouldServoMove1 = true;
//   }
// }
