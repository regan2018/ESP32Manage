#include <WiFi.h>
#include <HTTPClient.h>
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include <PubSubClient.h> //mqtt支持库

#ifndef LittleFS_H //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define LittleFS_H
#include "LittleFS.h" 
#endif

#ifndef LittleFSManage //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define LittleFSManage
#include "LittleFSManage.h" //文件系统工具类
#endif

#ifndef extUtil_H //使用 #ifndef，#define 和 #endif 来进行保护，也就是缺少了防止类被多次包含的宏；
#define extUtil_H
#include "extUtil.h" //文件系统工具类
#endif


DNSServer dnsserver;
AsyncWebServer server(80);      //创建一个服务器对象，WEB服务器端口:80

WiFiClient espClient;
PubSubClient client(espClient);

LittleFSWrapper fsManageUtil;

String wifiCfgPath="/WIFIConfig.conf";


//mqtt配置
const char* mqttServer = "bemfa.com";
const int mqttPort = 9501;
const char* mqttUser = "991905968@qq.com";
const char* mqttPassword = "63d67cb328a3480d80d18e4cc8c41002";
#define ID_MQTT  "63d67cb328a3480d80d18e4cc8c41002"     //用户私钥，控制台获取
const char*  topic = "esp32mqtt";        //主题名字，可在巴法云控制台自行创建，名称随意

#pragma region mqtt的回调函数
//mqtt的回调函数
 void mqttCallback(char* topic, byte* payload, unsigned int length) {
    
    Serial.print("消息来源主题: ");Serial.println(topic);
    
    Serial.print("消息内容:");
   
    String str=byteToString2(payload,length);
    Serial.println(str);

}
#pragma endregion


#pragma region web服务器配置
void call_back(AsyncWebServerRequest *request){

    const String indexHtml PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>EPS32教程</title>
    </head>
    <body>
        <h1>ESP32 HTML 标题</h1>
        <p>ESP32 HTML 段落</p>
    </body>
    </html>
    )rawliteral";
    Serial.println("User requested");
    //request->send(200,"text/html",indexHtml);   //响应请求

    // if(!LittleFS.begin(true,"/htmlPage")){
    //     Serial.println("An Error has occurred while mounting LittleFS");
    //     return;
    // }

    request->send(LittleFS,"/index.html");
    // handleFileRead("/index.html");

}
    
/***********************************************************************************
 * web服务器初始化
***********************************************************************************/
void web_server(){
    Serial.println("初始化WEB服务器");
    // if(!LittleFS.begin(true,"/htmlPage")){
    //     Serial.println("An Error has occurred while mounting LittleFS");
    //     return;
    // }
    // server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");			//响应网站根目录的GET请求,返回文件index.html
    server.on("/",HTTP_GET,call_back);
    // server.on("/setwifi"  ,HTTP_POST ,get_WIFI_set_CALLback);                //响应设置WIFI按钮的请求
    server.begin();                       										                //初始化
}
#pragma endregion

//网络请求工具类
class NetworkUtils {
public:
    

    //mqtt配置
    void mqttCfg(){
        client.setServer(mqttServer, mqttPort);
        client.setCallback(mqttCallback);
        
        while (!client.connected()) {
            Serial.println("正在连接MQTT...");
        
            if (client.connect(ID_MQTT)) {
                Serial.println("连接MQTT成功");  
            } else {
                Serial.print("连接MQTT失败，状态为：");
                Serial.print(client.state());
                delay(2000);
            }
        }
        
        client.publish(topic, "Hello from ESP32");
        client.subscribe(topic);
    }
    
    //mqtt处理请求的循环
    void mqtt_loop(){
         client.loop();
    }

    //断开WIFI连接
    bool disconnect() {
        client.disconnect();
        bool isflag= WiFi.disconnect();
        Serial.println("已断开与WiFi网络的连接。");
        return isflag;
    }


