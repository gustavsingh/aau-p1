#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <string.h>

/* ---- Ben ---- */
#define PIN_DHT22_DATA   4
#define PIN_JORD_ANALOG 34
#define PIN_LORA_NSS    5
#define PIN_LORA_RST    14
#define PIN_LORA_DIO0   26

/* ---- LoRa ---- */
#define LORA_FREKVENS   433E6   /* RA-01 = 433 MHz */
#define LORA_SYNC_WORD  0xA5     /* jeres private "netvaerks-id" */
#define LORA_TX_POWER   10       /* dBm, lovligt niv. i DK/EU paa 433 MHz */

/* ---- Maaling ---- */
#define NODE_ID          1
#define MAAL_INTERVAL_MS 10000UL
#define SOIL_MV_TOR   2347      /* jeres kalibrering: tørluft */
#define SOIL_MV_VAAT   380      /* jeres kalibrering: vand */

typedef struct {
  uint8_t  node_id;
  uint32_t maale_nr;
  float    temp_c;
  float    fugt_rh;
  float    jord_pct;
  uint16_t jord_mv;
} maaling_t;

DHT dht(PIN_DHT22_DATA, DHT22);

/* --- Jord: gennemsnit af 8 maalinger i millivolt --- */
uint16_t laes_jord_mv(void) {
  uint32_t sum = 0;
  int i;
  for (i = 0; i < 8; i++) {
    sum += analogReadMilliVolts(PIN_JORD_ANALOG);
    delay(5);
  }
  return (uint16_t)(sum / 8);
}

/* --- Jord: millivolt om til procent med jeres kalibrering --- */
float jord_pct_af_mv(uint16_t mv) {
  float pct = 100.0f * (float)(SOIL_MV_TOR - mv)
                    / (float)(SOIL_MV_TOR - SOIL_MV_VAAT);
  if (pct < 0.0f)   pct = 0.0f;   /* aldrig under 0%  */
  if (pct > 100.0f) pct = 100.0f; /* aldrig over 100% */
  return pct;
}

/* --- DHT22: laeser, proever op til 3 gange --- */
int laes_dht22(float *temp_ud, float *fugt_ud) {
  int forsoeg;
  for (forsoeg = 0; forsoeg < 3; forsoeg++) {
    float t = dht.readTemperature();
    float f = dht.readHumidity();
    if (!isnan(t) && !isnan(f)) {
      *temp_ud = t;    /* skriv igennem pointeren... */
      *fugt_ud = f;    /* ...og send svar ud til kalderen */
      return 0;
    }
    delay(2000);
  }
  return -1;
}

/* --- Sender hele maalingen via LoRa --- */
void send_maaling(maaling_t *m) {
  char payload[80];
  snprintf(payload, sizeof(payload), "%u,%.1f,%.1f,%.1f,%u,%lu",
           (unsigned)m->node_id, m->temp_c, m->fugt_rh, m->jord_pct,
           (unsigned)m->jord_mv, (unsigned long)m->maale_nr);

  LoRa.beginPacket();
  LoRa.write((const uint8_t *)payload, strlen(payload));
  int ok = LoRa.endPacket();

  Serial.print("Sendt: \"");
  Serial.print(payload);
  Serial.println(ok == 1 ? "\"" : "\"  <-- FEJL!");
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  dht.begin();

  LoRa.setPins(PIN_LORA_NSS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (LoRa.begin(LORA_FREKVENS) == 0) {
    Serial.println("FEJL: RA-01 ikke fundet. Tjek 3.3V, SPI-ledninger, antenne.");
    while (1) delay(100);
  }
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(LORA_TX_POWER, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(7);

  Serial.println("P1 sensor-node klar!");
}

void loop() {
  static uint32_t maale_nr = 0;   /* husker sit vaerdi mellem koersler */
  maaling_t m;

  m.node_id  = NODE_ID;
  m.maale_nr = maale_nr++;

  if (laes_dht22(&m.temp_c, &m.fugt_rh) != 0) {
    Serial.println("DHT22 fejler - sender fejlvaerdier.");
    m.temp_c  = -99.9f;
    m.fugt_rh = -1.0f;
  }

  m.jord_mv  = laes_jord_mv();
  m.jord_pct = jord_pct_af_mv(m.jord_mv);

  Serial.print("Temp: ");   Serial.print(m.temp_c);   Serial.print(" C, ");
  Serial.print("Fugt: ");   Serial.print(m.fugt_rh);  Serial.print(" %RH, ");
  Serial.print("Jord: ");   Serial.print(m.jord_pct);  Serial.print(" % (");
  Serial.print(m.jord_mv);  Serial.println(" mV)");

  send_maaling(&m);
  delay(MAAL_INTERVAL_MS);
}