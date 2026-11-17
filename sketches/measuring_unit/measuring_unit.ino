#include <DHT22.h>
#include <SPI.h>
#include <LoRa.h>

#define DHT_pin 2
DHT22 dht(DHT_pin);

const bool LORA_ENABLE = false;

void setup (){   
  Serial.begin(115200);
  Serial.println("Connection Successful");

  delay(2000);
  Serial.println("");
  Serial.println("Device will now attempt to establish connection with LoRa module...");

  if (LORA_ENABLE) {
    LoRa.setPins(5, 14, 26);

    if (LoRa.begin(433E6) == 0) {
      Serial.println("RA-01 Device could not be found on the ESP 32 Unit");
      while (1) delay(100);
    }

    LoRa.setSyncWord(0xA5);

    
  }
}

void loop(){
  // Placeholder
}

void perform_dht_measurements() {
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  if (dht.getLastError() != dht.OK){
    Serial.print("Error: ");
    Serial.println(dht.getLastError());

  }
  Serial.print("Humidity: ");
  Serial.print(humidity,2);
  Serial.print("\n");

  Serial.print("Temperature: ");
  Serial.print(temperature,2);
  Serial.print("\n");
}
