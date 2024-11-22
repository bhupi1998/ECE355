//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include <string.h>

#include "cmsis/cmsis_device.h"

volatile int edge_flag_FG=0;
volatile int edge_flag_555=0;
volatile int inSig = 1;  //default input from signal generator PB changes to 0 for 555 input

unsigned int Freq = 0;  // Example: measured frequency value (global variable)
unsigned int Res = 0;   // Example: measured resistance value (global variable)

SPI_HandleTypeDef SPI_Handle;


//
// LED Display initialization commands
//
unsigned char oled_init_cmds[] =
{
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};


//
// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)
//
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};



// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
#define myTIM3_PRESCALER ((uint16_t) 999) // divide clock by 1000 to get 40KHz -> max delay of 1.64s
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void); //PA IO Setup
void myTIM2_Init(void);
void myTIM3_Init(void); //Initialize TIM3
void TIM3Delay (uint16_t); // timer used for delay purposes
void myEXTI_Init(void);
void myADC_Init(void); // initialize ADC for Potentiometer Reading
void myDAC_Init(void); // Initialize DAC
// Copy and pasted
void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);
void oled_config(void);
void refresh_OLED(void);




// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

 void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/


int
main(int argc, char* argv[])
{
	volatile uint16_t potValRaw;
	volatile uint16_t potValResistance;
	volatile float potValVoltage;

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab by Bhupinder and Micah...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myTIM3_Init();
	myEXTI_Init();		/* Initialize EXTI */
	myADC_Init(); 		/* Initialize ADC*/
	myDAC_Init();		/* Initialize DAC*/
	oled_config();  	/*Oled Configuration*/


	while (1)
	{
		// Start ADC Process,
		ADC1->CR |= ADC_CR_ADSTART;
		// Wait for EOC. Autoclears once ADC_DR is read
		while(!(ADC1->ISR & ADC_ISR_EOC )){};
		potValRaw = ADC1->DR; // Send to DAC Later
		// Voltage value being read by ADC
		potValVoltage=((float)potValRaw/4095)*3.3;
		// Potentiometer Resistance
		Res=1.22*potValRaw;

		// Send to DAC
		DAC1->DHR12R1 = potValRaw;
		// Refresh OLED
		refresh_OLED();

	}

	return 0;

}

//
// LED Display Functions
//


void refresh_OLED( void )
{
    // Buffer size = at most 16 characters per PAGE + terminating '\0'
    unsigned char Buffer[17];

    snprintf( Buffer, sizeof( Buffer ), "R: %5u Ohms   ", Res );
    /* Buffer now contains your character ASCII codes for LED Display
       - select PAGE (LED Display line) and set starting SEG (column)
       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
           send 8 bytes in Characters[c][0-7] to LED Display
    */
   // start from row 2 col 16
   oled_Write_Cmd(0xB2); // select row. Bx where x is the row
   oled_Write_Cmd(0x02); // select col lower. 0x where x is the lower 4 bits of the col number
   oled_Write_Cmd(0x10); // select col upper. 1x where x is the upper 4 bits of the col number
	for(int i = 0; i<17; i++){
		for(int j = 0; j<8; j++){
			oled_Write_Data(Characters[Buffer[i]][j]);
		}
	}


    snprintf( Buffer, sizeof( Buffer ), "F: %5u Hz   ", Freq );
    /* Buffer now contains your character ASCII codes for LED Display
       - select PAGE (LED Display line) and set starting SEG (column)
       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
           send 8 bytes in Characters[c][0-7] to LED Display
    */
   // start from row 4 col 16
   oled_Write_Cmd(0xB4); // select row
   oled_Write_Cmd(0x02); // select col lower
   oled_Write_Cmd(0x10); // select col upper
	for(int i = 0; i<17; i++){
		for(int j = 0; j<8; j++){
			oled_Write_Data(Characters[Buffer[i]][j]);
		}
	}


	/* Wait for ~100 ms (for example) to get ~10 frames/sec refresh rate
       - You should use TIM3 to implement this delay (e.g., via polling)
    */
	// too slow with 100
    TIM3Delay(50);

}


void oled_Write_Cmd( unsigned char cmd )
{
    // make PB6 = CS# = 1
	GPIOB->BSRR = GPIO_BSRR_BS_6;
    // make PB7 = D/C# = 0
	GPIOB->BSRR = GPIO_BSRR_BR_7;
    // make PB6 = CS# = 0
	GPIOB->BSRR = GPIO_BSRR_BR_6;
    oled_Write( cmd );
    // make PB6 = CS# = 1
	GPIOB->BSRR = GPIO_BSRR_BS_6;
}

