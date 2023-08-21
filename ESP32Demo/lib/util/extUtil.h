
#include <ArduinoJson.h>


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
