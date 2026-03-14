#include <WiFi.h>

class ChatroomClient
{
    void (*onMessageReceived)(String);
    void (*onMessageSent)(String);
    void (*onUserJoined)(String);
    void (*onUserLeft)(String);
    void (*onDisconnect)(String);

    WiFiClient client;
    bool notified = false;

public:
    ChatroomClient(void (*onMessageReceived)(String), void (*onMessageSent)(String), void (*onUserJoined)(String), void (*onUserLeft)(String), void (*onDisconnect)(String))
    {
        this->onMessageReceived = onMessageReceived;
        this->onMessageSent = onMessageSent;
        this->onUserJoined = onUserJoined;
        this->onUserLeft = onUserLeft;
        this->onDisconnect = onDisconnect;
    }

    void connectToServer(const char *serverIp, uint16_t serverPort)
    {
        Serial.println("Connecting to server at " + String(serverIp) + ":" + String(serverPort) + "...");
        if (!client.connect(serverIp, serverPort))
        {
            Serial.println("Connection to server failed");
        }
        else
        {
            Serial.println("Connected to server");
        }
    }

    void update()
    {
        if (!client.connected() && !notified)
        {
            onDisconnect("Connection to server lost");
            notified = true;
        }

        if (client.available())
        {
            String message = client.readStringUntil('\n');
            onMessageReceived(message);
        }

        if (Serial.available())
        {
            String input = Serial.readStringUntil('\n');
            client.println(input);
            onMessageSent(input);
        }
    }
};