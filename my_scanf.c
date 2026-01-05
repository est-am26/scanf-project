#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include "my_scanf.h"

/* =========================================================================
 * HELPER FUNCTIONS
 * ========================================================================= */

/**
 * Internal Helper: Consumes whitespace and returns the first non-space char.
 */
int skip_whitespace() {
    int c;
    do {
        c = getchar();
    } while (isspace(c));
    return c; // Devuelve el primer carácter útil (o EOF)
}

/**
 * Reads a signed integer with support for field width.
 * @param out: Pointer to store the result (long long to support all int sizes).
 * @param width: Max characters to read. -1 indicates no limit.
 * @return 1 on success, 0 on failure.
 */
int read_int(long long *out, int width) {
    long long sign = 1;
    long long value = 0;
    int chars_processed = 0;
    int digits_read = 0;
    int has_width = (width > 0);

    // Skip leading whitespace (standard scanf behavior)
    int c = skip_whitespace();

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

    // Restore the stopper character
    if (c != EOF && !isdigit(c)) {
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

    for (int i = 0; i < width; i++) {
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
    int chars_read = 0;

    // If no width specified, default to INT_MAX (read until whitespace).
    if (width == -1) width = INT_MAX;

    // Skip leading whitespace
    int c = skip_whitespace();

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
    unsigned long long value = 0;
    int digits_read = 0;
    int chars_processed = 0;
    int has_width = (width > 0);

    // Skip leading whitespace
    int c = skip_whitespace();

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
    int chars = 0;
    int has_width = (width > 0);

    double value = 0.0;
    double sign = 1.0;
    int has_digits = 0;

    // Skip leading whitespace
    int c = skip_whitespace();

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

            // FIX: Prevent double ungetc at 'finish' label
            // We already manually restored 'c', so we mark it as handled.
            c = EOF;
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

/**
 * Reads a binary number (base 2).
 * BEHAVIOR:
 * 1. Skips leading whitespace.
 * 2. Reads only characters '0' and '1'.
 *
 * @param out: Pointer to store result (unsigned long long).
 * If NULL, acts as suppression (%*b).
 * @param width: Max chars to read. -1 means no limit.
 * @return 1 on success, 0 on failure.
 */
int read_binary(unsigned long long *out, int width) {
    unsigned long long value = 0;
    int digits_read = 0;
    int chars_processed = 0;
    int has_width = (width > 0);

    // Skip leading whitespace
    int c = skip_whitespace();

    if (c == EOF) return 0;

    // Process binary digits ('0' and '1' only)
    while (c == '0' || c == '1') {
        // Check width limit
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin); // Limit reached, push back char
            break;
        }

        // Bitwise Shift Logic:
        // Left shift by 1 (value << 1) is equivalent to multiplying by 2, but faster at the hardware level.
        // Then OR (|) the new bit (0 or 1) into the least significant position.
        value = (value << 1) | (c - '0');

        digits_read++;
        chars_processed++;
        c = getchar();
    }

    // Restore the stopping character
    // (Unless we stopped exactly because of the width limit)
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // Validation: Must read at least one binary digit
    if (digits_read == 0) return 0;

    // Store result (if not suppressed)
    if (out != NULL) {
        *out = value;
    }

    return 1;
}

/**
 * Helper function to read a full line until newline.
 * BEHAVIOR:
 * 1. Skips leading whitespace (unlike standard %[^\n], but safer for mixed inputs).
 * 2. Reads characters until '\n', EOF, or width limit.
 * 3. Leaves the newline ('\n') in the buffer (standard scanf behavior).
 *
 * @param out: Buffer to store string. If NULL, acts as suppression.
 * @param width: Max chars to read. -1 means no limit.
 * @return 1 on success, 0 on failure (EOF).
 */
int read_line(char *out, int width) {
    int chars_read = 0;

    // Handle default width: Use a safe maximum integer if width is -1
    if (width == -1) {
        width = 2147483647; // INT_MAX
    }

    // Skip leading whitespace
    // DESIGN CHOICE: Standard scanf("%[^\n]") does NOT skip leading whitespace.
    // However, we skip it here to consume any leftover '\n' from previous inputs,
    // making the function more robust for interactive use.
    int c = skip_whitespace();

    // Check EOF
    if (c == EOF) return 0;

    // Read until Newline ('\n') or Width limit
    while (c != EOF && c != '\n' && chars_read < width) {
        if (out != NULL) {
            *out = (char)c;
            out++;
        }
        chars_read++;
        c = getchar();
    }

    // Null-terminate the string
    if (out != NULL) {
        *out = '\0';
    }

    // Handle the delimiter
    // Standard scanf behavior leaves the delimiter ('\n') in the buffer.
    // We push it back so the next read operation can handle it (or ignore it).
    if (c == '\n') {
        ungetc(c, stdin);
    } else if (c != EOF) {
        // If we stopped due to width limit (and not newline), we also push back
        // the character we just read but couldn't store.
        ungetc(c, stdin);
    }

    return 1;
}

/* --------------------------------------------------------------------------
 * INTERNAL HELPER (Static)
 * Scope: Private (only visible in this file).
 * Purpose: Reads exactly 2 hex digits and converts them to a byte (0-255).
 * Used by: read_color (to parse RR, GG, or BB components).
 *
 * Returns: Integer value (0-255) on success, -1 on failure.
 * -------------------------------------------------------------------------- */
static int read_hex_pair() {
    // 1. Read High Nibble (First digit, e.g., the 'F' in "FF")
    int c1 = getchar();
    int d1 = -1;

    // Convert ASCII char to integer value (0-15)
    if (isdigit(c1)) d1 = c1 - '0';
    else if (c1 >= 'a' && c1 <= 'f') d1 = c1 - 'a' + 10;
    else if (c1 >= 'A' && c1 <= 'F') d1 = c1 - 'A' + 10;

    // Validation: If not a hex digit, push back and fail
    if (d1 == -1) {
        if (c1 != EOF) ungetc(c1, stdin);
        return -1;
    }

    // 2. Read Low Nibble (Second digit, e.g., the second 'F' in "FF")
    int c2 = getchar();
    int d2 = -1;

    if (isdigit(c2)) d2 = c2 - '0';
    else if (c2 >= 'a' && c2 <= 'f') d2 = c2 - 'a' + 10;
    else if (c2 >= 'A' && c2 <= 'F') d2 = c2 - 'A' + 10;

    // Validation
    if (d2 == -1) {
        if (c2 != EOF) ungetc(c2, stdin);
        // Note: We deliberately do NOT push back c1 here.
        // If we read "Fz", we consumed 'F' (valid) but failed on 'z'.
        // The 'F' is considered consumed.
        return -1;
    }

    // 3. Combine nibbles into a byte
    // Formula: (High * 16) + Low
    return (d1 * 16) + d2;
}

/**
 * Reads a hex color string in the format "#RRGGBB".
 * REQUIRES:
 * 1. A leading '#' character.
 * 2. Exactly 6 hex digits following the hash (parsed as 3 pairs).
 *
 * @param out: Pointer to RGBColor struct to store result.
 * @param width: Max characters to read.
 * @return 1 on success, 0 on failure.
 */
int read_color(RGBColor *out, int width) {
    int chars_processed = 0;
    int has_width = (width > 0);

    // Skip leading whitespace
    int c = skip_whitespace();

    if (c == EOF) return 0;

    // Match mandatory prefix '#'
    if (c != '#') {
        ungetc(c, stdin); // Not a color, rollback
        return 0;
    }
    chars_processed++;

    // Parse RGB components (3 pairs of hex digits)
    int components[3]; // [0]=R, [1]=G, [2]=B

    for (int i = 0; i < 3; i++) {
        // Width validation: We need exactly 2 chars for the next hex pair
        if (has_width && (chars_processed + 2 > width)) {
            // Note: We consumed '#' but failed on width for the components.
            // In scanf philosophy, this is a matching failure after partial success.
            return 0;
        }

        // Call internal static helper
        int val = read_hex_pair();

        if (val == -1) {
            // Parsing failed (invalid hex digit or EOF inside the pair)
            return 0;
        }

        components[i] = val;
        chars_processed += 2;
    }

    // Store result (if pointer is valid)
    if (out != NULL) {
        out->r = components[0];
        out->g = components[1];
        out->b = components[2];
    }

    return 1;
}

/**
 * Parses a date string in format DD/MM/YYYY or DD-MM-YYYY.
 * ENFORCES:
 * 1. Strict format matching (digits and separators).
 * 2. Separator consistency (must match: 12/12/2020 or 12-12-2020, not mixed).
 * 3. Logical range validation (Day: 1-31, Month: 1-12).
 *
 * @param out: Pointer to Date struct.
 * @param width: Max chars to read.
 * @return 1 on success, 0 on failure.
 */
int read_date(Date *out, int width) {
    int d = 0, m = 0, y = 0;
    int sep1 = 0; // Store first separator to ensure consistency with the second
    int chars_processed = 0;
    int has_width = (width > 0);

    // Skip leading whitespace
    int c = skip_whitespace();

    if (c == EOF) return 0;

    // Parse Day
    int digits_d = 0;
    while (isdigit(c)) {
        if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

        d = d * 10 + (c - '0');
        digits_d++;
        chars_processed++;
        c = getchar();
    }

    // Validation: Must have at least one digit
    if (digits_d == 0) {
        if (c != EOF) ungetc(c, stdin);
        return 0;
    }

    // Parse First Separator (/ or -)
    if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

    if (c == '/' || c == '-') {
        sep1 = c; // Save separator type
        chars_processed++;
        c = getchar();
    } else {
        // Missing mandatory separator
        ungetc(c, stdin);
        return 0;
    }

    // Parse Month
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

    // Parse Second Separator
    if (has_width && chars_processed >= width) { ungetc(c, stdin); return 0; }

    // Strict Validation: Second separator must match the first (e.g., 12-12/2022 is invalid)
    if (c != sep1) {
        ungetc(c, stdin);
        return 0;
    }
    chars_processed++;
    c = getchar();

    // Parse Year
    int digits_y = 0;
    while (isdigit(c)) {
        // If width limit hit during year, we stop but accept what we have so far
        if (has_width && chars_processed >= width) {
            ungetc(c, stdin);
            break;
        }

        y = y * 10 + (c - '0');
        digits_y++;
        chars_processed++;
        c = getchar();
    }
    if (digits_y == 0) {
        ungetc(c, stdin); return 0;
    }

    // Restore trailing character (stopper)
    if (c != EOF && (!has_width || chars_processed < width)) {
        ungetc(c, stdin);
    }

    // --- Advanced Logical Validation ---

    // Basic Month Check
    if (m < 1 || m > 12) return 0;
    if (d < 1) return 0;

    // Standard Days per Month Lookup Table
    // Index 0 is dummy. Index 1=Jan (31), 2=Feb (28*), 3=Mar (31)...
    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int max_days = days_in_month[m];

    // Leap Year Logic (The "Bisiesto" Check)
    // Only matters if month is February (2)
    if (m == 2) {
        // Rule: Year divisible by 4...
        // ...EXCEPT if divisible by 100 (e.g., 1900 was NOT leap)
        // ...UNLESS divisible by 400 (e.g., 2000 WAS leap)
        int is_leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);

        if (is_leap) {
            max_days = 29;
        }
    }

    // Final Day Check
    if (d > max_days) {
        return 0; // e.g., April 31st or Feb 29th (non-leap)
    }

    // Store Result
    if (out != NULL) {
        out->day = d;
        out->month = m;
        out->year = y;
    }

    return 1; // Success
}

