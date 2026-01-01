#ifndef MY_SCANF_H
#define MY_SCANF_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

// Definición de la estructura para el color
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

// Esta es la "firma" de tu función
int my_scanf(const char *format, ...);

#endif