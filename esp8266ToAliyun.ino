#include <DHT.h>
#include<ESP8266WiFi.h>
#define DHTPIN D4 //温度传感器引脚
#hello
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
static WiFiClient espClient;
#include <ArduinoJson.h>
#include <AliyunIoTSDK.h>
AliyunIoTSDK iot;

#define PRODUCT_KEY "gwlquw1s5ae"
#define DEVICE_NAME "test"
#define DEVICE_SECRET "cb008fa8903e510bcc811820215e7098"
#define REGION_ID "cn-shanghai"

#define WIFI_SSID "张哈嘿的电脑"
#define WIFI_PASSWD "zhang159753!"
void setup()
{
    Serial.begin(115200);
    pinMode(D8, OUTPUT);
    digitalWrite(D8, HIGH);
    dht.begin();
    wifiInit(WIFI_SSID, WIFI_PASSWD);

    AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);
    // 绑定属性回调
    AliyunIoTSDK::bindData("LEDSwitch", ledSwitchCallback);
}
unsigned long lastMsMain = 0;
void loop()
{
    float Humidity = dht.readHumidity();
    float Temperature = dht.readTemperature();
    if (isnan(Humidity) || isnan(Temperature))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    AliyunIoTSDK::loop();
    if (millis() - lastMsMain >= 5000)
    {
        lastMsMain = millis();
        // 发送事件到阿里云平台
        //AliyunIoTSDK::sendEvent("xxx"); 
        // 发送模型属性到阿里云平台
        AliyunIoTSDK::send("Humidity", Humidity);
        AliyunIoTSDK::send("temperature", Temperature);

    }
    //串口输出温湿度
    Serial.print(F("Humidity(%):"));
    Serial.println(Humidity);
    Serial.print(F("Temperature(°C):"));
    Serial.println(Temperature);
    Serial.println();
    delay(2000);
}
//wifi初始化
void wifiInit(const char *ssid, const char *passphrase)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);
    WiFi.setAutoConnect (true);
    WiFi.setAutoReconnect (true);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("WiFi not Connect");
    }
    Serial.println("Connected to AP");
    Serial.println(WiFi.localIP());
    
}
//led灯的属性回调
void ledSwitchCallback(JsonVariant p)
{
    bool LEDSwitch = p["LEDSwitch"];
    if (LEDSwitch)
    {
        //点亮led
        digitalWrite(D8, HIGH);
        Serial.println("led is lighting");
    }
    else
    {
        digitalWrite(D8, LOW);
        Serial.println("led is down");
    }
}
