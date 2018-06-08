main:
                       @ Configure the LED GPIO pin for output

    ldr r0,=0x3f200000  @ Load r0 with hexadecimal number: 0x3f200000 (Raspberry Pi 2)
    mov r1,#1           @ Move decimal number 1 to r1
    lsl r1,#21          @ Logical shift left the value in r1 by 21 places
    str r1,[r0,#16]      @ Store r1 in the address calculated by [r0 + 16]  (Raspberry Pi 2)
                        @ Turn the LED on
    mov r1, #1          @ Move decimal number 1 to r1
    lsl r1, #15         @ Logical shift left the value in r1 by 15 places
    str r1, [r0,#32]    @ Store r1 in the address calculated by [r0 + 32] (Raspberry Pi 2).
                        @ Now on pull-high pin (47) instead of pull-low pin (16), so write
                        @ to GPSET register instead of GPCLR.
   b main
loop$:
    b loop$             @ Branch to loop$

// incomplete
// reference: https://electronics.stackexchange.com/questions/271895/make-an-led-blink-on-rpi2-in-assembly