void oled_Write_Data( unsigned char data )
{
    // make PB6 = CS# = 1
	GPIOB->BSRR = GPIO_BSRR_BS_6;
    // make PB7 = D/C# = 1
	GPIOB->BSRR = GPIO_BSRR_BS_7;
    // make PB6 = CS# = 0
	GPIOB->BSRR = GPIO_BSRR_BR_6;
    oled_Write( data );
    // make PB6 = CS# = 1
	GPIOB->BSRR = GPIO_BSRR_BS_6;
}


void oled_Write( unsigned char Value )
{

    /* Wait until SPI1 is ready for writing (TXE = 1 in SPI1_SR) */

    while(!(SPI1->SR & SPI_SR_TXE));

    /* Send one 8-bit character:
       - This function also sets BIDIOE = 1 in SPI1_CR1
    */
    HAL_SPI_Transmit( &SPI_Handle, &Value, 1, HAL_MAX_DELAY );


    /* Wait until transmission is complete (TXE = 1 in SPI1_SR) */

    while(!(SPI1->SR & SPI_SR_TXE));

}


void oled_config( void )
{

	// Don't forget to enable GPIOB clock in RCC
	/* Enable clock for GPIOB peripheral */
    // This turns on the clock to PortB so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Don't forget to configure PB3/PB5 as AF0
	/* Configure PB3, PB5 as AF0*/
	GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER5);
	GPIOB->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER5_1 ;
	// Don't forget to enable SPI1 clock in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Setup PB4(RES),PB6(CS) and PB7(D/C) as outputs
	// Clear mode bits for PB4, PB6, and PB7
	GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	// Set PB4, PB6, and PB7 to output mode
	GPIOB->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);


    SPI_Handle.Instance = SPI1;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.CRCPolynomial = 7;

//
// Initialize the SPI interface
//
    HAL_SPI_Init( &SPI_Handle );

//
// Enable the SPI
//
    __HAL_SPI_ENABLE( &SPI_Handle );

    /* Reset LED Display (RES# = PB4):
       - make pin PB4 = 0, wait for a few ms
       - make pin PB4 = 1, wait for a few ms
    */
   // set pb4
	GPIOB->BSRR = GPIO_BSRR_BR_4;
	TIM3Delay(5);
	// clear pb4`
	GPIOB->BSRR = GPIO_BSRR_BS_4;
    TIM3Delay(5);

//
// Send initialization commands to LED Display
//
    for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
    {
        oled_Write_Cmd( oled_init_cmds[i] );
    }


    /* Fill LED Display data memory (GDDRAM) with zeros:
       - for each PAGE = 0, 1, ..., 7
           set starting SEG = 0
           call oled_Write_Data( 0x00 ) 128 times
    */
    oled_Write_Cmd(0xB0); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB1); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB2); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB3); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB4); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB5); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB6); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}
    oled_Write_Cmd(0xB7); // select row
    oled_Write_Cmd(0x02); // select col lower
    oled_Write_Cmd(0x10); // select col upper
	for(int COL = 0; COL<131; COL++ ){
		oled_Write_Data(0x00);
	}


}

/*
Set up ADC Operation on PA5
ADC setup is reset on powerup.
Setup operation:
	1-Enable clock for PA and ADC
	2-PA[5] Set to Analog mode
	3-Configure ADC_CHSELR[5] to 1 to select PA5 as a channel
	4-Configure SMPR to 111, slowest sample rate
	5-Configure CR[0]=1 to enable ADC
	6-Wait for ISR[0] = 1
*/
void myADC_Init(){
	// 1
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
    // This turns on the clock to PortA so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Enable ADC clock. BIT9 set to 1
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	// Set PA[5] to Analog Mode. Set to 11 for analog mode
	GPIOA->MODER |= GPIO_MODER_MODER5;
	// Configure ADC Register
	// 12 bit resolution, right align, overrun mode,continous mode
	ADC1->CFGR1 |= ADC_CFGR1_OVRMOD + ADC_CFGR1_CONT;
	ADC1->CFGR1 &= ~(ADC_CFGR1_RES+ADC_CFGR1_ALIGN);
	// Selecting channel 5
	ADC1->CHSELR = ADC_CHSELR_CHSEL5;
	// Select ADC Sampling time
	ADC1->SMPR = ADC_SMPR_SMP;
	// Enable ADC
	ADC1->CR |= ADC_CR_ADEN;
	// Wait for ADRDY Flag, indicating ADC is ready for operation
	while(!(ADC1->ISR & ADC_ISR_ADRDY)){};
}

