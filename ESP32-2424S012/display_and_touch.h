void draw_weather(Meteo meteo) {
  uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42);  // #f04142 (rosso pulsanti)
  uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);       // #eae0c3 (sfondo pergamena)
  uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);     // #000000 (testo)
  uint16_t COL_ACCENT = tft.color565(0xEA, 0xAF, 0x7C);   // #EAAF7C (decorazioni)

  int16_t w = 128;
  int16_t h = 128;
  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;
  int16_t r = 30;  // raggio base sole/nuvole

  switch (meteo) {
    case Meteo::Clear:
      {
        // Sole
        tft.fillCircle(cx, cy, r, TFT_YELLOW);
        for (int i = 0; i < 360; i += 30) {
          int x1 = cx + cos(radians(i)) * (r + 5);
          int y1 = cy + sin(radians(i)) * (r + 5);
          int x2 = cx + cos(radians(i)) * (r + 20);
          int y2 = cy + sin(radians(i)) * (r + 20);
          tft.drawWedgeLine(x1, y1, x2, y2, 3, 3, TFT_YELLOW, COL_BG);
        }
        break;
      }
    case Meteo::Cloud:
      {
        // Nuvola
        tft.fillCircle(cx - 20, cy, 20, TFT_LIGHTGREY);
        tft.fillCircle(cx, cy - 10, 25, TFT_LIGHTGREY);
        tft.fillCircle(cx + 25, cy, 20, TFT_LIGHTGREY);
        tft.fillRect(cx - 20, cy, 45, 20, TFT_LIGHTGREY);
        break;
      }
    case Meteo::Rain:
      {
        // Nuvola
        draw_weather(Meteo::Cloud);
        // Gocce
        for (int i = -15; i <= 15; i += 15) {
          // tft.drawLine(cx + i, cy + 15, cx + i - 3, cy + 30, TFT_BLUE);
          tft.drawWedgeLine(cx + i, cy + 15 + (i / 5), cx + i - 3, cy + 30 + (i / 5), 1, 2, TFT_BLUE, COL_BG);
        }
        break;
      }
    case Meteo::Snow:
      {
        // Nuvola
        draw_weather(Meteo::Cloud);
        // Fiocchi
        for (int i = -15; i <= 15; i += 15) {
          int fx = cx + i;
          int fy = cy + 20;
          tft.drawLine(fx - 5, fy + abs(i), fx + 5, fy + abs(i), TFT_WHITE);
          tft.drawLine(fx, fy - 5 + abs(i), fx, fy + 5 + abs(i), TFT_WHITE);
          tft.drawLine(fx - 4, fy - 4 + abs(i), fx + 4, fy + 4 + abs(i), TFT_WHITE);
          tft.drawLine(fx - 4, fy + 4 + abs(i), fx + 4, fy - 4 + abs(i), TFT_WHITE);
        }
        break;
      }
    case Meteo::Storm:
      {
        // Nuvola
        draw_weather(Meteo::Cloud);

        // Gocce
        for (int i = -15; i <= 15; i += 30) {
          tft.drawWedgeLine(cx + i, cy + 15 + (i / 5),
                            cx + i - 3, cy + 30 + (i / 5),
                            1, 2, TFT_BLUE, COL_BG);
        }

        // Fulmine (semplice zig-zag giallo)
        int lx = cx;       // punto di partenza (centro nuvola)
        int ly = cy + 20;  // poco sotto la nuvola

        tft.drawWedgeLine(lx, ly, lx + 5, ly + 8, 4, 3, TFT_YELLOW, COL_BG);
        tft.drawWedgeLine(lx + 5, ly + 8, lx - 5, ly + 16, 3, 2, TFT_YELLOW, COL_BG);
        tft.drawWedgeLine(lx - 5, ly + 16, lx + 8, ly + 24, 2, 1, TFT_YELLOW, COL_BG);

        break;
      }
    case Meteo::ScorchingSun:
      {
        // Sole con alone rosso
        tft.fillCircle(cx, cy, r + 10, TFT_ORANGE);
        tft.fillCircle(cx, cy, r, TFT_YELLOW);
        for (int i = 0; i < 360; i += 20) {
          int x1 = cx + cos(radians(i)) * (r + 5);
          int y1 = cy + sin(radians(i)) * (r + 5);
          int x2 = cx + cos(radians(i)) * (r + 25);
          int y2 = cy + sin(radians(i)) * (r + 25);
          tft.drawWedgeLine(x1, y1, x2, y2, 2, 5, TFT_YELLOW, COL_BG);
        }
        break;
      }
    case Meteo::Frost:
      {
        // Rami principali del fiocco (ogni 60 gradi)
        for (int i = 0; i < 360; i += 60) {
          int x1 = cx + cos(radians(i)) * 30;
          int y1 = cy + sin(radians(i)) * 30;
          tft.drawWedgeLine(cx, cy, x1, y1, 2, 3, TFT_WHITE, COL_BG);

          // Rametti laterali a metà del ramo
          int xm = cx + cos(radians(i)) * 15;
          int ym = cy + sin(radians(i)) * 15;

          // Angoli laterali (+30° e -30° rispetto al ramo principale)
          int xL = xm + cos(radians(i + 30)) * 10;
          int yL = ym + sin(radians(i + 30)) * 10;
          int xR = xm + cos(radians(i - 30)) * 10;
          int yR = ym + sin(radians(i - 30)) * 10;

          tft.drawWedgeLine(xm, ym, xL, yL, 1, 2, TFT_WHITE, COL_BG);
          tft.drawWedgeLine(xm, ym, xR, yR, 1, 2, TFT_WHITE, COL_BG);
        }

        // Cerchio centrale piccolo (ghiaccio al centro)
        tft.fillCircle(cx, cy, 4, TFT_WHITE);

        break;
      }
    case Meteo::Sandstorm:
      {
        // Onde di sabbia semplificate (2 onde grandi)
        for (int i = 0; i < 2; i++) {
          int y = cy - 10 + i * 20;  // più distanziate

          for (int x = cx - 40; x < cx + 40; x += 20) {
            tft.drawWedgeLine(x, y,
                              x + 10, y + 6,
                              3, 4, TFT_ORANGE, COL_BG);
            tft.drawWedgeLine(x + 10, y + 6,
                              x + 20, y,
                              3, 4, TFT_ORANGE, COL_BG);
          }
        }

        // Granelli opzionali (per dare movimento)
        for (int i = 0; i < 5; i++) {
          int px = cx - 35 + random(70);
          int py = cy - 20 + random(40);
          tft.fillCircle(px, py, 1, TFT_ORANGE);
        }

        break;
      }
  }
}

