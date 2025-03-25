#include <FastLED.h>

#define LED_PIN     5      // Pin für WS2812-LEDs
#define NUM_LEDS    300    // Anzahl der LEDs
#define BUTTON_PIN  18     // Pin für den Taster
#define RELAY_PIN   2      // Pin für das Relais

#define DECAY_RATE  0.1    // Geschwindigkeit des Leeren (pro Loop-Durchlauf)
#define INCREMENT   2      // Anzahl der LEDs pro Knopfdruck
#define RELAY_TIME  3000   // Relais-Aktivierungszeit in ms

CRGB leds[NUM_LEDS];
volatile bool buttonPressed = false;
int ledLevel = 0;
unsigned long lastPressTime = 0;
bool relayActive = false;
unsigned long relayStartTime = 0;

void IRAM_ATTR handleButtonPress() {
    buttonPressed = true;
}

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    attachInterrupt(BUTTON_PIN, handleButtonPress, FALLING);
    digitalWrite(RELAY_PIN, LOW);
}

void loop() {
    if (buttonPressed) {
        Serial.print("Button pressed");
        buttonPressed = false;
        ledLevel += INCREMENT;
        if (ledLevel > NUM_LEDS) ledLevel = NUM_LEDS;
        lastPressTime = millis();
    }
    
    // LEDs leeren sich langsam
    if (millis() - lastPressTime > 100) {
        ledLevel -= DECAY_RATE;
        if (ledLevel < 0) ledLevel = 0;
    }
    
    // LEDs anzeigen
    for (int i = 0; i < NUM_LEDS; i++) {
        if (i < ledLevel) {
            leds[i] = CRGB::Green;
        } else {
            leds[i] = CRGB::Black;
        }
    }
    FastLED.show();

    // Relais aktivieren, wenn voll
    if (ledLevel >= NUM_LEDS && !relayActive) {
        digitalWrite(RELAY_PIN, HIGH);
        relayActive = true;
        relayStartTime = millis();
    }
    
    // Relais nach einer Zeit wieder deaktivieren
    if (relayActive && millis() - relayStartTime > RELAY_TIME) {
        digitalWrite(RELAY_PIN, LOW);
        relayActive = false;
        ledLevel = 0; // Spiel resetten
    }
}
