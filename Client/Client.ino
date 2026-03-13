#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "PicoW"
#define WIFI_PASSWORD "picoWpassword432"
#define SERVER_IP "192.168.4.1"

WiFiClient client;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

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
        return;
    }

    Serial.println("Connected to WiFi");

    if (!client.connect(SERVER_IP, 80))
    {
        Serial.println("Connection to server failed");
        return;
    }

    Serial.println("Connected to server\n");
}

void loop()
{
    if (!client.connected())
    {
        Serial.println("Disconnected from server. Please restart the device.");
        while (true)
            ;
    }

    if (client.available())
    {
        String line = client.readStringUntil('\n');
        Serial.println("Received from server: " + line);
    }

    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        client.print(input);
    }
}
