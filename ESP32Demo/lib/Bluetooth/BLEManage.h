#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAddress.h>
#include <ArduinoJson.h>

#ifndef WIFIManage //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
    #define WIFIManage
    #include "WIFIManage.h" //WIFI管理工具类
#endif


//多线程工具类
#include "Multithreading.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" //蓝牙特征对应服务的 UUID
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" //蓝牙特征的 UUID

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


//蓝牙连接状态
bool connected = false;

NetworkUtils network2;

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

//业务处理方法
void businessHandleData(const char* comm_val){
    //const char* comm_val=(char*)parameter;
    Serial.print("线程收到的数据：");Serial.println(comm_val);
    String cmdName=analysisJson(comm_val,"cmdName");
    String cmdVal=analysisJson(comm_val,"cmdVal");
   
    Serial.print("cmdName：");Serial.println(cmdName);
    Serial.print("cmdVal：");Serial.println(cmdVal);

    //strcmp 该函数接受两个参数，分别为要比较的两个字符串的指针。返回值为整形，表示两个字符串的大小关系。 
    if((strcmp(RECONNECT_WIFI, comm_val) == 0)){
        Serial.println("正在重新连接WIFI....");
        //network2.wifi_connect();
    }
    if(cmdName.equals(SET_WIFI)){
        String wifiName=analysisJson(cmdVal,"wifiName");
        String wifiPwd=analysisJson(cmdVal,"wifiPwd");
        
        saveWIFICfg(wifiName,wifiPwd);
        
        //清理LittleFS
        LittleFS.end();
        //重启ESP32
        ESP.restart();
    }

    if(cmdName.equals(DEVICE_RUN_TIME)){
        long runTime=millis();//设备运行时长，毫秒
        Serial.print("设备运行时长【毫秒】：");Serial.println(runTime);
        Serial.print("设备运行时长【秒】：");Serial.println(runTime/1000);
        Serial.print("设备运行时长【分】：");Serial.println(runTime/(1000*60.0));
        Serial.print("设备运行时长【时】：");Serial.println(runTime/(1000*60.0*60));
        Serial.print("设备运行时长【天】：");Serial.println(runTime/(1000*60.0*60*24));
    }
    if(cmdName.equals(SET_PWM_PERCENT)){
        String pwmPercentValStr=analysisJson(cmdVal,"pwmPercentVal");
        int pwmPercentVal=pwmPercentValStr.toInt();
        pwm_percent=pwmPercentVal;
        Serial.print("已设置PWM的占空比为："); Serial.println(pwmPercentVal);
        saveInfoCfg("pwmPercentVal",pwmPercentValStr);
    }
    if(cmdName.equals(SWITCH_LED)){
        String switchLedStateStr=analysisJson(cmdVal,"switchLedState");
        int switchLedState=switchLedStateStr.toInt();
        switch_led_state=switchLedState;
        Serial.print("已设置开头状态为："); Serial.println(switchLedState);
        saveInfoCfg("switchLedState",switchLedStateStr);
    }

}

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        connected=true;//连接成功
        Serial.println("蓝牙连接成功");
    }

    void onDisconnect(BLEServer* pServer) {
        connected=false;//连接失败
        Serial.println("蓝牙断开连接");
        BLEDevice::startAdvertising();//重新广播蓝牙
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        // 处理接收到的数据
        if (value.length() > 0) {
            Serial.println("*********");
            Serial.print("收到新的数据: ");
            for (int i = 0; i < value.length(); i++){
                Serial.print(value[i]);
            }
            Serial.println();
            
            #pragma region 蓝牙接收指令并处理
            
            const char* comm_val=value.c_str();
            //业务处理
            businessHandleData(comm_val);

            /*String cmdName=analysisJson(comm_val,"cmdName");
            String cmdVal=analysisJson(comm_val,"cmdVal");
           
            if(cmdName.equals(SET_WIFI)){
                String wifiName=analysisJson(cmdVal,"wifiName");
                String wifiPwd=analysisJson(cmdVal,"wifiPwd");
                
                saveWIFICfg(wifiName,wifiPwd);
                
                //保存文件内容后，重新启动才会生效
                //清理LittleFS
                LittleFS.end();
                //重启ESP32
                ESP.restart(); 
            }*/
           
            Serial.println("指令发送成功");            

            #pragma endregion
            
            Serial.println();
            Serial.println("*********");
        }
    }
};

//低功耗蓝牙(BLE)工具类
class BluetoothUtils {
    private:
        const char* deviceName;
        BLECharacteristic *pCharacteristic;
        
    public:
        BluetoothUtils(const char* deviceName)
            : deviceName(deviceName) {}

        void start() {
            BLEDevice::init(deviceName);
            BLEServer *pServer = BLEDevice::createServer();
            pServer->setCallbacks(new MyServerCallbacks());

            BLEService *pService = pServer->createService(SERVICE_UUID);

            BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                                CHARACTERISTIC_UUID,
                                                BLECharacteristic::PROPERTY_READ |
                                                BLECharacteristic::PROPERTY_WRITE
                                                );

            pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

            //pCharacteristic->setValue("Hello World");
            pService->start();

            BLEAdvertising *pAdvertising = pServer->getAdvertising();
            pAdvertising->start();
            Serial.println(String(deviceName)+"蓝牙设备已启动");
        }

        void setCharacteristicValue(const std::string& value) {
            pCharacteristic->setValue(value);
        }

        std::string getCharacteristicValue() {
            return pCharacteristic->getValue();
        }

        //获取蓝牙连接状态
        bool getConnected(){
            return connected;
        }

};


