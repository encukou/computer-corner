#include <Adafruit_NeoPixel.h>

#define LED_PIN 12

#define CLOCK_FACE_SIZE 21
#define NUM_LEDS ((CLOCK_FACE_SIZE) * 4)

#define PI 3.1415926

const uint32_t SEC = 1000L;
const uint32_t MIN = 60L * SEC;
const uint32_t HR = 60L * MIN;
const uint32_t HALFDAY = 12L * HR;

#define RGB(r, g, b) (((uint32_t(r) & 0xff) << 16) | ((uint32_t(g) & 0xff) << 8) | (uint32_t(b) & 0xff))
#define R(rgb) ((uint32_t(rgb)>>16) & 0xff)
#define G(rgb) ((uint32_t(rgb)>>8) & 0xff)
#define B(rgb) ((uint32_t(rgb)) & 0xff)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB | NEO_KHZ800);

uint32_t lastMillis;

uint32_t epochMillis;
uint32_t epochTime = 0;
float drift = 1;

const long CLOCK_MARK_COLOR = RGB(0, 0, 10);

int set_color_at_clock(uint32_t num, uint32_t max, uint32_t color) {
    num %= max;
    float value = float(num) / max;
    value += 0.25 / CLOCK_FACE_SIZE;
    int quadrant=0;
    for(; value > 1./8; value -= 1./4) quadrant++;
    float t_value = 1.5 - quadrant - tan(value * 2 * PI) / 2;
    while (t_value < 0) t_value += 4;
    t_value = t_value * CLOCK_FACE_SIZE;
    float ratio = 1 - t_value + int(t_value);
    int location = int(t_value) % NUM_LEDS;
    float r = ratio*ratio;
    strip.setPixelColor(location, RGB(R(color)*r,G(color)*r,B(color)*r));
    location = (location + 1) % NUM_LEDS;
    ratio = 1 - ratio;
    r = ratio*ratio;
    strip.setPixelColor(location, RGB(R(color)*r,G(color)*r,B(color)*r));
}

byte read() {
    int val;
    do {
        val = Serial.read();
    } while (val == -1);
    return val;
}

void setup() {
    Serial.begin(9600);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    lastMillis = millis();
    pinMode(13, OUTPUT);

    randomSeed(analogRead(0));
}

void loop() {
    uint32_t nowMillis = millis();
    unsigned delta = nowMillis - lastMillis;

    uint32_t time = epochTime + (nowMillis - epochMillis) * drift;

    if (Serial.available()) {
        switch (Serial.read()) {
            case 't': {
                // Set time. args:
                // no. of milliseconds since midnight (32-bit unsigned int)
                uint32_t lastEpochTime = epochTime;
                uint32_t lastEpochMillis = epochMillis;
                epochMillis = millis();
                epochTime = uint32_t(read()) << uint32_t(24);
                epochTime |= uint32_t(read()) << uint32_t(16);
                epochTime |= uint32_t(read()) << uint32_t(8);
                epochTime |= uint32_t(read());
                if (lastEpochTime) {
                    drift = float(epochTime - lastEpochTime) /
                            float(epochMillis - lastEpochMillis);
                }
            } break;
        }
    }

    for (int i=0; i<NUM_LEDS; i++) {
        strip.setPixelColor(i, 0);
    }

    for (int i=0; i<4; i++) {
        set_color_at_clock(i * 3 + 1, 12, CLOCK_MARK_COLOR);
        strip.setPixelColor(21 * i + 9, CLOCK_MARK_COLOR);
        strip.setPixelColor(21 * i + 10, CLOCK_MARK_COLOR);
        set_color_at_clock(i * 3 + 11, 12, CLOCK_MARK_COLOR);
    }

    set_color_at_clock(time/1000, MIN/1000, RGB(5, 5, 5));
    set_color_at_clock(time, HR, RGB(0, 20, 20));
    set_color_at_clock(time, HALFDAY, RGB(15, 20, 15));

    lastMillis = nowMillis;
    strip.show();
}