/* =========================================================================
 * INTERNAL PARSING HELPERS (Static)
 * These abstract the format string parsing logic from the main loop.
 * ========================================================================= */

/**
 * Helper: Parses the field width from the format string.
 * Updates the pointer 'p' automatically.
 * @param p: Pointer to the current position in the format string.
 * @return: The parsed width, or -1 if no width specified.
 */
static int parse_width(const char **p) {
    int width = -1;
    // We check if the current character is a digit
    if (isdigit(**p)) {
        width = 0;
        while (isdigit(**p)) {
            width = width * 10 + (**p - '0');
            (*p)++; // We advance to the original pointer
        }
    }
    return width;
}

/**
 * Helper: Parses length modifiers (h, l, ll, etc).
 * Updates the pointer 'p' automatically.
 * @param p: Pointer to the current position in the format string.
 * @return: Modifier code (0=none, 1=h, 2=hh, 3=l, 4=ll, 5=j, 6=z, 7=t).
 */
static int parse_length_modifier(const char **p) {
    int mod = 0;
    if (**p == 'h') {
        (*p)++;
        if (**p == 'h') { mod = 2; (*p)++; } // hh
        else            { mod = 1; }         // h
    }
    else if (**p == 'l') {
        (*p)++;
        if (**p == 'l') { mod = 4; (*p)++; } // ll
        else            { mod = 3; }         // l
    }
    else if (**p == 'j') { mod = 5; (*p)++; }
    else if (**p == 'z') { mod = 6; (*p)++; }
    else if (**p == 't') { mod = 7; (*p)++; }

    return mod;
}

