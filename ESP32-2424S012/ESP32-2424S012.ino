#include <CST816S.h>
#include <Preferences.h>
#include <Wire.h>
#include <TFT_eSPI.h> // Libreria TFT
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

#include "noise.h"
#include "time_and_weather.h"
#include "display_and_touch.h"

void setup()
{
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    touch.begin();

    prefs.begin("meteo", false);

    // Recupero il seed, se non esiste lo genero
    seed = prefs.getUInt("seed", 0);
    if (seed == 0)
    {
        prefs.putUInt("seed", 1);
    }
    else
    {
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

    // Disegno la prima schermata
    stagione = getStagione(ts, offset);
    temp = generaTemperatura(ts, stagione, bioma, seed);
    meteo = generaMeteo(ts, offset, bioma, seed);
    periodo = getPeriodo(ts);

    draw(true, ts, stagione, temp, meteo, periodo);

    prefs.end();
}

void loop()
{
    stagione = getStagione(ts, offset);
    temp = generaTemperatura(ts, stagione, bioma, seed);
    meteo = generaMeteo(ts, offset, bioma, seed);
    periodo = getPeriodo(ts);

    if (touch.available())
    {
        int16_t w = tft.width();
        int16_t h = tft.height();
        int16_t cx = w / 2;
        int16_t cy = h / 2;
        int btn_w = 40;
        int btn_h = 60;
        int btn_y = cy - (btn_h / 2);

        int x = touch.data.x;
        int y = touch.data.y;

        // --- Mappatura per rotazione 90° ---
        int tx = y;     // X sullo schermo = Y del touch
        int ty = w - x; // Y sullo schermo = larghezza - X del touch

        bool leftPressed = (tx >= 0 && tx <= btn_w && ty >= btn_y && ty <= btn_y + btn_h);
        bool rightPressed = (tx >= w - btn_w && tx <= w && ty >= btn_y && ty <= btn_y + btn_h);

        if (leftPressed || rightPressed)
        {
            handleTouch(leftPressed ? -1 : +1); // -1 per sinistra, +1 per destra
        }
        else
        {
            resetTouchState();
        }
    }
    else
    {
        resetTouchState();
    }

    delay(20); // loop veloce per gestire bene i tempi
}