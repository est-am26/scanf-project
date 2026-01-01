#include "my_scanf.h"

// Función ayudante para leer un entero
int read_int() {
    int c; // Guarda el carácter leído desde stdin.
    int valor = 0; // Acumula el valor numérico del entero mientras se leen los dígitos.
    int sign = 1; // Asume que el número es positivo por defecto (se multiplica al final con el resultado).

    // The loop skips over leading whitespace by repeatedly reading characters as long as they are classified as whitespace.
    // It stops as soon as a non-whitespace character is encountered, which is the start of meaningful input.
    do {
        c = getchar();
    } while (isspace(c)); // Usa isspace para cubrir espacios, tabs, newlines, etc.

    /* Check for an optional sign character.
     * If a '-' is found, record that the number is negative
     * and advance to the next character.
     * If a '+' is found, simply advance to the next character.
     */
    if (c == '-') {
        sign = -1;
        c = getchar();
    } else if (c == '+') {
        c = getchar();
    }

    /* Read consecutive digit characters and build the integer value.
     * Each new digit shifts the previous value left by one decimal place
     * and adds the numeric value of the current character.
     */
    while (isdigit(c)) {
        valor = valor * 10 + (c - '0');
        c = getchar();
    }

    /* The loop terminates after reading one character beyond the number.
     * Push this non-digit character back into the input stream so it can
     * be processed by subsequent format specifiers.
     */
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return valor * sign;
}

// Helper function to read a single character, skipping leading whitespace
char read_char() {
    int c;

    // 1. Skip leading whitespace/newlines
    do {
        c = getchar();
    } while (isspace(c));

    /* 2. Type casting from int to char.
     * Since getchar() returns an 'int' (to handle EOF), we explicitly
     * cast it to a 'char' to match our return type and store the symbol.
     */
    return (char)c;
}

// Helper function to read a string into a provided memory buffer
void read_string(char *dest) {
    int c;

    /* 1. Skip leading whitespace.
     * Like %d and %c, we ignore any spaces or newlines before the
     * actual word starts.
     */
    do {
        c = getchar();
    } while (isspace(c));

    /* 2. Read characters until whitespace or EOF is encountered.
     * We store each character in the memory address pointed to by 'dest',
     * then increment 'dest' to move to the next byte in the array.
     */
    while (c != EOF && !isspace(c)) {
        *dest = (char)c;
        dest++;
        c = getchar();
    }

    /* 3. Null-terminate the string.
     * Crucial step: we add '\0' at the end so C knows where the string finishes.
     */
    *dest = '\0';

    /* 4. Restore the buffer.
     * We push the last character read (the space or newline that stopped the loop)
     * back to the input stream for the next scan.
     */
    if (c != EOF) {
        ungetc(c, stdin);
    }
}

int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0;
    const char *p = format;

    while (*p != '\0') {
        if (*p == '%') {
            p++; // Saltamos el % para ver qué letra sigue

            if (*p == 'd') {
                int *dest = va_arg(args, int *); // Obtenemos la dirección
                *dest = read_int();             // Llamamos a la función pequeña
                count++;
            }
            else if (*p == 'c') {
                char *dest = va_arg(args, char *);
                *dest = read_char(); // Llamamos al ayudante
                count++;
            }
            else if (*p == 's') {
                char *dest = va_arg(args, char *); // Obtenemos el puntero del array
                read_string(dest);                // Le pasamos el puntero al ayudante
                count++;
            }
            else if (*p == 'x') {
                unsigned int *dest = va_arg(args, unsigned int *);
                int c;

                // 1. Skip spaces ()
                do {
                    c = getchar();
                } while (isspace(c));

                // 2. Lógica para Hexadecimal (Base 16)
                unsigned int valor = 0;
                while (isxdigit(c)) { // isxdigit reconoce 0-9, a-f, A-F
                    int digit;
                    if (isdigit(c)) {
                        digit = c - '0';
                    } else {
                        // Convierte 'a' o 'A' en 10, 'b' en 11, etc.
                        digit = tolower(c) - 'a' + 10;
                    }
                    valor = valor * 16 + digit;
                    c = getchar();
                }

                *dest = valor;
                ungetc(c, stdin);
                count++;
            }
            else if (*p == 'f') {
                float *dest = va_arg(args, float *);
                int c;

                // 1. Saltar espacios
                do {
                    c = getchar();
                } while (isspace(c));

                // 2. Manejar el signo
                float sign = 1.0;
                if (c == '-') {
                    sign = -1.0;
                    c = getchar();
                }

                // 3. Leer la parte entera
                float value = 0.0;
                while (isdigit(c)) {
                    value = value * 10.0 + (c - '0');
                    c = getchar();
                }

                // 4. Leer la parte decimal si existe
                if (c == '.') {
                    c = getchar();
                    float divisor = 10.0;
                    while (isdigit(c)) {
                        value += (c - '0') / divisor;
                        divisor *= 10.0;
                        c = getchar();
                    }
                }

                *dest = value * sign;
                ungetc(c, stdin);
                count++;
            }
        }
        p++;
    }

    va_end(args);
    return count;
}