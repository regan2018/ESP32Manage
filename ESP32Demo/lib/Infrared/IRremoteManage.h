#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

#include <ir_Midea.h>

#ifndef cfgInfo_H // 使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define cfgInfo_H
#include "cfgInfo.h" //配置参数信息类
#endif

//红外接收类
IRrecv irrecv(RECV_RX0);
//红外发送类
IRsend irsend(RECV_TX0);

decode_results recvRes;

//美的
// IRMideaAC mideaAC(RECV_TX0);

#define AC_POWER_CODE 0xD051C303 //关闭/打开电源
#define AC_on_code 0x7E474900 //打开空调


//红外接收、发送工具类
class RremoteUtil{
    public:
        //启用红外接收器
        void startRecv(){
            irrecv.enableIRIn();
            // mideaAC.begin();
        }

        //循环获取
        void irrecv_loop(){
            if (irrecv.decode(&recvRes)) {
                Serial.println(recvRes.value, HEX);
                irrecv.resume(); // 接收下一个值
                sendACPower();
            }
            delay(100);
        }

        //电源开关
        void sendACPower() {
            // mideaAC.send(AC_power_code);
            Serial.println("打开空调");
            irsend.sendNEC(AC_POWER_CODE, 32);
        }

};