#include <Arduino.h>
#include <WiFi.h> // WiFiNINA

#define ssid "PicoW"
#define apPassword "picoWpassword432"
#define maxClients 10

WiFiServer server(80);
IPAddress ClientsIP[maxClients];
WiFiClient clients[maxClients];
String clientName[maxClients];
byte timeout = 0;

void setup()
{
    Serial.begin(115200);

    Serial.println("Starting WiFi Access Point...");

    if (WiFi.beginAP(ssid, apPassword))
    {
        Serial.println("AP started!");
        Serial.print("Pico W IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Failed to start AP");
        while (true)
            ;
    }

    server.begin();
}

void loop()
{
    // Check for new clients
    WiFiClient newClient = server.available();
    if (newClient)
    {
        Serial.println("New client connected");

        // Find an empty slot for the new client
        for (int i = 0; i < maxClients; i++)
        {
            if (!clients[i] || !clients[i].connected())
            {
                clients[i] = newClient;
                ClientsIP[i] = newClient.remoteIP();

                while(!clients[i].available())
                    ;
                String Name = clients[i].read();
                clientName[i] = Name;
                Serial.print(String(clientName[i]) + " connected with IP: ");
                Serial.println(ClientsIP[i].toString());


                break;
            }
        }
    }

    // Check serial monitor for input and send to all connected clients
    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        for (int i = 0; i < maxClients; i++)
        {
            if (clients[i])
            {
                clients[i].print(input);
            }
        }
    }

    // Check for incoming data from clients and print to serial monitor and relay to other clients
    for (int i = 0; i < maxClients; i++)
    {
        if (clients[i] && clients[i].available())
        {
            String clientData = clients[i].readStringUntil('\n');
            clientData.trim(); // Remove any trailing newline characters
            Serial.print("Received from" + String(clientName[i]) + String(i) + ": ");
            Serial.println(clientData);

            // Relay to other clients
            for (int j = 0; j < maxClients; j++)
            {
                if (j != i && clients[j])
                {
                    clients[j].print(clientData);
                }
            }
        }
    }

    // Check for disconnected clients and remove them from the list
    for (int i = 0; i < maxClients; i++)
    {
        if (clients[i] && !clients[i].connected())
        {
            Serial.println(String(clientName[i]) + String(i) + "disconnected");
            clients[i].stop();
            clients[i] = WiFiClient(); // Reset the client slot
        }
    }
}
