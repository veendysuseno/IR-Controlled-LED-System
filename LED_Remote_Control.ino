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
IRrecv irrecv( RECV_PIN );
decode_results results;

byte leds = 0b00000000;
int brightness = MAX_BRIGHTNESS;

void setup() {
    pinMode( DATA_PIN, OUTPUT );
    pinMode( LATCH_PIN, OUTPUT );
    pinMode( CLOCK_PIN, OUTPUT );
    pinMode( BRIGHTNESS_PIN, OUTPUT );
    pinMode( SPEAKER_PIN, OUTPUT );
    analogWrite( BRIGHTNESS_PIN, 255 - brightness );
    
    Serial.begin( 9600 );
    irrecv.enableIRIn(); // Start the receiver
}

void loop() {
    
     if( irrecv.decode(&results) ) {
        Serial.print( "Command: " ); Serial.println( results.value, HEX );
            
        switch( results.value ) {
            case POWER_EVENT:
                if( leds == 0 ) {
                    Serial.println( ">[POWER]: Menyalakan semua lampu..." );
                    leds = 0b11111111;
                } else {
                    Serial.println( ">[POWER]: Mematikan semua lampu..." );
                    leds = 0;
                }
            break;

            case PLUS_EVENT:
                brightness += (brightness < MAX_BRIGHTNESS) ? 5 : 0;
                Serial.print(">[PLUS]: Menambah level brightness -> ");
                Serial.println( brightness );
                analogWrite( BRIGHTNESS_PIN, 255 - brightness );
            break;

            case MINUS_EVENT:
                brightness -= (brightness > 0) ? 5 : 0;
                Serial.print(">[MINUS]: Mengurangi level brightness -> ");
                Serial.println( brightness );
                analogWrite( BRIGHTNESS_PIN, 255 - brightness );
            break;

            case FORWARD_EVENT:
                Serial.println(">[FORWARD]: Mode LED forward");
                led_animation( LED_FORWARD );
                analogWrite( BRIGHTNESS_PIN, brightness ); // kembalikan brightness ke semula
            break;

            case BACKWARD_EVENT:
                Serial.println(">[BACKWARD]: Mode LED backward");
                led_animation( LED_BACKWARD );
                analogWrite( BRIGHTNESS_PIN, brightness );
            break;

            default:
                check_number_button();
            break;
        }

        irrecv.resume();
        delay( 10 );
    } 
    
    update_leds();
}

void led_animation( bool ascending ) {

    int increment = (ascending) ? 1 : -1;
    int led_start = (ascending) ? 0 : 7;
    int led_end   = (ascending) ? 8 : -1;

    for( int i = led_start; i != led_end; i += increment ) {
        
        leds = 0;
        bitSet( leds, i );
        update_leds();
        
        for( int j = 0; j < 255; j++ ) {
            analogWrite( BRIGHTNESS_PIN, 255 - j );
            delayMicroseconds( 600 );
        }
        for( int j = 255; j >= 0; j-- ) {
            analogWrite( BRIGHTNESS_PIN, 255 - j ); 
            delayMicroseconds( 600 );
        }
    }

    leds = 0;
    update_leds();
}

void update_leds() {
    digitalWrite( LATCH_PIN, LOW );
    shiftOut( DATA_PIN, CLOCK_PIN, MSBFIRST, leds );
    digitalWrite( LATCH_PIN, HIGH );
}

void check_number_button() {
    for( int i = 0; i <= 7; i++ ) {
        if( NUMBERS_EVENT[i] == results.value ) {
            Serial.print(">[BUTTON]: "); Serial.print( i );
            if( is_led_on(i) ) {
                Serial.println( " | Mematikan LED" );
                turn_off_led( i );
            } else {
                Serial.println( " | Menyalakan LED" );
                turn_on_led( i );
            }
            break;
        }
    }
}

bool is_led_on( int i ) {
    return leds & (1 << i);
}

void turn_on_led( int led ) {
    bitSet( leds, led );
}

void turn_off_led( int led ) {
    bitClear( leds, led );
}
