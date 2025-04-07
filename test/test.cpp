// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <Wire.h>

// // Wi-Fi Credentials
// const char *ssid = "viet";
// const char *password = "20252025";

// // ThingsBoard MQTT Broker
// const char *mqttServer = "app.coreiot.io";
// const int mqttPort = 1883;
// const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7"; // Token của thiết bị MQ2

// WiFiClient espClient;
// PubSubClient client(espClient);

// // Chân Analog MQ2 (AO)
// #define MQ2_AO_PIN 1

// unsigned long lastTelemetryTime = 0;
// const long telemetryInterval = 5000; // Gửi dữ liệu mỗi 5 giây

// // Task handles
// TaskHandle_t WiFiTaskHandle = NULL;
// TaskHandle_t MQTTaskHandle = NULL;
// TaskHandle_t MQ2TaskHandle = NULL;

// // **Kết nối WiFi** - Tạo một tác vụ riêng cho WiFi
// void WiFiTask(void *pvParameters)
// {
//     // Kết nối Wi-Fi một lần
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         Serial.print(".");
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 giây
//     }
//     Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

//     // Kiểm tra kết nối Wi-Fi trong chu kỳ 10 giây
//     unsigned long lastCheck = millis();
//     while (true)
//     {
//         if (millis() - lastCheck >= 10000) // Kiểm tra kết nối Wi-Fi mỗi 10 giây
//         {
//             lastCheck = millis();
//             if (WiFi.status() != WL_CONNECTED)
//             {
//                 Serial.println("WiFi lost, reconnecting...");
//                 WiFi.reconnect();
//                 while (WiFi.status() != WL_CONNECTED)
//                 {
//                     vTaskDelay(pdMS_TO_TICKS(1000)); // Chờ trong khi kết nối lại
//                 }
//                 Serial.println("WiFi reconnected: " + WiFi.localIP().toString());
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Tiếp tục thực hiện công việc khác nếu không có sự thay đổi
//     }
// }

// // **Kết nối MQTT đến ThingsBoard** - Tạo một tác vụ riêng cho MQTT
// void MQTTask(void *pvParameters)
// {
//     // Kết nối MQTT một lần
//     while (!client.connected())
//     {
//         Serial.print("Connecting to MQTT...");
//         if (client.connect("ESP32_MQ2", ACCESS_TOKEN, ""))
//         {
//             Serial.println("Connected to ThingsBoard!");
//         }
//         else
//         {
//             Serial.print("Failed, rc=");
//             Serial.print(client.state());
//             Serial.println(" retrying in 5 seconds...");
//             vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 5 giây
//         }
//     }

//     // Kiểm tra kết nối MQTT trong chu kỳ 10 giây
//     unsigned long lastCheck = millis();
//     while (true)
//     {
//         if (millis() - lastCheck >= 10000) // Kiểm tra kết nối MQTT mỗi 10 giây
//         {
//             lastCheck = millis();
//             if (!client.connected())
//             {
//                 Serial.println("MQTT disconnected, reconnecting...");
//                 while (!client.connected())
//                 {
//                     if (client.connect("ESP32_MQ2", ACCESS_TOKEN, ""))
//                     {
//                         Serial.println("Reconnected to ThingsBoard!");
//                     }
//                     else
//                     {
//                         Serial.print("Failed to reconnect, rc=");
//                         Serial.print(client.state());
//                         Serial.println(" retrying in 5 seconds...");
//                         vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 5 giây nếu không kết nối lại được
//                     }
//                 }
//             }
//         }
//         client.loop();                   // Giữ MQTT hoạt động
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Kiểm tra mỗi giây
//     }
// }

// // **Gửi dữ liệu MQ2 lên ThingsBoard** - Tạo một tác vụ riêng để gửi dữ liệu MQ2
// void sendMQ2Data(void *pvParameters)
// {
//     while (1)
//     {
//         int gasAnalogValue = analogRead(MQ2_AO_PIN);
//         Serial.println(gasAnalogValue);

//         StaticJsonDocument<128> doc;
//         doc["mq2_analog"] = gasAnalogValue;

//         char buffer[128];
//         serializeJson(doc, buffer);

//         // Gửi dữ liệu nếu MQTT đã kết nối
//         if (client.connected())
//         {
//             client.publish("v1/devices/me/telemetry", buffer);
//             // Serial.println("Sent MQ2 data: " + String(buffer));
//         }
//         else
//         {
//             Serial.println("MQTT not connected, cannot send MQ2 data!");
//         }

//         vTaskDelay(pdMS_TO_TICKS(telemetryInterval)); // Gửi dữ liệu mỗi 5 giây
//     }
// }

// void setup()
// {
//     Serial.begin(115200);

//     pinMode(MQ2_AO_PIN, INPUT);

//     // Create tasks
//     xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 1, &WiFiTaskHandle);
//     xTaskCreate(MQTTask, "MQTTask", 4096, NULL, 1, &MQTTaskHandle);
//     xTaskCreate(sendMQ2Data, "SendMQ2Data", 2048, NULL, 1, &MQ2TaskHandle);
// }

