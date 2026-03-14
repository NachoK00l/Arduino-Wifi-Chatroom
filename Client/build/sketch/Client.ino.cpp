#line 1 "C:\\Users\\zeno7\\Documents\\Arduino-Wifi-Chatroom\\Client\\Client.ino"
#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "PicoW"
#define SERVER_IP "192.168.4.1"
String WIFI_PASSWORD;
bool connected = false;
WiFiClient client;

#line 10 "C:\\Users\\zeno7\\Documents\\Arduino-Wifi-Chatroom\\Client\\Client.ino"
void setup();
#line 17 "C:\\Users\\zeno7\\Documents\\Arduino-Wifi-Chatroom\\Client\\Client.ino"
void loop();
#line 10 "C:\\Users\\zeno7\\Documents\\Arduino-Wifi-Chatroom\\Client\\Client.ino"
void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Password?");
}

void loop()
{
    if(!connected && Serial.available())
    {
        WIFI_PASSWORD = Serial.readStringUntil('\n');
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.print("Connecting to WiFi");

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Failed to connect to WiFi");
        }
        else 
        {
            Serial.println("Connected to WiFi");
        }
        if (!client.connect(SERVER_IP, 80))
        {
            Serial.println("Connection to server failed");
        }
        else
        {
            Serial.println("Connected to server\n");
        }
    }
    while(connected)
    {
        if (!client.connected())
        {
            Serial.println("Disconnected from server. Please restart the device.");
            connected == false;
        }

        if (client.available())
        {
            String line = client.readStringUntil('\n');
            Serial.println("Received from server: " + line);
        }

        if (Serial.available())
        {
            String input = Serial.readStringUntil('\n');
            client.println(input);
        }
    }
}

