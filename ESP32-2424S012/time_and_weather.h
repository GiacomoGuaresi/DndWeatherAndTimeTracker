String timestampToTime(int ts) {
  int quartiDay = 96;  // 24 ore * 4
  int quarti = ts % quartiDay;
  int ore = quarti / 4;
  int minuti = (quarti % 4) * 15;

  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", ore, minuti);
  return String(buffer);
}

String timestampToDate(int ts) {
  int quartiDay = 96;  // 24 ore * 4
  int Day = ts / quartiDay + 1;

  char buffer[15];
  snprintf(buffer, sizeof(buffer), "Day %d", Day);
  return String(buffer);
}

Periodo getPeriodo(int ts) {
  int quarti = ts % 96;
  int ore = quarti / 4;
  int minuti = (quarti % 4) * 15;
  int totaleMinuti = ore * 60 + minuti;

  if (totaleMinuti >= 6 * 60 && totaleMinuti < 12 * 60)
    return Periodo::Moring;
  if (totaleMinuti >= 12 * 60 && totaleMinuti < 18 * 60)
    return Periodo::Day;
  if (totaleMinuti >= 18 * 60 && totaleMinuti < 22 * 60)
    return Periodo::Evening;
  return Periodo::Night;
}

Stagione getStagione(int ts, int offset) {
  int Day = ts / 96 + 1;
  int DayAnno = (Day + offset) % 366;

  if (DayAnno >= 80 && DayAnno < 172)
    return Stagione::Spring;
  if (DayAnno >= 172 && DayAnno < 266)
    return Stagione::Summer;
  if (DayAnno >= 266 && DayAnno <= 365)
    return Stagione::Autumn;
  return Stagione::Winter;
}

double baseTemp(Stagione s, Bioma b) {
  double t = 0;
  switch (s) {
    case Stagione::Winter:
      t = 0;
      break;
    case Stagione::Spring:
      t = 10;
      break;
    case Stagione::Summer:
      t = 20;
      break;
    case Stagione::Autumn:
      t = 10;
      break;
  }
  switch (b) {
    case Bioma::Hot:
      t += 10;
      break;
    case Bioma::Cold:
      t -= 10;
      break;
    default:
      break;
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

Meteo generaMeteo(int ts, int offset, Bioma b, int seed)
{
    Stagione s = getStagione(ts, offset);
    double variabile = perlin1D(ts * 0.05, seed + 123); // tra -1 e 1
    variabile = (variabile + 1.0) * 0.5; // normalizza in [0,1]

    // Tabella dei meteo possibili per (Bioma, Stagione)
    // Nota: più a sinistra = più comune, più a destra = più raro
    const Meteo hot_summer[]    = { Meteo::ScorchingSun, Meteo::Clear, Meteo::Sandstorm };
    const Meteo hot_autumn[]    = { Meteo::Clear, Meteo::Cloud, Meteo::Sandstorm };
    const Meteo hot_winter[]    = { Meteo::Clear, Meteo::Cloud, Meteo::Sandstorm, Meteo::Rain };
    const Meteo hot_spring[]    = { Meteo::Clear, Meteo::Cloud, Meteo::Sandstorm, Meteo::Rain };

    const Meteo temp_summer[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain };
    const Meteo temp_autumn[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain, Meteo::Storm };
    const Meteo temp_winter[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain, Meteo::Snow };
    const Meteo temp_spring[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain, Meteo::Storm };

    const Meteo cold_summer[]   = { Meteo::Clearar, Meteo::Cloud, Meteo::Rain };
    const Meteo cold_autumn[]   = { Meteo::Clearar, Meteo::Cloud, Meteo::Rain, Meteo::Snow };
    const Meteo cold_winter[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain, Meteo::Snow, Meteo::Frost };
    const Meteo cold_spring[]   = { Meteo::Clear, Meteo::Cloud, Meteo::Rain, Meteo::Snow };

    const Meteo *lista = nullptr;
    int n = 0;

    // Scelta della lista in base a bioma e stagione
    if (b == Bioma::Hot) {
        if (s == Stagione::Summer) { lista = hot_summer; n = 4; }
        else if (s == Stagione::Autumn) { lista = hot_autumn; n = 3; }
        else if (s == Stagione::Winter) { lista = hot_winter; n = 4; }
        else { lista = hot_spring; n = 4; }
    }
    else if (b == Bioma::Temperate) {
        if (s == Stagione::Summer) { lista = temp_summer; n = 3; }
        else if (s == Stagione::Autumn) { lista = temp_autumn; n = 4; }
        else if (s == Stagione::Winter) { lista = temp_winter; n = 4; }
        else { lista = temp_spring; n = 4; }
    }
    else if (b == Bioma::Cold) {
        if (s == Stagione::Summer) { lista = cold_summer; n = 3; }
        else if (s == Stagione::Autumn) { lista = cold_autumn; n = 4; }
        else if (s == Stagione::Winter) { lista = cold_winter; n = 5; }
        else { lista = cold_spring; n = 4; }
    }

    if (lista == nullptr || n == 0) return Meteo::Clear;

    // Distribuzione a favore dei primi elementi
    double sommaPesi = 0;
    double pesi[6]; // max 5 elementi
    for (int i = 0; i < n; i++) {
        pesi[i] = pow(0.5, i); // 0.5, 0.25, 0.125 ...
        sommaPesi += pesi[i];
    }
    for (int i = 0; i < n; i++) {
        pesi[i] /= sommaPesi;
    }

    // Selezione in base a variabile
    double cumul = 0;
    for (int i = 0; i < n; i++) {
        cumul += pesi[i];
        if (variabile <= cumul) {
            return lista[i];
        }
    }

    return lista[n - 1];
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
