#include <iostream>
#include <string>
using namespace std;

const double M_PI = 3.14159265358979323846;

enum class Stagione { Inverno, Primavera, Estate, Autunno };
enum class Periodo { Notte, Mattina, Giorno, Sera };
enum class Bioma { Caldo, Temperato, Freddo };
enum class Meteo { Soleggiato, Nuvoloso, Pioggia, Neve, Tempesta, SoleTorrido, Gelo, TempestaDiSabbia };

// Utility per stampare
string toString(Stagione s) {
    switch (s) {
    case Stagione::Inverno: return "Inverno";
    case Stagione::Primavera: return "Primavera";
    case Stagione::Estate: return "Estate";
    case Stagione::Autunno: return "Autunno";
    }
    return "?";
}

string toString(Periodo p) {
    switch (p) {
    case Periodo::Notte: return "Notte";
    case Periodo::Mattina: return "Mattina";
    case Periodo::Giorno: return "Giorno";
    case Periodo::Sera: return "Sera";
    }
    return "?";
}

string toString(Bioma b) {
    switch (b) {
    case Bioma::Caldo: return "Caldo";
    case Bioma::Temperato: return "Temperato";
    case Bioma::Freddo: return "Freddo";
    }
    return "?";
}

string toString(Meteo m) {
    switch (m) {
    case Meteo::Soleggiato: return "Soleggiato";
    case Meteo::Nuvoloso: return "Nuvoloso";
    case Meteo::Pioggia: return "Pioggia";
    case Meteo::Neve: return "Neve";
    case Meteo::Tempesta: return "Tempesta";
    case Meteo::SoleTorrido: return "Sole torrido";
    case Meteo::Gelo: return "Gelo";
    case Meteo::TempestaDiSabbia: return "Tempesta di sabbia";
    }                               \
    return "?";
}

// --- Funzioni di rumore Perlin 1D ---

// Hash deterministico -> valore in [-1, 1]
double noiseHash(int x, int seed) {
    unsigned int n = (x * 1619 + seed * 31337) & 0x7fffffff;
    n = (n >> 13) ^ n;
    return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

// Interpolazione (cosine interpolation)
double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

// Fade function tipo Perlin
double fade(double t) {
    return t * t * (3 - 2 * t);
}

// Rumore 1D: input x continuo, output ~[-1,1]
double perlin1D(double x, int seed) {
    int xi = (int)floor(x);
    double xf = x - xi;

    double v1 = noiseHash(xi, seed);
    double v2 = noiseHash(xi + 1, seed);

    double u = fade(xf);

    return lerp(v1, v2, u);
}




// --- Funzioni di utilità per gestione tempo e stagioni ---

// Converte timestamp (in quarti d'ora) in stringa "Giorno X - HH:MM"
string timestampToDateTime(int ts) {
    int quartiGiorno = 96;  // 24 ore * 4
    int giorno = ts / quartiGiorno + 1;
    int quarti = ts % quartiGiorno;

    int ore = quarti / 4;
    int minuti = (quarti % 4) * 15;

    char buffer[50];
    sprintf_s(buffer, "Giorno %d - %02d:%02d", giorno, ore, minuti);
    return string(buffer);
}

// Restituisce periodo della giornata
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


// Restituisce stagione in base al giorno+offset (giorni nell’anno)
Stagione getStagione(int ts, int offset) {
    int giorno = ts / 96 + 1;
    int giornoAnno = (giorno + offset) % 366;

    if (giornoAnno >= 80 && giornoAnno < 172) return Stagione::Primavera;
    if (giornoAnno >= 172 && giornoAnno < 266) return Stagione::Estate;
    if (giornoAnno >= 266 && giornoAnno <= 365) return Stagione::Autunno;
    return Stagione::Inverno;
}


// Ritorna temperatura media stagionale + bioma
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
        case Bioma::Temperato: default: break;
    }
    
    return t;
}


// Genera temperatura
double generaTemperatura(int ts, Stagione s, Bioma b, int seed) {
    int quarti = ts % 96;
    double ora = quarti / 4.0; // ore in decimale

    double temp = baseTemp(s, b);

    // variazione giorno/notte
    double daily = 5 * sin((ora - 4) / 24.0 * 2 * M_PI);
    temp += daily;

    // rumore
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
        }
        else {
            if (variabile > 0.5) meteo = Meteo::Pioggia;
            else if (variabile > 0.0) meteo = Meteo::Nuvoloso;
        }
    }
    else if (s == Stagione::Estate) {
        if (variabile > 0.6) meteo = Meteo::Tempesta;
        else if (variabile > 0.3) meteo = Meteo::Nuvoloso;
    }
    else { // Primavera o Autunno
        if (variabile > 0.6) meteo = Meteo::Pioggia;
        else if (variabile > 0.2) meteo = Meteo::Nuvoloso;
    }

    // Controlli estremi temperatura
    if (temp > 35) meteo = Meteo::SoleTorrido;
    if (temp < -10) meteo = Meteo::Gelo;

    // --- Vincoli di bioma ---
    if (b == Bioma::Caldo) {
        // Niente pioggia nei deserti → rimpiazzo con "Tempesta di sabbia" o "Soleggiato"
        if (meteo == Meteo::Tempesta) meteo = Meteo::TempestaDiSabbia;
        if (meteo == Meteo::Pioggia) meteo = Meteo::Soleggiato;
        if (meteo == Meteo::Neve)   meteo = Meteo::Soleggiato; // no neve nei deserti
    }
    if (b == Bioma::Freddo) {
        // Niente sole torrido in montagna → sostituisco con "Soleggiato"
        if (meteo == Meteo::SoleTorrido) meteo = Meteo::Soleggiato;
    }

    return meteo;
}


// --- MAIN di test ---

int main() {
    int offset = 365 * 3/4;
    int seed = 42;
    Bioma bioma = Bioma::Temperato;

    cout << "timestamp,giorno e ora,periodo,stagione,bioma,temp,meteo\n";

    for(int stagione = 0; stagione < 4; stagione++) {
        int offset = 365 * stagione / 4;
        for (int ts = 0; ts < 96 * 7; ts += 4) {
            Stagione stagione = getStagione(ts, offset);
            double temp = generaTemperatura(ts, stagione, bioma, seed);
            Meteo meteo = generaMeteo(ts, offset, bioma, seed);
            Periodo periodo = getPeriodo(ts);

            cout << ts << ","
                << timestampToDateTime(ts) << ","
                << toString(periodo) << ","
                << toString(stagione) << ","
                << toString(bioma) << ","
                << temp << ","
                << toString(meteo) << "\n";
        }

	}

    

    return 0;
}
