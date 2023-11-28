#include <Arduino.h>
#include <WiFi.h> //WIFI库
#include <HTTPClient.h> //网络请求库
#include <BLEDevice.h> //蓝牙设备库


#ifndef cfgInfo_H // 使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
  #define cfgInfo_H
  #include "cfgInfo.h" //配置参数信息类
#endif


#pragma region 初始化调用方法
void setup() {
  Serial.begin(115200);

  fsUtil.LittleFS_begin();   //LittleFS文件系统初始化
  if (LittleFS.begin(true,"/cfg")) {
    Serial.println("LittleFS mounted successfully.");
  } else {
    Serial.println("LittleFS mount failed.");
    return;
  }
  

  //remoteUtil.startRecv();//红外管理

  #pragma region WIFI启动并连接
   if(!network.isConnected()){
      //saveWIFICfg(ssid,password);
      network.connect_NET();       //网络初始化
      //network.mqttCfg();
      
   }
  #pragma endregion

  #pragma region 低功耗蓝牙相关操作
    bluetooth.start();
    // bluetooth.getConnected();
  #pragma endregion

  #pragma region 经典蓝牙相关操作
    // classicBT.initBT(deviceName);
    // classicBT.businessProcessing();
  #pragma endregion


}
#pragma endregion

#pragma region 设备循环执行的方法
void loop() {
  network.mqtt_loop();//mqtt服务的请求处理

  // 在循环中设置占空比
  pwm.setDutyCycle(pwm_percent); // 设置为50%的占空比

  // rremoteUtil.irrecv_loop();//红外循环接收处理

  // network.DNS_request_loop();   //DNS服务请求处理
  #pragma region LED灯控制
  // initGPIO(R,G,B,SevenColorChange);
  // ChangeColor((int)Color_R);
  // delay(5000);
  // LEDChangeColor();
  // initGPIO(R,G,B,GradientChange);
  // LEDGradientColor();
  #pragma endregion

  #pragma region 网络相关请求
    if(!network.isConnected()){
      network.connect_NET();
      delay(2000);
    }

    if(network.isConnected()){
      // 发送GET请求
      String response = network.httpGet("https://www.eyijiao.com");
      Serial.println("GET response:");
      Serial.println(response);
      delay(5000);

   // 发送POST请求
      // const char* payload = "{\"name\": \"John\", \"age\": 30}";
      // String response = network.httpPost("https://apis.bemfa.com/va/postJsonMsg", payload);
      // Serial.println("POST response:");
      // Serial.println(response);
    }

  #pragma endregion


}
#pragma endregion

