#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>
#include <mc_client.h>

#define MAX_CLIENTS 2

WiFiServer server(25565);

McClient clients[MAX_CLIENTS];
bool clientConnected[MAX_CLIENTS] = {false};

void setup()
{
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("---------------------------------");

  server.begin();
}

void loop()
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (!clientConnected[i])
    {
      clients[i] = McClient(server.available());
      if (clients[i].connected())
      {
        clientConnected[i] = true;
        Serial.println("Client connected");
      }
      continue;
    }

    if (clients[i].connected())
    {
      clients[i].handle();
      continue;
    }

    clientConnected[i] = false;
    clients[i].stop();
    Serial.println("Client disconnected");
  }
}