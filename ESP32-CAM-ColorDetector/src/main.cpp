#include <Arduino.h>
#include <Wire.h>
#include "esp_camera.h"
#include "camera_pins.h"
#include "color_config.h"

// ESP32-CAM I2C Slave mode
#define I2C_SDA_CAM   14
#define I2C_SCL_CAM   15
#define CAM_I2C_ADDR  0x08

static bool s_camera_ready = false;
static volatile uint8_t s_latest_color = (uint8_t)COLOR_NONE;
static volatile uint8_t s_latest_h = 0, s_latest_s = 0, s_latest_v = 0;

// RGB565 to RGB8
static void rgb565_to_rgb(uint8_t b0, uint8_t b1,
                          uint8_t &r, uint8_t &g, uint8_t &b) {
    uint16_t px = ((uint16_t)b0 << 8) | b1;
    r = ((px >> 11) & 0x1F) * 255 / 31;
    g = ((px >>  5) & 0x3F) * 255 / 63;
    b = ( px        & 0x1F) * 255 / 31;
}

// RGB to HSV (OpenCV format: H=0-179)
static void rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b,
                       uint8_t &h, uint8_t &s, uint8_t &v) {
    uint8_t cmax = max(r, max(g, b));
    uint8_t cmin = min(r, min(g, b));
    uint8_t diff = cmax - cmin;

    v = cmax;
    s = (cmax == 0) ? 0 : (uint8_t)((diff * 255UL) / cmax);

    if (diff == 0) { h = 0; return; }

    int hh;
    if      (cmax == r) hh = (int)(60.0f * (g - b) / (float)diff + 360) % 360;
    else if (cmax == g) hh = (int)(60.0f * ((b - r) / (float)diff) + 120);
    else                hh = (int)(60.0f * ((r - g) / (float)diff) + 240);

    if (hh < 0)   hh += 360;
    if (hh > 359) hh -= 360;

    h = (uint8_t)(hh / 2);
}

static inline bool in_range(uint8_t val, uint8_t lo, uint8_t hi) {
    return val >= lo && val <= hi;
}

static bool match_hsv(uint8_t h, uint8_t s, uint8_t vv,
                      const HSVRange &rng) {
    return in_range(h, rng.h_min, rng.h_max) &&
           in_range(s, rng.s_min, rng.s_max) &&
           in_range(vv, rng.v_min, rng.v_max);
}

bool camera_init() {
    camera_config_t cfg = {};

    cfg.ledc_channel = LEDC_CHANNEL_0;
    cfg.ledc_timer   = LEDC_TIMER_0;
    cfg.pin_d0       = Y2_GPIO_NUM;
    cfg.pin_d1       = Y3_GPIO_NUM;
    cfg.pin_d2       = Y4_GPIO_NUM;
    cfg.pin_d3       = Y5_GPIO_NUM;
    cfg.pin_d4       = Y6_GPIO_NUM;
    cfg.pin_d5       = Y7_GPIO_NUM;
    cfg.pin_d6       = Y8_GPIO_NUM;
    cfg.pin_d7       = Y9_GPIO_NUM;
    cfg.pin_xclk     = XCLK_GPIO_NUM;
    cfg.pin_pclk     = PCLK_GPIO_NUM;
    cfg.pin_vsync    = VSYNC_GPIO_NUM;
    cfg.pin_href     = HREF_GPIO_NUM;
    cfg.pin_sscb_sda = SIOD_GPIO_NUM;
    cfg.pin_sscb_scl = SIOC_GPIO_NUM;
    cfg.pin_pwdn     = PWDN_GPIO_NUM;
    cfg.pin_reset    = RESET_GPIO_NUM;
    cfg.xclk_freq_hz = 20000000;
    cfg.pixel_format = PIXFORMAT_RGB565;
    cfg.frame_size   = FRAMESIZE_QQVGA;  // 160x120
    cfg.jpeg_quality = 12;
    cfg.fb_count     = 1;
    cfg.fb_location  = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    cfg.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;

    esp_err_t err = esp_camera_init(&cfg);
    if (err != ESP_OK) {
        Serial.printf("[CAM] Init failed: 0x%04X\n", (unsigned)err);
        return false;
    }

    s_camera_ready = true;
    Serial.println("[CAM] Ready");
    return true;
}

ColorID detect_color() {
    if (!s_camera_ready) return COLOR_NONE;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return COLOR_NONE;

    const int W = (int)fb->width;
    const int H = (int)fb->height;
    const uint8_t *buf = fb->buf;

    int counts[NUM_COLORS] = {};
    uint8_t h_sum = 0, s_sum = 0, v_sum = 0;
    int pixel_samples = 0;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            uint8_t r, g, b, h, s, v;
            int idx = (y * W + x) * 2;
            rgb565_to_rgb(buf[idx], buf[idx + 1], r, g, b);
            rgb_to_hsv(r, g, b, h, s, v);

            // Acumular samples cada 10 píxeles para promedio
            if ((y * W + x) % 10 == 0 && pixel_samples < 100) {
                h_sum += h;
                s_sum += s;
                v_sum += v;
                pixel_samples++;
            }

            for (int c = 1; c < NUM_COLORS; c++) {
                bool matched = match_hsv(h, s, v, COLOR_HSV[c]);
                if (c == (int)COLOR_RED && !matched) {
                    matched = match_hsv(h, s, v, RED_RANGE2);
                }
                if (matched) {
                    counts[c]++;
                    break;
                }
            }
        }
    }

    esp_camera_fb_return(fb);

    // Calcular HSV promedio
    if (pixel_samples > 0) {
        s_latest_h = h_sum / pixel_samples;
        s_latest_s = s_sum / pixel_samples;
        s_latest_v = v_sum / pixel_samples;
    }

    int best_color = (int)COLOR_NONE;
    int best_count = COLOR_DETECT_THRESHOLD;

    for (int c = 1; c < NUM_COLORS; c++) {
        if (counts[c] > best_count) {
            best_count = counts[c];
            best_color = c;
        }
    }

    return (ColorID)best_color;
}

bool i2c_init() {
    if (!Wire.begin((uint8_t)CAM_I2C_ADDR, I2C_SDA_CAM, I2C_SCL_CAM, 100000)) {
        Serial.println("[I2C] Init failed");
        return false;
    }
    Wire.onRequest([]() {
        Wire.write(s_latest_color);
    });
    Serial.printf("[I2C] Slave ready at 0x%02X (SDA=%d SCL=%d)\n", CAM_I2C_ADDR, I2C_SDA_CAM, I2C_SCL_CAM);
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n=== ESP32-CAM COLOR DETECTOR ===");
    
    if (!camera_init()) {
        Serial.println("ERR: Camera init failed");
        while(1) delay(100);
    }
    
    if (!i2c_init()) {
        Serial.println("ERR: I2C init failed");
        while (1) delay(100);
    }

    Serial.println("Ready. Detecting colors for I2C requests...");
    Serial.println("\n*** CALIBRATION MODE ***");
    Serial.println("Apunta cada color frente a la cámara y anota los valores HSV");
}

void loop() {
    ColorID color = detect_color();
    s_latest_color = (uint8_t)color;
    
    // Output formato: [timestamp] Color detectado | HSV raw
    Serial.printf("[%lu] CAM: %s (%d) | HSV: H=%3d S=%3d V=%3d\n", 
                  millis(), 
                  COLOR_NAMES[color], 
                  (int)color, 
                  s_latest_h, s_latest_s, s_latest_v);
    
    delay(500); // Mostrar cada 500ms para mejor legibilidad
}
