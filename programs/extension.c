#include "extension.h"
#include <stdint.h>

/* Program that generates a random number between 0 and 9 on a "NUMBER_SEGMENT display".
 * Then turns on the corresponding LEDs on the Raspberry Pi to show the number
 * on the LED seven segment display
 */

int main(void) {
    if (wiringPiSetup() == -1) {                // if the initialisation of wiringPi fails,
        printf("setup wiringPi failed !\n");    // print message to screen
        return -1;
    }

    init_pins(NUMBER_SEGMENTS);

    while(1) {
        int limit = 9;
        int random = rand() % limit;

        int* num_random = malloc(sizeof(int) * NUMBER_SEGMENTS);
        num_to_led_digit(random);

        num_to_led(num_random, NUMBER_SEGMENTS, 1); // turn LEDs on
        delay(2000);                            // wait 2 seconds

        num_to_led(num_random, NUMBER_SEGMENTS, 0); // turn LEDs off
        delay(2000);                            // wait 2 seconds
    }
    return 0;
}

// Initialises all of the needed pins
void init_pins(int pins) {
    for (int i = 0; i < pins; i++) {
        pinMode(i, OUTPUT);
    }
}

int* num_to_led_digit(int num) {
    switch(num) {
        case 0 : return num0;
        case 1 : return num1;
        case 2 : return num2;
        case 3 : return num3;
        case 4 : return num4;
        case 5 : return num5;
        case 6 : return num6;
        case 7 : return num7;
        case 8 : return num8;
        case 9 : return num9;
        default : break;
    }
}

/*
// Converts an unsigned integer into a binary number, stored as an integer array
void int_to_bin_digit(unsigned int in, int digits, int* out) {
    assert: digits <= sizeof(int)*CHAR_BIT;

    unsigned int mask = 1U << (digits - 1);
    int i;
    for (i = 0; i < digits; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}
*/



// Takes a number and turns on the LEDs to display it on the counter
// NOTE: if LEDs to be turned on then should on = 1,
// else if to be turned off then off = 1;
void num_to_led(int* num, int digits, int on) {
    if (on) {
        for (int i = 0; i < digits; i++) {
            if (num[i]) {
                digitalWrite(i, LOW);   // LED on
                printf("LED of bit %d is on\n", i);
            }
        }
    }

    else {
        for (int i = 0; i < digits; i++) {
            digitalWrite(i, HIGH);   // LED off
            printf("LED of bit %d is off\n", i);
        }
    }
}