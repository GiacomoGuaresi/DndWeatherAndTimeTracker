#include <CST816S.h>
#include <Preferences.h>
#include <Wire.h>
#include <TFT_eSPI.h>  // Libreria TFT
#include <SPI.h>

#define M_PI 3.14159265358979323846
#define SDA_PIN 4
#define SCL_PIN 5
#define INT_PIN 0
#define RST_PIN 1

TFT_eSPI tft = TFT_eSPI();
CST816S touch(SDA_PIN, SCL_PIN, RST_PIN, INT_PIN);
Preferences prefs;

// ================= ENUM =================
enum class Stagione { Inverno,
                      Primavera,
                      Estate,
                      Autunno };
enum class Periodo { Notte,
                     Mattina,
                     Giorno,
                     Sera };
enum class Bioma { Caldo,
                   Temperato,
                   Freddo };
enum class Meteo { Soleggiato,
                   Nuvoloso,
                   Pioggia,
                   Neve,
                   Tempesta,
                   SoleTorrido,
                   Gelo,
                   TempestaDiSabbia };

// ================= FUNZIONI TO STRING =================
String stagioneToString(Stagione s) {
  switch (s) {
    case Stagione::Inverno: return "Inverno";
    case Stagione::Primavera: return "Primavera";
    case Stagione::Estate: return "Estate";
    case Stagione::Autunno: return "Autunno";
  }
  return "?";
}

String periodoToString(Periodo p) {
  switch (p) {
    case Periodo::Notte: return "Notte";
    case Periodo::Mattina: return "Mattina";
    case Periodo::Giorno: return "Giorno";
    case Periodo::Sera: return "Sera";
  }
  return "?";
}

String biomaToString(Bioma b) {
  switch (b) {
    case Bioma::Caldo: return "Caldo";
    case Bioma::Temperato: return "Temperato";
    case Bioma::Freddo: return "Freddo";
  }
  return "?";
}

String meteoToString(Meteo m) {
  switch (m) {
    case Meteo::Soleggiato: return "Soleggiato";
    case Meteo::Nuvoloso: return "Nuvoloso";
    case Meteo::Pioggia: return "Pioggia";
    case Meteo::Neve: return "Neve";
    case Meteo::Tempesta: return "Tempesta";
    case Meteo::SoleTorrido: return "Sole torrido";
    case Meteo::Gelo: return "Gelo";
    case Meteo::TempestaDiSabbia: return "Tempesta di sabbia";
  }
  return "?";
}

