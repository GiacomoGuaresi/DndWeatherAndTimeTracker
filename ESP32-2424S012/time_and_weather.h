String timestampToTime(int ts)
{
    int quartiDay = 96; // 24 ore * 4
    int quarti = ts % quartiDay;
    int ore = quarti / 4;
    int minuti = (quarti % 4) * 15;

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", ore, minuti);
    return String(buffer);
}

String timestampToDate(int ts)
{
    int quartiDay = 96; // 24 ore * 4
    int Day = ts / quartiDay + 1;

    char buffer[15];
    snprintf(buffer, sizeof(buffer), "Day %d", Day);
    return String(buffer);
}

Periodo getPeriodo(int ts)
{
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

Stagione getStagione(int ts, int offset)
{
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

double baseTemp(Stagione s, Bioma b)
{
    double t = 0;
    switch (s)
    {
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
    switch (b)
    {
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

double generaTemperatura(int ts, Stagione s, Bioma b, int seed)
{
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
    double temp = generaTemperatura(ts, s, b, seed);
    double variabile = perlin1D(ts * 0.05, seed + 123);
    Meteo meteo = Meteo::Sun;

    if (s == Stagione::Winter)
    {
        if (temp <= 0)
        {
            if (variabile > 0.5)
                meteo = Meteo::Snow;
            else if (variabile > 0.0)
                meteo = Meteo::Cloud;
        }
        else
        {
            if (variabile > 0.5)
                meteo = Meteo::Rain;
            else if (variabile > 0.0)
                meteo = Meteo::Cloud;
        }
    }
    else if (s == Stagione::Summer)
    {
        if (variabile > 0.6)
            meteo = Meteo::Storm;
        else if (variabile > 0.3)
            meteo = Meteo::Cloud;
    }
    else
    {
        if (variabile > 0.6)
            meteo = Meteo::Rain;
        else if (variabile > 0.2)
            meteo = Meteo::Cloud;
    }

    if (temp > 35)
        meteo = Meteo::ScorchingSun;
    if (temp < -10)
        meteo = Meteo::Frost;

    if (b == Bioma::Hot)
    {
        if (meteo == Meteo::Storm)
            meteo = Meteo::Sandstorm;
        if (meteo == Meteo::Rain)
            meteo = Meteo::Sun;
        if (meteo == Meteo::Snow)
            meteo = Meteo::Sun;
    }
    if (b == Bioma::Cold)
    {
        if (meteo == Meteo::ScorchingSun)
            meteo = Meteo::Sun;
    }

    return meteo;
}

uint16_t getOffsetFromSeed(uint32_t seed)
{
    // Hashing semplice basato su LCG
    uint32_t x = seed;
    x ^= x >> 16;
    x *= 0x45d9f3b; // costante scelta per diffusione dei bit
    x ^= x >> 16;
    x *= 0x45d9f3b;
    x ^= x >> 16;

    return x % 366; // rimappiamo tra 0 e 365
}
