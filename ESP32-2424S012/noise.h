double noiseHash(int x, int seed)
{
    unsigned int n = (x * 1619 + seed * 31337) & 0x7fffffff;
    n = (n >> 13) ^ n;
    return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

double lerp(double a, double b, double t)
{
    return a + t * (b - a);
}

double fade(double t)
{
    return t * t * (3 - 2 * t);
}

double perlin1D(double x, int seed)
{
    int xi = (int)floor(x);
    double xf = x - xi;
    double v1 = noiseHash(xi, seed);
    double v2 = noiseHash(xi + 1, seed);
    double u = fade(xf);
    return lerp(v1, v2, u);
}

double noiseHash2D(int x, int y, int seed)
{
    unsigned int n = (x * 1619 + y * 31337 + seed * 1013) & 0x7fffffff;
    n = (n >> 13) ^ n;
    return 1.0 - ((n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

double perlin2D(double x, double y, int seed)
{
    int xi = (int)floor(x);
    int yi = (int)floor(y);

    double xf = x - xi;
    double yf = y - yi;

    double v00 = noiseHash2D(xi, yi, seed);
    double v10 = noiseHash2D(xi + 1, yi, seed);
    double v01 = noiseHash2D(xi, yi + 1, seed);
    double v11 = noiseHash2D(xi + 1, yi + 1, seed);

    double u = fade(xf);
    double v = fade(yf);

    double x1 = lerp(v00, v10, u);
    double x2 = lerp(v01, v11, u);
    return lerp(x1, x2, v);
}
