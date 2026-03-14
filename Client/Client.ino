#include <Arduino.h>
#include <WiFi.h>
#include "ChatroomClient.h"

#define WIFI_SSID "PicoW"
#define SERVER_IP "192.168.4.1"
#define SERVER_PORT 80
char WIFI_PASSWORD[64] = ""; // Assuming a maximum password length of 64 characters
WiFiClient client;

// Function prototypes for event handlers
void onMessageReceived(String message);
void onMessageSent(String message);
void onUserJoined(String username);
void onUserLeft(String username);
void onDisconnect(String reason);

ChatroomClient chatroomClient(onMessageReceived, onMessageSent, onUserJoined, onUserLeft, onDisconnect);

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

    chatroomClient.connectToServer(SERVER_IP, SERVER_PORT);
}

void loop()
{
    chatroomClient.update();
}

void onMessageReceived(String message)
{
    Serial.println("Message received: " + message);
}

void onMessageSent(String message)
{
    Serial.println("Message sent: " + message);
}

void onUserJoined(String username)
{
    Serial.println("User joined: " + username);
}

void onUserLeft(String username)
{
    Serial.println("User left: " + username);
}

void onDisconnect(String reason)
{
    Serial.println("Disconnected from server: " + reason);
}
