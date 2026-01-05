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

/* Helper function to read a floating-point number with modifiers.
 * out: Pointer to store result (double). If NULL, acts as suppression (%*f).
 * width: Max chars to read. -1 means no limit.
 * Returns: 1 on success, 0 on failure.
 */
int read_float(double *out, int width) {
    int c;
    double value = 0.0;
    double sign = 1.0;
    int has_digits = 0;
    int chars_processed = 0;
    int has_width = (width > 0);

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF
    if (c == EOF) return 0;

    // 3. Handle optional sign
    if (c == '+' || c == '-') {
        // Chequeo de ancho: Si width=1, solo cabe el signo -> Fallo técnico o parada
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
            return 0;
        }
        if (c == '-') sign = -1.0;
        chars_processed++;
        c = getchar();
    }

    // 4. Process Integer Part
    while (isdigit(c)) {
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Devolvemos el char que sobra
            break; // Salimos del bucle
        }
        has_digits = 1;
        value = value * 10.0 + (c - '0');
        chars_processed++;
        c = getchar();
    }

    // 5. Process Fractional Part
    // Solo entramos si no hemos llegado al límite y encontramos un punto
    if (c == '.' && (!has_width || chars_processed < width)) {
        chars_processed++; // Contamos el punto
        c = getchar();
        double divisor = 10.0;

        while (isdigit(c)) {
            if (has_width && chars_processed >= width) {
                ungetc(c, stdin);
                break;
            }
            has_digits = 1;
            value += (c - '0') / divisor;
            divisor *= 10.0;
            chars_processed++;
            c = getchar();
        }
    }

    // 6. Restore non-digit character
    // IMPORTANTE: Solo hacemos ungetc si NO salimos por culpa del break (width limit)
    // Pero para simplificar: verificamos si 'c' sigue siendo el carácter actual leído
    // Si el bucle while rompió, 'c' es el carácter que sobró.
    if (c != EOF && (!has_width || chars_processed < width)) {
         ungetc(c, stdin);
    }
    // Nota: Si el bucle rompió por width, ya hicimos ungetc dentro.

    // 7. Validation
    if (!has_digits) return 0;

    // 8. Store result
    if (out != NULL) {
        *out = value * sign;
    }

    return 1;
}
/* Helper function to read a binary number with modifiers.
 * out: Pointer to store result (unsigned long long).
 * If NULL, acts as suppression (%*b).
 * width: Max chars to read. -1 means no limit.
 * Returns: 1 on success, 0 on failure.
 */
int read_binary(unsigned long long *out, int width) {
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

    // 3. Process binary digits (0 and 1 only)
    while (c == '0' || c == '1') {
        // Verificar ancho
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Devolvemos el sobrante
            break;
        }

        // Bitwise Shift: Desplazar a la izquierda es multiplicar por 2
        // Ejemplo: tenemos 1 (binary 1). Llega 0.
        // 1 << 1 = 10 (binary 2). 10 | 0 = 10.
        value = (value << 1) | (c - '0');

        digits_read++;
        chars_processed++;
        c = getchar();
    }

    // 4. Restore non-binary character
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // 5. Validation
    if (digits_read == 0) return 0;

    // 6. Store result
    if (out != NULL) {
        *out = value;
    }

    return 1;
}

/* Helper function to read a full line until newline.
 * Mimics %[^\n]: Reads spaces, tabs, words... stops ONLY at '\n'.
 * width: Max chars to read.
 * out: Buffer. If NULL, acts as suppression.
 */
int read_line(char *out, int width) {
    int c;
    int chars_read = 0;

    // Si width es -1, ponemos límite de seguridad (INT_MAX)
    if (width == -1) {
        width = 2147483647;
    }

    // 1. Skip leading whitespace?
    // DECISIÓN DE DISEÑO: scanf("%[^\n]") estándar NO salta espacios iniciales.
    // Pero tu código original SÍ lo hacía. Mantendré TU lógica original
    // porque es más cómoda para el usuario (salta el Enter del input anterior).
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF
    if (c == EOF) return 0;

    // 3. Read until Newline ('\n') or Width limit
    while (c != EOF && c != '\n' && chars_read < width) {
        if (out != NULL) {
            *out = (char)c;
            out++;
        }
        chars_read++;
        c = getchar();
    }

    // 4. Null-terminate
    if (out != NULL) {
        *out = '\0';
    }

    // 5. Handling the Newline logic
    // Aquí hay dos escuelas de pensamiento:
    // A) Scanf estándar: Deja el '\n' en el buffer para la siguiente lectura.
    // B) Gets/User friendly: Consume el '\n' para limpiar el buffer.
    // Tu código original consumía el '\n' (porque no hacías ungetc si era \n).
    // Para ser consistente con scanf, DEBERÍAMOS devolverlo.
    if (c == '\n') {
        ungetc(c, stdin);
    } else if (c != EOF) {
        // Si paramos por width limit (y no es \n), también lo devolvemos.
        ungetc(c, stdin);
    }

    return 1;
}

