#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DHT20.h"

const char *wifiSSID = "viet";
const char *wifiPassword = "20252025";

const char *mqttHost = "app.coreiot.io";
const int mqttPortNumber = 1883;
const char *mqttAccessToken = "gB69jhkhOWD1wEYj6mm7";

WiFiClient espWiFiClient;
PubSubClient mqttClient(espWiFiClient);

#define MQ2_SENSOR_PIN 1
DHT20 dhtSensor;

unsigned long prevTelemetryTimestamp = 0;
unsigned long prevMQ2Timestamp = 0;
const long telemetryUpdateInterval = 5000;
const long MQ2UpdateInterval = 5000;

TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t mqttTaskHandle = NULL;
TaskHandle_t mq2TaskHandle = NULL;
TaskHandle_t telemetryTaskHandle = NULL;

void connectToWiFi()
{
    Serial.print("Connecting to WiFi...");
    WiFi.begin(wifiSSID, wifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void connectToMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.print("Connecting to MQTT...");
        if (mqttClient.connect("ESP32_MQ2", mqttAccessToken, ""))
        {
            Serial.println("Connected to ThingsBoard!");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000)); 
        }
    }
}

void publishMQ2Data(void *pvParameters)
{
    while (1)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - prevMQ2Timestamp >= MQ2UpdateInterval)
        {
            prevMQ2Timestamp = currentMillis;

            int gasSensorValue = analogRead(MQ2_SENSOR_PIN);
            Serial.println(gasSensorValue);

            StaticJsonDocument<128> jsonDoc;
            jsonDoc["mq2_analog"] = gasSensorValue;

            char jsonBuffer[128];
            serializeJson(jsonDoc, jsonBuffer);

            if (mqttClient.connected())
            {
                mqttClient.publish("v1/devices/me/telemetry", jsonBuffer);
                Serial.println("Sent MQ2 data: " + String(jsonBuffer));
            }
            else
            {
                Serial.println("MQTT not connected, cannot send MQ2 data!");
                connectToMQTT(); 
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

void publishTelemetryData(void *pvParameters)
{
    float tempValue = 30;
    float humidityValue = 50;
    dhtSensor.begin();
    while (1)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - prevTelemetryTimestamp >= telemetryUpdateInterval)
        {
            prevTelemetryTimestamp = currentMillis;

            if (dhtSensor.read())
            {
                Serial.println(tempValue);
                Serial.println(humidityValue);

                StaticJsonDocument<128> jsonDoc;
                jsonDoc["temperature"] = tempValue;
                jsonDoc["humidity"] = humidityValue;

                char jsonBuffer[128];
                serializeJson(jsonDoc, jsonBuffer);

                if (mqttClient.connected())
                {
                    mqttClient.publish("v1/devices/me/telemetry", jsonBuffer);
                    Serial.println("Sent telemetry: " + String(jsonBuffer));
                }
                else
                {
                    Serial.println("MQTT not connected, cannot send telemetry data!");
                    connectToMQTT(); 
                }
            }
            else
            {
                Serial.println("Failed to read DHT20 sensor!");
            }

            tempValue++;
            humidityValue++;
        }
        vTaskDelay(pdMS_TO_TICKS(telemetryUpdateInterval)); 
    }
}

void wifiTask(void *pvParameters)
{
    connectToWiFi();
}

void mqttTask(void *pvParameters)
{
    mqttClient.setServer(mqttHost, mqttPortNumber);
    connectToMQTT();
}

void setup()
{
    Serial.begin(115200);

    pinMode(MQ2_SENSOR_PIN, INPUT);

    xTaskCreate(wifiTask, "wifiTask", 4096, NULL, 1, &wifiTaskHandle);
    xTaskCreate(mqttTask, "mqttTask", 4096, NULL, 1, &mqttTaskHandle);
    xTaskCreate(publishMQ2Data, "mq2Task", 2048, NULL, 1, &mq2TaskHandle);
    xTaskCreate(publishTelemetryData, "telemetryTask", 2048, NULL, 1, &telemetryTaskHandle);
}

void loop()
{
    // TODO
}