    /***********************************************************************************
     * 函数:连接WIFI
     * ssid:        WIFI名称
     * password:    WIFI密码
     * return:      连接成功返回true
    ***********************************************************************************/
    bool connect_WIFI(String ssid, String password){
        WiFi.begin(ssid.c_str(), password.c_str());               //连接WIFI
        Serial.print("连接WIFI");
        //循环60秒后连接不上跳出循环
        int i = 0;
        while(WiFi.status() != WL_CONNECTED){
            Serial.print(".");
            delay(1000);
            i++;
            if(i>60){
                Serial.println();
                Serial.println("WIFI连接失败，请重新配置网络");
                return false;
            }
        }
        Serial.println();
        IPAddress local_IP = WiFi.localIP();
        Serial.print("WIFI连接成功，本地IP地址:"); //连接成功提示
        Serial.println(local_IP); 
        return true;
    }
    
    
    /***********************************************************************************
     * 函数：/WIFIConfig.conf文件中读取设置数据并连接WIFI
    ***********************************************************************************/
    void wifi_connect(){
        try
        {
            Serial.println("在conf文件中读取数据并连接WIFI");
            String str = fsManageUtil.str_read(wifiCfgPath);                  //读取文件内容
            if(str.length()==0){
                Serial.println("未保存WIFI连接信息，无法从文件中读取信息进行连接");
                return;
            }
            Serial.print("连接WIFI的参数：");Serial.println(str);
            String wifiname = analysisJson(str, "wifiname");           //解析WIFI名称
            String wifipassword = analysisJson(str, "wifipassword");   //解析WIFI密码
            
            if(connect_WIFI(wifiname, wifipassword)){                 //连接WIFI
                mqttCfg();

                // 发送POST请求
                // String response = httpGet("http://www.baidu.com");
                // Serial.println("GET response:");
                // Serial.println(response);
                // 发送POST请求
                //  const char* payload = "{\"uid\":\"63d67cb328a3480d80d18e4cc8c41002\" \"\",\"topic\":\"esp32mqtt\", \"type\": 1,\"msg\":\"2434sfdsd\"}";
                //  String response = httpPost("https://apis.bemfa.com/va/postJsonMsg", payload);
                //  Serial.println("POST response:");
                //  Serial.println(response);
            }
        }
        catch(const std::exception& e)
        {
            e.what();
        }        
        
    }

    //配置DNS服务器
    void DNSServerCfg(){

        const byte DNS_PORT = 53;                     //DNS端口
        const String url = "esp32ap.com";               //域名
        IPAddress APIp(10,0,10,1);                    //AP IP
        IPAddress APGateway(10,0,10,1);               //AP网关
        IPAddress APSubnetMask(255,255,255,0);        //AP子网掩码
        const char* APSsid = "esp32_AP";              //AP SSID
        const char* APPassword = "12345678";          //AP wifi密码
        WiFi.mode(WIFI_AP_STA);                             //打开AP和STA共存模式
        WiFi.softAPConfig(APIp, APGateway, APSubnetMask);   //设置AP的IP地址，网关和子网掩码
        WiFi.softAP(APSsid, APPassword, 6);                 //设置AP模式的登陆名和密码
        dnsserver.start(DNS_PORT, url, APIp);               //设置DNS的端口、网址、和IP
        Serial.print("AP模式IP地址为:");
        Serial.println(WiFi.softAPIP());

        web_server();       //WEB服务器初始化
    }

    /***********************************************************************************
     * 设置AP和STA共存模式，设置DNS服务器
    ***********************************************************************************/
    void connect_NET(){
        wifi_connect();                               //连接WIFI
        //DNSServerCfg();
        //mqttCfg();
    }
    
    
    /***********************************************************************************
     * DNS处理请求的循环
    ***********************************************************************************/
    void DNS_request_loop(){
        dnsserver.processNextRequest();
    }

    //获取IP地址
    IPAddress getIP() {
        return WiFi.localIP();
    }

    //是否已连接
    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
    //get请求
    String httpGet(const String url) {
        HTTPClient http;
        String result;

        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode == HTTP_CODE_OK) {
            result = http.getString();
        } else {
            Serial.printf("GET请求失败, error: %s", http.errorToString(httpResponseCode).c_str());
        }

        http.end();
        return result;
    }
    //post请求
    String httpPost(const String url, const String payload) {
        HTTPClient http;
        String result;

        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(payload);

        if (httpResponseCode == HTTP_CODE_OK) {
            result = http.getString();
        } else {
            Serial.printf("POST请求失败, error: %s", http.errorToString(httpResponseCode).c_str());
        }

        http.end();
        return result;
    }