void draw_biome(Bioma bioma) {
  uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42);  // #f04142 (rosso pulsanti)
  uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);       // #eae0c3 (sfondo pergamena)
  uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);     // #000000 (testo)
  uint16_t COL_ACCENT = tft.color565(0xEA, 0xAF, 0x7C);   // #EAAF7C (decorazioni)

  int r = 16;
  int x = 44;
  int y = 195;

  tft.fillCircle(x, y, r + 5, COL_BG);

  // Resetto 0,0
  x = x - r;
  y = y - r;

  switch (bioma) {
    case Bioma::Hot:
      // Cactus 🌵
      tft.fillCircle(x + 7, y + 9, 1, COL_ACCENT);
      tft.fillCircle(x + 23, y + 11, 1, COL_ACCENT);
      tft.fillCircle(x + 15, y + 6, 3, COL_ACCENT);

      tft.fillRect(x + 6, y + 10, 4, 7, COL_ACCENT);
      tft.fillRect(x + 22, y + 12, 4, 7, COL_ACCENT);
      tft.fillRect(x + 12, y + 6, 8, 23, COL_ACCENT);

      tft.fillRect(x + 9, y + 16, 3, 4, COL_ACCENT);
      tft.fillRect(x + 7, y + 17, 2, 2, COL_ACCENT);

      tft.fillRect(x + 20, y + 18, 3, 4, COL_ACCENT);
      tft.fillRect(x + 23, y + 19, 2, 2, COL_ACCENT);
      break;

    case Bioma::Temperate:
      // Albero 🌳
      // Tronco
      tft.fillRect(x + 14, y + 16, 4, 12, COL_ACCENT);

      // Chioma (tondeggiante)
      tft.fillCircle(x + 12, y + 12, 7, COL_ACCENT);
      tft.fillCircle(x + 12, y + 14, 5, COL_ACCENT);
      tft.fillCircle(x + 20, y + 15, 5, COL_ACCENT);
      tft.fillCircle(x + 20, y + 11, 4, COL_ACCENT);
      break;

    case Bioma::Cold:
      // Montagna 🏔
      // Base triangolare
      tft.fillTriangle(x + 12, y + 7,   
                       x + 3, y + 25,  
                       x + 21, y + 25,   
                       COL_ACCENT);

      tft.fillTriangle(x + 15, y + 25,
                       x + 21, y + 14,
                       x + 28, y + 25,
                       COL_ACCENT);
      
      tft.fillTriangle(x + 12, y + 9,
                       x + 11, y + 12,
                       x + 14, y + 13,
                       COL_BG);
      
      break;
  }
}