/* --------------------------------------------------------------------------
 * INTERNAL HELPER (Static)
 * Esta función es privada. Solo read_color la puede ver.
 * Reads exactly 2 hex digits and converts to int (0-255).
 * -------------------------------------------------------------------------- */
static int read_hex_pair() {
    int total = 0;

    // Leemos el primer dígito (High nibble)
    int c1 = getchar();
    int d1 = -1;

    // Lógica de conversión
    if (isdigit(c1)) d1 = c1 - '0';
    else if (c1 >= 'a' && c1 <= 'f') d1 = c1 - 'a' + 10;
    else if (c1 >= 'A' && c1 <= 'F') d1 = c1 - 'A' + 10;

    if (d1 == -1) {
        if (c1 != EOF) ungetc(c1, stdin);
        return -1;
    }

    // Leemos el segundo dígito (Low nibble)
    int c2 = getchar();
    int d2 = -1;

    if (isdigit(c2)) d2 = c2 - '0';
    else if (c2 >= 'a' && c2 <= 'f') d2 = c2 - 'a' + 10;
    else if (c2 >= 'A' && c2 <= 'F') d2 = c2 - 'A' + 10;

    if (d2 == -1) {
        if (c2 != EOF) ungetc(c2, stdin);
        return -1;
    }

    return (d1 * 16) + d2;
}

/* --------------------------------------------------------------------------
 * PUBLIC HELPER (Non-static)
 * Esta función es pública para que my_scanf la use.
 * Reads an RGB color in #RRGGBB format.
 * -------------------------------------------------------------------------- */
int read_color(RGBColor *out, int width) {
    int c;
    int chars_processed = 0;
    int has_width = (width > 0);

    // 1. Skip whitespace
    do {
        c = getchar();
    } while (isspace(c));

    if (c == EOF) return 0;

    // 2. Check Width at start
    if (has_width && chars_processed >= width) {
        ungetc(c, stdin);
        return 0;
    }

    // 3. Match '#'
    if (c != '#') {
        ungetc(c, stdin);
        return 0;
    }
    chars_processed++;

    // 4. Read Pairs using the STATIC helper
    int components[3]; // R, G, B

    for (int i = 0; i < 3; i++) {
        // Chequeo de seguridad: Necesitamos espacio para 2 caracteres más
        if (has_width && (chars_processed + 2 > width)) {
            return 0;
        }

        int val = read_hex_pair();
        if (val == -1) {
            return 0;
        }
        components[i] = val;
        chars_processed += 2;
    }

    // 5. Store result
    if (out != NULL) {
        out->r = components[0];
        out->g = components[1];
        out->b = components[2];
    }

    return 1;
}

/* Helper function to read a date (DD/MM/YYYY or DD-MM-YYYY).
 * out: Pointer to Date struct. If NULL, logic acts as suppression.
 * width: Max total characters to read. -1 means no limit.
 * Returns: 1 on success (valid format), 0 on failure.
 */