// void loop()
// {
//     // FreeRTOS handles tasks, so no need for code here
// }

// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <Wire.h>

// // Wi-Fi Credentials
// const char *ssid = "viet";
// const char *password = "20252025";

// // ThingsBoard MQTT Broker
// const char *mqttServer = "app.coreiot.io";
// const int mqttPort = 1883;
// const char *ACCESS_TOKEN = "gB69jhkhOWD1wEYj6mm7"; // Token của thiết bị MQ2

// WiFiClient espClient;
// PubSubClient client(espClient);

// // Chân Analog MQ2 (AO)
// #define MQ2_AO_PIN 1

// unsigned long lastTelemetryTime = 0;
// const long telemetryInterval = 5000; // Gửi dữ liệu mỗi 5 giây

// // Task handles
// TaskHandle_t WiFiTaskHandle = NULL;
// TaskHandle_t MQTTaskHandle = NULL;
// TaskHandle_t MQ2TaskHandle = NULL;

// // **Kết nối WiFi** - Tạo một tác vụ riêng cho WiFi
// void WiFiTask(void *pvParameters)
// {
//     WiFi.begin(ssid, password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         Serial.print(".");
//         vTaskDelay(pdMS_TO_TICKS(2000)); // Delay 2 giây
//     }
//     Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

//     unsigned long lastCheck = millis();
//     while (true)
//     {
//         if (millis() - lastCheck >= 10000) // Kiểm tra Wi-Fi mỗi 10 giây
//         {
//             lastCheck = millis();
//             if (WiFi.status() != WL_CONNECTED)
//             {
//                 Serial.println("WiFi lost, reconnecting...");
//                 WiFi.reconnect();
//                 while (WiFi.status() != WL_CONNECTED)
//                 {
//                     vTaskDelay(pdMS_TO_TICKS(1000)); // Chờ trong khi kết nối lại
//                 }
//                 Serial.println("WiFi reconnected: " + WiFi.localIP().toString());
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Tiếp tục kiểm tra mỗi giây
//     }
// }

// // **Kết nối MQTT đến ThingsBoard** - Tạo một tác vụ riêng cho MQTT
// void MQTTask(void *pvParameters)
// {
//     while (!client.connected())
//     {
//         Serial.print("Connecting to MQTT...");
//         if (client.connect("ESP32_MQ2", ACCESS_TOKEN, ""))
//         {
//             Serial.println("Connected to ThingsBoard!");
//         }
//         else
//         {
//             Serial.print("Failed, rc=");
//             Serial.print(client.state());
//             Serial.println(" retrying in 5 seconds...");
//             vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 5 giây
//         }
//     }

//     unsigned long lastCheck = millis();
//     while (true)
//     {
//         if (millis() - lastCheck >= 10000) // Kiểm tra MQTT mỗi 10 giây
//         {
//             lastCheck = millis();
//             if (!client.connected())
//             {
//                 Serial.println("MQTT disconnected, reconnecting...");
//                 while (!client.connected())
//                 {
//                     if (client.connect("ESP32_MQ2", ACCESS_TOKEN, ""))
//                     {
//                         Serial.println("Reconnected to ThingsBoard!");
//                     }
//                     else
//                     {
//                         Serial.print("Failed to reconnect, rc=");
//                         Serial.print(client.state());
//                         Serial.println(" retrying in 5 seconds...");
//                         vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 5 giây
//                     }
//                 }
//             }
//         }
//         client.loop();                   // Giữ MQTT hoạt động
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Kiểm tra mỗi giây
//     }
// }

// // **Gửi dữ liệu MQ2 lên ThingsBoard** - Tạo một tác vụ riêng để gửi dữ liệu MQ2
// void sendMQ2Data(void *pvParameters)
// {
//     while (1)
//     {
//         int gasAnalogValue = analogRead(MQ2_AO_PIN);
//         Serial.println(gasAnalogValue);

//         StaticJsonDocument<128> doc;
//         doc["mq2_analog"] = gasAnalogValue;

//         char buffer[128];
//         serializeJson(doc, buffer);

//         // Gửi dữ liệu nếu MQTT đã kết nối
//         if (client.connected())
//         {
//             client.publish("v1/devices/me/telemetry", buffer);
//         }
//         else
//         {
//             Serial.println("MQTT not connected, cannot send MQ2 data!");
//         }

//         vTaskDelay(pdMS_TO_TICKS(telemetryInterval)); // Gửi dữ liệu mỗi 5 giây
//     }
// }

// void setup()
// {
//     Serial.begin(115200);

//     pinMode(MQ2_AO_PIN, INPUT);

//     // Create tasks
//     xTaskCreate(WiFiTask, "WiFiTask", 2048, NULL, 1, &WiFiTaskHandle);
//     xTaskCreate(MQTTask, "MQTTask", 2048, NULL, 1, &MQTTaskHandle);
//     xTaskCreate(sendMQ2Data, "SendMQ2Data", 2048, NULL, 1, &MQ2TaskHandle);
// }

// void loop()
// {
//     // FreeRTOS handles tasks, so no need for code here
// }
