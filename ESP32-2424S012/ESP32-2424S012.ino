#include <CST816S.h>
#include <Preferences.h>
#include <Wire.h>
#include <TFT_eSPI.h>  // Libreria TFT
#include <SPI.h>

#include "enums.h"

#define M_PI 3.14159265358979323846
#define SDA_PIN 4
#define SCL_PIN 5
#define INT_PIN 0
#define RST_PIN 1

TFT_eSPI tft = TFT_eSPI();
CST816S touch(SDA_PIN, SCL_PIN, RST_PIN, INT_PIN);
Preferences prefs;

unsigned long lastTouchTime = 0;
unsigned long pressStartTime = 0;
bool isPressing = false;
bool wasPressed = false;

uint32_t seed;
uint16_t offset;
int ts;
Bioma bioma;
Stagione stagione;
double temp;
Meteo meteo;
Periodo periodo;

Screen screen;
int brightness;
int ledChannel = 0;
int freq = 5000;      // 5 kHz
int resolution = 10;  // 10 bit (0–1023)

#include "noise.h"
#include "time_and_weather.h"
#include "display_and_touch.h"

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  pinMode(TFT_BL, OUTPUT);
  // digitalWrite(TFT_BL, HIGH);

  touch.begin();

  screen = Screen::MainScreen;

  prefs.begin("meteo", false);

  // Recupero il seed, se non esiste lo genero
  seed = prefs.getUInt("seed", 0);
  if (seed == 0) {
    prefs.putUInt("seed", 1);
  } else {
    Serial.printf("Seed caricato: %lu\n", seed);
  }

  // Offset dipende dal seed
  offset = getOffsetFromSeed(seed);
  Serial.printf("Offset calcolato: %u\n", offset);

  // Recupero il timestamp (default 0)
  ts = prefs.getUInt("ts", 0);
  Serial.printf("Timestamp: %lu\n", ts);

  // Recupero il bioma (default Temperate)
  uint8_t biomaVal = prefs.getUChar("bioma", (uint8_t)Bioma::Temperate);
  bioma = (Bioma)biomaVal;
  Serial.printf("Bioma: %u\n", bioma);

  // Recupero la luminosità (default 1024)
  brightness = prefs.getUInt("brightness", 1024);
  Serial.printf("brightness: %lu\n", brightness);


  // Disegno la prima schermata
  stagione = getStagione(ts, offset);
  temp = generaTemperatura(ts, stagione, bioma, seed);
  meteo = generaMeteo(ts, offset, bioma, seed);
  periodo = getPeriodo(ts);

  draw(true, ts, stagione, temp, meteo, periodo);

  prefs.end();

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(TFT_BL, ledChannel);

  int equivalent_brightness = gammaCorrect(brightness);

  // Animazione fade-in
  for (int ramp = 0; ramp <= equivalent_brightness; ramp += 8) {
    ledcWrite(ledChannel, ramp);
    delay(20);
  }

  // Assicura il valore finale preciso
  ledcWrite(ledChannel, equivalent_brightness);
}

void loop() {
  checkTouchInput();

  delay(20);  // loop veloce per gestire bene i tempi
}