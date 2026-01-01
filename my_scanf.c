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

// Helper function to read a binary number (base 2)
unsigned int read_binary() {
    int c;
    unsigned int valor = 0;

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    /* 2. Process binary digits (0 and 1 only).
     * If any other character appears, the number ends there.
     */
    while (c == '0' || c == '1') {
        /* Shift current value left by 1 bit (multiply by 2)
         * and add the new digit.
         */
        valor = (valor << 1) | (c - '0');
        c = getchar();
    }

    // 3. Put back the character that stopped the loop
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return valor;
}

// Helper function to read a full line (including spaces)
void read_line(char *dest) {
    int c;

    /* 1. Skip ONLY leading newlines or tabs,
     * but we might want to keep spaces if it's a full line.
     * For now, let's follow the standard and skip leading whitespace.
     */
    do {
        c = getchar();
    } while (isspace(c));

    /* 2. Read everything until we hit a newline or EOF.
     * This is what allows "Juan Perez" to be read as one single string.
     */
    while (c != EOF && c != '\n') {
        *dest = (char)c;
        dest++;
        c = getchar();
    }

    // 3. Null-terminate
    *dest = '\0';
}

/* Converts two hex characters from stdin to an integer (0-255).
 * Returns -1 if an invalid hex character is encountered.
 */
static int hex_pair_to_int() {
    int total = 0;
    for (int i = 0; i < 2; i++) {
        int c = getchar();
        int digit = -1; // Empezamos asumiendo error

        if (isdigit(c)) digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;

        // Si el carácter no era hex, devolvemos error inmediatamente
        if (digit == -1) {
            if (c != EOF) ungetc(c, stdin); // Devolvemos el carácter erróneo
            return -1;
        }

        if (i == 0) total += digit * 16;
        else total += digit;
    }
    return total;
}

/* Reads an RGB color in #RRGGBB format.
 * Returns 1 on success, 0 on failure.
 */
static int read_color(RGBColor *out) {
    int c;

    // Skip whitespace
    do {
        c = getchar();
    } while (isspace(c));

    if (c != '#') {
        if (c != EOF) ungetc(c, stdin);
        return 0; // Error: No empieza con '#'
    }

    // Leemos a variables temporales para no romper 'out' si falla a la mitad
    int r = hex_pair_to_int();
    if (r == -1) return 0;

    int g = hex_pair_to_int();
    if (g == -1) return 0;

    int b = hex_pair_to_int();
    if (b == -1) return 0;

    // Si llegamos aquí, todo es válido. Guardamos.
    out->r = r;
    out->g = g;
    out->b = b;
    return 1;
}

/* Helper function to read a date (DD/MM/YYYY or DD-MM-YYYY).
 * Returns 1 on success, 0 on failure.
 */
static int read_date(Date *out) {
    int d = 0, m = 0, y = 0;
    int c;
    int sep1, sep2;
    int digits_read;

    // --- PASO 1: LEER EL DÍA ---

    // Saltar espacios en blanco iniciales
    do {
        c = getchar();
    } while (isspace(c));

    // Leer dígitos del día
    digits_read = 0;
    while (isdigit(c)) {
        d = d * 10 + (c - '0');
        digits_read++;
        c = getchar();
    }
    // Si no leímos ningún número (ej: empezó con una letra), fallamos.
    if (digits_read == 0) {
        if (c != EOF) ungetc(c, stdin);
        return 0;
    }

    // --- PASO 2: VERIFICAR PRIMER SEPARADOR ---

    // 'c' tiene el carácter que detuvo el número anterior. Debe ser / o -
    sep1 = c;
    if (sep1 != '/' && sep1 != '-') {
        if (sep1 != EOF) ungetc(sep1, stdin);
        return 0;
    }

    // --- PASO 3: LEER EL MES ---

    c = getchar(); // Leemos el siguiente carácter después del separador
    digits_read = 0;
    while (isdigit(c)) {
        m = m * 10 + (c - '0');
        digits_read++;
        c = getchar();
    }
    if (digits_read == 0) {
        if (c != EOF) ungetc(c, stdin);
        return 0;
    }

    // --- PASO 4: VERIFICAR SEGUNDO SEPARADOR ---

    sep2 = c;
    // Debe ser igual al primero (no vale mezclar / con -)
    if (sep2 != sep1) {
        if (sep2 != EOF) ungetc(sep2, stdin);
        return 0;
    }

    // --- PASO 5: LEER EL AÑO ---

    c = getchar();
    digits_read = 0;
    while (isdigit(c)) {
        y = y * 10 + (c - '0');
        digits_read++;
        c = getchar();
    }
    if (digits_read == 0) {
        if (c != EOF) ungetc(c, stdin);
        return 0;
    }

    // Devolvemos el carácter que no es número (espacio o enter) al buffer
    if (c != EOF) ungetc(c, stdin);

    // --- PASO 6: VALIDACIÓN LÓGICA ---
    if (d < 1 || d > 31) return 0;
    if (m < 1 || m > 12) return 0;
    if (y < 0) return 0; // Aceptamos año 0, pero no negativos

    // Todo correcto: guardamos en la estructura
    out->day = d;
    out->month = m;
    out->year = y;
    return 1;
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
            else if (*p == 'b') {
                unsigned int *dest = va_arg(args, unsigned int *);
                *dest = read_binary();
                count++;
            }
            else if (*p == 'S') { // S mayúscula para "Super String" o "Line String"
                char *dest = va_arg(args, char *);
                read_line(dest);
                count++;
            }
            else if (*p == 'D') {
                Date *date_dest = va_arg(args, Date *);
                if (read_date(date_dest)) { // Usamos la misma lógica de éxito/fracaso
                    count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
        }
        p++;
    }

    va_end(args);
    return count;
}