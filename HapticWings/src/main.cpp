#include <AccelStepper.h>
#include "include.h"
#include "SerialServo.h"
#include "communication.hpp"

#define DIR_PIN_LEFT 2 // 定义步进和方向引脚
#define STEP_PIN_LEFT 4

#define DIR_PIN_RIGHT 6
#define STEP_PIN_RIGHT 5

#define NUMBER_OF_CIRCURES 200

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
bool shouldMove = false;
bool servoMoved = true;
int i = 1000;

void setup()
{
  pinMode(switchPin, INPUT_PULLUP); // 设置数字口为输入模式，启用内部上拉电阻
  Serial.begin(115200);             // 初始化串口通信
  // stepper.stop();

  /*当前似乎1200和350已经是极限了，但该模型使用的导轨和滑块比较陈旧，
  因此实际上会更高一些*/

  stepperLeft.setCurrentPosition(0); // 设置当前位置为0
  stepperLeft.setMaxSpeed(1200);     // 设置最大速度
  stepperLeft.setAcceleration(350);  // 设置加速度

  stepperRight.setCurrentPosition(0);
  stepperRight.setMaxSpeed(1200);
  stepperRight.setAcceleration(350);
  i = 0;
}

void loop()
{
  switchState = digitalRead(switchPin); // 读取开关状态

  long *Command = SerialReceive();
  
  if (Command[0] == 0)
  {
    Serial.println("Target position set to: " + String(Command[1]));
  }

  if (switchState == LOW)
  {
    // Serial.println("Switch is ON");
    shouldMove = true;
    // stepperLeft.moveTo(-700); // 按圈数移动指定的步数
    //  stepperRight.moveTo(700);
    servoMoved = false;
  }

  if (shouldMove)
  {
    stepperLeft.run(); // 不断调用run()来实际移动电机
    // stepperRight.run();

    if (!servoMoved)
    {
      LobotSerialServoMove(Serial, ID_ALL, 500, 500);
      // LobotSerialServoMove(Serial, ID_ALL, 500, 500);
      servoMoved = true;
    }
  }

  else
  {
    // Serial.println("Switch is OFF");
  }

  // delay(500); // 简单的延时，防止信息打印过快
}
