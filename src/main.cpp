// Code freeRTOS connect WiFi, MQTT và gửi dữ liệu lên ThingsBoard

#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DHT20.h"

// WiFi Credentials
const char *ssid = "vinh";
const char *password = "88888888";

// ThingsBoard MQTT Broker
const char *mqttServer = "app.coreiot.io";
const int mqttPort = 1883;
const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7";

WiFiClient espClient;
PubSubClient client(espClient);

// Cảm biến
DHT20 dht20;
#define MQ2_AO_PIN 34 //


const long telemetryInterval = 5000;
const long mq2Interval = 5000;
SemaphoreHandle_t i2cMutex;

TaskHandle_t WiFiTaskHandle = NULL;
TaskHandle_t MQTTaskHandle = NULL;
TaskHandle_t MQ2TaskHandle = NULL;
TaskHandle_t TelemetryTaskHandle = NULL;

//  WiFi Task: Kiểm tra reconnect nếu rớt mạng
void checkWiFiTask(void *pvParameters)
{
    for (;;)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(" WiFi disconnected! Reconnecting...");
            WiFi.begin(ssid, password);

            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
            {
                delay(500);
                Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print("\n WiFi connected: ");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("\n WiFi reconnect failed.");
            }
        }
        else
        {
            Serial.println(" WiFi OK");
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS); // Kiểm tra mỗi 10 giây
    }
}

//  MQTT Task
void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.print(" Connecting to MQTT...");
        if (client.connect("ESP32", ACCESS_TOKEN, ""))
        {
            Serial.println(" MQTT Connected to ThingsBoard");
        }
        else
        {
            Serial.print(" Failed, rc=");
            Serial.print(client.state());
            Serial.println(" → retry in 5s");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

void MQTTask(void *pvParameters)
{
    for (;;)
    {
        if (!client.connected())
        {
            reconnectMQTT();
        }
        client.loop();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// 🌡 Task gửi DHT20 telemetry
void sendTelemetry(void *pvParameters)
{
    dht20.begin();

    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED && client.connected())
        {
            //  Lấy quyền truy cập I2C
            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                bool ok = dht20.read();
                if (!ok)
                {
                    float temp = dht20.getTemperature();
                    float hum = dht20.getHumidity();

                    StaticJsonDocument<128> doc;
                    doc["temperature"] = temp;
                    doc["humidity"] = hum;

                    char buffer[128];
                    serializeJson(doc, buffer);

                    client.publish("v1/devices/me/telemetry", buffer);
                    Serial.println(" Sent DHT20: " + String(buffer));
                }
                else
                {
                    Serial.println(" DHT20 read failed");
                }

                //  Trả quyền lại
                xSemaphoreGive(i2cMutex);
            }
            else
            {
                Serial.println(" I2C busy, skip DHT20 read");
            }
        }

        vTaskDelay(telemetryInterval / portTICK_PERIOD_MS);
    }
}

//  Task gửi MQ2 telemetry
void sendMQ2Data(void *pvParameters)
{
    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED && client.connected())
        {
            int mq2Value = analogRead(MQ2_AO_PIN);
            StaticJsonDocument<128> doc;
            doc["mq2_analog"] = mq2Value;

            char buffer[128];
            serializeJson(doc, buffer);
            client.publish("v1/devices/me/telemetry", buffer);

            Serial.println(" Sent MQ2: " + String(buffer));
        }

        vTaskDelay(mq2Interval / portTICK_PERIOD_MS);
    }
}

//  setup()
void setup()
{
    Serial.begin(115200);
    delay(1000);
    Wire.begin();
    i2cMutex = xSemaphoreCreateMutex();
    pinMode(MQ2_AO_PIN, INPUT);
    WiFi.begin(ssid, password);
    client.setServer(mqttServer, mqttPort);

    //  Tạo task FreeRTOS
    xTaskCreate(checkWiFiTask, "WiFiTask", 4096, NULL, 1, &WiFiTaskHandle);
    xTaskCreate(MQTTask, "MQTTask", 4096, NULL, 1, &MQTTaskHandle);
    xTaskCreate(sendTelemetry, "TelemetryTask", 4096, NULL, 1, &TelemetryTaskHandle);
    xTaskCreate(sendMQ2Data, "MQ2Task", 4096, NULL, 1, &MQ2TaskHandle);
}

void loop() {}
