#include <WiFi.h>

const char *SSID = "eduroam-2.4Ghz";
const char *USER = "######@net.aau.dk";
const char *PASS = "######";

void setup() {
  Serial.begin(115200);
  Serial.println("Ready!");
  Serial.print("Attempting WiFi connection to \'");
  Serial.print(SSID);
  Serial.println("\'");

  // ENUM for WiFi Client
  WiFi.mode(WIFI_STA);

  WiFi.begin(SSID, WPA2_AUTH_PEAP, USER, USER, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Successfully connected to WiFi Network \'");
  Serial.print(SSID);
  Serial.println("\'");
  Serial.print("The IP Address of this Host is ");
  Serial.println(WiFi.localIP());
  
}

void loop() {

}