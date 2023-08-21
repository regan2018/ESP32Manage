//自定义库
#ifndef LEDManage_H //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
  #define LEDManage_H
  #include  <LEDManage.h> //LED灯控制工具类
#endif


#ifndef WIFIManage //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
  #define WIFIManage
  #include "WIFIManage.h" //WIFI管理工具类
#endif

#include "BLEManage.h" //低功耗蓝牙工具类
#include "BTManage.h"  //经典蓝牙工具类

#ifndef LittleFSManage // 使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define LittleFSManage
#include "LittleFSManage.h" //文件系统工具类
#endif

// 定义三基色针脚
gpio_num_t R = GPIO_NUM_12;
gpio_num_t G = GPIO_NUM_13;
gpio_num_t B = GPIO_NUM_14;

//红外使用针脚
//红外发送引脚
gpio_num_t RECV_TX0 = GPIO_NUM_13;  //红外发送模块的TX0引脚
//红外接收引脚
gpio_num_t RECV_RX0 = GPIO_NUM_12;  //红外接收模块的RX0引脚

#ifndef IRremoteManage_H //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
  #define IRremoteManage_H
  #include  <IRremoteManage.h>//红外控制工具类
#endif
RremoteUtil rremoteUtil;


//wifi配置参数
const char* ssid = "@Ruijie-m38D1";
const char* password = "";
NetworkUtils network;

//蓝牙配置参数
const char* deviceName = "MyESP32";//蓝牙名称
//启用低功耗蓝牙
BluetoothUtils bluetooth(deviceName);

//启用经典蓝牙
ClassicBluetooth classicBT;

//LittleFS文件系统
LittleFSWrapper fsUtil;