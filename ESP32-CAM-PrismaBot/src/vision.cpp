#include "vision.h"
#include "robot_config.h"
#include "camera_pins.h"
#include "esp_camera.h"
#include <Arduino.h>

static bool s_camera_ready = false;

// ── Low-level pixel helpers ───────────────────────────────────────

// Unpack two raw bytes from the frame buffer (big-endian RGB565,
// as produced by the OV2640 DMA output) into 8-bit R, G, B.
// MSB-first: byte0 = RRRRRGGG, byte1 = GGGBBBBB
static void rgb565_to_rgb(uint8_t b0, uint8_t b1,
                          uint8_t &r, uint8_t &g, uint8_t &b) {
    uint16_t px = ((uint16_t)b0 << 8) | b1;
    r = ((px >> 11) & 0x1F) * 255 / 31;
    g = ((px >>  5) & 0x3F) * 255 / 63;
    b = ( px        & 0x1F) * 255 / 31;
}

// Convert 8-bit RGB to OpenCV-style HSV:
//   H : 0–179  (half of 0-360°)
//   S : 0–255
//   V : 0–255
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

    h = (uint8_t)(hh / 2);   // compress to 0-179
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

// ── Camera initialisation ─────────────────────────────────────────

bool vision_init() {
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
    cfg.pin_sscb_sda = SIOD_GPIO_NUM;   // I2C-like camera bus data
    cfg.pin_sscb_scl = SIOC_GPIO_NUM;   // I2C-like camera bus clock
    cfg.pin_pwdn     = PWDN_GPIO_NUM;
    cfg.pin_reset    = RESET_GPIO_NUM;
    cfg.xclk_freq_hz = CAM_XCLK_FREQ_HZ;
    cfg.pixel_format = CAM_PIXEL_FORMAT;
    cfg.frame_size   = CAM_FRAME_SIZE;
    cfg.jpeg_quality = 12;
    cfg.fb_count     = 1;
    cfg.fb_location  = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    cfg.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;

    esp_err_t err = esp_camera_init(&cfg);
    if (err != ESP_OK) {
        Serial.printf("[vision] Camera init failed: 0x%04X\n", (unsigned)err);
        return false;
    }

    s_camera_ready = true;
    Serial.println("[vision] Camera ready.");
    return true;
}

// ── Frame capture and colour detection ───────────────────────────

VisionResult vision_capture() {
    VisionResult result = { COLOR_NONE, POS_CENTER };
    if (!s_camera_ready) return result;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[vision] Frame grab failed.");
        return result;
    }

    const int W     = (int)fb->width;
    const int H     = (int)fb->height;
    const int third = W / 3;
    const uint8_t *buf = fb->buf;  // raw byte buffer (big-endian RGB565)

    // Pixel counts per [color][column-region: 0=left,1=center,2=right]
    int counts[NUM_COLORS][3] = {};

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            uint8_t r, g, b, h, s, v;
            int idx = (y * W + x) * 2;
            rgb565_to_rgb(buf[idx], buf[idx + 1], r, g, b);
            rgb_to_hsv(r, g, b, h, s, v);

            int region = (x < third) ? 0 : (x < 2 * third) ? 1 : 2;

            for (int c = 1; c < NUM_COLORS; c++) {
                bool matched = match_hsv(h, s, v, COLOR_HSV[c]);
                // Red wraps around 0/180 — check the second range too
                if (c == (int)COLOR_RED && !matched) {
                    matched = match_hsv(h, s, v, RED_RANGE2);
                }
                if (matched) {
                    counts[c][region]++;
                    break;
                }
            }
        }
    }

    esp_camera_fb_return(fb);

    // ── Determine dominant colour ─────────────────────────────────
    int best_color = (int)COLOR_NONE;
    int best_count = COLOR_DETECT_THRESHOLD;

    for (int c = 1; c < NUM_COLORS; c++) {
        int total = counts[c][0] + counts[c][1] + counts[c][2];
        if (total > best_count) {
            best_count = total;
            best_color = c;
        }
    }

    // ── Determine lateral position from colour distribution ───────
    Position pos = POS_CENTER;
    if (best_color != (int)COLOR_NONE) {
        int l = counts[best_color][0];
        int c = counts[best_color][1];
        int r = counts[best_color][2];
        if      (l > c && l > r) pos = POS_LEFT;
        else if (r > c && r > l) pos = POS_RIGHT;
        else                     pos = POS_CENTER;
    }

    result.color    = (ColorID)best_color;
    result.position = pos;
    return result;
}
