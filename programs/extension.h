#ifndef EXTENSION_H
#define EXTENSION_H

#include <stdint.h>
#include <wiringPi.h>

#define NUMBER_SEGMENTS 7

const int num0[] = {1, 1, 1, 0, 1, 1, 1};

const int num1[] = {0, 1, 0, 0, 1, 0, 0};

const int num2[] = {1, 0, 1, 1, 1, 0, 1};

const int num3[] = {1, 0, 1, 1, 0, 1, 1};

const int num4[] = {0, 1, 1, 1, 0, 1, 0};

const int num5[] = {1, 1, 0, 1, 0, 1, 1};

const int num6[] = {1, 1, 0, 1, 1, 1, 1};

const int num7[] = {1, 0, 1, 0, 0, 1, 0};

const int num8[] = {1, 1, 1, 1, 1, 1, 1};

const int num9[] = {1, 1, 1, 1, 0, 1, 1};

void init_pins(int pins);

int* num_to_led_digit(int num);

void num_to_led(int* num, int digits, int on);


