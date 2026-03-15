#include <Arduino.h>
#include <WiFi.h> // WiFiNINA

#define ssid "PicoW"
#define apPassword "picoWpassword432"
#define maxClients 10
#define TIMEOUT 5000

WiFiServer server(80);
IPAddress ClientsIP[maxClients];
WiFiClient clients[maxClients];
String clientName[maxClients];
unsigned long lastSeen[maxClients];
unsigned long start = 0;
unsigned long debug = 0;
bool debugMode = false;
bool command = false;

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
                {
                    if(millis() - start > 2000) break;
                }
                String Name = clients[i].readStringUntil('\n');
                clientName[i] = Name;
                lastSeen[i] = millis();
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
        input.trim();

        if(input == "--debugmode")
        {

            debugMode = !debugMode;
            command = true;
        }

        for (int i = 0; i < maxClients; i++)
        {
            if (clients[i] && !command)
            {
                clients[i].println("Server: " + String(input));
            }
        }
    }
    command = false;

    // Check for incoming data from clients and print to serial monitor and relay to other clients
    for (int i = 0; i < maxClients; i++)
    {
        if (clients[i] && clients[i].available())
        {
            String clientData = clients[i].readStringUntil('\n');
            clientData.trim(); // Remove any trailing newline characters
            lastSeen[i] = millis();

            if(clientData == "PING")
            continue;

            Serial.print("Received from " + String(clientName[i]) +": ");
            Serial.println(clientData);

            // Relay to other clients
            for (int j = 0; j < maxClients; j++)
            {
                if (j != i && clients[j] && clients[j].connected())
                {
                    clients[j].println(String(clientName[i]) + clientData);
                }
            }
        }
    }

    // Check for disconnected clients and remove them from the list
    for (int i = 0; i < maxClients; i++)
    {
        if (clients[i])
        {
            if(millis() - lastSeen[i] > TIMEOUT || !clients[i].connected())
            {
                Serial.println(String(clientName[i]) + " disconnected");
                clients[i].stop();
                clients[i] = WiFiClient(); // Reset the client slot
                clientName[i] = "";
                ClientsIP[i] = IPAddress();
                lastSeen[i] = 0;
            }
        }
    }
    if(debugMode)
    {
        if(millis() - debug > 1000)
        {
            debug = millis();
            Serial.println("---- Client Status ----");
            for (int i = 0; i < maxClients; i++)
            {
                Serial.print("Slot ");
                Serial.print(i);

                Serial.print(" | Name: ");
                Serial.print(clientName[i]);

                Serial.print(" | IP: ");
                Serial.print(ClientsIP[i]);

                Serial.print(" | Connected: ");
                Serial.print(clients[i].connected());

                Serial.print(" | LastSeen: ");
                Serial.print(lastSeen[i]);

                Serial.print(" | Timeout in: ");
                Serial.println(TIMEOUT - (millis() - lastSeen[i]));
            }

            Serial.println("-----------------------");
        }
    }
}
