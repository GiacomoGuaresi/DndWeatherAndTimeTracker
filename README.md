# DnD Weather and time Traker

A small **weather and time tracker for D&D campaigns** based on **ESP32-2424S012** (ESP32 with integrated round touch display).  
The device shows **time, day, weather and temperature** simulated organically using **Perlin noise**, configured with a **campaign seed**.

Perfect as a tabletop tool for Dungeon Masters and players who want to add variability and immersion to their game world.  

## ✨ Features

- **Clock with 15-minute steps** (forward/backward with touch buttons).  
- **Dynamic weather simulation** (clear, rain, clouds, snow, etc.).  
- **Campaign day/hour tracking**.  
- **3 available biomes**: hot, temperate, cold.  
- **Customizable seed**: keeps consistency between different campaigns.  
- **Persistent storage**: current day/time and seed are saved in non-volatile memory and restored at reboot.  
- **Battery powered**.  
- **STL file included** to 3D-print the base for the round display.  

## 🖼️ Hardware

- ESP32-2424S012 (ESP32 with round GC9A01 display + touch).  
    - [Datasheet](https://www.espboards.dev/esp32/cyd-esp32-2424s012/)
- Li-Ion / LiPo battery.  
- 3D printed base (STL file in repo).  

## ⚙️ Software Setup

### 1. Arduino Libraries
Make sure you have installed:  

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)  
- [CST816S](https://github.com/fbiego/CST816S) (for touch)  

⚠️ Use **ESP32 boards version 2.0.14** to avoid loop issues.  

### 2. `User_Setup.h` configuration for TFT_eSPI
Edit `User_Setup.h` with:  

```c
#define SPI_FREQUENCY  80000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
#define GC9A01_DRIVER        

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_SCLK  6   // SCL
#define TFT_MOSI  7   // SDA
#define TFT_DC    2   // DC
#define TFT_CS   10   // CS
#define TFT_RST  -1   // RST

#define TFT_BL    3   // BACKLIGHT
```


## 🚀 Usage

1. Upload the `.ino` sketch to your ESP32.  
2. On first boot, set the **campaign seed** in settings.  
3. Use the on-screen touch buttons to **move forward/backward in 15-minute increments**.  
4. Weather and temperature are dynamically generated based on time, day, and chosen biome.  


## 📂 Repository Structure

- `ESP32-2424S012/` → main code.  
- `STL/` → 3D printable base file.  
- `README.md` → this document.  


## 📜 License

Distributed under the **MIT License**.  
Feel free to use, modify, and share!  