int read_date(Date *out, int width) {
    int d = 0, m = 0, y = 0;
    int c;
    int sep1 = 0; // Guardamos el primer separador para ver si coincide con el segundo
    int chars_processed = 0;
    int has_width = (width > 0);

    // 1. Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    // 2. Check EOF
    if (c == EOF) return 0;

    // --- PASO A: LEER EL DÍA ---
    int digits_d = 0;
    while (isdigit(c)) {
        if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

        d = d * 10 + (c - '0');
        digits_d++;
        chars_processed++;
        c = getchar();
    }
    if (digits_d == 0) { // No leímos número
        if (c != EOF) ungetc(c, stdin);
        return 0;
    }

    // --- PASO B: LEER PRIMER SEPARADOR (/ o -) ---
    if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

    if (c == '/' || c == '-') {
        sep1 = c;
        chars_processed++;
        c = getchar();
    } else {
        // Falta el separador obligatorio
        ungetc(c, stdin);
        return 0;
    }

    // --- PASO C: LEER EL MES ---
    int digits_m = 0;
    while (isdigit(c)) {
        if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

        m = m * 10 + (c - '0');
        digits_m++;
        chars_processed++;
        c = getchar();
    }
    if (digits_m == 0) {
        ungetc(c, stdin); return 0;
    }

    // --- PASO D: LEER SEGUNDO SEPARADOR ---
    if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

    // Validación estricta: El segundo separador debe ser igual al primero (ej: 12-12-2022)
    if (c != sep1) {
        ungetc(c, stdin);
        return 0; // Formato mixto (12/12-2022) o falta separador
    }
    chars_processed++;
    c = getchar(); // Consumimos el separador

    // --- PASO E: LEER EL AÑO ---
    int digits_y = 0;
    while (isdigit(c)) {
        if (has_width && chars_processed >= width) { ungetc(c, stdin); break; }

        y = y * 10 + (c - '0');
        digits_y++;
        chars_processed++;
        c = getchar();
    }
    if (digits_y == 0) {
        ungetc(c, stdin); return 0;
    }

    // Restore trailing character
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // --- PASO F: VALIDACIÓN LÓGICA BÁSICA ---
    // (Opcional, pero recomendado para una fecha real)
    if (m < 1 || m > 12 || d < 1 || d > 31) {
        return 0; // Fecha imposible
    }

    // --- PASO G: GUARDAR RESULTADO ---
    if (out != NULL) {
        out->day = d;
        out->month = m;
        out->year = y;
    }

    return 1; // Success
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
                unsigned long long buffer_val;
                unsigned long long *ptr_to_pass = &buffer_val;

                // Supresión
                if (suppress) {
                    ptr_to_pass = NULL;
                }

                // Llamada a la función
                if (!read_binary(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                // Reparto de tipos (Casting)
                if (!suppress) {
                    if (length_mod == 4) { // ll (long long)
                        unsigned long long *dest = va_arg(args, unsigned long long *);
                        *dest = buffer_val;
                    }
                    else if (length_mod == 3) { // l (long)
                        unsigned long *dest = va_arg(args, unsigned long *);
                        *dest = (unsigned long)buffer_val;
                    }
                    else if (length_mod == 1) { // h (short)
                        unsigned short *dest = va_arg(args, unsigned short *);
                        *dest = (unsigned short)buffer_val;
                    }
                    else if (length_mod == 2) { // hh (char / byte)
                        unsigned char *dest = va_arg(args, unsigned char *);
                        *dest = (unsigned char)buffer_val;
                    }
                    else { // 0: int normal
                        unsigned int *dest = va_arg(args, unsigned int *);
                        *dest = (unsigned int)buffer_val;
                    }
                    count++;
                }
            }
            else if (*p == 'L') { // O la letra que uses para "Leer Línea"
                char *dest = NULL;

                if (!suppress) {
                    dest = va_arg(args, char *);
                }

                if (read_line(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
            else if (*p == 'D') {
                // Puntero a Date
                Date *date_dest = NULL;

                // Si NO es supresión, sacamos el argumento
                if (!suppress) {
                    date_dest = va_arg(args, Date *);
                }

                // Llamamos a read_date pasando el width
                // Si suppress es true, date_dest es NULL, y read_date solo validará sin guardar.
                if (read_date(date_dest, width)) {
                    if (!suppress) {
                        count++;
                    }
                } else {
                    va_end(args);
                    return count; // Formato de fecha inválido
                }
            }
            else if (*p == 'R') {
                RGBColor *col_dest = NULL;

                // Si NO es supresión, sacamos el puntero
                if (!suppress) {
                    col_dest = va_arg(args, RGBColor *);
                }

                // Llamamos a read_color con el width
                if (read_color(col_dest, width)) {
                    if (!suppress) {
                        count++;
                    }
                } else {
                    va_end(args);
                    return count; // Formato de color inválido
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