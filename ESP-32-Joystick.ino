#include <WiFi.h>
#include <WiFiClient.h>

#define LED_BUILTIN 2

const char* ssid = "Jne829";
const char* password = "33333333";
const uint16_t port = 5000;
char host[16];
volatile bool found = false;
WiFiClient client;

void scanIP(void * parameter) {
  int startIP = *((int*)parameter);
  WiFiClient localClient;
  Serial.print("Task starting for IP range: ");
  Serial.println(startIP);

  for (int i = startIP; i < startIP + 25 && !found; i++) {
    char ipToTest[20];
    sprintf(ipToTest, "%s%d", host, i);
    Serial.print("Trying: ");
    Serial.println(ipToTest);

    if (localClient.connect(ipToTest, port)) {
      localClient.stop();
      strcpy(host, ipToTest);
      Serial.print("Connected to: ");
      Serial.println(host);
      found = true;
      break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  Serial.println("Task ending");
  delete (int*)parameter;
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(LED_BUILTIN, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  IPAddress ip = WiFi.localIP();
  sprintf(host, "%d.%d.%d.", ip[0], ip[1], ip[2]);

  int segmentSize = 2;
  for (int i = 0; i <= 255; i += segmentSize) {
    int *startIP = new int(i);
    xTaskCreate(scanIP, "ScanIPTask", 10000, startIP, 1, NULL);
    Serial.print("Creating task for range starting at ");
    Serial.println(i);
  }
}

void loop() {
  if (found && client.connected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    int x = analogRead(34) / 4;
    int y = analogRead(35) / 4;
    if(x > 475 && x < 520)
      x = 512;
    if(y > 455 && y < 520)
      y = 512;
    client.println(String(x) + "," + String(y));
    Serial.println(String(x) + "," + String(y));
    delay(10);
  } else if (found && !client.connected()) {
    client.connect(host, port);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
