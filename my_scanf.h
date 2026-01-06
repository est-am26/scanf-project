#ifndef MY_SCANF_H
#define MY_SCANF_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

// Definition of the structure for the color
typedef struct {
    int r;
    int g;
    int b;
} RGBColor;

typedef struct {
    int day;
    int month;
    int year;
} Date;

// This is the "signature" of your function
int my_scanf(const char *format, ...);

#endif