/**
 * Custom implementation of scanf.
 * Supports standard specifiers: %d, %x, %f, %c, %s
 * Supports custom specifiers:   %b (binary), %L (line), %D (date), %R (color)
 * Supports modifiers:           Width, Suppression (*), Length (h, hh, l, ll)
 *
 * @param format:
 * @param ...: Variable arguments matching the format string.
 * @return Number of input items successfully matched and assigned.
 */
int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0;
    const char *p = format;

    while (*p != '\0') {
        // =========================================================
        // A. HANDLE FORMAT SPECIFIERS (%)
        // =========================================================
        if (*p == '%') {
            p++; // Skip '%'

            // 1. Check for Assignment Suppression (*)
            int suppress = 0;
            if (*p == '*') {
                suppress = 1;
                p++;
            }

            // 2. Parse Field Width (Using Helper)
            // Pasamos &p (la dirección del puntero) para que el helper lo mueva
            int width = parse_width(&p);

            // 3. Parse Length Modifiers (Using Helper)
            int length_mod = parse_length_modifier(&p);

            // =====================================================
            // B. CHECK CONVERSION SPECIFIERS
            // =====================================================

            // --- Case: Signed Integer (%d) ---
            if (*p == 'd') {
                long long buffer_val;
                long long *ptr_to_pass = suppress ? NULL : &buffer_val;

                // Call helper (returns 0 on failure)
                if (!read_int(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                // Store result with correct type promotion
                if (!suppress) {
                    if      (length_mod == 4) *va_arg(args, long long *)   = buffer_val;
                    else if (length_mod == 3) *va_arg(args, long *)        = (long)buffer_val;
                    else if (length_mod == 1) *va_arg(args, short *)       = (short)buffer_val;
                    else if (length_mod == 2) *va_arg(args, signed char *) = (signed char)buffer_val;
                    else                      *va_arg(args, int *)         = (int)buffer_val;

                    count++;
                }
            }
            // --- Case: Hexadecimal (%x) ---
            else if (*p == 'x') {
                unsigned long long buffer_val;
                unsigned long long *ptr_to_pass = suppress ? NULL : &buffer_val;

                if (!read_hex(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                if (!suppress) {
                    if      (length_mod == 4) *va_arg(args, unsigned long long *) = buffer_val;
                    else if (length_mod == 3) *va_arg(args, unsigned long *)      = (unsigned long)buffer_val;
                    else if (length_mod == 1) *va_arg(args, unsigned short *)     = (unsigned short)buffer_val;
                    else if (length_mod == 2) *va_arg(args, unsigned char *)      = (unsigned char)buffer_val;
                    else                      *va_arg(args, unsigned int *)       = (unsigned int)buffer_val;

                    count++;
                }
            }
            // --- Case: Floating Point (%f) ---
            else if (*p == 'f') {
                double buffer_val;
                double *ptr_to_pass = suppress ? NULL : &buffer_val;

                if (!read_float(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                if (!suppress) {
                    // Standard scanf: %f -> float*, %lf -> double*, %Lf -> long double*
                    if      (length_mod == 3) *va_arg(args, double *)      = buffer_val;       // %lf
                    else if (length_mod == 4) *va_arg(args, long double *) = (long double)buffer_val; // %Lf (using ll logic)
                    else                      *va_arg(args, float *)       = (float)buffer_val; // %f

                    count++;
                }
            }
            // --- Case: Binary (%b) [Custom] ---
            else if (*p == 'b') {
                unsigned long long buffer_val;
                unsigned long long *ptr_to_pass = suppress ? NULL : &buffer_val;

                if (!read_binary(ptr_to_pass, width)) {
                    va_end(args);
                    return count;
                }

                if (!suppress) {
                    if      (length_mod == 4) *va_arg(args, unsigned long long *) = buffer_val;
                    else if (length_mod == 3) *va_arg(args, unsigned long *)      = (unsigned long)buffer_val;
                    else if (length_mod == 1) *va_arg(args, unsigned short *)     = (unsigned short)buffer_val;
                    else if (length_mod == 2) *va_arg(args, unsigned char *)      = (unsigned char)buffer_val;
                    else                      *va_arg(args, unsigned int *)       = (unsigned int)buffer_val;

                    count++;
                }
            }
            // --- Case: Character (%c) ---
            else if (*p == 'c') {
                char *dest = suppress ? NULL : va_arg(args, char *);

                // read_char handles the looping logic based on width
                if (read_char(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
            // --- Case: String (%s) ---
            else if (*p == 's') {
                char *dest = suppress ? NULL : va_arg(args, char *);

                if (read_string(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
            // --- Case: Full Line (%L) [Custom] ---
            else if (*p == 'L') {
                char *dest = suppress ? NULL : va_arg(args, char *);

                if (read_line(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
            // --- Case: Date (%D) [Custom] ---
            else if (*p == 'D') {
                Date *dest = suppress ? NULL : va_arg(args, Date *);

                if (read_date(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
            // --- Case: Color (%R) [Custom] ---
            else if (*p == 'R') {
                RGBColor *dest = suppress ? NULL : va_arg(args, RGBColor *);

                if (read_color(dest, width)) {
                    if (!suppress) count++;
                } else {
                    va_end(args);
                    return count;
                }
            }
        }
        // =========================================================
        // B. MATCHING LITERAL CHARACTERS
        // =========================================================
        else {
            // Case 1: Whitespace in format string matches ANY amount of whitespace
            if (isspace(*p)) {
                int c;
                while (isspace(c = getchar())) {} // Consume all whitespace
                if (c != EOF) {
                    ungetc(c, stdin); // Put back the first non-space char
                }
            }
            // Case 2: Exact character match (e.g., "Age: %d" expects "Age:")
            else {
                int c = getchar();
                if (c != *p) {
                    if (c != EOF) ungetc(c, stdin); // Mismatch, restore buffer
                    va_end(args);
                    return count; // Stop parsing immediately
                }
            }
        }
        p++; // Advance to next character in format string
    }

    va_end(args);
    return count;
}