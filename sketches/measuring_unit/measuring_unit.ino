#include <DHT22.h>

#define DHT_pin 2
DHT22 dht(DHT_pin);

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

void setup (){   
  Serial.begin(9600);
  Serial.println("connection succesfull");
}

void loop(){
  perform_dht_measurements();

  delay(4000);
}