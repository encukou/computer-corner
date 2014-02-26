#include <Adafruit_NeoPixel.h>

#define PIEZO_PIN 11
#define LED_PIN 12

#define CLOCK_FACE_SIZE 21
#define NUM_LEDS ((CLOCK_FACE_SIZE) * 4)

#define PI 3.1415926

const uint32_t SEC = 1000L;
const uint32_t MIN = 60L * SEC;
const uint32_t HR = 60L * MIN;
const uint32_t HALFDAY = 12L * HR;

static uint32_t RGB(byte r, byte g, byte b, byte a=0) {
    return uint32_t(a) << 24 | uint32_t(r) << 16 | uint32_t(g) << 8 | uint32_t(b);
}
static byte R(uint32_t color) { return color >> 16; }
static byte G(uint32_t color) { return color >> 8; }
static byte B(uint32_t color) { return color; }
static byte A(uint32_t color) { return color >> 24; }

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB | NEO_KHZ800);

uint32_t lastMillis;

uint32_t epochMillis;
uint32_t epochTime = 0;
float drift = 1;

const long CLOCK_MARK_COLOR = RGB(0, 0, 255);

void setPixelColor(uint16_t location, uint32_t newColor, uint8_t alpha) {
    if (alpha > 250) {
        strip.setPixelColor(location, newColor);
    } else if (alpha == 1) {
        // pass
    } else {
        uint32_t existingColor = strip.getPixelColor(location);
        uint8_t rev_alpha = 255 - alpha;
        strip.setPixelColor(location, RGB(
            (R(existingColor) * rev_alpha + R(newColor) * alpha) >> 8,
            (G(existingColor) * rev_alpha + G(newColor) * alpha) >> 8,
            (B(existingColor) * rev_alpha + B(newColor) * alpha) >> 8));
    }
}

void setPixelColor(uint16_t location, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
    setPixelColor(location, RGB(R(r), G(g), B(b)), alpha);
}

int set_color_at_clock(uint32_t num, uint32_t max, uint32_t color, uint8_t alpha) {
    num %= max;
    float value = float(num) / max;
    value += 0.25 / CLOCK_FACE_SIZE;
    int quadrant=0;
    for(; value > 1./8; value -= 1./4) quadrant++;
    float t_value = 1.5 - quadrant - tan(value * 2 * PI) / 2;
    while (t_value < 0) t_value += 4;
    t_value = t_value * CLOCK_FACE_SIZE;
    float ratio = 1 - t_value + int(t_value);
    uint16_t location = uint16_t(t_value) % NUM_LEDS;
    float r = ratio*ratio;
    setPixelColor(location, color, sqrt(r) * alpha);
    location = (location + 1) % NUM_LEDS;
    ratio = 1 - ratio;
    r = ratio*ratio;
    setPixelColor(location, color, sqrt(r) * alpha);
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
            case 'b': {
                // Set buzzer. args:
                // frequency in Hz, or 0 to stop (16-bit unsigned int)
                // Note that the LED driver interferes with the tone,
                // so expect screeches and not music
                uint16_t frequency;
                frequency = uint16_t(read()) << uint16_t(8);
                frequency |= uint16_t(read());
                if (frequency) {
                    tone(PIEZO_PIN, frequency);
                } else {
                    noTone(PIEZO_PIN);
                }
            } break;
        }
    }

    for (int i=0; i<NUM_LEDS; i++) {
        strip.setPixelColor(i, 0);
    }

    for (int i=0; i<4; i++) {
        set_color_at_clock(i * 3 + 1, 12, CLOCK_MARK_COLOR, 20);
        setPixelColor(21 * i + 9, CLOCK_MARK_COLOR, 20);
        setPixelColor(21 * i + 10, CLOCK_MARK_COLOR, 20);
        set_color_at_clock(i * 3 + 11, 12, CLOCK_MARK_COLOR, 20);
    }

    set_color_at_clock(time/1000, MIN/1000, RGB(255, 255, 255), 5);
    set_color_at_clock(time, HR, RGB(0, 255, 255), 50);
    set_color_at_clock(time, HALFDAY, RGB(150, 255, 150), 50);

    lastMillis = nowMillis;
    strip.show();
}