void draw(bool fullRedraw, int ts, Stagione stagione, double temp, Meteo meteo, Periodo periodo) {
  // Definizione colori (convertiti HEX -> RGB565)
  uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42);  // #f04142 (rosso pulsanti)
  uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);       // #eae0c3 (sfondo pergamena)
  uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);     // #000000 (testo)
  uint16_t COL_ACCENT = tft.color565(0xEA, 0xAF, 0x7C);   // #EAAF7C (decorazioni)

  int16_t w = tft.width();
  int16_t h = tft.height();
  int16_t cx = w / 2;
  int16_t cy = h / 2;

  if (fullRedraw) {
    tft.fillScreen(COL_BG);
    int btn_w = 36;
    int btn_h = 90;
    int btn_y = cy - (btn_h / 2);

    tft.setTextColor(COL_BG, COL_PRIMARY);

    tft.fillSmoothCircle(cx, cy, 110, COL_ACCENT, COL_BG);
    tft.fillSmoothCircle(cx, cy, 105, COL_BG, COL_ACCENT);

    // Pulsante sinistro
    tft.fillRect(0, btn_y - 5, btn_w, btn_h + 10, COL_BG);
    tft.fillRoundRect(0, btn_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("<", btn_w / 2, cy - 10, 4);

    // Pulsante destro
    tft.fillRect(w - btn_w, btn_y - 5, btn_w, btn_h + 10, COL_BG);
    tft.fillRoundRect(w - btn_w, btn_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString(">", w - btn_w / 2, cy - 10, 4);

    // Pulsante config
    btn_w = 90;
    btn_h = 36;
    tft.fillRect((w - btn_w - 10) / 2, h - btn_h, btn_w + 10, btn_h, COL_BG);
    tft.fillRoundRect((w - btn_w) / 2, h - btn_h, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("Config", w / 2, h - 25, 2);
  }

  // Clear info area
  tft.fillRect((w - 130) / 2, 0, 130, h - 40, COL_BG);


  // Disegna/scrivi Timestamp (Orario + Giorno) + stagione
  tft.setTextColor(COL_TEXT);
  tft.drawCentreString(timestampToTime(ts), cx, 10, 4);
  tft.drawCentreString(timestampToDate(ts) + " - " + stagioneToString(stagione), cx, 35, 2);
  // tft.drawCentreString(periodoToString(periodo) + " - " + stagioneToString(stagione), cx, 55, 2);

  // Disegna/scrivi temperatura + meteo
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%d", (int)temp);
  tft.setTextColor(COL_TEXT);
  tft.drawCentreString(String(buffer) + "c " + meteoToString(meteo), cx, h - 55, 2);

  // Disegna/scrivi meteo
  draw_weather(meteo);

  // Disegna/scrivi periodo giornata

  // Disegna/scrivi Bioma
  draw_biome(bioma);
}

void drawConfigScreen(bool fullRedraw) {
  // Colori
  uint16_t COL_PRIMARY = tft.color565(0xF0, 0x41, 0x42);
  uint16_t COL_BG = tft.color565(0xEA, 0xE0, 0xC3);
  uint16_t COL_TEXT = tft.color565(0x00, 0x00, 0x00);
  uint16_t COL_ACCENT = tft.color565(0xEA, 0xAF, 0x7C);

  int16_t w = tft.width();
  int16_t h = tft.height();
  int16_t cx = w / 2;
  int16_t cy = h / 2;

  // Spaziatura verticale uniforme
  int itemSpacing = 40;
  int first_y = cy - itemSpacing;  // Bioma in alto
  int bioma_y = first_y;
  int seed_y = first_y + itemSpacing;
  int bright_y = seed_y + itemSpacing;  // nuova riga brightness

  if (fullRedraw) {
    tft.fillScreen(COL_BG);

    tft.fillSmoothCircle(cx, cy, 110, COL_ACCENT, COL_BG);
    tft.fillSmoothCircle(cx, cy, 105, COL_BG, COL_ACCENT);

    // Pulsante close
    int btn_w = 90;
    int btn_h = 36;
    tft.fillRect((w - btn_w - 10) / 2, h - btn_h, btn_w + 10, btn_h, COL_BG);
    tft.fillRoundRect((w - btn_w) / 2, h - btn_h, btn_w, btn_h, 8, COL_PRIMARY);
    tft.setTextColor(COL_BG);
    tft.drawCentreString("Close", w / 2, h - 25, 2);

    // Pulsanti bioma < >
    btn_w = 36;
    btn_h = 36;
    tft.fillRoundRect(0, bioma_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.setTextColor(COL_BG);
    tft.drawCentreString("<", btn_w / 2, bioma_y + 6, 4);
    tft.fillRoundRect(w - btn_w, bioma_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString(">", w - (btn_w / 2), bioma_y + 6, 4);

    // Pulsanti seed - +
    tft.fillRoundRect(0, seed_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("-", btn_w / 2, seed_y + 6, 4);
    tft.fillRoundRect(w - btn_w, seed_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("+", w - (btn_w / 2), seed_y + 6, 4);

    // Pulsanti brightness - +
    tft.fillRoundRect(0, bright_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("-", btn_w / 2, bright_y + 6, 4);
    tft.fillRoundRect(w - btn_w, bright_y, btn_w, btn_h, 8, COL_PRIMARY);
    tft.drawCentreString("+", w - (btn_w / 2), bright_y + 6, 4);
  }

  // Clear info area
  tft.fillRect((w - 130) / 2, 0, 130, h - 40, COL_BG);

  // Titolo
  tft.setTextColor(COL_TEXT);
  tft.drawCentreString("Config", cx, 10, 4);
  tft.drawCentreString("Version 0.0.1", cx, 35, 2);

  // Bioma
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawCentreString(biomaToString(bioma), cx, bioma_y + 7, 4);

  // Seed
  tft.drawCentreString("Campaign number " + String(seed), cx, seed_y + 7, 2);

  // Brightness (10-100)
  tft.drawCentreString("Brightness " + String(brightness) + "%", cx, bright_y + 7, 2);
}

void changeTime(int dir, int step) {
  ts += dir * step;
  if (ts < 0)
    ts = 0;  // limite inferiore

  prefs.begin("meteo", false);  // namespace "meteo", modalità read/write
  prefs.putUInt("ts", ts);      // salvo il timestamp
  prefs.end();                  // chiudo per liberare risorse

  stagione = getStagione(ts, offset);
  temp = generaTemperatura(ts, stagione, bioma, seed);
  meteo = generaMeteo(ts, offset, bioma, seed);
  periodo = getPeriodo(ts);

  draw(false, ts, stagione, temp, meteo, periodo);
}

void changeBiome(int step) {
  // Ricavo il valore numerico dell'enum
  int biomaVal = static_cast<int>(bioma);

  // Avanzo o indietro di step
  biomaVal += step;

  // Numero totale di biomi (devi aggiornare se aggiungi altri)
  const int numBiomi = 3;

  // Ciclo (wrap-around)
  if (biomaVal < 0)
    biomaVal = numBiomi - 1;
  else if (biomaVal >= numBiomi)
    biomaVal = 0;

  // Aggiorno il bioma
  bioma = static_cast<Bioma>(biomaVal);

  // Salvo nelle preferences
  prefs.begin("meteo", false);
  prefs.putUChar("bioma", static_cast<uint8_t>(bioma));
  prefs.end();

  // Ricalcolo parametri e ridisegno
  stagione = getStagione(ts, offset);
  temp = generaTemperatura(ts, stagione, bioma, seed);
  meteo = generaMeteo(ts, offset, bioma, seed);
  periodo = getPeriodo(ts);

  drawConfigScreen(false);
}


void changeSeed(int step) {
  seed += step;

  if (seed < 1)
    seed = 1;

  prefs.begin("meteo", false);  // namespace "meteo", modalità read/write
  prefs.putUInt("seed", seed);  // salvo il timestamp
  prefs.end();                  // chiudo per liberare risorse

  // Offset dipende dal seed
  offset = getOffsetFromSeed(seed);

  // Disegno la prima schermata
  stagione = getStagione(ts, offset);
  temp = generaTemperatura(ts, stagione, bioma, seed);
  meteo = generaMeteo(ts, offset, bioma, seed);
  periodo = getPeriodo(ts);

  drawConfigScreen(false);
}

void handleTouch(int dir) {
  unsigned long now = millis();

  if (!isPressing) {
    // Primo tocco
    isPressing = true;
    wasPressed = true;
    pressStartTime = now;
    lastTouchTime = now;
    changeTime(dir, 4);  // variazione iniziale di 1
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

      changeTime(dir, offset * 4);
      lastTouchTime = now;
    }
    return;
  }


  // gestione ripetizione "normale"
  if (now - lastTouchTime >= interval) {
    changeTime(dir, 4);
    lastTouchTime = now;
  }
}

void resetTouchState() {
  isPressing = false;
  wasPressed = false;
}

int gammaCorrect(int percent) {
  float gamma = 2.2;  // correzione standard
  float normalized = percent / 100.0;
  return (int)(pow(normalized, gamma) * 1023);
}

void changeBrightness(int delta) {
  brightness += delta;
  if (brightness < 10) brightness = 10;
  if (brightness > 100) brightness = 100;

  prefs.begin("meteo", false);
  prefs.putUInt("brightness", brightness);
  prefs.end();

  // Applica correzione gamma (≈2.2)
  int equivalent_brightness = gammaCorrect(brightness);

  ledcWrite(ledChannel, equivalent_brightness);

  drawConfigScreen(false);
}

void checkTouchInput() {
  if (touch.available()) {
    switch (screen) {
      case Screen::MainScreen:
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
          int tx = y;      // X sullo schermo = Y del touch
          int ty = w - x;  // Y sullo schermo = larghezza - X del touch

          bool leftPressed = (tx >= 0 && tx <= btn_w && ty >= btn_y && ty <= btn_y + btn_h);
          bool rightPressed = (tx >= w - btn_w && tx <= w && ty >= btn_y && ty <= btn_y + btn_h);

          // --- Bottone centrale in basso ---
          int cfg_w = 60;  // larghezza bottone
          int cfg_h = 40;  // altezza bottone
          int cfg_x1 = cx - (cfg_w / 2);
          int cfg_x2 = cx + (cfg_w / 2);
          int cfg_y1 = h - cfg_h;  // in basso
          int cfg_y2 = h;

          bool configPressed = (tx >= cfg_x1 && tx <= cfg_x2 && ty >= cfg_y1 && ty <= cfg_y2);

          if (leftPressed || rightPressed) {
            handleTouch(leftPressed ? -1 : +1);  // -1 per sinistra, +1 per destra
          } else if (configPressed) {
            screen = Screen::ConfigScreen;
            drawConfigScreen(true);
            while (touch.available()) delay(20);
          } else {
            resetTouchState();
          }
          break;
        }

      case Screen::ConfigScreen:
        {
          int16_t w = tft.width();
          int16_t h = tft.height();
          int16_t cx = w / 2;
          int16_t cy = h / 2;
          int x = touch.data.x;
          int y = touch.data.y;

          // Spaziatura come nel drawConfigScreen()
          int itemSpacing = 40;
          int first_y = cy - itemSpacing;
          int bioma_y = first_y;
          int seed_y = first_y + itemSpacing;
          int bright_y = seed_y + itemSpacing;  // nuova riga brightness

          // --- Mappatura per rotazione 90° ---
          int tx = y;      // X sullo schermo = Y del touch
          int ty = w - x;  // Y sullo schermo = larghezza - X del touch

          // --- Bottone centrale in basso ---
          int cfg_w = 60;
          int cfg_h = 40;
          int cfg_x1 = cx - (cfg_w / 2);
          int cfg_x2 = cx + (cfg_w / 2);
          int cfg_y1 = h - cfg_h;
          int cfg_y2 = h;

          bool closePressed = (tx >= cfg_x1 && tx <= cfg_x2 && ty >= cfg_y1 && ty <= cfg_y2);

          // --- Dimensioni bottoni ---
          int btn_w = 36;
          int btn_h = 36;

          // --- Pulsanti Bioma ---
          bool biomaLeftPressed = (tx >= 0 && tx <= btn_w && ty >= bioma_y && ty <= bioma_y + btn_h);
          bool biomaRightPressed = (tx >= w - btn_w && tx <= w && ty >= bioma_y && ty <= bioma_y + btn_h);

          // --- Pulsanti Seed ---
          bool seedLeftPressed = (tx >= 0 && tx <= btn_w && ty >= seed_y && ty <= seed_y + btn_h);
          bool seedRightPressed = (tx >= w - btn_w && tx <= w && ty >= seed_y && ty <= seed_y + btn_h);

          // --- Pulsanti Brightness ---
          bool brightLeftPressed = (tx >= 0 && tx <= btn_w && ty >= bright_y && ty <= bright_y + btn_h);
          bool brightRightPressed = (tx >= w - btn_w && tx <= w && ty >= bright_y && ty <= bright_y + btn_h);

          if (closePressed) {
            screen = Screen::MainScreen;
            draw(true, ts, stagione, temp, meteo, periodo);
            while (touch.available()) delay(20);
          } else if (biomaLeftPressed) {
            changeBiome(-1);
            while (touch.available()) delay(20);
          } else if (biomaRightPressed) {
            changeBiome(1);
            while (touch.available()) delay(20);
          } else if (seedLeftPressed) {
            changeSeed(-1);
            while (touch.available()) delay(20);
          } else if (seedRightPressed) {
            changeSeed(1);
            while (touch.available()) delay(20);
          } else if (brightLeftPressed) {
            changeBrightness(-10);  // decremento di 10
            while (touch.available()) delay(20);
          } else if (brightRightPressed) {
            changeBrightness(10);  // incremento di 10
            while (touch.available()) delay(20);
          } else {
            resetTouchState();
          }
          break;
        }
      default:
        {
          screen = Screen::MainScreen;
          drawConfigScreen(true);
          break;
        }
    }
  } else {
    resetTouchState();
  }
}