/*
Set up DAC operation on PA4
Setup Operation:
	1-Enable clock for GPIOA peripheral
	2-PA4 set to analog mode
	3-Enable clock for DAC
	4-Configure DAC_CR
		DAC Enable, Enable TriState buffer, Disable Channel 1 trigger
*/
void myDAC_Init(){

	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
    // This turns on the clock to PortA so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Enable DAC clock. BIT29 set to 1
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	// Set PA[4] to Analog Mode. Set to 11 for analog mode
	GPIOA->MODER |= GPIO_MODER_MODER4;
	// Configure DAC CR
	DAC1->CR &= ~(DAC_CR_BOFF1+DAC_CR_TEN1_Msk);
	// Enable DAC
	DAC1->CR |= DAC_CR_EN1;
}
void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
    // This turns on the clock to PortA so that it's active
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //User button PA0 configuration
	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
	/* Ensure no pull-up/pull-down for PA0 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

    // PA1 configuration - 555 timer
	/* Configure PA1 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);
	/* Ensure no pull-up/pull-down for PA1 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	/* Configure PA2 as input */
	// Function Generator Input
	// Relevant register: GPIOA->MODER
    // GPIO_MODER_MODER2 = 00110000
    GPIOA->MODER &= ~(GPIO_MODER_MODER2);
	/* Ensure no pull-up/pull-down for PA2 */
    // set to 00 for no pull ups
	// Relevant register: GPIOA->PUPDR
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);


}
// Initialize Timer3. This will be used to implement dalays
void myTIM3_Init(){
	// Enable TIM3 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	/* TIM3 Configuration:
		-Auto-reload disabled. Using it as a one shot timer
		-Count Down mode. Starting value will be the delay desired
		-CEN:0 Counter is not enabled
		-UDIS:0 Enable update event.
		-URS:1 Only underflow generated an update interrut
		-!OPM:1 One Pulse Mode. Counter stops at update event. CEN is cleared
		-DIR:1 Count down
		-CMS:00: Let DIR dictate count direction
		-ARPE:0: No Auto Reload Buffer
	*/
	TIM3->CR1 = 0xb00011100;
	TIM3->PSC = myTIM3_PRESCALER;
}
// Used to create a delay
// Input: delay in ms
// Returns: Nothing
// TIM3 is 16bit
// Max delay of 1.64s
void TIM3Delay (uint16_t delay){
	int counterVal = delay*SystemCoreClock/((myTIM3_PRESCALER+1)*1000);
	TIM3->ARR = counterVal;
	// Clear UIF flag
	TIM3->SR &= ~TIM_SR_UIF;

	// Enable counter
	TIM3->CR1 |= TIM_CR1_CEN;

	// Wait for the update event (timer underflow)
    while (!(TIM3->SR & TIM_SR_UIF));
	// since timer is in ONS mode CEN gets cleared
    // Clear the update interrupt flag
    TIM3->SR &= ~TIM_SR_UIF;
}
// Initialize Timer 2. This will be used to measure frequency of an incoming signal
// The interrupt is enabled in case of overflow
void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	// Relevant register: TIM2->CR1
    // Set to 0b10001100
    TIM2->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;			;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
    TIM2->EGR = ((uint16_t)0x0001);
	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
    NVIC_SetPriority(TIM2_IRQn, 0);
	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
    NVIC_EnableIRQ(TIM2_IRQn);
	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
    TIM2->DIER |= TIM_DIER_UIE;
}

