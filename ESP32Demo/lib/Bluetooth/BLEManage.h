#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAddress.h>

#ifndef WIFIManage //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
    #define WIFIManage
    #include "WIFIManage.h" //WIFI管理工具类
#endif

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//重新连接WIFI
#define RECONNECT_WIFI "重新连接WIFI"
//设备运行时长
#define DEVICE_RUN_TIME "设备运行时长"

//蓝牙连接状态
bool connected = false;

NetworkUtils network2;

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
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            
            #pragma region 蓝牙接收指令并处理
            
            const char* comm_val=value.c_str();

            if((strcmp(RECONNECT_WIFI, comm_val) == 0)){
                Serial.println("正在重新连接WIFI....");
                network2.wifi_connect();
            }
            if(strcmp(DEVICE_RUN_TIME,comm_val)==0){
                long runTime=millis();//设备运行时长，毫秒
                Serial.print("设备运行时长【毫秒】：");Serial.println(runTime);
                Serial.print("设备运行时长【秒】：");Serial.println(runTime/1000);
                Serial.print("设备运行时长【分】：");Serial.println(runTime/(1000*60.0));
                Serial.print("设备运行时长【时】：");Serial.println(runTime/(1000*60.0*60));
                Serial.print("设备运行时长【天】：");Serial.println(runTime/(1000*60.0*60*24));
            }

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


