#ifndef LittleFS_H //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define LittleFS_H
#include "LittleFS.h" 
#endif


class LittleFSWrapper {
public:
    LittleFSWrapper() {
        // if (!LittleFS.begin(true)) {
        //     Serial.println("无法初始化LittleFS");
        // }else{
        //     Serial.println("初始化LittleFS成功");
        // }
    }

    ~LittleFSWrapper() {
        LittleFS.end();
    }

    bool writeFile(const char* filePath, const char* data) {
        File file = LittleFS.open(filePath, "w");
        if (!file) {
            Serial.println("无法打开文件进行写入");
            return false;
        }

        if (file.print(data)) {
            Serial.println("文件写入成功");
        } else {
            Serial.println("文件写入失败");
        }

        file.close();
        return true;
    }

    bool readFile(const char* filePath, String& data) {
        File file = LittleFS.open(filePath, "r");
        if (!file) {
            Serial.println("无法打开文件进行读取");
            return false;
        }

        data = "";
        while (file.available()) {
            data += char(file.read());
        }

        file.close();
        return true;
    }

    bool exists(const char* filePath) {
        return LittleFS.exists(filePath);
    }

    bool remove(const char* filePath) {
        Serial.println("删除文件");
        return LittleFS.remove(filePath);
    }

    /**
     * 删除文件
    */
    bool delFile(String filePath) {
        if(LittleFS.remove(filePath)){
            Serial.println("删除文件成功");
            return true;
        }else{
            Serial.println("删除文件失败");
            return false;
        }
    }

    /**************************************************************************************
   * 函数:字符串写入文件，文件如果存在，将被清零并新建，文件不存在，将新建该文件
   * path:    文件的绝对路径
   * str:     要写入的字符串
   *************************************************************************************/
  void str_write(String path, String str){
      Serial.println("写入文件");
      File wf = LittleFS.open(path,"w");                           //以写入模式打开文件
      if(!wf){                                                     //如果无法打开文件
        Serial.println("打开文件写入时错误");  //显示错误信息
        return;                                                    //无法打开文件直接返回
      }
      wf.print(str);                                               //字符串写入文件
      wf.close();                                                  //关闭文件
      File rf = LittleFS.open(path,"r");                           //以读取模式打开文件
      Serial.print("FILE:");Serial.println(rf.readString());       //读取文件
      rf.close();                                                  //关闭文件 
  }
  
  /**********************************************************************************
   * 函数:从文件path中读取字符串
   * path:      文件的绝对路径
   * return:    返回读取的字符串
   *********************************************************************************/
  String str_read(String path){
      Serial.println("读取文件");
      File rf = LittleFS.open(path,"r");                 //以读取模式打开文件
      if(!rf){                                           //如果无法打开文件
        Serial.println("打开文件读取时错误");               //显示错误信息
        return "";                                       //无法打开文件直接返回
      }
      String str = rf.readString();                      //读取字符串
      rf.close();                                        //关闭文件 
      return str;
  }
  
  
  /********************************************************************************
   * LittleFS文件系统初始化
   *********************************************************************************/
  void LittleFS_begin(){
    Serial.println();
    Serial.println("初始化文件系统");
    if(!LittleFS.begin(true)){
      Serial.println("初始化LittleFS时出错");
      return;
    }else{
        Serial.println("初始化LittleFS成功");
    }
  }
};

// void setup() {
//     Serial.begin(115200);

//     LittleFSWrapper fs;

//     if (fs.writeFile("/example.txt", "Hello, LittleFS!")) {
//         Serial.println("文件写入成功");
//     } else {
//         Serial.println("文件写入失败");
//     }

//     String content;
//     if (fs.readFile("/example.txt", content)) {
//         Serial.print("读取的内容：");
//         Serial.println(content);
//     } else {
//         Serial.println("无法读取文件");
//     }

//     if (fs.exists("/example.txt")) {
//         Serial.println("文件存在");
//     } else {
//         Serial.println("文件不存在");
//     }

//     if (fs.remove("/example.txt")) {
//         Serial.println("文件删除成功");
//     } else {
//         Serial.println("文件删除失败");
//     }
// }

// void loop() {
//     // 主循环
// }