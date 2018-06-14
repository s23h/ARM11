#include <wiringPi.h>
#include <stdio.h>

#define NUMBER_BITS 5

/* Program that generates a random number between 0 and 2^{number of bits}.
 * Then turns on the corresponding LEDs on the Raspberry Pi to show the number
 * as a binary on the LED counter
 */

int main(void) {
    if (wiringPiSetup() == -1) {                // if the initialisation of wiringPi fails,
        printf("setup wiringPi failed !\n");    // print message to screen
        return -1;
    }

    init_pins(NUMBER_BITS);

    while(1) {
        int limit = (int) pow(2, NUMBER_BITS) - 1;
        int random = rand() % limit;

        int* bin_random = malloc(sizeof(int) * NUMBER_BITS);
        int_to_bin_digit(random, NUMBER_BITS, bin_random);

        bin_to_led(bin_random, NUMBER_BITS, 1); // turn LEDs on
        delay(2000);                            // wait 2 seconds

        bin_to_led(bin_random, NUMBER_BITS, 0); // turn LEDs off
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

// Takes a binary number and turns on the LEDs to display it on the counter
// NOTE: if LEDs to be turned on then should on = 1,
// else if to be turned off then off = 1;
void bin_to_led(int* bin, int digits, int on) {
    if (on) {
        for (int i = 0; i < digits; i++) {
            if (bin[i]) {
                digitalWrite(i, LOW);   // LED on
                printf("LED of bit %d is on\n", i);
            }
        }
    }

    else {
        for (int i = 0; i < digits; i++) {
            if (bin[i]) {
                digitalWrite(i, HIGH);   // LED off
                printf("LED of bit %d is off\n", i);
            }
        }
    }
}