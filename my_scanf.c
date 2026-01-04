#include "my_scanf.h"

/* Helper function to read a signed integer with modifiers.
 * * out: Pointer to store the result (long long to cover all sizes).
 * If NULL, we assume assignment suppression (%*d) -> read but don't store.
 * width: Maximum characters to read. Pass -1 (or INT_MAX) for no limit.
 *
 * Returns: 1 on success, 0 on failure.
 */
int read_int(long long *out, int width) {
    int c;
    long long sign = 1;
    long long value = 0;
    int chars_processed = 0;
    int digits_read = 0;
    int has_width = (width > 0); // Bandera para saber si hay límite

    // 1. Skip leading whitespace
    // IMPORTANTE: Los espacios NO cuentan para el 'width'.
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF after skipping spaces
    if (c == EOF) return 0;

    // 3. Handle optional sign
    // El signo SÍ cuenta para el ancho (ej: %3d de "-123" lee "-12")
    if (c == '-' || c == '+') {
        // Chequeo de seguridad: Si width es 1, solo cabe el signo -> Error o parada
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
            return 0; // No hay espacio para dígitos
        }

        if (c == '-') sign = -1;
        chars_processed++;
        c = getchar();
    }

    // 4. Leer dígitos respetando el ancho
    while (isdigit(c)) {
        // Verificamos límite de ancho antes de procesar
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Devolvemos el sobrante (el dígito que ya no cabe)
            break;
        }

        // Si 'out' es NULL (supresión), calculamos pero no guardamos (o solo ignoramos)
        // Calculamos igual para validar desbordamientos si quisieras,
        // pero lo mínimo es avanzar.
        value = value * 10 + (c - '0');

        digits_read++;
        chars_processed++;
        c = getchar();
    }

    // 5. Restore non-digit character
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }
    // Nota: Si salimos por width limit, el ungetc ya se hizo arriba o el char actual
    // es el que rompió el loop. Hay que tener cuidado aquí.
    // Simplificación: Siempre hacemos ungetc del último char leído si no lo usamos.
    // (La lógica del break arriba hace ungetc, así que estamos cubiertos).

    // 6. Validation
    if (digits_read == 0) return 0;

    // 7. Store result ONLY if not suppressed (out != NULL)
    if (out != NULL) {
        *out = value * sign;
    }

    return 1; // Success
}

/* Helper function to read characters.
 * width: Number of characters to read.
 * If width is -1 (or 0), defaults to 1.
 * out: Pointer to array. If NULL, acts as suppression (%*c).
 * NOTE: Does NOT append null terminator '\0'.
 */
int read_char(char *out, int width) {
    // Si el usuario pone "%c", width viene como -1. Lo cambiamos a 1.
    if (width <= 0) {
        width = 1;
    }

    int i;
    for (i = 0; i < width; i++) {
        int c = getchar();

        // Si encontramos EOF antes de terminar de leer todo el bloque, fallamos.
        // (scanf estándar requiere leer TODO el ancho pedido o falla).
        if (c == EOF) {
            return 0;
        }

        // Si no es supresión, guardamos y avanzamos el puntero
        if (out != NULL) {
            out[i] = (char)c;
            // Nota: No usamos *out++ aquí porque estamos indexando out[i]
            // que es más claro para arrays.
        }
    }

    return 1; // Éxito (se leyeron 'width' caracteres)
}

/* Helper function to read a string with modifiers.
 * width: Max characters to read. -1 means "no limit" (unsafe!).
 * dest: Pointer to buffer. If NULL, logic acts as suppression (%*s).
 * Returns: 1 on success, 0 on failure (EOF at start).
 */
int read_string(char *dest, int width) {
    int c;
    int chars_read = 0;

    // Si width no está definido (-1), permitimos leer "infinito" (hasta que se acabe la RAM o el string)
    // Para evitar loops infinitos reales, usamos INT_MAX.
    if (width == -1) {
        width = 2147483647; // INT_MAX
    }

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF
    if (c == EOF) return 0;

    // 3. Read loop
    // Condición: No es EOF, No es espacio, Y no hemos llegado al límite de width
    while (c != EOF && !isspace(c) && chars_read < width) {
        // Solo escribimos si NO es supresión (dest != NULL)
        if (dest != NULL) {
            *dest = (char)c;
            dest++;
        }
        chars_read++;
        c = getchar();
    }

    // 4. Null-terminate
    // SIEMPRE ponemos el \0 al final, incluso si cortamos por width.
    if (dest != NULL) {
        *dest = '\0';
    }

    // 5. Restore delimiter
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return 1;
}