// ================= RUMORE PERLIN 1D =================
double noiseHash(int x, int seed) {
  unsigned int n = (x * 1619 + seed * 31337) & 0x7fffffff;
  n = (n >> 13) ^ n;
  return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

double lerp(double a, double b, double t) {
  return a + t * (b - a);
}

double fade(double t) {
  return t * t * (3 - 2 * t);
}

double perlin1D(double x, int seed) {
  int xi = (int)floor(x);
  double xf = x - xi;
  double v1 = noiseHash(xi, seed);
  double v2 = noiseHash(xi + 1, seed);
  double u = fade(xf);
  return lerp(v1, v2, u);
}

// ================= GESTIONE TEMPO E STAGIONI =================
String timestampToTime(int ts) {
  int quartiGiorno = 96;  // 24 ore * 4
  int quarti = ts % quartiGiorno;
  int ore = quarti / 4;
  int minuti = (quarti % 4) * 15;

  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", ore, minuti);
  return String(buffer);
}

String timestampToDate(int ts) {
  int quartiGiorno = 96;  // 24 ore * 4
  int giorno = ts / quartiGiorno + 1;

  char buffer[15];
  snprintf(buffer, sizeof(buffer), "Giorno %d", giorno);
  return String(buffer);
}

Periodo getPeriodo(int ts) {
  int quarti = ts % 96;
  int ore = quarti / 4;
  int minuti = (quarti % 4) * 15;
  int totaleMinuti = ore * 60 + minuti;

  if (totaleMinuti >= 6 * 60 && totaleMinuti < 12 * 60) return Periodo::Mattina;
  if (totaleMinuti >= 12 * 60 && totaleMinuti < 18 * 60) return Periodo::Giorno;
  if (totaleMinuti >= 18 * 60 && totaleMinuti < 22 * 60) return Periodo::Sera;
  return Periodo::Notte;
}

Stagione getStagione(int ts, int offset) {
  int giorno = ts / 96 + 1;
  int giornoAnno = (giorno + offset) % 366;

  if (giornoAnno >= 80 && giornoAnno < 172) return Stagione::Primavera;
  if (giornoAnno >= 172 && giornoAnno < 266) return Stagione::Estate;
  if (giornoAnno >= 266 && giornoAnno <= 365) return Stagione::Autunno;
  return Stagione::Inverno;
}

double baseTemp(Stagione s, Bioma b) {
  double t = 0;
  switch (s) {
    case Stagione::Inverno: t = 0; break;
    case Stagione::Primavera: t = 10; break;
    case Stagione::Estate: t = 20; break;
    case Stagione::Autunno: t = 10; break;
  }
  switch (b) {
    case Bioma::Caldo: t += 10; break;
    case Bioma::Freddo: t -= 10; break;
    default: break;
  }
  return t;
}

double generaTemperatura(int ts, Stagione s, Bioma b, int seed) {
  int quarti = ts % 96;
  double ora = quarti / 4.0;
  double temp = baseTemp(s, b);
  double daily = 5 * sin((ora - 4) / 24.0 * 2 * M_PI);
  temp += daily;
  double noise = perlin1D(ts * 0.1, seed) * 2.0;
  temp += noise;
  return round(temp);
}

Meteo generaMeteo(int ts, int offset, Bioma b, int seed) {
  Stagione s = getStagione(ts, offset);
  double temp = generaTemperatura(ts, s, b, seed);
  double variabile = perlin1D(ts * 0.05, seed + 123);
  Meteo meteo = Meteo::Soleggiato;

  if (s == Stagione::Inverno) {
    if (temp <= 0) {
      if (variabile > 0.5) meteo = Meteo::Neve;
      else if (variabile > 0.0) meteo = Meteo::Nuvoloso;
    } else {
      if (variabile > 0.5) meteo = Meteo::Pioggia;
      else if (variabile > 0.0) meteo = Meteo::Nuvoloso;
    }
  } else if (s == Stagione::Estate) {
    if (variabile > 0.6) meteo = Meteo::Tempesta;
    else if (variabile > 0.3) meteo = Meteo::Nuvoloso;
  } else {
    if (variabile > 0.6) meteo = Meteo::Pioggia;
    else if (variabile > 0.2) meteo = Meteo::Nuvoloso;
  }

  if (temp > 35) meteo = Meteo::SoleTorrido;
  if (temp < -10) meteo = Meteo::Gelo;

  if (b == Bioma::Caldo) {
    if (meteo == Meteo::Tempesta) meteo = Meteo::TempestaDiSabbia;
    if (meteo == Meteo::Pioggia) meteo = Meteo::Soleggiato;
    if (meteo == Meteo::Neve) meteo = Meteo::Soleggiato;
  }
  if (b == Bioma::Freddo) {
    if (meteo == Meteo::SoleTorrido) meteo = Meteo::Soleggiato;
  }

  return meteo;
}

// ================= DISPLAY =================
void draw(bool fullRedraw, int ts, Stagione stagione, double temp, Meteo meteo, Periodo periodo) {
  // Definizione colori (convertiti HEX -> RGB565)
  uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42);  // #f04142 (rosso pulsanti)
  uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);       // #eae0c3 (sfondo pergamena)
  uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);     // #000000 (testo)
  uint16_t COL_ACCENT = tft.color565(0xEC, 0xEE, 0xF2);   // #eceef2 (decorazioni)

  int16_t w = tft.width();
  int16_t h = tft.height();
  int16_t cx = w / 2;
  int16_t cy = h / 2;

  // Sfondo color pergamena
  if (fullRedraw)
    tft.fillScreen(COL_BG);
  else {
    tft.fillRect(0, 0, w, 80, COL_BG);
    tft.fillRect(0, cy + 38, w, 24, COL_BG);
  }

  // Testi principali
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawCentreString(timestampToDate(ts), cx, 30, 4);

  tft.setTextColor(COL_PRIMARY, COL_BG);
  tft.drawCentreString(periodoToString(periodo) + " - " + stagioneToString(stagione), cx, 55, 2);

  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawCentreString(timestampToTime(ts), cx, cy - 20, 6);

  // Pulsanti
  if (fullRedraw) {
    int btn_w = 46;
    int btn_h = 70;
    int btn_y = cy - (btn_h / 2);

    tft.setTextColor(COL_BG, COL_PRIMARY);

    // Pulsante sinistro
    tft.fillRoundRect(0, btn_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("<", btn_w / 2, cy - 10, 4);

    // Pulsante destro
    tft.fillRoundRect(w - btn_w, btn_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString(">", w - btn_w / 2, cy - 10, 4);

    // Pulsante config
    btn_w = 70;
    btn_h = 36;

    tft.fillRoundRect((w - btn_w) / 2, h - btn_h, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("Config", w / 2, h - 25, 2);
  }


  // Sezioni descrittive (periodo, stagione, meteo)
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%d", (int)temp);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawCentreString(String(buffer) + "c " + meteoToString(meteo), cx, cy + 40, 4);
}


// ================= SETUP / LOOP =================
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

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  touch.begin();

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

  // Recupero il bioma (default Temperato)
  uint8_t biomaVal = prefs.getUChar("bioma", (uint8_t)Bioma::Temperato);
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

uint16_t getOffsetFromSeed(uint32_t seed) {
  // Hashing semplice basato su LCG
  uint32_t x = seed;
  x ^= x >> 16;
  x *= 0x45d9f3b;  // costante scelta per diffusione dei bit
  x ^= x >> 16;
  x *= 0x45d9f3b;
  x ^= x >> 16;

  return x % 366;  // rimappiamo tra 0 e 365
}

// --- Loop principale ---
void loop() {
  stagione = getStagione(ts, offset);
  temp = generaTemperatura(ts, stagione, bioma, seed);
  meteo = generaMeteo(ts, offset, bioma, seed);
  periodo = getPeriodo(ts);

  if (touch.available()) {
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
    int tx = y;      // X sullo schermo = Y del touch
    int ty = w - x;  // Y sullo schermo = larghezza - X del touch

    bool leftPressed = (tx >= 0 && tx <= btn_w && ty >= btn_y && ty <= btn_y + btn_h);
    bool rightPressed = (tx >= w - btn_w && tx <= w && ty >= btn_y && ty <= btn_y + btn_h);

    if (leftPressed || rightPressed) {
      handleTouch(leftPressed ? -1 : +1);  // -1 per sinistra, +1 per destra
    } else {
      resetTouchState();
    }
  } else {
    resetTouchState();
  }


  delay(20);  // loop veloce per gestire bene i tempi
}

// --- Gestione del tocco con autorepeat ---
void handleTouch(int dir) {
  unsigned long now = millis();

  if (!isPressing) {
    // Primo tocco
    isPressing = true;
    wasPressed = true;
    pressStartTime = now;
    lastTouchTime = now;
    changeTime(dir, 1);  // variazione iniziale di 1
    return;
  }

  // Quanto tempo è passato da quando tengo premuto
  unsigned long pressDuration = now - pressStartTime;

  // Intervallo di ripetizione dinamico
  unsigned long interval = 400;  // default: 1s

  if (pressDuration > 1000 && pressDuration <= 2000) interval = 300;
  else if (pressDuration > 2000 && pressDuration <= 3000) interval = 200;
  else if (pressDuration > 3000 && pressDuration <= 4000) interval = 100;
  else if (pressDuration > 4000 && pressDuration <= 6000) interval = 10;
  else if (pressDuration > 6000) {
    // dopo 6s → vai sempre al giorno successivo o precedente alle 08:00
    if (now - lastTouchTime >= 500) {  // ogni mezzo secondo
      int day = (ts - 1) / 96;         // calcola il giorno corrente
      int targetTs;

      if (dir == 1) {
        targetTs = (day + 1) * 96 + 32;  // giorno successivo alle 08:00
      } else {
        targetTs = (day - 1) * 96 + 32;  // giorno precedente alle 08:00
      }

      int offset = abs(targetTs - ts);  // offset sempre positivo

      changeTime(dir, offset);
      lastTouchTime = now;
    }
    return;
  }


  // gestione ripetizione "normale"
  if (now - lastTouchTime >= interval) {
    changeTime(dir, 1);
    lastTouchTime = now;
  }
}

// --- Reset quando rilascio ---
void resetTouchState() {
  isPressing = false;
  wasPressed = false;
}

// --- Modifica ts con i limiti ---
void changeTime(int dir, int step) {
  ts += dir * step;
  if (ts < 0) ts = 0;  // limite inferiore

  prefs.begin("meteo", false);  // namespace "meteo", modalità read/write
  prefs.putUInt("ts", ts);      // salvo il timestamp
  prefs.end();                  // chiudo per liberare risorse

  draw(false, ts, stagione, temp, meteo, periodo);
}
