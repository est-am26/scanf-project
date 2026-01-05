#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include "my_scanf.h"

/* =========================================================================
 * HELPER FUNCTIONS
 * ========================================================================= */

/**
 * Reads a signed integer with support for field width.
 * @param out: Pointer to store the result (long long to support all int sizes).
 * @param width: Max characters to read. -1 indicates no limit.
 * @return 1 on success, 0 on failure.
 */
int read_int(long long *out, int width) {
    int c;
    long long sign = 1;
    long long value = 0;
    int chars_processed = 0;
    int digits_read = 0;
    int has_width = (width > 0);

    // Skip leading whitespace (standard scanf behavior)
    do {
        c = getchar();
    } while (isspace(c));

    if (c == EOF) return 0;

    // Handle Sign
    if (c == '-' || c == '+') {
        // Critical: The sign counts towards the width.
        // If width is 1, we cannot read a signed number (e.g., "-5").
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
            return 0;
        }

        if (c == '-') sign = -1;
        chars_processed++;
        c = getchar();
    }

    // Process Digits
    while (isdigit(c)) {
        // Stop if we reached the maximum field width
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Push back the extra character
            break;
        }

        value = value * 10 + (c - '0');

        digits_read++;
        chars_processed++;
        c = getchar();
    }

    // Restore the stopper character IF it wasn't handled by the width check.
    // (i.e., we stopped because we found a non-digit letter)
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    if (digits_read == 0) return 0;

    // Store result if not suppressed (%*d)
    if (out != NULL) {
        *out = value * sign;
    }

    return 1;
}

/**
 * Reads a specific sequence of characters.
 * UNIQUE BEHAVIOR:
 * 1. Does NOT skip leading whitespace (reads spaces, tabs, newlines).
 * 2. Does NOT append a null terminator ('\0').
 *
 * @param out: Pointer to the buffer. If NULL, acts as suppression (%*c).
 * @param width: Exact number of characters to read.
 * @return 1 on success (all requested characters read), 0 on failure.
 */
int read_char(char *out, int width) {
    // Default to 1 if no width specified (e.g., "%c")
    if (width <= 0) {
        width = 1;
    }

    int i;
    for (i = 0; i < width; i++) {
        int c = getchar();

        // Strict EOF check: standard scanf fails if it can't read the full width
        if (c == EOF) {
            return 0;
        }

        // Store Character (if not suppressed)
        if (out != NULL) {
            out[i] = (char)c;
        }
    }

    return 1;
}

/**
 * Reads a string (sequence of non-whitespace characters).
 * CRITICAL BEHAVIOR:
 * 1. Skips leading whitespace.
 * 2. Stops at the first whitespace character.
 * 3. ALWAYS appends a null terminator ('\0').
 *
 * @param out: Buffer to store the string.
 * @param width: Max characters to read. -1 means unlimited (UNSAFE).
 * @return 1 on success, 0 on failure.
 */
int read_string(char *out, int width) {
    int c;
    int chars_read = 0;

    // If no width specified, default to INT_MAX (read until whitespace).
    if (width == -1) width = INT_MAX;

    // Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    if (c == EOF) return 0;

    // Read until whitespace OR width limit reached
    while (c != EOF && !isspace(c) && chars_read < width) {
        if (out != NULL) {
            *out = (char)c;
            out++;
        }
        chars_read++;

        // Peek at the next character
        c = getchar();
    }

    // Append Null Terminator (Crucial for %s vs %c)
    if (out != NULL) {
        *out = '\0';
    }

    // Push back the character that stopped the loop
    // (It's either a space, new line, or the char that exceeded width)
    if (c != EOF) {
        ungetc(c, stdin);
    }

    return 1;
}

/**
 * Reads a hexadecimal integer (base 16).
 * BEHAVIOR:
 * 1. Skips leading whitespace.
 * 2. Handles optional sign (+/-).
 * 3. Handles optional "0x" or "0X" prefix.
 * 4. Reads digits 0-9, a-f, A-F.
 *
 * @param out: Pointer to store result.
 * @param width: Max chars. -1 for no limit.
 * @return 1 on success, 0 on failure.
 */
int read_hex(unsigned long long *out, int width) {
    int c;
    unsigned long long value = 0;
    int digits_read = 0;
    int chars_processed = 0;
    int has_width = (width > 0);

    // Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    if (c == EOF) return 0;

    // Handle Sign
    int sign_multiplier = 1;
    if (c == '-' || c == '+') {
        // Check width before consuming sign
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
            return 0;
        }
        if (c == '-') sign_multiplier = -1;
        chars_processed++;
        c = getchar();
    }

    // Handle optional "0x" or "0X"
    if (c == '0') {
        // Check if there is room for 'x' in the width limit
        if (!has_width || chars_processed + 1 < width) {
            int next = getchar();
            if (next == 'x' || next == 'X') {
                chars_processed += 2; // Consume "0x"
                c = getchar();
            } else {
                ungetc(next, stdin); // Not a prefix
            }
        }
    }

    // Read Hex Digits
    while (isxdigit(c)) {
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
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

    // Restore stopper
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // Validate
    if (digits_read == 0) return 0;

    // Store
    if (out != NULL) {
        *out = value * sign_multiplier;
    }

    return 1;
}

/**
 * Reads a floating-point number (scanf-like behavior).
 * Supports:
 * - Signed numbers (+/-)
 * - Decimal notation (123.456)
 * - Scientific notation (1.2e-3) with rollback for invalid exponents.
 * - Width limiting.
 *
 * @param out: Pointer to store the result (double).
 * @param width: Max characters to read.
 * @return 1 on success, 0 on failure.
 */