/* Helper function to read a hexadecimal integer with modifiers.
 * out: Pointer to store the result (unsigned long long).
 * If NULL, acts as suppression (%*x).
 * width: Max characters to read. -1 means no limit.
 * Returns: 1 on success, 0 on failure.
 */
int read_hex(unsigned long long *out, int width) {
    int c;
    unsigned long long value = 0;
    int digits_read = 0;
    int chars_processed = 0;
    int has_width = (width > 0);

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF
    if (c == EOF) return 0;

    // 3. Read Hex Digits respecting width
    while (isxdigit(c)) {
        // Verificar límite de ancho
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Devolvemos el sobrante
            break;
        }

        int digit;
        if (isdigit(c)) {
            digit = c - '0';
        } else {
            digit = tolower(c) - 'a' + 10;
        }

        value = value * 16 + digit;
        digits_read++;
        chars_processed++;
        c = getchar();
    }

    // 4. Restore non-hex character
    // Solo si no salimos por el break del width
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // 5. Validation
    if (digits_read == 0) return 0;

    // 6. Store result (Only if not suppressed)
    if (out != NULL) {
        *out = value;
    }

    return 1;
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
        // ---------------------------------------------------------
        // A. SI ENCONTRAMOS UN %, EMPIEZA LA FIESTA DE LOS MODIFICADORES
        // ---------------------------------------------------------
        if (*p == '%') {
            p++; // Saltamos el '%'

            // 1. CHECK SUPPRESSION (*)
            // Si hay un asterisco, leemos el dato pero no lo guardamos.
            int suppress = 0;
            if (*p == '*') {
                suppress = 1;
                p++;
            }

            // 2. CHECK WIDTH (Ancho Máximo)
            int width = -1; // -1 indica "sin límite especificado"
            if (isdigit(*p)) {
                width = 0;
                while (isdigit(*p)) {
                    // Construimos el número (ej: "12" -> 1*10 + 2 = 12)
                    width = width * 10 + (*p - '0');
                    p++;
                }
            }
            // Nota: Si el usuario pone %0d, asumimos width=0.

            // 3. CHECK LENGTH MODIFIERS (Tamaño de la variable)
            // Mapa de valores:
            // 0 = int (default)
            // 1 = short (h)
            // 2 = signed char (hh)
            // 3 = long (l)
            // 4 = long long (ll)
            int length_mod = 0;

            if (*p == 'j') {
                length_mod = 5;
                p++;
            }
            else if (*p == 'z') {
                length_mod = 6;
                p++;
            }
            else if (*p == 't') {
                length_mod = 7;
                p++;
            }
            if (*p == 'h') {
                p++;
                if (*p == 'h') {
                    length_mod = 2; // hh
                    p++;
                } else {
                    length_mod = 1; // h
                }
            }
            else if (*p == 'l') {
                p++;
                if (*p == 'l') {
                    length_mod = 4; // ll
                    p++;
                } else {
                    length_mod = 3; // l
                }
            }

            // ---------------------------------------------------------
            // 4. CHECK SPECIFIER: Entero con Signo ('d')
            // ---------------------------------------------------------
            if (*p == 'd') {
                long long buffer_val; // Caja grande temporal
                long long *ptr_to_pass = &buffer_val;

                // Si hay supresión (*), pasamos NULL a la función
                if (suppress) {
                    ptr_to_pass = NULL;
                }

                // Llamamos a read_int pasándole el puntero y el ancho
                // Si devuelve 0, es que hubo un fallo de lectura o EOF
                if (!read_int(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                // Si NO hubo supresión, guardamos el dato en la variable real
                if (!suppress) {
                    if (length_mod == 4) { // ll (long long)
                        long long *dest = va_arg(args, long long *);
                        *dest = buffer_val;
                    }
                    else if (length_mod == 3) { // l (long)
                        long *dest = va_arg(args, long *);
                        *dest = (long)buffer_val;
                    }
                    else if (length_mod == 1) { // h (short)
                        short *dest = va_arg(args, short *);
                        *dest = (short)buffer_val;
                    }
                    else if (length_mod == 2) { // hh (char como número)
                        signed char *dest = va_arg(args, signed char *);
                        *dest = (signed char)buffer_val;
                    }
                    else { // 0: int normal (default)
                        int *dest = va_arg(args, int *);
                        *dest = (int)buffer_val;
                    }
                    count++;
                }
            }
            else if (*p == 'c') {
                char *dest = NULL;

                // Solo sacamos el argumento si NO hay supresión
                if (!suppress) {
                    dest = va_arg(args, char *);
                }

                // Llamamos a la función.
                // Nota: width ya lo calculaste arriba en el bloque genérico.
                // Si el usuario puso "%c", width vale -1 y read_char lo convertirá a 1.
                // Si el usuario puso "%5c", width vale 5.
                if (read_char(dest, width)) {
                    if (!suppress) {
                        count++;
                    }
                } else {
                    va_end(args);
                    return count; // EOF encontrado antes de completar la lectura
                }
            }
            else if (*p == 's') {
                char *dest = NULL;

                // Si NO hay supresión (*), sacamos la dirección del argumento
                if (!suppress) {
                    dest = va_arg(args, char *);
                }

                // Llamamos a la función pasando el 'width' parseado anteriormente
                // (Si el usuario puso %10s, width es 10. Si puso %s, es -1).
                if (read_string(dest, width)) {
                    if (!suppress) {
                        count++;
                    }
                } else {
                    va_end(args);
                    return count; // EOF encontrado
                }
            }
            else if (*p == 'x') {
                unsigned long long buffer_val; // Caja grande temporal
                unsigned long long *ptr_to_pass = &buffer_val;

                // Si hay supresión (*), pasamos NULL
                if (suppress) {
                    ptr_to_pass = NULL;
                }

                // Llamamos a read_hex con el ancho parseado
                if (!read_hex(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                // Si no hubo supresión, guardamos el dato en la variable real
                if (!suppress) {
                    if (length_mod == 4) { // ll (unsigned long long)
                        unsigned long long *dest = va_arg(args, unsigned long long *);
                        *dest = buffer_val;
                    }
                    else if (length_mod == 3) { // l (unsigned long)
                        unsigned long *dest = va_arg(args, unsigned long *);
                        *dest = (unsigned long)buffer_val;
                    }
                    else if (length_mod == 1) { // h (unsigned short)
                        unsigned short *dest = va_arg(args, unsigned short *);
                        *dest = (unsigned short)buffer_val;
                    }
                    else if (length_mod == 2) { // hh (unsigned char)
                        unsigned char *dest = va_arg(args, unsigned char *);
                        *dest = (unsigned char)buffer_val;
                    }
                    else { // 0: unsigned int normal (default)
                        unsigned int *dest = va_arg(args, unsigned int *);
                        *dest = (unsigned int)buffer_val;
                    }
                    count++;
                }
            }
            else if (*p == 'f') {
                double buffer_val; // Usamos double (caja grande) para calcular
                double *ptr_to_pass = &buffer_val;

                if (suppress) {
                    ptr_to_pass = NULL;
                }

                // Llamamos a la función
                if (!read_float(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                if (!suppress) {
                    // REPARTO SEGÚN TAMAÑO (Length Modifier)
                    // length_mod == 3 significa 'l' (long), o sea %lf -> double
                    if (length_mod == 3) {
                        double *dest = va_arg(args, double *);
                        *dest = buffer_val; // Ya es double, entra directo
                    }
                    // length_mod == 4 significa 'L' (long double) - Opcional
                    else if (length_mod == 4) {
                        long double *dest = va_arg(args, long double *);
                        *dest = (long double)buffer_val;
                    }
                    // Por defecto (0), es %f -> float
                    else {
                        float *dest = va_arg(args, float *);
                        *dest = (float)buffer_val; // Convertimos double a float (casting)
                    }
                    count++;
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
                if (read_date(date_dest)) {
                    count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
        } // <--- ESTA ES LA LLAVE CLAVE QUE CIERRA EL if (*p == '%')

        else {
            // ---------------------------------------------------------
            // B. MATCHING LITERAL (Si *p NO era un '%')
            // ---------------------------------------------------------

            // CASO 1: El formato tiene un espacio en blanco (ej: " %d")
            if (isspace(*p)) {
                int c;
                while (isspace(c = getchar()));
                if (c != EOF) {
                    ungetc(c, stdin);
                }
            }
            // CASO 2: Coincidencia exacta de texto (ej: "Edad:")
            else {
                int c = getchar();
                if (c != *p) {
                    if (c != EOF) ungetc(c, stdin);
                    va_end(args);
                    return count;
                }
            }
        }
        p++; // Avanzamos al siguiente carácter del string de formato
    }

    va_end(args);
    return count;
}