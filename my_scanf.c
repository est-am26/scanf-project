#include "my_scanf.h"

/* Helper function to read a signed integer.
 * Behavior mimics scanf: skips leading whitespace, handles +/- signs.
 * Returns 1 on success (digits read), 0 on failure.
 */
int read_int(int *out) {
    int c;
    int sign = 1;
    long value = 0; // Usamos long para evitar desbordamiento durante la lectura
    int digits_read = 0;

    // 1. Saltar espacios en blanco (Leading whitespace)
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Manejo del signo opcional (+ o -)
    if (c == '-' || c == '+') {
        if (c == '-') {
            sign = -1;
        }
        // Leemos el siguiente carácter después del signo
        c = getchar();
    }

    // 3. Leer dígitos
    while (isdigit(c)) {
        value = value * 10 + (c - '0');
        digits_read++;
        c = getchar();
    }

    // 4. Devolver al buffer el carácter que nos detuvo (ej: espacio o letra)
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // 5. Verificación de Éxito
    // Si leímos un signo pero NO leímos dígitos (ej: "-a"), es un fallo.
    if (digits_read == 0) {
        return 0; // Matching failure
    }

    // 6. Escribir resultado
    *out = (int)(value * sign);
    return 1; // Success
}

/* Helper function to read a single character.
 * STANDARD BEHAVIOR: Does NOT skip leading whitespace.
 * Returns 1 on success, 0 on failure (EOF).
 */
int read_char(char *out) {
    int c = getchar();

    // 1. Verificar EOF inmediatamente
    if (c == EOF) {
        return 0; // Failure
    }

    // 2. Guardar el carácter (sea espacio, letra o salto de línea)
    *out = (char)c;
    return 1; // Success
}

/* Helper function to read a string.
 * Mimics scanf %s: Skips leading whitespace, reads until next whitespace.
 * Returns 1 on success, 0 on failure (EOF before reading any chars).
 * WARNING: Without a width modifier, this can cause buffer overflow (just like real scanf).
 */
int read_string(char *dest) { // <--- Ya no es void, ahora devuelve int (éxito/fallo)
    int c;

    // 1. Skip leading whitespace
    // (Correcto: %s SÍ salta espacios, a diferencia de %c)
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check for EOF immediately after skipping whitespace
    // Si llegamos al final del archivo y no encontramos palabra, fallamos.
    if (c == EOF) {
        return 0;
    }

    // 3. Read characters until whitespace or EOF
    while (c != EOF && !isspace(c)) {
        *dest = (char)c;
        dest++;
        c = getchar();
    }

    // 4. Null-terminate (Crucial para que sea un string válido en C)
    *dest = '\0';

    // 5. Restore the delimiter to the buffer
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return 1; // Success
}

/* Helper function to read a hexadecimal integer.
 * Mimics scanf %x: Skips whitespace, reads 0-9, a-f, A-F.
 * Returns 1 on success (at least one digit read), 0 on failure.
 */
int read_hex(unsigned int *out) {
    int c;
    unsigned long value = 0; // Usamos long para seguridad durante el cálculo
    int digits_read = 0;

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Read Hex Digits
    // isxdigit() verifica si es 0-9, a-f o A-F.
    while (isxdigit(c)) {
        int digit;

        if (isdigit(c)) {
            digit = c - '0';
        } else {
            // Tu lógica original: Convertimos letra a 10-15
            digit = tolower(c) - 'a' + 10;
        }

        value = value * 16 + digit;
        digits_read++;
        c = getchar();
    }

    // 3. Restore non-hex character
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // 4. Validation: Did we read anything?
    // Si encontramos "gato", saltamos espacios, leemos 'g' (no es hex),
    // digits_read es 0 -> Devolvemos fallo.
    if (digits_read == 0) {
        return 0;
    }

    // 5. Store result
    *out = (unsigned int)value;
    return 1; // Success
}

/* Helper function to read a floating-point number.
 * Mimics scanf %f: Handles signs, integer part, and fractional part.
 * Returns 1 on success (read a valid number), 0 on failure.
 */
int read_float(float *out) {
    int c;
    float value = 0.0f;
    float sign = 1.0f;
    int has_digits = 0; // Bandera para saber si leímos al menos un número

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Handle optional sign
    if (c == '+' || c == '-') {
        if (c == '-') {
            sign = -1.0f;
        }
        c = getchar();
    }

    // 3. Process the integer part (Left of dot)
    while (isdigit(c)) {
        has_digits = 1; // ¡Encontramos un dígito!
        value = value * 10.0f + (c - '0');
        c = getchar();
    }

    // 4. Process the fractional part (Right of dot)
    if (c == '.') {
        c = getchar(); // Consumimos el punto
        float divisor = 10.0f;

        while (isdigit(c)) {
            has_digits = 1; // ¡Encontramos un dígito!
            value += (c - '0') / divisor;
            divisor *= 10.0f;
            c = getchar();
        }
    }

    // 5. Restore the non-digit character
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // 6. Validation Check
    // Si solo leímos un signo '+' o un punto '.' sin números, fallamos.
    if (!has_digits) {
        return 0; // Failure
    }

    // 7. Store result
    *out = value * sign;
    return 1; // Success
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
int read_color(RGBColor *out) {
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
                int *dest = va_arg(args, int *); // declarando una variable dest que guarda una direccion que viene. Vas a guardar la dirección de memoria de la variable del usuario DENTRO de tu variable local puntero.
                if (read_int(dest)) { // Ahora usamos el if // pasa la direccion donde guardarlo
                    count++;
                } else {
                    va_end(args);
                    return count; // Se detiene si falla (ej: era una letra)
                }
            }
            else if (*p == 'c') {
                char *dest = va_arg(args, char *); // 1. Pedimos la dirección

                // 2. Llamamos a la función pasando la dirección
                //    y verificamos si devolvió 1 (éxito) o 0 (EOF)
                if (read_char(dest)) {
                    count++;
                } else {
                    va_end(args); // Limpieza antes de salir
                    return count; // EOF encontrado
                }
            }
            else if (*p == 's') {
                char *dest = va_arg(args, char *);
                // Ahora verificamos si realmente se leyó algo
                if (read_string(dest)) {
                    count++;
                } else {
                    va_end(args);
                    return count; // EOF encontrado antes de leer la palabra
                }
            }
            else if (*p == 'x') {
                unsigned int *dest = va_arg(args, unsigned int *);
                // Verificamos si read_hex tuvo éxito
                if (read_hex(dest)) {
                    count++;
                } else {
                    va_end(args);
                    return count; // Fallo (ej: no era un número hex)
                }
            }
            else if (*p == 'f') {
                float *dest = va_arg(args, float *); // Pedimos la dirección

                if (read_float(dest)) { // Verificamos si se leyó un número válido
                    count++;
                } else {
                    va_end(args);
                    return count; // Fallo (ej: usuario escribió "hola" o solo ".")
                }
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