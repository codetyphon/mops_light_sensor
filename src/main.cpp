#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define STASSID "ChinaUnion"
#define STAPSK "67cf23f9"

const char *ssid = STASSID;
const char *password = STAPSK;

const int btn = 13;

int powerState = -1;

char serverAddress[] = "esp8266.local";  // server address
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

ESP8266WebServer server(80);

void setup(void)
{
  pinMode(btn, INPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("connection to wifi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("lightsensor"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/status", []() {
    server.send(200, "text/plain", String(powerState));
  });

  server.begin();
}
void loop(void)
{
  MDNS.update();
  server.handleClient();
  int buttonState = digitalRead(btn);
  if (buttonState != powerState)
  {
    powerState = buttonState;
    Serial.println("button click");
    client.beginRequest();
    if(powerState==HIGH){
      client.get("/on");
    }else{
      client.get("/off");
    }
    client.endRequest();
    delay(1000);
  }
}