// Initialize interrupts to external inputs
void myEXTI_Init()
{
	/******PA2 (Function Generator)********/
	// Relevant register: SYSCFG->EXTICR[0]
	// Select EXT2 Source as function generator pin
	SYSCFG->EXTICR[0] &= ~(0xF<<8) ;
	/* EXTI2 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2; // enable rising edge for PA2
	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	// set EXTI IMR bit 2 (...43210) to 1 to unmask
	EXTI->IMR |= EXTI_IMR_MR2;

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	// set PA2 priority to 0 (highest priority)
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	//interrupt set enable register -> EXTI2 (bit 6) -> 1 enables
	NVIC_EnableIRQ(EXTI2_3_IRQn);

	/******PA1 (555 Timer)********/
	// Unmask Interrupt Requests for line 1
	EXTI->IMR &= ~EXTI_IMR_MR1; // keeping one masked for now
	// Select Rising Edge
	EXTI->RTSR |= EXTI_RTSR_TR1;
	// Select EXT1 Sourse As Timer pin
	SYSCFG->EXTICR[0] &= ~(0xF<<4) ;

	/******PA0 (User Button)********/
	// Unmask Interrupt Requests for line 0
	EXTI->IMR |= EXTI_IMR_MR0;
	// Select Rising Edge
	EXTI->RTSR |= EXTI_RTSR_TR0;
	// Select EXT0 Sourse As user button pin
	SYSCFG->EXTICR[0] &= ~(0xF);

	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

// !This is unchanged
/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR (status register, clear ^^Flag)
		TIM2->SR &= ~(TIM_SR_UIF);
		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}

// declared elsewhere - system/src/cmsis/vectors_stm32f051x8.c
void EXTI0_1_IRQHandler()
{
	//logic for 555 Timer
	if((EXTI->PR & EXTI_PR_PR1)!=0){
		// Declare/initialize your local variables here...
		volatile unsigned int count=0; // variable to save counter value
		float freq, period;
		// 1. If this is the first edge:
		if(edge_flag_555 == 0){
			// set edge flag to 1
			edge_flag_555 = 1;
			//	- Clear count register (TIM2->CNT).
			TIM2->CNT = 0x0;
			//	- Start timer (TIM2->CR1).
			TIM2->CR1 |= (0x1);

		//    Else (this is the second edge):
		} else
		{
			//	- Stop timer (TIM2->CR1).
			TIM2->CR1 &= ~(0x1);

			// clear flag to prepare for next period
			edge_flag_555 = 0;

			//	- Read out count register (TIM2->CNT).
			count = TIM2->CNT;
			//	- Calculate signal period and frequency.
			period = (float)count / (float)SystemCoreClock;
			freq = 1/period;
			Freq = (int)freq; // assigning it to the global value.
			//trace_printf("FG Period is: %f ms     -   FG Freq is: %f kHz\n", (float) period*1000,(float) freq/1000);

		}
		EXTI->PR |= EXTI_PR_PR1;
	}

	// logic for button press

	if((EXTI->PR & EXTI_PR_PR0)!=0){
		for(int i = 0; i<500000; i++){} // debounce
		if(inSig == 0){
			inSig = 1;
			//Disable EXTI1 interrupt
			//mask interrupts from EXTI0 line
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 1 (...43210) to 0 to mask
			EXTI->IMR &= ~EXTI_IMR_MR1;
			//Enable EXTI2 interrupt
			/* Unmask interrupts from EXTI2 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 2 (...43210) to 1 to unmask
			EXTI->IMR |= EXTI_IMR_MR2;
		}else{
			inSig = 0;
			//Disable EXTI2 interrupt
			/* mask interrupts from EXTI2 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 2 (...43210) to 0 to mask
			EXTI->IMR &= ~EXTI_IMR_MR2;
			//Enable EXTI1 interrup
			/* Unmask interrupts from EXTI0 line */
			// Relevant register: EXTI->IMR
			// set EXTI IMR bit 0 (...43210) to 1 to unmask
			EXTI->IMR |= EXTI_IMR_MR1;
		}
		EXTI->PR |= EXTI_PR_PR0;
	}

}

// This is unchanged

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...
	volatile unsigned int count=0; // variable to save counter value
	float freq, period;


	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{
		//
		// 1. If this is the first edge:
		if(edge_flag_FG == 0){
			// set edge flag to 1
			edge_flag_FG = 1;
			//	- Clear count register (TIM2->CNT).
			TIM2->CNT = 0x0;
			//	- Start timer (TIM2->CR1).
			TIM2->CR1 |= (0x1);

		//    Else (this is the second edge):
		} else
		{
			//	- Stop timer (TIM2->CR1).
			TIM2->CR1 &= ~(0x1);

			// clear flag to prepare for next period
			edge_flag_FG = 0;

			//	- Read out count register (TIM2->CNT).
			count = TIM2->CNT;
			//	- Calculate signal period and frequency.
			period = (float)count / (float)SystemCoreClock;
			freq = 1/period;
			Freq = (int)freq; // assigning it to the global value.
			//trace_printf("FG Period is: %f ms     -   FG Freq is: %f kHz\n", (float) period*1000,(float) freq/1000);

		}
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		EXTI->PR |= EXTI_PR_PR2;

		}
	}





#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