int read_float(double *out, int width) {
    int c;
    int chars = 0;
    int has_width = (width > 0);

    double value = 0.0;
    double sign = 1.0;
    int has_digits = 0;

    // Skip leading whitespace
    do {
        c = getchar();
    } while (isspace(c));

    if (c == EOF) return 0;

    // Optional sign
    int sign_char = 0;      // Backup: Store sign char to restore it if parsing fails
    int sign_consumed = 0;

    if (c == '+' || c == '-') {
        if (has_width && chars + 1 > width) {
            ungetc(c, stdin);
            return 0;
        }
        if (c == '-') sign = -1.0;
        sign_char = c;
        sign_consumed = 1;
        chars++;
        c = getchar();
    }

    // Integer part
    while (isdigit(c)) {
        if (has_width && chars + 1 > width) break;
        value = value * 10.0 + (c - '0');
        has_digits = 1;
        chars++;
        c = getchar();
    }

    // Fractional part
    if (c == '.' && (!has_width || chars + 1 <= width)) {
        double divisor = 10.0;
        chars++;
        c = getchar();

        while (isdigit(c)) {
            if (has_width && chars + 1 > width) break;
            value += (c - '0') / divisor;
            divisor *= 10.0;
            has_digits = 1;
            chars++;
            c = getchar();
        }
    }

    // Exponent part
    if (has_digits && (c == 'e' || c == 'E') &&
        (!has_width || chars + 1 <= width)) {

        int exp_sign = 1;
        int exponent = 0;
        int exp_digits = 0; // Only count actual digits to ensure validity

        int e_char = c;        // Backup 'e'
        int exp_sign_char = 0; // Backup '+' or '-'
        int has_exp_sign = 0;

        c = getchar(); // Consume 'e', but do NOT add to 'chars' yet (pending validation)

        // Optional exponent sign
        if (c == '+' || c == '-') {
            // Width check: ensure space for 'e' + sign + at least 1 digit
            if (has_width && chars + 3 > width) {
                ungetc(c, stdin);
                ungetc(e_char, stdin);
                goto finish; // Abort exponent parsing
            }
            if (c == '-') exp_sign = -1;
            exp_sign_char = c;
            has_exp_sign = 1;
            c = getchar();
        }

        // Exponent digits
        while (isdigit(c)) {
            // Check width against: current chars + 'e' + (sign?) + prev digits + new digit
            int current_len = chars + 1 + has_exp_sign + exp_digits + 1;
            if (has_width && current_len > width) break;

            exponent = exponent * 10 + (c - '0');
            exp_digits++;
            c = getchar();
        }

        // Valid exponent = at least one DIGIT found
        if (exp_digits > 0) {
            // Commit the consumed characters
            chars += 1 + has_exp_sign + exp_digits;

            // Apply exponent manually (avoids <math.h> dependency)
            while (exponent-- > 0) {
                if (exp_sign > 0) value *= 10.0;
                else value /= 10.0;
            }
        } else {
            // Rollback invalid exponent (e.g., input was "1.2e+")
            if (c != EOF) ungetc(c, stdin);                 // 1. Push back the stopper char
            if (has_exp_sign) ungetc(exp_sign_char, stdin); // 2. Push back the sign
            ungetc(e_char, stdin);                          // 3. Push back 'e'
        }
    }

finish:
    // Final stopper
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // Validation: If no digits were read, fail and restore initial sign
    if (!has_digits) {
        if (sign_consumed) ungetc(sign_char, stdin);
        return 0;
    }

    if (out) *out = value * sign;
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
                // Le cambiamos el nombre a 'out' aquí también si prefieres
                char *out = suppress ? NULL : va_arg(args, char *);

                if (!read_string(out, width)) { // Llamamos pasando 'out'
                    va_end(args);
                    return count;
                }

                if (!suppress) count++;
            }
            else if (*p == 'x') {
                unsigned long long buffer_val;

                // 1. Ternario: Ahorramos 4 líneas de setup
                unsigned long long *ptr_to_pass = suppress ? NULL : &buffer_val;

                // 2. Llamada y Check de Error
                if (!read_hex(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                // 3. Guardado (Esta parte NO se puede acortar, es la lógica real)
                if (!suppress) {
                    if (length_mod == 4) { // ll
                        *va_arg(args, unsigned long long *) = buffer_val;
                    }
                    else if (length_mod == 3) { // l
                        *va_arg(args, unsigned long *) = (unsigned long)buffer_val;
                    }
                    else if (length_mod == 1) { // h
                        *va_arg(args, unsigned short *) = (unsigned short)buffer_val;
                    }
                    else if (length_mod == 2) { // hh
                        *va_arg(args, unsigned char *) = (unsigned char)buffer_val;
                    }
                    else { // default (int)
                        *va_arg(args, unsigned int *) = (unsigned int)buffer_val;
                    }
                    count++;
                }
            }
            else if (*p == 'f') {
                double buffer_val;
                double *ptr = suppress ? NULL : &buffer_val;

                // Llamamos a read_float (que ya soporta exponentes internamente)
                if (!read_float(ptr, width)) {
                    va_end(args);
                    return count;
                }

                if (!suppress) {
                    if (length_mod == 3) *va_arg(args, double *) = buffer_val;
                    else if (length_mod == 4) *va_arg(args, long double *) = (long double)buffer_val;
                    else *va_arg(args, float *) = (float)buffer_val;
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