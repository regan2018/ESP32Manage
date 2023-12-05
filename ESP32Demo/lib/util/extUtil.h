
#include <ArduinoJson.h>

#ifndef LittleFSManage //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define LittleFSManage
#include "LittleFSManage.h" //文件系统工具类
#endif

//重新连接WIFI
#define RECONNECT_WIFI "重新连接WIFI"
//设备运行时长
#define DEVICE_RUN_TIME "设备运行时长"
//设置WIFI信息
#define SET_WIFI "setWifi"
//设置PWM的占空比
#define SET_PWM_PERCENT "setPwmPercent"
//LED开头
#define SWITCH_LED "switchLed"

LittleFSWrapper fsManageUtil;

//基本信息配置路径
String infoCfgPath="/InfoConfig.json";

//重写全局变量
extern uint8_t pwm_percent;//PWM的占空比，电机调速时调整这个值

//重写全局变量
extern uint8_t switch_led_state;//LED灯开头状态


//保存配置信息
bool saveInfoCfg(String key,String val){
    
    String cfgInfo=fsManageUtil.str_read(infoCfgPath);
    Serial.print("保存前的配置信息：");Serial.println(cfgInfo);
    
    //把写成一个JSON格式
    StaticJsonDocument<200> info_json; 

    // 解析JSON字符串
    const size_t capacity = JSON_OBJECT_SIZE(1024);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, cfgInfo);

    // 检查解析错误
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return false;
    }

    bool isNewAddKey=true;//是否新增key
    // 遍历JSON数据
    for (JsonPair kv : doc.as<JsonObject>()) {
        const char* tempKey = kv.key().c_str();
        const String tempValue = kv.value().as<String>();

        Serial.print("Key: ");
        Serial.println(tempKey);
        Serial.print("Value: ");
        Serial.println(tempValue);
        Serial.println("-----");

        if(key.equals(tempKey)){
            info_json[key] = val;
            isNewAddKey=false;
        }else{
            info_json[tempKey] = tempValue;
        }
    }
    if(isNewAddKey){
        info_json[key] = val;
    }
    
    String info_json_str;                                                         //定义一个字符串变量
    serializeJson(info_json, info_json_str);                                      //生成JOSN的字符串
    fsManageUtil.str_write(infoCfgPath,info_json_str);
    Serial.println("基本信息配置保存成功");

    String cfgInfoPrint=fsManageUtil.str_read(infoCfgPath);
    Serial.print("保存后的配置信息：");Serial.println(cfgInfoPrint);
    return true;
    
}

//byte数组传字符串【方式一】
String byteToString1(byte* byteVal, int length){
    String str;
    for (int i = 0; i < length; i++) {
        str.concat((char)byteVal[i]);
    }
    return str;
}

//byte数组传字符串【方式二】
String byteToString2(byte* byteVal, int length){
    String str(reinterpret_cast<char*>(byteVal), length);
    return str;
}


/***************************************************************************************
 * 函数:解析JSON字符串，从JSON字符串名称得到该值
 * str:   JSON字符串
 * Name:  JSON集合的名称
 * return: 返回值的字符串
 ***************************************************************************************/
String analysisJson(String str, String Name){
    DynamicJsonDocument doc(str.length()*2);    //定义一个JSON对象
    deserializeJson(doc, str);                  //反序列数据
    String value = doc[Name].as<String>();      //从Name中读取对应的值
    return value;
}