private:
    const char* ssid;
    const char* password;
};
    //保存WIFI配置信息
    void saveWIFICfg(String ssid,String password){
        //删除WIFI配置文件
        bool delFlat= fsManageUtil.delFile(wifiCfgPath);
        Serial.print("文件状态：");Serial.println(delFlat);
        if(!delFlat){
            Serial.println("删除文件失败，无法保存WIFI信息");
        }
        
        //把SSID和password写成一个JSON格式
        StaticJsonDocument<200> wifi_json;                                            //创建一个JSON对象,wifi_json
        wifi_json["wifiname"] = ssid;                                                          //写入一个建和值
        wifi_json["wifipassword"] = password;                                                          //写入一个键和值
        String wifi_json_str;                                                         //定义一个字符串变量
        serializeJson(wifi_json, wifi_json_str);                                      //生成JOSN的字符串
        fsManageUtil.str_write(wifiCfgPath,wifi_json_str);
        Serial.println("WIFI配置信息保存成功");

        String cfgInfo=fsManageUtil.str_read(wifiCfgPath);
        Serial.print("保存后的配置信息：");Serial.println(cfgInfo);
    }

    /**********************************************************************************
     * 函数:响应网站/setwifi目录的POST请求,收到请求后，运行get_WIFI_set_CALLback回调函数
     * 获取并格式化收到的POST数据
     *********************************************************************************/
    void get_WIFI_set_CALLback(AsyncWebServerRequest *request){
        Serial.println("收到设置WIFI按钮");
        if(request->hasParam("wifiname",true)){
            AsyncWebParameter* wifiname = request->getParam("wifiname",true);					    //获取POST数据
            AsyncWebParameter* wifipassword = request->getParam("wifipassword",true);			//获取POST数据
            String wn  = wifiname->name().c_str();
            String wnv = wifiname->value().c_str();
            String wp  = wifipassword->name().c_str();
            String wpv = wifipassword->value().c_str();

            saveWIFICfg(wnv,wpv);
            //把SSID和password写成一个JSON格式
            // StaticJsonDocument<200> wifi_json;                                            //创建一个JSON对象,wifi_json
            // wifi_json[wn] = wnv;                                                          //写入一个建和值
            // wifi_json[wp] = wpv;                                                          //写入一个键和值
            // String wifi_json_str;                                                         //定义一个字符串变量
            // serializeJson(wifi_json, wifi_json_str);                                      //生成JOSN的字符串
            // fsManageUtil.str_write("/WIFIConfig.conf",wifi_json_str);                                  //字符串写入
        }
    }

    
    // String getContentType(String filename){
    //     if (server.hasArg("download")) 
    //     {
    //         return "application/octet-stream";
    //     } 
    //     else if (filename.endsWith(".htm")) 
    //     {
    //         return "text/html";
    //     } 
    //     else if (filename.endsWith(".html")) 
    //     {
    //         return "text/html";
    //     } 
    //     else if (filename.endsWith(".css")) 
    //     {
    //         return "text/css";
    //     } 
    //     else if (filename.endsWith(".js")) 
    //     {
    //         return "application/javascript";
    //     } 
    //     else if (filename.endsWith(".png")) 
    //     {
    //         return "image/png";
    //     } 
    //     else if (filename.endsWith(".gif")) 
    //     {
    //         return "image/gif";
    //     } 
    //     else if (filename.endsWith(".jpg")) 
    //     {
    //         return "image/jpeg";
    //     } 
    //     else if (filename.endsWith(".ico")) 
    //     {
    //         return "image/x-icon";
    //     } 
    //     else if (filename.endsWith(".xml")) 
    //     {
    //         return "text/xml";
    //     } 
    //     else if (filename.endsWith(".pdf")) 
    //     {
    //         return "application/x-pdf";
    //     } 
    //     else if (filename.endsWith(".zip")) 
    //     {
    //         return "application/x-zip";
    //     } 
    //     else if (filename.endsWith(".gz")) 
    //     {
    //         return "application/x-gzip";
    //     }
    //     return "text/plain";
    // }

    // bool handleFileRead(String path) 
    // {
    // Serial.println("handleFileRead: " + path);
    
    // if (path.endsWith("/")) 
    // {
    //     path += "index.html";
    // }
    
    // String contentType = getContentType(path);
    // String pathWithGz = path + ".gz";
    
    // if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) 
    // {
    //     if (LittleFS.exists(pathWithGz)) 
    //     {
    //         path += ".gz";
    //     }
        
    //     File file = LittleFS.open(path, "r");
    //     server.s.streamFile(file, contentType);
    //     file.close();
    //     return true;
    // }
    
    // return false;
    // }
