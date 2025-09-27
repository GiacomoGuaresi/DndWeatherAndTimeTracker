enum class Stagione
{
    Winter,
    Spring,
    Summer,
    Autumn
};

enum class Periodo
{
    Night,
    Moring,
    Day,
    Evening
};
enum class Bioma
{
    Hot,
    Temperate,
    Cold
};
enum class Meteo
{
    Clear,
    Cloud,
    Rain,
    Snow,
    Storm,
    ScorchingSun,
    Frost,
    Sandstorm
};

enum class Screen{
    MainScreen,
    ConfigScreen,
};

String stagioneToString(Stagione s)
{
    switch (s)
    {
    case Stagione::Winter:
        return "Winter";
    case Stagione::Spring:
        return "Spring";
    case Stagione::Summer:
        return "Summer";
    case Stagione::Autumn:
        return "Autumn";
    }
    return "?";
}

String periodoToString(Periodo p)
{
    switch (p)
    {
    case Periodo::Night:
        return "Night";
    case Periodo::Moring:
        return "Moring";
    case Periodo::Day:
        return "Day";
    case Periodo::Evening:
        return "Evening";
    }
    return "?";
}

String biomaToString(Bioma b)
{
    switch (b)
    {
    case Bioma::Hot:
        return "Hot";
    case Bioma::Temperate:
        return "Temperate";
    case Bioma::Cold:
        return "Cold";
    }
    return "?";
}

String meteoToString(Meteo m)
{
    switch (m)
    {
    case Meteo::Clear:
        return "Clear";
    case Meteo::Cloud:
        return "Cloud";
    case Meteo::Rain:
        return "Rain";
    case Meteo::Snow:
        return "Snow";
    case Meteo::Storm:
        return "Storm";
    case Meteo::ScorchingSun:
        return "Scorching sun";
    case Meteo::Frost:
        return "Frost";
    case Meteo::Sandstorm:
        return "Sandstorm";
    }
    return "?";
}
