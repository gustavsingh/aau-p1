#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>

const char *SSID = "eduroam-2.4Ghz";

// Make sure not to upload your credentials :)
const char *USER = "#####@net.aau.dk";
const char *PASS = "#####";

// Toggle for LoRa module
const bool LORA_ENABLE = false;

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
  
  delay(2000);
  Serial.println("");
  Serial.println("Device will now attempt to establish connection with LoRa module...");

  if (LORA_ENABLE) {
    // Sets the LoRa Pins (NSS, RST, DIO0)
    LoRa.setPins(5, 14, 26);

    // Ensure the LoRa device can be found
    if (LoRa.begin(433E6) == 0) {
      Serial.println("RA-01 Device could not be found on the ESP 32 Unit");
      while (1) delay(100);
    }
    // Ensure this is the same on Slave/Sender Devices
    LoRa.setSyncWord(0xA5);

    // Callback to dataReceived Function and begin data retrieval
    LoRa.onReceive(dataReceived);
    LoRa.receive();

    Serial.println("RA-01 LoRa Unit was connected successfully");
  }
}

void loop() {

}

// Callback Function for Data Retrieval
void dataReceived(int packetSize) {
  Serial.println("Packet received");
  
  // TODO: Implement full data retrieval here
}
