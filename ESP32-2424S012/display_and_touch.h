void drawBackgroundNoiseRect(int x0, int y0, int w, int h, uint16_t baseColor, int seed = 1234)
{
    // decomponi il colore base
    uint8_t r = (baseColor >> 11) & 0x1F;
    uint8_t g = (baseColor >> 5) & 0x3F;
    uint8_t b = baseColor & 0x1F;

    // scala per rumore
    double scale = 0.1;   // più basso = rumore più grande e liscio
    double intensity = 3; // quanto schiarire/scurire

    for (int y = y0; y < y0 + h; y++)
    {
        for (int x = x0; x < x0 + w; x++)
        {
            double n = perlin2D(x * scale, y * scale, seed); // -1..1
            int nr = constrain(r + (int)(n * intensity), 0, 31);
            int ng = constrain(g + (int)(n * intensity), 0, 63);
            int nb = constrain(b + (int)(n * intensity), 0, 31);
            tft.drawPixel(x, y, tft.color565(nr << 3, ng << 2, nb << 3)); // riconverti RGB565
        }
    }
}

void draw(bool fullRedraw, int ts, Stagione stagione, double temp, Meteo meteo, Periodo periodo)
{
    // Definizione colori (convertiti HEX -> RGB565)
    uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42); // #f04142 (rosso pulsanti)
    uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);      // #eae0c3 (sfondo pergamena)
    uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);    // #000000 (testo)
    uint16_t COL_ACCENT = tft.color565(0xEC, 0xEE, 0xF2);  // #eceef2 (decorazioni)

    int16_t w = tft.width();
    int16_t h = tft.height();
    int16_t cx = w / 2;
    int16_t cy = h / 2;

    // Sfondo color pergamena
    if (fullRedraw)
        drawBackgroundNoiseRect(0, 0, w, h, COL_BG);

    tft.fillRoundRect(40, 0, w - 80, 80, 8, COL_ACCENT);
    tft.drawRoundRect(40, 0, w - 80, 80, 8, COL_PRIMARY);

    tft.fillRoundRect(30, cy + 38, w - 60, 28, 8, COL_ACCENT);
    tft.drawRoundRect(30, cy + 38, w - 60, 28, 8, COL_PRIMARY);

    tft.fillRoundRect(50, (h / 2) - 35, w - 100, 70, 8, COL_ACCENT);
    tft.drawRoundRect(50, (h / 2) - 35, w - 100, 70, 8, COL_PRIMARY);

    // Testi principali
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(COL_TEXT);
    tft.drawCentreString(timestampToDate(ts), cx, 30, 4);

    tft.setTextColor(COL_PRIMARY);
    tft.drawCentreString(periodoToString(periodo) + " - " + stagioneToString(stagione), cx, 55, 2);

    tft.setTextColor(COL_TEXT);
    tft.drawCentreString(timestampToTime(ts), cx, cy - 20, 6);

    // Pulsanti
    if (fullRedraw)
    {
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
    tft.setTextColor(COL_TEXT);
    tft.drawCentreString(String(buffer) + "c " + meteoToString(meteo), cx, cy + 40, 4);
}

void changeTime(int dir, int step)
{
    ts += dir * step;
    if (ts < 0)
        ts = 0; // limite inferiore

    prefs.begin("meteo", false); // namespace "meteo", modalità read/write
    prefs.putUInt("ts", ts);     // salvo il timestamp
    prefs.end();                 // chiudo per liberare risorse

    draw(false, ts, stagione, temp, meteo, periodo);
}

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
    // dopo 6s → vai sempre al Day successivo o precedente alle 08:00
    if (now - lastTouchTime >= 500) {  // ogni mezzo secondo
      int day = (ts - 1) / 96;         // calcola il Day corrente
      int targetTs;

      if (dir == 1) {
        targetTs = (day + 1) * 96 + 32;  // Day successivo alle 08:00
      } else {
        targetTs = (day - 1) * 96 + 32;  // Day precedente alle 08:00
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

void resetTouchState() {
  isPressing = false;
  wasPressed = false;
}
