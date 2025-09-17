#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Usa i pin da User_Setup.h

#define M_PI 3.14159265358979323846

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

  char buffer[10];
  snprintf(buffer, sizeof(buffer), "Day %d", giorno);
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
void draw(int ts, Stagione stagione, double temp, Meteo meteo, Periodo periodo) {
  tft.fillScreen(TFT_BLACK);
  int16_t w = tft.width();
  int16_t h = tft.height();
  int16_t cx = w / 2;
  int16_t cy = h / 2;

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(timestampToDate(ts), cx, 30, 4);
  tft.drawCentreString(timestampToTime(ts), cx, cy - 18, 6);

  int btn_w = 40;
  int btn_h = 60;
  int btn_y = cy - (btn_h / 2);

  tft.fillRect(0, btn_y, btn_w, btn_h, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.drawCentreString("<", btn_w / 2, cy - 6, 2);

  tft.fillRect(w - btn_w, btn_y, btn_w, btn_h, TFT_DARKGREY);
  tft.drawCentreString(">", w - btn_w / 2, cy - 6, 2);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int spacing = 20;
  int startY = cy + 50;
  tft.drawCentreString(periodoToString(periodo), cx, startY, 2);
  tft.drawCentreString(stagioneToString(stagione), cx, startY + spacing, 2);
  tft.drawCentreString(meteoToString(meteo), cx, startY + 2 * spacing, 2);

  // Serial.print(biomaToString(bioma));
  // Serial.print(temp);
}

// ================= SETUP / LOOP =================

int ts = 0;
int offset = 0;
int seed = 0;
Bioma bioma = Bioma::Temperato;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // TODO: Recuperare il seed dalla EEPROM
  seed = 0;

  // TODO: randomizzare l'offset (aggiungendo 0-365) basandosi sul Seed
  offset = 0;

    // TODO: Recuperare il TS dalla EEPROM
    ts = 0;

  // TODO: recuperare il bioma dalla EEPROM
  Bioma bioma = Bioma::Temperato;
}

void loop() {
  ts++;

  Stagione stagione = getStagione(ts, offset);
  double temp = generaTemperatura(ts, stagione, bioma, seed);
  Meteo meteo = generaMeteo(ts, offset, bioma, seed);
  Periodo periodo = getPeriodo(ts);

  draw(ts, stagione, temp, meteo, periodo);
  delay(100);
}
