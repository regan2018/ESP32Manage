#include <BluetoothSerial.h>

BluetoothSerial serialBT;

// 经典蓝牙API
// BluetoothSerial SerialBT ：创建一个蓝牙串口，命名为SerialBT
// SerialBT.begin() ：开启蓝牙
// SerialBT.available() ：返回蓝牙串口缓冲区中当前剩余的字符个数。
// SerialBT.print() ：蓝牙串口发送的是字符，
// SerialBT.write() ：蓝牙串口发送的字节.
// Serial.read() ：返回蓝牙串口接收的字符

//经典蓝牙工具类
class ClassicBluetooth {
public:
    //初始化蓝牙
    void initBT(const char* deviceName) {
        serialBT.begin(deviceName);
    }
    //蓝牙通讯业务处理
    void businessProcessing() {
       if (Serial.available()) {
            serialBT.write(Serial.read());//将串口收到的数据，再通过蓝牙串口转发出去
            Serial.println("由SerialBT打印");
        }
        if (serialBT.available()) {//将蓝牙串口收到的数据，再通过串口把信息发回给电脑
            Serial.write(serialBT.read());
            Serial.println("由Serial打印");
        }

    }

};
