#include "my_scanf.h"

int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0;
    const char *p = format;

    while (*p != '\0') {
        if (*p == '%') {
            p++; // Saltamos el % para ver qué letra sigue

            if (*p == 'd') {
                // 1. Obtenemos la dirección de la variable (el puntero)
                int *dest = va_arg(args, int *);
                int c;

                // 2. Saltamos espacios en blanco iniciales
                do {
                    c = getchar();
                } while (isspace(c));

                // 3. Convertimos los caracteres en un número real
                int valor = 0;
                int sign = 1;

                if (c == '-') {
                    sign = -1;
                    c = getchar();
                }

                while (isdigit(c)) {
                    valor = valor * 10 + (c - '0');
                    c = getchar();
                }

                // 4. Guardamos el resultado en la variable
                *dest = valor * sign;

                // 5. Devolvemos el carácter que no era número al teclado
                ungetc(c, stdin);
                count++;
            }
            // Aquí irán los otros: else if (*p == 'c'), etc.
        }
        p++;
    }

    va_end(args);
    return count;
}