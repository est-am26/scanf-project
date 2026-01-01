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

// Helper function to read a hexadecimal integer (base 16)
// Unlike %d (decimal), the C standard typically treats hexadecimal numbers as unsigned (pure positives
// representing memory addresses or binary values). That's why we use `unsigned int` both in the function return and
// the dest pointer.
unsigned int read_hex() {
    int c;
    unsigned int valor = 0;

    /* 1. Skip leading whitespace.
     * Standard scanf behavior: ignore any spaces, tabs or newlines
     * before the actual hexadecimal digits.
     */
    do {
        c = getchar();
    } while (isspace(c));

    /* 2. HEXADECIMAL CONVERSION LOGIC:
     * To convert hex letters (a-f) to numbers (10-15), we calculate the
     * "distance" from 'a' using ASCII math (e.g., 'f' (102) - 'a' (97) = 5).
     * Then, we add 10 because 'a' represents the value 10 in base 16.
     * Example: 'f' is 5 positions away from 'a'; therefore, 5 + 10 = 15.
     */
    while (isxdigit(c)) {
        int digit;
        if (isdigit(c)) {
            // Convert '0'-'9' to 0-9
            digit = c - '0';
        } else {
            /* Convert 'a'/'A' to 10, 'b'/'B' to 11, and so on.
             * tolower() ensures we handle both uppercase and lowercase.
             */
            digit = tolower(c) - 'a' + 10;
        }

        /* Shift current value by 4 bits (base 16) and add the new digit.
         * valor * 16 is the same as moving to the next hex position.
         */
        valor = valor * 16 + digit;
        c = getchar();
    }

    /* 3. Return the extra character to the buffer.
     * We stop at the first non-hex character, so we push it back
     * for the next format specifier to process it.
     */
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return valor;
}

// Helper function to read a floating-point number
float read_float() {
    int c;
    float value = 0.0;
    float sign = 1.0;

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Handle optional sign
    if (c == '-') {
        sign = -1.0;
        c = getchar();
    } else if (c == '+') {
        c = getchar();
    }

    /* 3. Process the integer part.
     * We multiply by 10.0 to shift decimals to the left.
     */
    while (isdigit(c)) {
        value = value * 10.0 + (c - '0');
        c = getchar();
    }

    /* 4. Process the fractional part.
     * If a dot is found, we start dividing each new digit by a divisor
     * that grows by powers of 10 (10, 100, 1000...) to place them correctly.
     */
    /* 4. Process the fractional part (The "Magic Loop").
     * Example: User inputs "3.14"
     * * Initial: value = 3.0, divisor = 10.0
     * * Iteration 1 (Reading '1'):
     * - Convert: ('1' - '0') becomes the integer 1.
     * - Position: 1 / 10.0 = 0.1.
     * - Add: value becomes 3.1.
     * - Next Level: divisor becomes 100.0 (hundredths).
     * * Iteration 2 (Reading '4'):
     * - Convert: ('4' - '0') becomes the integer 4.
     * - Position: 4 / 100.0 = 0.04.
     * - Add: value becomes 3.14.
     * - Next Level: divisor becomes 1000.0.
     * * Termination: If c is a space, newline, or a letter (like 'a'),
     * isdigit(c) returns false, and the loop stops. The non-digit
     * character is preserved using ungetc().
     */
    if (c == '.') {
        c = getchar();
        float divisor = 10.0;
        while (isdigit(c)) {
            value += (c - '0') / divisor;
            divisor *= 10.0;
            c = getchar();
        }
    }

    // 5. Restore the non-digit character to the buffer
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return value * sign;
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
                *dest = read_hex(); // Llamamos al "Cajero" para que nos de el número
                count++;
            }
            else if (*p == 'f') {
                float *dest = va_arg(args, float *);
                *dest = read_float(); // El ayudante nos da el decimal terminado
                count++;
            }
        }
        p++;
    }

    va_end(args);
    return count;
}