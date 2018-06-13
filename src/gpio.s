//=========================================================================
// ARM11 Assembly code to flash GPIO 16 LED on and off


ldr r1,=0x20200004
ldr r2,=0x00040000
str r2, [r1]
ldr r3,=0x20200028
ldr r4,=0x2020001c
ldr r5,=0x00010000

loop:
str r5, [r3]
mov r6,#0x01100000

wait:
sub r6,r6,#1
cmp r6,#0
bne wait
str r5,[r4]
mov r6,#0x01100000

second_wait:
sub r6,r6,#1
cmp r6,#0
bne second_wait
b loop