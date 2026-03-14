#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "PicoW"
#define SERVER_IP "192.168.4.1"
#define SERVER_PORT 80
char WIFI_PASSWORD[64] = ""; // Assuming a maximum password length of 64 characters
WiFiClient client;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    // Retreive password from user input
    Serial.println("What is the WiFi password for " + String(WIFI_SSID) + "?");
    while (!Serial.available())
        ; // Wait for user input

    Serial.readStringUntil('\n').toCharArray(WIFI_PASSWORD, sizeof(WIFI_PASSWORD)); // Read the password and store it in the WIFI_PASSWORD variable

    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi. IP address: " + WiFi.localIP().toString());

    // Connect to the server
    Serial.println("Connecting to server at " + String(SERVER_IP) + ":" + String(SERVER_PORT) + "...");
    if (!client.connect(SERVER_IP, SERVER_PORT))
    {
        Serial.println("Connection to server failed");
    }
    else
    {
        Serial.println("Connected to server");
    }
}

void loop()
{
    if (!client.connected())
    {
        Serial.println("Disconnected from server. Please restart the device.");
    }

    if (client.available())
    {
        String message = client.readStringUntil('\n');
        Serial.println("Received from server: " + message);
    }

    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        client.println(input);
    }
}
