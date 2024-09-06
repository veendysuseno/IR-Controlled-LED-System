# IR-Controlled LED System

This project uses an Arduino to control an LED array with an IR remote. It allows you to turn LEDs on and off, adjust brightness, and animate LEDs in forward or backward modes.

## Components Used

- Arduino Uno
- 8x LED array (connected through a shift register)
- IR Receiver
- Shift Register (e.g., 74HC595)
- Resistors
- Jumper Wires
- Breadboard

## Pin Configuration

- **PIN_IR_RECEIVER**: connected to the IR receiver (Pin 11)
- **PIN_RED**: connected to the red LED pin (Pin 3)
- **PIN_GREEN**: connected to the green LED pin (Pin 9)
- **PIN_BLUE**: connected to the blue LED pin (Pin 10)
- **BRIGHTNESS_PIN**: connected to the PWM pin for brightness control (Pin 5)
- **DATA_PIN**: connected to the data input of the shift register (Pin 8)
- **LATCH_PIN**: connected to the latch pin of the shift register (Pin 9)
- **CLOCK_PIN**: connected to the clock pin of the shift register (Pin 10)

## Remote Control Mapping

- **POWER_EVENT**: Toggles all LEDs on/off
- **NUMBERS_EVENT[]**: Controls individual LEDs (0-7)
- **PLUS_EVENT**: Increases brightness
- **MINUS_EVENT**: Decreases brightness
- **FORWARD_EVENT**: Animates LEDs in forward mode
- **BACKWARD_EVENT**: Animates LEDs in backward mode

## Code

```cpp
#include <IRremote.h>

// Pin Definitions
const int BRIGHTNESS_PIN = 5;
const int DATA_PIN = 8;
const int LATCH_PIN = 9;
const int CLOCK_PIN = 10;
const int RECV_PIN = 11;

// Mapping Button Signals
const unsigned long POWER_EVENT = 0xE318261B;
const unsigned long NUMBERS_EVENT[] = {
    0xC101E57B,
    0x9716BE3F,
    0x3D9AE3F7,
    0x6182021B,
    0x8C22657B,
    0x488F3CBB,
    0x449E79F,
    0x32C6FDF7,
    0x1BC0157B,
    0x3EC3FC1B
};
const unsigned long PLUS_EVENT = 0xE5CFBD7F;
const unsigned long MINUS_EVENT = 0xA3C8EDDB;
const unsigned long FORWARD_EVENT = 0x20FE4DBB;
const unsigned long BACKWARD_EVENT = 0xD7E84B1B;

// Constants
bool LED_FORWARD = true;
bool LED_BACKWARD = false;
const int MAX_BRIGHTNESS = 50;

// Global Vars
IRrecv irrecv(RECV_PIN);
decode_results results;

byte leds = 0b00000000;
int brightness = MAX_BRIGHTNESS;

void setup() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(BRIGHTNESS_PIN, OUTPUT);
    analogWrite(BRIGHTNESS_PIN, 255 - brightness);

    Serial.begin(9600);
    irrecv.enableIRIn(); // Start the receiver
}

void loop() {
    if (irrecv.decode(&results)) {
        Serial.print("Command: "); Serial.println(results.value, HEX);

        switch (results.value) {
            case POWER_EVENT:
                if (leds == 0) {
                    Serial.println("> [POWER]: Turning on all LEDs...");
                    leds = 0b11111111;
                } else {
                    Serial.println("> [POWER]: Turning off all LEDs...");
                    leds = 0;
                }
                break;

            case PLUS_EVENT:
                brightness += (brightness < MAX_BRIGHTNESS) ? 5 : 0;
                Serial.print("> [PLUS]: Increasing brightness -> ");
                Serial.println(brightness);
                analogWrite(BRIGHTNESS_PIN, 255 - brightness);
                break;

            case MINUS_EVENT:
                brightness -= (brightness > 0) ? 5 : 0;
                Serial.print("> [MINUS]: Decreasing brightness -> ");
                Serial.println(brightness);
                analogWrite(BRIGHTNESS_PIN, 255 - brightness);
                break;

            case FORWARD_EVENT:
                Serial.println("> [FORWARD]: LED forward mode");
                led_animation(LED_FORWARD);
                analogWrite(BRIGHTNESS_PIN, brightness); // Restore brightness
                break;

            case BACKWARD_EVENT:
                Serial.println("> [BACKWARD]: LED backward mode");
                led_animation(LED_BACKWARD);
                analogWrite(BRIGHTNESS_PIN, brightness);
                break;

            default:
                check_number_button();
                break;
        }

        irrecv.resume();
        delay(10);
    }

    update_leds();
}

void led_animation(bool ascending) {
    int increment = (ascending) ? 1 : -1;
    int led_start = (ascending) ? 0 : 7;
    int led_end   = (ascending) ? 8 : -1;

    for (int i = led_start; i != led_end; i += increment) {
        leds = 0;
        bitSet(leds, i);
        update_leds();

        for (int j = 0; j < 255; j++) {
            analogWrite(BRIGHTNESS_PIN, 255 - j);
            delayMicroseconds(600);
        }
        for (int j = 255; j >= 0; j--) {
            analogWrite(BRIGHTNESS_PIN, 255 - j);
            delayMicroseconds(600);
        }
    }

    leds = 0;
    update_leds();
}

void update_leds() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, leds);
    digitalWrite(LATCH_PIN, HIGH);
}

void check_number_button() {
    for (int i = 0; i <= 7; i++) {
        if (NUMBERS_EVENT[i] == results.value) {
            Serial.print("> [BUTTON]: "); Serial.print(i);
            if (is_led_on(i)) {
                Serial.println(" | Turning off LED");
                turn_off_led(i);
            } else {
                Serial.println(" | Turning on LED");
                turn_on_led(i);
            }
            break;
        }
    }
}

bool is_led_on(int i) {
    return leds & (1 << i);
}

void turn_on_led(int led) {
    bitSet(leds, led);
}

void turn_off_led(int led) {
    bitClear(leds, led);
}
```

## How to Use

1. Connect the components according to the pin configuration.
2. Upload the code to your Arduino.
3. Point your IR remote at the IR receiver.
4. Use the remote to control the LEDs: toggle all LEDs, adjust brightness, and animate LEDs.

## Notes

- The analogWrite(BRIGHTNESS_PIN, 255 - brightness) line adjusts the LED brightness by mapping the brightness value.
- Ensure that the shift register is properly connected and functioning to control the LEDs.

#### Feel free to modify the code and connections based on your project requirements!
