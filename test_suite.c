#include <stdio.h>
#include <string.h>
#include <math.h>
#include "my_scanf.h"

static int tests_run = 0;
static int tests_failed = 0;

/* --- UTILITIES --- */

// Helper: Writes text to a temp file and redirects stdin to read from it.
void prepare_input(const char *text) {
    FILE *fp = fopen("test_input.tmp", "w");
    if (fp) {
        fprintf(fp, "%s", text);
        fclose(fp);
    }
    freopen("test_input.tmp", "r", stdin);
}

// Macro: Checks if actual integer matches expected value.
#define ASSERT_EQ(expected, actual, code) do { \
    tests_run++; \
    long long exp_val = (long long)(expected); \
    long long act_val = (long long)(actual); \
    if (exp_val != act_val) { \
        tests_failed++; \
        printf("❌ [FAIL %s] Expected %lld, got %lld\n", code, exp_val, act_val); \
    } else { \
        printf("✅ [PASS %s]\n", code); \
    } \
} while (0)

#define ASSERT_DBL_NEAR(expected, actual, tol, code) do { \
    tests_run++; \
    double diff = fabs((expected) - (actual)); \
    if (diff > (tol)) { \
        tests_failed++; \
        printf("❌ [FAIL %s] Expected %f, got %f (diff %g)\n", code, (expected), (actual), diff); \
    } else { \
        printf("✅ [PASS %s]\n", code); \
    } \
} while (0)

// Macro: Checks if two strings are identical.
#define ASSERT_STREQ(expected, actual, code) do { \
    tests_run++; \
    if (strcmp((expected), (actual)) != 0) { \
        tests_failed++; \
        printf("❌ [FAIL %s] Expected \"%s\", got \"%s\"\n", code, (expected), (actual)); \
    } else { \
        printf("✅ [PASS %s]\n", code); \
    } \
} while (0)

/* =========================================================================
 * INTEGER TESTS (%d)
 * ========================================================================= */

void test_d_basic_positive(void) {
    // Test: Standard positive integer.
    int x = 0;
    prepare_input("123");
    ASSERT_EQ(1, my_scanf("%d", &x), "D01_Ret");
    ASSERT_EQ(123, x, "D02_Val");
}

void test_d_basic_negative(void) {
    // Test: Standard negative integer.
    int x = 0;
    prepare_input("-456");
    ASSERT_EQ(1, my_scanf("%d", &x), "D03_Ret");
    ASSERT_EQ(-456, x, "D04_Val");
}

void test_d_zero(void) {
    // Test: Zero value.
    int x = -1;
    prepare_input("0");
    ASSERT_EQ(1, my_scanf("%d", &x), "D05_Ret");
    ASSERT_EQ(0, x, "D06_Val");
}

void test_d_leading_whitespace(void) {
    // Test: Skips spaces, tabs, and newlines before number.
    int x = 0;
    prepare_input(" \t\n42");
    ASSERT_EQ(1, my_scanf("%d", &x), "D07_Ret");
    ASSERT_EQ(42, x, "D08_Val");
}

void test_d_plus_sign(void) {
    // Test: Explicit positive sign (+).
    int x = 0;
    prepare_input("+99");
    ASSERT_EQ(1, my_scanf("%d", &x), "D09_Ret");
    ASSERT_EQ(99, x, "D10_Val");
}

void test_d_minus_only_fail(void) {
    // Test: Fail if input is just a minus sign with no digits.
    int x = 0;
    prepare_input("-\n");
    ASSERT_EQ(0, my_scanf("%d", &x), "D11_Ret");
}

void test_d_width_basic(void) {
    // Test: Width limit stops reading early (reads "123").
    int x = 0;
    prepare_input("12345");
    ASSERT_EQ(1, my_scanf("%3d", &x), "D12_Ret");
    ASSERT_EQ(123, x, "D13_Val");
}

void test_d_width_truncation(void) {
    // Test: Width includes the sign character (reads "-987").
    int x = 0;
    prepare_input("-9876");
    ASSERT_EQ(1, my_scanf("%4d", &x), "D14_Ret");
    ASSERT_EQ(-987, x, "D15_Val");
}

void test_d_width_sign_only_fail(void) {
    // Test: Fail if width cuts off the number right after the sign.
    int x = 0;
    prepare_input("-5\n");
    ASSERT_EQ(0, my_scanf("%1d", &x), "D16_Ret");
}

void test_d_stops_at_nondigit(void) {
    // Test: Stops reading when a letter is encountered.
    int x = 0;
    prepare_input("123abc");
    ASSERT_EQ(1, my_scanf("%d", &x), "D17_Ret");
    ASSERT_EQ(123, x, "D18_Val");
}

void test_d_invalid_input(void) {
    // Test: Fail on completely non-numeric input.
    int x = 0;
    prepare_input("abc");
    ASSERT_EQ(0, my_scanf("%d", &x), "D19_Ret");
}

void test_d_assignment_suppressed(void) {
    // Test: Suppression (*) consumes input but does NOT count as an assignment.
    prepare_input("456");
    ASSERT_EQ(0, my_scanf("%*d"), "D20_Ret");
}

void test_d_weird_double_sign(void) {
    // Test: Fail on double signs (-- or +-).
    int x = 0;
    prepare_input("--5");
    ASSERT_EQ(0, my_scanf("%d", &x), "D21_Ret");

    prepare_input("+-5");
    ASSERT_EQ(0, my_scanf("%d", &x), "D22_Ret");
}

void test_d_space_between_sign_and_digit(void) {
    // Test: Fail if there is a space between sign and digit.
    int x = 0;
    prepare_input("- 5");
    ASSERT_EQ(0, my_scanf("%d", &x), "D23_Ret");
}

void test_d_leading_zeros_decimal(void) {
    // Edge Case: Leading zeros should NOT be treated as Octal (must remain Decimal).
    int x = -1;
    prepare_input("007 010");

    my_scanf("%d", &x);
    ASSERT_EQ(7, x, "D_ZeroPrefix_7");

    my_scanf("%d", &x);
    ASSERT_EQ(10, x, "D_ZeroPrefix_10"); // If it reads 8, you implemented Octal by mistake.
}

void test_d_stop_at_hex_x(void) {
    // Edge Case: Standard %d stops at 'x' in "0x12". It reads "0", then stops.
    int x = -1;
    char next = ' ';

    prepare_input("0x12");

    my_scanf("%d", &x);
    ASSERT_EQ(0, x, "D_Hex_ReadZero"); // Reads the leading '0'

    // Check that 'x' is still in the buffer
    my_scanf("%c", &next);
    ASSERT_EQ('x', next, "D_Hex_NextChar");
}

void test_d_limits_32bit(void) {
    // Edge Case: Handle MAX and MIN values for 32-bit integers.
    int max = 0;
    int min = 0;
    prepare_input("2147483647 -2147483648");

    my_scanf("%d", &max);
    ASSERT_EQ(2147483647, max, "D_Max_Int");

    my_scanf("%d", &min);
    ASSERT_EQ((int)-2147483648, min, "D_Min_Int");
}

void test_d_multiple_reads_flow(void) {
    // Edge Case: Sequential reads to ensure buffer/state is cleared correctly.
    int a=0, b=0, c=0;
    prepare_input("10 20 -30");

    int count = my_scanf("%d %d %d", &a, &b, &c);

    ASSERT_EQ(3, count, "D_Flow_Count");
    ASSERT_EQ(10, a, "D_Flow_1");
    ASSERT_EQ(20, b, "D_Flow_2");
    ASSERT_EQ(-30, c, "D_Flow_3");
}

void test_d_width_exact_match(void) {
    // Edge Case: Width limit matches the number length exactly.
    int x = 0;
    char next = ' ';
    prepare_input("123A");

    // Read exactly 3 chars. Should succeed.
    my_scanf("%3d", &x);
    ASSERT_EQ(123, x, "D_Width_Exact");

    // Next char must be 'A', not consumed.
    my_scanf("%c", &next);
    ASSERT_EQ('A', next, "D_Width_Next");
}

void test_d_width_modifier_combo(void) {
    // Test: Combine field width limit with short modifier (%3hd).
    short h = 0;
    prepare_input("12345");
    ASSERT_EQ(1, my_scanf("%3hd", &h), "D_WidthMod_Ret");
    ASSERT_EQ(123, h, "D_WidthMod_Val");
}

void test_d_suppression_width(void) {
    // Test: Suppression with width limit (consumes 2 chars, returns 0).
    prepare_input("999");
    ASSERT_EQ(0, my_scanf("%*2d"), "D_SuppressWidth_Ret");
}

void test_modifiers_lld(void) {
    // Test: Long Long modifier handles large numbers > 32 bit.
    long long ll = 0;
    prepare_input("9999999999");
    ASSERT_EQ(1, my_scanf("%lld", &ll), "MOD01_LL_Ret");
    ASSERT_EQ(9999999999LL, ll, "MOD02_LL_Val");
}

void test_modifiers_hd(void) {
    // Test: Short modifier handles small integers correctly.
    short h = 0;
    prepare_input("32000");
    ASSERT_EQ(1, my_scanf("%hd", &h), "MOD03_Short_Ret");
    ASSERT_EQ(32000, h, "MOD04_Short_Val");
}

/* =========================================================================
 * CHARACTER TESTS (%c)
 * ========================================================================= */

void test_c_basic_letter(void) {
    // Test: Reads a standard uppercase letter.
    char c = 0;
    prepare_input("A");
    ASSERT_EQ(1, my_scanf("%c", &c), "C01_Ret");
    ASSERT_EQ('A', c, "C02_Val");
}

void test_c_basic_digit(void) {
    // Test: Reads a digit character (not as a number).
    char c = 0;
    prepare_input("7");
    ASSERT_EQ(1, my_scanf("%c", &c), "C03_Ret");
    ASSERT_EQ('7', c, "C04_Val");
}

void test_c_basic_symbol(void) {
    // Test: Reads a special symbol.
    char c = 0;
    prepare_input("@");
    ASSERT_EQ(1, my_scanf("%c", &c), "C05_Ret");
    ASSERT_EQ('@', c, "C06_Val");
}

void test_c_reads_space(void) {
    // Test: %c does NOT skip leading whitespace (reads the space).
    char c = 0;
    prepare_input(" ");
    ASSERT_EQ(1, my_scanf("%c", &c), "C07_Ret");
    ASSERT_EQ(' ', c, "C08_Val");
}

void test_c_reads_tab(void) {
    // Test: Reads a tab character.
    char c = 0;
    prepare_input("\t");
    ASSERT_EQ(1, my_scanf("%c", &c), "C09_Ret");
    ASSERT_EQ('\t', c, "C10_Val");
}

void test_c_reads_newline(void) {
    // Test: Reads a newline character.
    char c = 0;
    prepare_input("\n");
    ASSERT_EQ(1, my_scanf("%c", &c), "C11_Ret");
    ASSERT_EQ('\n', c, "C12_Val");
}

void test_c_after_d_reads_space(void) {
    // Test: %c reads the trailing space after a number.
    int x = 0;
    char c = 0;
    prepare_input("123 X");
    my_scanf("%d%c", &x, &c);
    ASSERT_EQ(123, x, "C13_Int");
    ASSERT_EQ(' ', c, "C14_Space");
}

void test_c_after_d_reads_letter(void) {
    // Test: %c reads the letter immediately following a number.
    int x = 0;
    char c = 0;
    prepare_input("123A");
    my_scanf("%d%c", &x, &c);
    ASSERT_EQ(123, x, "C15_Int");
    ASSERT_EQ('A', c, "C16_Letter");
}

void test_c_default_width_is_one(void) {
    // Test: Without width, %c reads exactly one char.
    char c = 0;
    prepare_input("ABC");
    my_scanf("%c", &c);
    ASSERT_EQ('A', c, "C17_DefaultWidth");
}

void test_c_width_two_reads_exact(void) {
    // Test: With width 2, reads exactly 2 chars into buffer.
    char buf[2] = {0};
    prepare_input("XY");
    ASSERT_EQ(1, my_scanf("%2c", buf), "C18_Ret");
    ASSERT_EQ('X', buf[0], "C19_Val1");
    ASSERT_EQ('Y', buf[1], "C20_Val2");
}

void test_c_width_does_not_null_terminate(void) {
    // Edge Case: %c does NOT add '\0' at the end (unlike %s).
    char buf[3] = {'A', 'A', 'A'};
    prepare_input("BC");
    my_scanf("%2c", buf);
    ASSERT_EQ('B', buf[0], "C21_Val1");
    ASSERT_EQ('C', buf[1], "C22_Val2");
    ASSERT_EQ('A', buf[2], "C23_NoNull"); // Should remain 'A'
}

void test_c_width_fail_on_eof(void) {
    // Test: Fail if width requires more chars than available (EOF).
    char buf[2];
    prepare_input("A");
    ASSERT_EQ(0, my_scanf("%2c", buf), "C24_EOF_Fail");
}

void test_c_single_char_eof_fail(void) {
    // Test: Fail on empty input.
    char c;
    prepare_input("");
    ASSERT_EQ(0, my_scanf("%c", &c), "C25_EOF_Single");
}

void test_c_suppression_consumes_char(void) {
    // Test: Suppression (*) reads char but does not store it.
    char c = 0;
    prepare_input("AB");
    my_scanf("%*c%c", &c);
    ASSERT_EQ('B', c, "C26_Suppress");
}

void test_c_suppression_only(void) {
    // Test: Suppression returns 0 matched items.
    prepare_input("X");
    ASSERT_EQ(0, my_scanf("%*c"), "C27_SuppressRet");
}

void test_c_multiple_reads_flow(void) {
    // Test: Sequential %c reads.
    char a=0, b=0, c=0;
    prepare_input("ABC");
    my_scanf("%c%c%c", &a, &b, &c);
    ASSERT_EQ('A', a, "C28_Flow1");
    ASSERT_EQ('B', b, "C29_Flow2");
    ASSERT_EQ('C', c, "C30_Flow3");
}

void test_c_interleaved_with_string(void) {
    // Use width limit %5s so it stops at "hello" and leaves "X" for %c.
    // Without width, %s would consume "helloX" entirely because there is no space.
    char c = 0;
    char buf[10];

    prepare_input("helloX");

    my_scanf("%5s%c", buf, &c);

    ASSERT_EQ('X', c, "C31_AfterString");
    if (strcmp(buf, "hello") != 0) printf("❌ String wrong\n");
}

void test_c_symbol_after_newline(void) {
    // Test: Verify correct handling of newlines in sequence.
    char c;
    prepare_input("\nZ");
    my_scanf("%c", &c);
    ASSERT_EQ('\n', c, "C32_Newline");
    my_scanf("%c", &c);
    ASSERT_EQ('Z', c, "C33_AfterNewline");
}

void test_c_width_three_with_spaces(void) {
    // Test: Width 3 reads spaces as valid characters.
    char buf[3];
    prepare_input(" A ");
    ASSERT_EQ(1, my_scanf("%3c", buf), "C34_WidthSpaces");
    ASSERT_EQ(' ', buf[0], "C35_S1");
    ASSERT_EQ('A', buf[1], "C36_S2");
    ASSERT_EQ(' ', buf[2], "C37_S3");
}

void test_c_skip_whitespace_directive(void) {
    // Fire Test: Format " %c" (with space) MUST skip leading whitespace.
    // Standard "%c" reads the space. " %c" skips it.
    char c = 'Z'; // Initial garbage value
    prepare_input("   A");

    // Notice the space before the %c
    ASSERT_EQ(1, my_scanf(" %c", &c), "C_SkipSpace_Ret");
    ASSERT_EQ('A', c, "C_SkipSpace_Val");
}

void test_c_suppression_width_combo(void) {
    // Fire Test: Consume exactly 3 chars, store nothing.
    char c = 0;
    prepare_input("123X");

    // %*3c consumes "123". The next %c should read 'X'.
    // Returns 1 because only %c counts (the suppression doesn't).
    ASSERT_EQ(1, my_scanf("%*3c%c", &c), "C_SuppressWidth_Ret");
    ASSERT_EQ('X', c, "C_SuppressWidth_Val");
}

void test_c_ignore_modifiers(void) {
    // Fire Test: Modifiers like 'h' or 'l' on %c are usually ignored
    // in simple implementations (or denote wide chars), but shouldn't crash.
    char c = 0;
    prepare_input("M");

    // The parser will read the 'h', see that it's a modifier, and then read the 'c'.
    // It should work the same as a normal %c.
    ASSERT_EQ(1, my_scanf("%hc", &c), "C_Modifier_Ret");
    ASSERT_EQ('M', c, "C_Modifier_Val");
}

/* =========================================================================
 * STRING TESTS (%s)
 * ========================================================================= */

void test_s_basic_word(void) {
    // Test: Reads a simple word.
    char buf[10];
    prepare_input("hello");
    ASSERT_EQ(1, my_scanf("%s", buf), "S01_Ret");
    ASSERT_EQ(0, strcmp(buf, "hello"), "S02_Val");
}

void test_s_single_character(void) {
    // Test: Reads a single character as a string.
    char buf[4];
    prepare_input("A");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "A"), "S03_OneChar");
}

void test_s_skips_leading_spaces(void) {
    // Test: Skips spaces before the word starts.
    char buf[10];
    prepare_input("   hello");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "hello"), "S04_Spaces");
}

void test_s_skips_tabs_and_newlines(void) {
    // Test: Skips tabs and newlines before the word.
    char buf[10];
    prepare_input("\t\nworld");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "world"), "S05_TabsNL");
}

void test_s_stops_at_space(void) {
    // Test: Reads until the first space.
    char buf[10];
    prepare_input("hello world");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "hello"), "S06_StopSpace");
}

void test_s_stops_at_tab(void) {
    // Test: Reads until the first tab.
    char buf[10];
    prepare_input("hello\tworld");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "hello"), "S07_StopTab");
}

void test_s_stops_at_newline(void) {
    // Test: Reads until the first newline.
    char buf[10];
    prepare_input("hello\nworld");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "hello"), "S08_StopNL");
}

void test_s_width_basic(void) {
    // Test: Width limits the number of characters read (standard case).
    char buf[10];
    prepare_input("abcdef");
    my_scanf("%3s", buf);
    ASSERT_EQ(0, strcmp(buf, "abc"), "S09_Width3");
}

void test_s_width_exact_match(void) {
    // Test: Width exactly matches the input length.
    char buf[4];
    prepare_input("abc ");
    my_scanf("%3s", buf);
    ASSERT_EQ(0, strcmp(buf, "abc"), "S10_WidthExact");
}

void test_s_width_smaller_than_word(void) {
    // Test: Input is longer than width; cuts off the string.
    char buf[10];
    prepare_input("longword");
    my_scanf("%4s", buf);
    ASSERT_EQ(0, strcmp(buf, "long"), "S11_WidthCut");
}

void test_s_null_terminated(void) {
    // Test: Ensures string is always null-terminated ('\0').
    char buf[5] = {'X','X','X','X','X'};
    prepare_input("hi");
    my_scanf("%s", buf);
    ASSERT_EQ('h', buf[0], "S12_NT_0");
    ASSERT_EQ('i', buf[1], "S13_NT_1");
    ASSERT_EQ('\0', buf[2], "S14_NT_Term");
}

void test_s_empty_input_fail(void) {
    // Test: Fails immediately if input is empty (EOF).
    char buf[10];
    prepare_input("");
    ASSERT_EQ(EOF, my_scanf("%s", buf), "S15_EmptyFail");
}

void test_s_only_whitespace_fail(void) {
    // Test: Fails if input is only whitespace (and then EOF).
    char buf[10];
    prepare_input("   \t\n");
    ASSERT_EQ(EOF, my_scanf("%s", buf), "S16_WhitespaceFail");
}

void test_s_suppression_consumes_word(void) {
    // Test: Suppression (*) reads word but discards it.
    char buf[10];
    prepare_input("hello world");
    my_scanf("%*s%s", buf);
    ASSERT_EQ(0, strcmp(buf, "world"), "S17_Suppress");
}

void test_s_suppression_only(void) {
    // Test: Suppression returns 0 match count.
    prepare_input("hello");
    ASSERT_EQ(0, my_scanf("%*s"), "S18_SuppressRet");
}

void test_s_multiple_reads(void) {
    // Test: Flow check with multiple strings.
    char a[10], b[10];
    prepare_input("foo bar");
    int count = my_scanf("%s %s", a, b);
    ASSERT_EQ(2, count, "S19_FlowCount");
    ASSERT_EQ(0, strcmp(a, "foo"), "S20_FlowA");
    ASSERT_EQ(0, strcmp(b, "bar"), "S21_FlowB");
}

void test_s_punctuation(void) {
    // Test: Punctuation is treated as part of the word (unless space).
    char buf[15];
    prepare_input("hello,world");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "hello,world"), "S22_Punct");
}

void test_s_number_string(void) {
    // Test: Numbers are valid strings.
    char buf[10];
    prepare_input("12345");
    my_scanf("%s", buf);
    ASSERT_EQ(0, strcmp(buf, "12345"), "S23_Number");
}

void test_s_width_then_space(void) {
    // Test: Width logic handles subsequent space correctly.
    char buf[10];
    prepare_input("abc def");
    my_scanf("%3s", buf);
    ASSERT_EQ(0, strcmp(buf, "abc"), "S24_WidthSpace");
}

void test_s_split_word_by_width(void) {
    // Fire Test: Read first part with width, second part with next scan.
    char part1[10], part2[10];
    prepare_input("abcdef");

    // Reads "abc", leaves "def" in buffer.
    my_scanf("%3s", part1);
    // Reads remaining "def".
    my_scanf("%s", part2);

    ASSERT_EQ(0, strcmp(part1, "abc"), "S25_Split1");
    ASSERT_EQ(0, strcmp(part2, "def"), "S26_Split2");
}

void test_s_ignore_modifiers(void) {
    // Fire Test: Modifiers like 'l' (wide) or 'h' should be ignored in this implementation.
    // It should behave like normal %s.
    char buf[10];
    prepare_input("wide");
    my_scanf("%ls", buf);
    ASSERT_EQ(0, strcmp(buf, "wide"), "S27_IgnoreMod");
}

void test_s_suppression_and_width(void) {
    // Fire Test: Combine suppression (*) and width.
    // Should consume 3 chars ("123") and discard them.
    char buf[10];
    prepare_input("123456");

    my_scanf("%*3s%s", buf);
    ASSERT_EQ(0, strcmp(buf, "456"), "S28_SuppressWidth");
}

void test_s_format_string_space(void) {
    // Fire Test: Space in format string (" %s") works same as "%s".
    char buf[10];
    prepare_input("  test");
    my_scanf(" %s", buf);
    ASSERT_EQ(0, strcmp(buf, "test"), "S29_FormatSpace");
}

void test_s_width_one(void) {
    // Edge Case: Smallest possible width.
    // Buffer needs size 2 (1 char + 1 null).
    char buf[2];
    prepare_input("ABC");

    my_scanf("%1s", buf);

    ASSERT_EQ('A', buf[0], "S30_Width1_Char");
    ASSERT_EQ('\0', buf[1], "S31_Width1_Null");
}

void test_s_literal_match(void) {
    // Integration Test: Match literal text before reading string.
    char buf[10];
    prepare_input("Name:Esther");

    int ret = my_scanf("Name:%s", buf);

    ASSERT_EQ(1, ret, "S32_Literal_Ret");
    ASSERT_EQ(0, strcmp(buf, "Esther"), "S33_Literal_Val");
}

void test_s_eof_no_newline(void) {
    // Edge Case: File ends exactly at the end of the word.
    char buf[10];
    prepare_input("End"); // No \n, no space. Just EOF.

    int ret = my_scanf("%s", buf);

    ASSERT_EQ(1, ret, "S34_EOF_Ret");
    ASSERT_EQ(0, strcmp(buf, "End"), "S35_EOF_Val");
}

/* =========================================================================
 * HEXADECIMAL TESTS (%x)
 * ========================================================================= */

void test_x_basic_lowercase(void) {
    // Test: Basic hex parsing with lowercase letters.
    unsigned int x = 0;
    prepare_input("ff");
    ASSERT_EQ(1, my_scanf("%x", &x), "X01_Ret");
    ASSERT_EQ(255, x, "X02_Val");
}

void test_x_basic_uppercase(void) {
    // Test: Basic hex parsing with uppercase letters.
    unsigned int x = 0;
    prepare_input("FF");
    ASSERT_EQ(1, my_scanf("%x", &x), "X03_Ret");
    ASSERT_EQ(255, x, "X04_Val");
}

void test_x_basic_digit(void) {
    // Test: Hex parsing of a single digit 'a' (10).
    unsigned int x = 0;
    prepare_input("a");
    my_scanf("%x", &x);
    ASSERT_EQ(10, x, "X05_Val");
}

void test_x_leading_whitespace(void) {
    // Test: Skips whitespace, tabs, and newlines before hex.
    unsigned int x = 0;
    prepare_input("   \t\n1A");
    my_scanf("%x", &x);
    ASSERT_EQ(26, x, "X06_WS");
}

void test_x_prefix_lowercase(void) {
    // Test: Handles "0x" prefix correctly.
    unsigned int x = 0;
    prepare_input("0xff");
    my_scanf("%x", &x);
    ASSERT_EQ(255, x, "X07_0x");
}

void test_x_prefix_uppercase(void) {
    // Test: Handles "0X" prefix correctly.
    unsigned int x = 0;
    prepare_input("0X1A");
    my_scanf("%x", &x);
    ASSERT_EQ(26, x, "X08_0X");
}

void test_x_prefix_without_digits_fail(void) {
    // Test: "0x" alone is not a valid hex number (should fail or return 0).
    // Note: Standard scanf usually returns 1 reading the '0' and leaving 'x',
    // but failing is also a valid strict interpretation.
    unsigned int x = 0;
    prepare_input("0x\n");
    ASSERT_EQ(0, my_scanf("%x", &x), "X09_0xFail");
}

void test_x_prefix_invalid_digit(void) {
    // Test: "0x" followed by invalid char. Should fail.
    unsigned int x = 0;
    prepare_input("0xG1");
    ASSERT_EQ(0, my_scanf("%x", &x), "X10_0xInvalid");
}

void test_x_negative(void) {
    // Test: Negative sign with hex (valid in scanf, result matches cast).
    int x = 0;
    prepare_input("-ff");
    my_scanf("%x", &x);
    ASSERT_EQ(-255, x, "X11_Neg");
}

void test_x_positive_sign(void) {
    // Test: Explicit positive sign.
    unsigned int x = 0;
    prepare_input("+1A");
    my_scanf("%x", &x);
    ASSERT_EQ(26, x, "X12_Pos");
}

void test_x_sign_only_fail(void) {
    // Test: Sign without digits fails.
    unsigned int x = 0;
    prepare_input("-\n");
    ASSERT_EQ(0, my_scanf("%x", &x), "X13_SignFail");
}

void test_x_width_basic(void) {
    // Test: Basic width limit.
    unsigned int x = 0;
    prepare_input("abcdef");
    my_scanf("%3x", &x);
    ASSERT_EQ(0xabc, x, "X14_Width3");
}

void test_x_width_with_prefix(void) {
    // Test: Width counts the prefix "0x" (2 chars).
    // Input "0x1234", Width 3 -> Reads "0x1".
    unsigned int x = 0;
    prepare_input("0x1234");
    my_scanf("%3x", &x);
    ASSERT_EQ(1, x, "X15_Width0x");
}

void test_x_width_blocks_prefix(void) {
    // Test: Width 2 on "0xFF" reads "0x". Not a number.
    // Should parse the '0', see 'x' as stopper, and return 0.
    unsigned int x = 0;
    prepare_input("0xFF");
    my_scanf("%2x", &x);
    ASSERT_EQ(0, x, "X16_BlockX");
}

void test_x_width_sign_only_fail(void) {
    // Test: Width cuts off right after sign.
    int x = 0;
    prepare_input("-f");
    ASSERT_EQ(0, my_scanf("%1x", &x), "X17_WidthSignFail");
}

void test_x_width_exact_match(void) {
    // Test: Width matches length exactly.
    unsigned int x = 0;
    prepare_input("0xFF");
    my_scanf("%4x", &x);
    ASSERT_EQ(255, x, "X18_WidthExact");
}

void test_x_stops_at_nondigit(void) {
    // Test: Stops reading at non-hex character.
    unsigned int x = 0;
    char c = 0;
    prepare_input("FFxyz");
    my_scanf("%x", &x);
    ASSERT_EQ(255, x, "X19_StopVal");

    // Ensure 'x' is still in buffer
    my_scanf("%c", &c);
    ASSERT_EQ('x', c, "X20_StopChar");
}

void test_x_invalid_letter_fail(void) {
    // Test: Immediate fail on non-hex letter 'g'.
    unsigned int x = 0;
    prepare_input("g");
    ASSERT_EQ(0, my_scanf("%x", &x), "X21_Invalid");
}

void test_x_only_whitespace_fail(void) {
    // Test: Fail on empty/whitespace input.
    unsigned int x = 0;
    prepare_input("   ");
    ASSERT_EQ(EOF, my_scanf("%x", &x), "X22_WSFail");
}

void test_x_leading_zeros(void) {
    // Test: Leading zeros are valid and ignored.
    unsigned int x = 0;
    prepare_input("000ff");
    my_scanf("%x", &x);
    ASSERT_EQ(255, x, "X23_LeadingZeros");
}

void test_x_suppression_consumes(void) {
    // Test: Suppression skips the hex number.
    unsigned int x = 0;
    prepare_input("ff aa");
    my_scanf("%*x%x", &x);
    ASSERT_EQ(170, x, "X24_Suppress");
}

void test_x_suppression_only(void) {
    // Test: Suppression returns 0 match count.
    prepare_input("ff");
    ASSERT_EQ(0, my_scanf("%*x"), "X25_SuppressRet");
}

void test_x_multiple_reads(void) {
    // Test: Flow check with multiple hex numbers.
    unsigned int a=0, b=0, c=0;
    prepare_input("10 ff 0x20");
    int count = my_scanf("%x %x %x", &a, &b, &c);
    ASSERT_EQ(3, count, "X26_FlowCount");
    ASSERT_EQ(16, a, "X27_Flow1");
    ASSERT_EQ(255, b, "X28_Flow2");
    ASSERT_EQ(32, c, "X29_Flow3");
}

void test_x_max_uint(void) {
    // Fire Test: Read the maximum 32-bit hex value.
    unsigned int x = 0;
    prepare_input("FFFFFFFF");
    my_scanf("%x", &x);
    ASSERT_EQ(0xFFFFFFFF, x, "X30_MaxUint");
}

void test_x_sign_and_prefix(void) {
    // Fire Test: Combination of sign and 0x prefix (-0xA = -10).
    int x = 0;
    prepare_input("-0xA");
    my_scanf("%x", &x);
    ASSERT_EQ(-10, x, "X31_SignPrefix");
}

void test_x_mixed_case_prefix(void) {
    // Fire Test: Mixed casing in digits (0xA, 0Xa, etc.).
    unsigned int x = 0;
    prepare_input("0XaB");
    my_scanf("%x", &x);
    ASSERT_EQ(171, x, "X32_MixedCase");
}

void test_x_ignore_modifiers(void) {
    // Fire Test: Should handle (or ignore) length modifiers like 'l' or 'h'.
    unsigned long x = 0;
    prepare_input("FF");
    // Even if we treat %lx as %x, it should work for small numbers.
    ASSERT_EQ(1, my_scanf("%lx", &x), "X33_ModifierRet");
    ASSERT_EQ(255, x, "X34_ModifierVal");
}

void test_x_zero_input(void) {
    // Fire Test: Simple zero.
    unsigned int x = 1;
    prepare_input("0");
    my_scanf("%x", &x);
    ASSERT_EQ(0, x, "X35_Zero");
}

/* =========================================================================
 * FLOATING POINT TESTS (%f)
 * ========================================================================= */

void test_f_basic_integer(void) {
    // Test: Basic integer input parsed into a float.
    float x = 0;
    prepare_input("123");
    ASSERT_EQ(1, my_scanf("%f", &x), "F01_Ret");
    ASSERT_DBL_NEAR(123.0, x, 1e-5, "F02_Val");
}

void test_f_basic_fraction(void) {
    // Test: Standard decimal point number.
    float x = 0;
    prepare_input("12.34");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(12.34, x, 1e-5, "F03_Frac");
}

void test_f_leading_whitespace(void) {
    // Test: Skip spaces, tabs, and newlines before the float.
    float x = 0;
    prepare_input("   \t\n42.5");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(42.5, x, 1e-5, "F04_WS");
}

void test_f_negative(void) {
    // Test: Negative floating point value.
    float x = 0;
    prepare_input("-3.5");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(-3.5, x, 1e-5, "F05_Neg");
}

void test_f_plus_sign(void) {
    // Test: Explicit positive sign (+).
    float x = 0;
    prepare_input("+7.25");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(7.25, x, 1e-5, "F06_Pos");
}

void test_f_leading_dot(void) {
    // Test: Number starting with a dot (implicit zero).
    float x = 0;
    prepare_input(".5");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(0.5, x, 1e-5, "F07_DotLead");
}

void test_f_trailing_dot(void) {
    // Test: Number ending with a dot (5. -> 5.0).
    float x = 0;
    prepare_input("5.");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(5.0, x, 1e-5, "F08_DotTrail");
}

void test_f_dot_only_fail(void) {
    // Test: Fail if input is only a dot with no digits.
    float x = 1.0f;
    prepare_input(".\n");
    ASSERT_EQ(0, my_scanf("%f", &x), "F09_DotFail");
}

void test_f_sign_only_fail(void) {
    // Test: Fail if input is only a sign with no digits.
    float x = 1.0f;
    prepare_input("-");
    ASSERT_EQ(0, my_scanf("%f", &x), "F10_SignFail");
}

void test_f_exponent_positive(void) {
    // Test: Scientific notation with positive exponent (1e3).
    float x = 0;
    prepare_input("1e3");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(1000.0, x, 1e-5, "F11_ExpPos");
}

void test_f_exponent_negative(void) {
    // Test: Scientific notation with negative exponent (e-2).
    float x = 0;
    prepare_input("2.5e-2");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(0.025, x, 1e-5, "F12_ExpNeg");
}

void test_f_exponent_plus_sign(void) {
    // Test: Scientific notation with explicit plus in exponent (e+1).
    float x = 0;
    prepare_input("3.2e+1");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(32.0, x, 1e-5, "F13_ExpPlus");
}

void test_f_invalid_exponent_rollback_e(void) {
    // Test: Rollback 'e' if it is not followed by a digit.
    float x = 0;
    char c = 0;
    prepare_input("1.2eX");

    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(1.2, x, 1e-5, "F14_RB_Val");

    my_scanf("%c", &c);
    ASSERT_EQ('e', c, "F15_RB_Char");
}

void test_f_invalid_exponent_rollback_e_plus(void) {
    // Test: Rollback 'e' and '+' if no digits follow.
    float x = 0;
    char c = 0;
    prepare_input("1.2e+X");

    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(1.2, x, 1e-5, "F16_RB_Val");

    my_scanf("%c", &c);
    ASSERT_EQ('e', c, "F17_RB_Char");
}

void test_f_width_basic(void) {
    // Test: Width limits the number of characters read.
    float x = 0;
    prepare_input("123.45");
    my_scanf("%5f", &x); // reads "123.4"
    ASSERT_DBL_NEAR(123.4, x, 1e-5, "F18_Width");
}

void test_f_width_blocks_fraction(void) {
    // Test: Width stops reading before the decimal point.
    float x = 0;
    prepare_input("12.34");
    my_scanf("%2f", &x); // reads "12"
    ASSERT_DBL_NEAR(12.0, x, 1e-5, "F19_WidthInt");
}

void test_f_width_blocks_exponent(void) {
    // Test: Width stops at 'e', requiring a rollback.
    float x = 0;
    char c = 0;
    prepare_input("1e3");

    my_scanf("%2f", &x); // reads "1", 'e' is pushed back
    ASSERT_DBL_NEAR(1.0, x, 1e-5, "F20_WidthExp");

    my_scanf("%c", &c);
    ASSERT_EQ('e', c, "F20b_NextChar");
}

void test_f_stops_at_letter(void) {
    // Test: Parsing stops when a non-numeric letter is found.
    float x = 0;
    char c = 0;
    prepare_input("12.3abc");

    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(12.3, x, 1e-5, "F21_StopVal");

    my_scanf("%c", &c);
    ASSERT_EQ('a', c, "F22_StopChar");
}

void test_f_multiple_reads(void) {
    // Test: Sequential reads of multiple floating point formats.
    float a=0, b=0, c=0;
    prepare_input("1.5 2e2 -3.25");

    int count = my_scanf("%f %f %f", &a, &b, &c);

    ASSERT_EQ(3, count, "F23_FlowCount");
    ASSERT_DBL_NEAR(1.5, a, 1e-5, "F24_Flow1");
    ASSERT_DBL_NEAR(200.0, b, 1e-5, "F25_Flow2");
    ASSERT_DBL_NEAR(-3.25, c, 1e-5, "F26_Flow3");
}

void test_f_uppercase_E(void) {
    // Test: Scientific notation with uppercase 'E'.
    float x = 0;
    prepare_input("1.5E2");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(150.0, x, 1e-5, "F27_UpperE");
}

void test_f_leading_zeros_scientific(void) {
    // Test: Handles leading zeros in scientific notation.
    float x = 0;
    prepare_input("001.2e1");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(12.0, x, 1e-5, "F28_LeadZeroExp");
}

void test_f_weird_width_cut_mid_exp(void) {
    // Test: Width cuts input right at 'e'.
    float x = 0;
    char c = 0;
    prepare_input("1.2e5");

    my_scanf("%4f", &x);
    ASSERT_DBL_NEAR(1.2, x, 1e-5, "F29_WidthCutE");

    my_scanf("%c", &c);
    ASSERT_EQ('e', c, "F30_WidthCutE_Char");
}

void test_f_suppression(void) {
    // Test: Assignment suppression (%*f) skips the value.
    float x = 0;
    prepare_input("1.5 2.5");

    my_scanf("%*f %f", &x);
    ASSERT_DBL_NEAR(2.5, x, 1e-5, "F31_Suppress");
}

void test_f_ignore_l_modifier(void) {
    // Test: Use double pointer for %lf (Long Float).
    double x = 0;
    prepare_input("3.14159");

    int ret = my_scanf("%lf", &x);
    ASSERT_EQ(1, ret, "F32_Lf_Ret");
    ASSERT_DBL_NEAR(3.14159, x, 1e-9, "F33_Lf_Val");
}

void test_f_valid_dot_exponent(void) {
    // Test: Valid scientific notation with no digits after decimal point.
    float x = 0;
    prepare_input("1.e2");
    my_scanf("%f", &x);
    ASSERT_DBL_NEAR(100.0, x, 1e-5, "F34_DotExp");
}

/* =========================================================================
 * BINARY TESTS (%b)
 * ========================================================================= */

void test_b_basic_zero(void) {
    // Test: Reading a single binary zero.
    unsigned long long x = 99;
    prepare_input("0");
    ASSERT_EQ(1, my_scanf("%b", &x), "B01_Ret");
    ASSERT_EQ(0ULL, x, "B02_Val");
}

void test_b_basic_one(void) {
    // Test: Reading a single binary one.
    unsigned long long x = 0;
    prepare_input("1");
    my_scanf("%b", &x);
    ASSERT_EQ(1ULL, x, "B03_Val");
}

void test_b_basic_multi_bits(void) {
    // Test: Reading multiple bits (1011 binary = 11 decimal).
    unsigned long long x = 0;
    prepare_input("1011");
    my_scanf("%b", &x);
    ASSERT_EQ(11ULL, x, "B04_Val");
}

void test_b_leading_whitespace(void) {
    // Test: Skip whitespace and tabs before binary number.
    unsigned long long x = 0;
    prepare_input("   \t\n101");
    my_scanf("%b", &x);
    ASSERT_EQ(5ULL, x, "B05_WS");
}

void test_b_stops_at_letter(void) {
    // Test: Parsing stops when a letter is encountered.
    unsigned long long x = 0;
    char c = 0;
    prepare_input("101abc");
    my_scanf("%b", &x);
    ASSERT_EQ(5ULL, x, "B06_StopVal");
    my_scanf("%c", &c);
    ASSERT_EQ('a', c, "B07_StopChar");
}

void test_b_stops_at_digit_two(void) {
    // Test: Stops at '2' because it is not a binary digit.
    unsigned long long x = 0;
    char c = 0;
    prepare_input("110210");
    my_scanf("%b", &x);
    ASSERT_EQ(6ULL, x, "B08_StopVal");
    my_scanf("%c", &c);
    ASSERT_EQ('2', c, "B09_StopChar");
}

void test_b_invalid_letter_fail(void) {
    // Test: Fail if input starts with a non-binary letter.
    unsigned long long x = 0;
    prepare_input("a101");
    ASSERT_EQ(0, my_scanf("%b", &x), "B10_Invalid");
}

void test_b_invalid_digit_fail(void) {
    // Test: Fail if input starts with a non-binary digit like '2'.
    unsigned long long x = 0;
    prepare_input("2");
    ASSERT_EQ(0, my_scanf("%b", &x), "B11_InvalidDigit");
}

void test_b_only_whitespace_fail(void) {
    // Test: Fail on input containing only whitespace.
    unsigned long long x = 0;
    prepare_input("   \t");
    ASSERT_EQ(EOF, my_scanf("%b", &x), "B12_WSFail");
}

void test_b_width_basic(void) {
    // Test: Limit bits read using a width specifier.
    unsigned long long x = 0;
    prepare_input("101101");
    my_scanf("%3b", &x);
    ASSERT_EQ(5ULL, x, "B13_Width3");
}

void test_b_width_exact_match(void) {
    // Test: Width matches exact number of bits provided.
    unsigned long long x = 0;
    prepare_input("110");
    my_scanf("%3b", &x);
    ASSERT_EQ(6ULL, x, "B14_WidthExact");
}

void test_b_width_blocks_extra_bits(void) {
    // Test: Ensure width stops and leaves remaining bits in buffer.
    unsigned long long x = 0;
    char c = 0;
    prepare_input("1011");
    my_scanf("%2b", &x);
    ASSERT_EQ(2ULL, x, "B15_WidthVal");
    my_scanf("%c", &c);
    ASSERT_EQ('1', c, "B16_WidthChar");
}

void test_b_width_eof_fail(void) {
    // Test: Successfully reads available bits even if width is not fully met.
    unsigned long long x = 0;
    prepare_input("1");
    // Changed expected from 0 to 1 (partial success before EOF)
    ASSERT_EQ(1, my_scanf("%2b", &x), "B17_EOFWidth");
}

void test_b_leading_zeros(void) {
    // Test: Ignore leading zeros in binary input.
    unsigned long long x = 0;
    prepare_input("000101");
    my_scanf("%b", &x);
    ASSERT_EQ(5ULL, x, "B18_LeadingZeros");
}

void test_b_suppression_consumes(void) {
    // Test: Skip binary value using assignment suppression (*).
    unsigned long long x = 0;
    prepare_input("101 11");
    my_scanf("%*b%b", &x);
    ASSERT_EQ(3ULL, x, "B19_Suppress");
}

void test_b_suppression_only(void) {
    // Test: Suppression returns 0 matched items.
    prepare_input("101");
    ASSERT_EQ(0, my_scanf("%*b"), "B20_SuppressRet");
}

void test_b_multiple_reads(void) {
    // Test: Read multiple binary values sequentially.
    unsigned long long a=0, b=0, c=0;
    prepare_input("10 1 111");
    int count = my_scanf("%b %b %b", &a, &b, &c);
    ASSERT_EQ(3, count, "B21_FlowCount");
    ASSERT_EQ(2ULL, a, "B22_Flow1");
    ASSERT_EQ(1ULL, b, "B23_Flow2");
    ASSERT_EQ(7ULL, c, "B24_Flow3");
}

void test_b_zero_then_letter(void) {
    // Test: Read '0' and stop at letter 'b' (not 0b prefix).
    unsigned long long x = 0;
    char c;
    prepare_input("0b101");
    my_scanf("%b", &x);
    ASSERT_EQ(0ULL, x, "B25_ZeroVal");
    my_scanf("%c", &c);
    ASSERT_EQ('b', c, "B26_ZeroChar");
}

void test_b_max_64bit(void) {
    unsigned long long x = 0;
    prepare_input("1111111111111111111111111111111111111111111111111111111111111111");
    // Added 'll' here to match the 64-bit variable 'x'
    my_scanf("%llb", &x);
    ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, x, "B27_Max64");
}

void test_b_negative_binary(void) {
    long long x = 0;
    prepare_input("-101");
    // Added 'll' here to match the 64-bit variable 'x'
    my_scanf("%llb", &x);
    ASSERT_EQ(-5LL, x, "B28_Negative");
}

void test_b_suppress_with_width(void) {
    // Fire Test: Combine suppression and width to skip specific bits.
    unsigned long long x = 0;
    prepare_input("111000");
    // Skip "111", read "000"
    my_scanf("%*3b%b", &x);
    ASSERT_EQ(0ULL, x, "B29_SuppressWidth");
}

void test_b_modifier_ignore(void) {
    // Fire Test: Ensure length modifiers (like %llb) are handled.
    unsigned long long x = 0;
    prepare_input("11");
    my_scanf("%llb", &x);
    ASSERT_EQ(3ULL, x, "B30_Modifier");
}

/* =========================================================================
 * LINE READING TESTS (%L) - Custom Specifier
 * ========================================================================= */

void test_L_basic_line(void) {
    // Test: Read a full line until newline.
    char buf[50];
    prepare_input("Hello world\n");
    ASSERT_EQ(1, my_scanf("%L", buf), "L01_Ret");
    ASSERT_STREQ("Hello world", buf, "L02_Val");
}

void test_L_skip_leading_whitespace(void) {
    // Test: Ensure leading whitespace is skipped before the line starts.
    char buf[50];
    prepare_input("   \t\nHello\n");
    ASSERT_EQ(1, my_scanf("%L", buf), "L03_Ret");
    ASSERT_STREQ("Hello", buf, "L04_Val");
}

void test_L_empty_line_after_ws(void) {
    // Test: Whitespace skip followed immediately by a newline results in empty string.
    char buf[10];
    prepare_input("   \n");
    ASSERT_EQ(1, my_scanf("%L", buf), "L05_Ret");
    ASSERT_STREQ("", buf, "L06_Empty");
}

void test_L_newline_left_in_buffer(void) {
    // Test: %L should leave the newline '\n' in the input stream (standard behavior).
    char buf[20];
    char next;
    prepare_input("abc\nX");
    my_scanf("%L", buf);
    my_scanf("%c", &next);
    ASSERT_STREQ("abc", buf, "L07_Val");
    ASSERT_EQ('\n', next, "L08_NewlineStillThere");
}

void test_L_width_cut(void) {
    // Test: Field width limits the number of characters read from the line.
    char buf[10];
    prepare_input("HelloWorld\n");
    ASSERT_EQ(1, my_scanf("%5L", buf), "L09_Ret");
    ASSERT_STREQ("Hello", buf, "L10_Val");
}

void test_L_width_exact(void) {
    // Test: Field width matches the line length exactly.
    char buf[10];
    prepare_input("Hello\n");
    ASSERT_EQ(1, my_scanf("%5L", buf), "L11_Ret");
    ASSERT_STREQ("Hello", buf, "L12_Exact");
}

void test_L_width_one(void) {
    // Test: Field width of 1 reads a single character.
    char buf[10];
    prepare_input("ABC\n");
    ASSERT_EQ(1, my_scanf("%1L", buf), "L13_Ret");
    ASSERT_STREQ("A", buf, "L14_Char");
}

void test_L_width_newline_preserved(void) {
    // Test: If width cuts the line, the remaining characters stay in buffer.
    char buf[10];
    char next;
    prepare_input("ABCDE\n");
    my_scanf("%3L", buf);
    my_scanf("%c", &next);
    ASSERT_STREQ("ABC", buf, "L15_Val");
    ASSERT_EQ('D', next, "L16_NextChar");
}

void test_L_eof_immediate(void) {
    // Test: Empty input should result in failure (EOF).
    char buf[10];
    prepare_input("");
    ASSERT_EQ(EOF, my_scanf("%L", buf), "L19_EOFFail");
}

void test_L_suppressed(void) {
    // Test: Assignment suppression (%*L) consumes line but stores nothing.
    char buf[10] = "UNCHANGED";
    prepare_input("Skip me\n");
    ASSERT_EQ(0, my_scanf("%*L"), "L20_SuppressRet");
    ASSERT_STREQ("UNCHANGED", buf, "L21_NoWrite");
}

void test_L_flow_multiple_lines(void) {
    // Test: Reading two lines sequentially.
    char a[10], b[10];
    prepare_input("One\nTwo\n");
    ASSERT_EQ(2, my_scanf("%L %L", a, b), "L22_FlowCount");
    ASSERT_STREQ("One", a, "L23_A");
    ASSERT_STREQ("Two", b, "L24_B");
}

void test_L_mixed_with_int(void) {
    // Test: Mixing line reading with integer parsing.
    char buf[20];
    int x;
    prepare_input("Line here\n42");
    ASSERT_EQ(2, my_scanf("%L %d", buf, &x), "L25_MixedRet");
    ASSERT_STREQ("Line here", buf, "L26_Line");
    ASSERT_EQ(42, x, "L27_Int");
}

void test_L_only_spaces(void) {
    // Test: String of spaces followed by newline results in empty string.
    char buf[10];
    prepare_input("     \n");
    ASSERT_EQ(1, my_scanf("%L", buf), "L28_Ret");
    ASSERT_STREQ("", buf, "L29_Empty");
}

void test_L_width_zero(void) {
    // Test: Field width of 0 results in an empty string.
    char buf[10];
    prepare_input("ABC\n");
    ASSERT_EQ(1, my_scanf("%0L", buf), "L30_Ret");
    ASSERT_STREQ("", buf, "L31_Empty");
}

void test_L_ignore_modifier(void) {
    // Test: Modifiers like 'l' should not break the custom %L logic.
    char buf[20];
    prepare_input("Hello\n");
    ASSERT_EQ(1, my_scanf("%lL", buf), "L32_ModRet");
    ASSERT_STREQ("Hello", buf, "L33_ModVal");
}

void test_L_single_newline(void) {
    // Test: Input consisting only of a newline results in empty string.
    char buf[10];
    prepare_input("\n");
    ASSERT_EQ(1, my_scanf("%L", buf), "L34_Ret");
    ASSERT_STREQ("", buf, "L35_Empty");
}

void test_L_literal_after(void) {
    // Test: Matching a literal character immediately after a line.
    char buf[10];
    char c;
    prepare_input("Hi\nX");
    ASSERT_EQ(2, my_scanf("%L%c", buf, &c), "L36_Ret");
    ASSERT_STREQ("Hi", buf, "L37_Val");
    ASSERT_EQ('\n', c, "L38_Literal");
}

void test_L_long_width_flow(void) {
    // Test: Sequential reads when the first line is cut by width.
    char a[10], b[10];
    prepare_input("ABCDEFGHIJK\nXYZ\n");
    ASSERT_EQ(2, my_scanf("%5L %L", a, b), "L39_Ret");
    ASSERT_STREQ("ABCDE", a, "L40_A");
    ASSERT_STREQ("FGHIJK", b, "L41_B");
}

void test_L_many_calls(void) {
    // Test: Multiple independent calls to my_scanf for different lines.
    char buf[5];
    prepare_input("A\nB\nC\n");

    my_scanf("%L", buf);
    ASSERT_STREQ("A", buf, "L42_A");

    my_scanf("%L", buf);
    ASSERT_STREQ("B", buf, "L43_B");

    my_scanf("%L", buf);
    ASSERT_STREQ("C", buf, "L44_C");
}

/* =========================================================================
 * DATE TESTS (%D) - Custom Specifier
 * Format: DD/MM/YYYY or DD-MM-YYYY
 * ========================================================================= */

void test_D_basic_slash(void) {
    // Test: Basic date with slashes.
    Date d;
    prepare_input("12/05/2024");
    ASSERT_EQ(1, my_scanf("%D", &d), "D01_Ret");
    ASSERT_EQ(12, d.day,   "D02_Day");
    ASSERT_EQ(5, d.month,  "D03_Month");
    ASSERT_EQ(2024, d.year,"D04_Year");
}

void test_D_basic_dash(void) {
    // Test: Basic date with dashes.
    Date d;
    prepare_input("01-01-2000");
    ASSERT_EQ(1, my_scanf("%D", &d), "D05_Ret");
    ASSERT_EQ(1, d.day,   "D06_Day");
    ASSERT_EQ(1, d.month, "D07_Month");
    ASSERT_EQ(2000, d.year,"D08_Year");
}

void test_D_leading_whitespace(void) {
    // Test: Skips leading whitespace before the date starts.
    Date d;
    prepare_input("   \n\t15/08/1999");
    ASSERT_EQ(1, my_scanf("%D", &d), "D09_Ret");
    ASSERT_EQ(15, d.day, "D10_Day");
}

void test_D_mixed_separators_fail(void) {
    // Test: Fail if separators are mixed (e.g., 12-05/2024).
    Date d;
    prepare_input("12-05/2024\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D11_MixedFail");
}

void test_D_missing_separator(void) {
    // Test: Fail if separators are missing entirely.
    Date d;
    prepare_input("12052024\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D12_NoSep");
}

void test_D_invalid_month(void) {
    // Test: Fail on invalid month > 12.
    Date d;
    prepare_input("10/13/2023\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D13_BadMonth");
}

void test_D_day_zero(void) {
    // Test: Fail if day is 0.
    Date d;
    prepare_input("0/10/2023\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D14_DayZero");
}

void test_D_day_overflow(void) {
    // Test: Fail if day > 31 (generic check).
    Date d;
    prepare_input("32/01/2023\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D15_DayOverflow");
}

void test_D_april_31(void) {
    // Test: Fail if day > 30 for April.
    Date d;
    prepare_input("31/04/2023\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D16_April31");
}

void test_D_feb29_non_leap(void) {
    // Test: Fail Feb 29 on non-leap year.
    Date d;
    prepare_input("29/02/2023\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D17_Feb29Fail");
}

void test_D_feb29_leap(void) {
    // Test: Accept Feb 29 on a leap year (2024).
    Date d;
    prepare_input("29/02/2024");
    ASSERT_EQ(1, my_scanf("%D", &d), "D18_Feb29OK");
    ASSERT_EQ(29, d.day, "D19_Day");
}

void test_D_1900_not_leap(void) {
    // Test: Fail Feb 29 on 1900 (divisible by 100 but not 400).
    Date d;
    prepare_input("29/02/1900\n");
    ASSERT_EQ(0, my_scanf("%D", &d), "D20_1900Fail");
}

void test_D_2000_leap(void) {
    // Test: Accept Feb 29 on 2000 (divisible by 400).
    Date d;
    prepare_input("29/02/2000");
    ASSERT_EQ(1, my_scanf("%D", &d), "D21_2000OK");
}

void test_D_short_year(void) {
    // Test: Accept short year digits (e.g., year 5).
    Date d;
    prepare_input("1/1/5");
    ASSERT_EQ(1, my_scanf("%D", &d), "D22_ShortYear");
    ASSERT_EQ(5, d.year, "D23_Year");
}

void test_D_letters_fail(void) {
    // Test: Fail if date contains letters.
    Date d;
    prepare_input("aa/bb/cccc");
    ASSERT_EQ(0, my_scanf("%D", &d), "D24_LettersFail");
}

void test_D_width_cut_month(void) {
    // Test: Width limit cuts parsing mid-month.
    // "12/3" (4 chars) is incomplete -> Fail.
    Date d;
    prepare_input("12/34/2020");
    ASSERT_EQ(0, my_scanf("%4D", &d), "D25_WidthCut");
}

void test_D_stopper_left(void) {
    // Test: Reading stops exactly after the date, leaving suffix.
    Date d;
    char next;
    prepare_input("12/12/2020X");
    my_scanf("%D", &d);
    my_scanf("%c", &next);
    ASSERT_EQ('X', next, "D28_Stopper");
}

void test_D_suppressed(void) {
    // Test: Suppression (*) consumes date but returns 0 matches.
    prepare_input("10/10/2010");
    ASSERT_EQ(0, my_scanf("%*D"), "D29_Suppress");
}

void test_D_flow_multiple(void) {
    // Test: Read multiple dates sequentially.
    Date a, b;
    prepare_input("01/01/2000 31/12/2024");
    ASSERT_EQ(2, my_scanf("%D %D", &a, &b), "D30_Flow");
    ASSERT_EQ(2000, a.year, "D31_A");
    ASSERT_EQ(2024, b.year, "D32_B");
}

void test_D_mixed_with_int(void) {
    // Test: Read date followed by integer.
    Date d;
    int x;
    prepare_input("05/06/2023 99");
    ASSERT_EQ(2, my_scanf("%D %d", &d, &x), "D33_Mixed");
    ASSERT_EQ(99, x, "D34_Int");
}

void test_D_only_separators(void) {
    // Test: Fail on input containing only separators.
    Date d;
    prepare_input("//--");
    ASSERT_EQ(0, my_scanf("%D", &d), "D35_OnlySep");
}

/* --- FIRE TESTS (New) --- */

void test_D_padded_year(void) {
    // Fire Test: Handles year with leading zeros (e.g. 0001).
    Date d;
    prepare_input("01/01/0001");
    ASSERT_EQ(1, my_scanf("%D", &d), "D36_PaddedYear");
    ASSERT_EQ(1, d.year, "D37_YearVal");
}

void test_D_format_space(void) {
    // Fire Test: Format string with spaces should consume date's trailing space.
    Date d;
    char c;
    prepare_input("01/01/2020 X");
    // "%D " should eat the space after 2020.
    my_scanf("%D %c", &d, &c);
    ASSERT_EQ('X', c, "D38_FormatSpace");
}

void test_D_complex_suppress(void) {
    // Fire Test: Suppress date, read int.
    int x;
    prepare_input("01/01/2000 500");
    // Skip date, read 500
    ASSERT_EQ(1, my_scanf("%*D %d", &x), "D39_SkipDate");
    ASSERT_EQ(500, x, "D40_Val");
}

/* =========================================================================
 * COLOR TESTS (%R) - Custom Specifier
 * Format: #RRGGBB (Hexadecimal)
 * ========================================================================= */

void test_R_basic(void) {
    // Test: Standard color input.
    RGBColor c;
    prepare_input("#FFA07A");
    ASSERT_EQ(1, my_scanf("%R", &c), "R01_Ret");
    ASSERT_EQ(255, c.r, "R02_R");
    ASSERT_EQ(160, c.g, "R03_G");
    ASSERT_EQ(122, c.b, "R04_B");
}

void test_R_lowercase(void) {
    // Test: Lowercase hex digits.
    RGBColor c;
    prepare_input("#ff00aa");
    ASSERT_EQ(1, my_scanf("%R", &c), "R05_Ret");
    ASSERT_EQ(255, c.r, "R06_R");
    ASSERT_EQ(0,   c.g, "R07_G");
    ASSERT_EQ(170, c.b, "R08_B");
}

void test_R_mixed_case(void) {
    // Test: Mixed upper/lower case.
    RGBColor c;
    prepare_input("#AaBbCc");
    ASSERT_EQ(1, my_scanf("%R", &c), "R09_Ret");
    ASSERT_EQ(170, c.r, "R10_R");
    ASSERT_EQ(187, c.g, "R11_G");
    ASSERT_EQ(204, c.b, "R12_B");
}

void test_R_black(void) {
    // Test: Boundary (0).
    RGBColor c;
    prepare_input("#000000");
    ASSERT_EQ(1, my_scanf("%R", &c), "R13_Black");
    ASSERT_EQ(0, c.r, "R13_R_Val");
}

void test_R_white(void) {
    // Test: Boundary (255).
    RGBColor c;
    prepare_input("#FFFFFF");
    ASSERT_EQ(1, my_scanf("%R", &c), "R14_White");
    ASSERT_EQ(255, c.r, "R14_R_Val");
}

void test_R_leading_whitespace(void) {
    // Test: Skips whitespace before the hash.
    RGBColor c;
    prepare_input("   \n\t#112233");
    ASSERT_EQ(1, my_scanf("%R", &c), "R15_WS");
    ASSERT_EQ(17, c.r, "R16_R");
}

void test_R_missing_hash(void) {
    // Test: Fail if '#' is missing.
    RGBColor c;
    char next;
    prepare_input("112233");
    ASSERT_EQ(0, my_scanf("%R", &c), "R17_NoHash");

    // Ensure the '1' was NOT consumed (rollback/peek)
    my_scanf("%c", &next);
    ASSERT_EQ('1', next, "R18_Rollback");
}

void test_R_only_hash(void) {
    // Test: Input ends immediately after hash.
    RGBColor c;
    prepare_input("#");
    ASSERT_EQ(EOF, my_scanf("%R", &c), "R19_OnlyHash");
}

void test_R_invalid_first_nibble(void) {
    // Test: Non-hex char in first position.
    RGBColor c;
    prepare_input("#G01234");
    ASSERT_EQ(0, my_scanf("%R", &c), "R20_InvalidFirst");
}

void test_R_invalid_second_nibble(void) {
    // Test: Non-hex char in second position of a pair.
    RGBColor c;
    char next;
    prepare_input("#FZ1122");

    ASSERT_EQ(0, my_scanf("%R", &c), "R21_InvalidSecond");

    // Based on your read_hex_pair logic:
    // 'F' is consumed, 'Z' fails and is pushed back.
    my_scanf("%c", &next);
    ASSERT_EQ('Z', next, "R22_StopperChar");
}

void test_R_too_short(void) {
    // Test: Not enough digits.
    RGBColor c;
    prepare_input("#1234");
    ASSERT_EQ(EOF, my_scanf("%R", &c), "R23_TooShort");
}

void test_R_extra_digits(void) {
    // Test: Correctly stops after 6 digits.
    RGBColor c;
    char next;
    prepare_input("#11223344");

    ASSERT_EQ(1, my_scanf("%R", &c), "R24_ExtraOK");
    my_scanf("%c", &next);
    ASSERT_EQ('4', next, "R25_Stopper");
}

void test_R_width_exact(void) {
    // Test: Exact width (1 hash + 6 digits = 7).
    RGBColor c;
    prepare_input("#AABBCCX");
    ASSERT_EQ(1, my_scanf("%7R", &c), "R26_WidthExact");
}

void test_R_width_too_small(void) {
    // Test: Width 6 cuts off the last digit -> Fail.
    RGBColor c;
    prepare_input("#AABBCC");
    ASSERT_EQ(0, my_scanf("%6R", &c), "R27_WidthFail");
}

void test_R_width_mid_component(void) {
    // Test: Width cuts off in the middle of a pair.
    RGBColor c;
    prepare_input("#AAB");
    ASSERT_EQ(0, my_scanf("%4R", &c), "R28_WidthMid");
}

void test_R_eof_after_hash(void) {
    // Test: EOF right after hash.
    RGBColor c;
    prepare_input("#");
    ASSERT_EQ(EOF, my_scanf("%R", &c), "R29_EOF");
}

void test_R_suppressed(void) {
    // Test: Assignment suppression.
    prepare_input("#ABCDEF");
    ASSERT_EQ(0, my_scanf("%*R"), "R30_Suppress");
}

void test_R_flow(void) {
    // Test: Sequential reads.
    RGBColor a, b;
    prepare_input("#000000 #FFFFFF");
    ASSERT_EQ(2, my_scanf("%R %R", &a, &b), "R31_Flow");
    ASSERT_EQ(255, b.r, "R32_Second");
}

void test_R_mixed_with_int(void) {
    // Test: Mix with integer.
    RGBColor c;
    int x;
    prepare_input("#010203 42");
    ASSERT_EQ(2, my_scanf("%R %d", &c, &x), "R33_Mixed");
    ASSERT_EQ(42, x, "R34_Int");
}

void test_R_space_after_hash(void) {
    // Test: Strict format (no space allowed after hash).
    RGBColor c;
    prepare_input("# 112233");
    ASSERT_EQ(0, my_scanf("%R", &c), "R35_SpaceAfterHash");
}

/* --- FIRE TESTS (Added) --- */

void test_R_literal_match_prefix(void) {
    // Fire Test: Matching literal text before color.
    RGBColor c;
    prepare_input("Color:#112233");
    // "Color:" matches literal, then %R matches hash
    ASSERT_EQ(1, my_scanf("Color:%R", &c), "R36_LiteralPrefix");
    ASSERT_EQ(17, c.r, "R37_Val");
}

void test_R_suppress_complex(void) {
    // Fire Test: Suppress color, read int, read color.
    int x;
    RGBColor c;
    prepare_input("#AAAAAA 100 #BBBBBB");

    // Skip AAAAAA, read 100, read BBBBBB
    ASSERT_EQ(2, my_scanf("%*R %d %R", &x, &c), "R38_SuppressComplex");
    ASSERT_EQ(100, x, "R39_IntVal");
    ASSERT_EQ(187, c.r, "R40_ColorVal");
}

/* =========================================================================
 * MAIN RUNNER
 * ========================================================================= */

int main(void) {
    printf("🚀 STARTING EXTENDED INTEGER TESTS...\n\n");

    test_d_basic_positive();
    test_d_basic_negative();
    test_d_zero();
    test_d_leading_whitespace();
    test_d_plus_sign();
    test_d_minus_only_fail();
    test_d_width_basic();
    test_d_width_truncation();
    test_d_width_sign_only_fail();
    test_d_stops_at_nondigit();
    test_d_invalid_input();
    test_d_assignment_suppressed();
    test_d_weird_double_sign();
    test_d_space_between_sign_and_digit();
    test_d_leading_zeros_decimal();
    test_d_stop_at_hex_x();
    test_d_limits_32bit();
    test_d_multiple_reads_flow();
    test_d_width_exact_match();
    test_d_width_modifier_combo();
    test_d_suppression_width();
    test_modifiers_lld();
    test_modifiers_hd();

    test_c_basic_letter();
    test_c_basic_digit();
    test_c_basic_symbol();
    test_c_reads_space();
    test_c_reads_tab();
    test_c_reads_newline();
    test_c_after_d_reads_space();
    test_c_after_d_reads_letter();
    test_c_default_width_is_one();
    test_c_width_two_reads_exact();
    test_c_width_does_not_null_terminate();
    test_c_width_fail_on_eof();
    test_c_single_char_eof_fail();
    test_c_suppression_consumes_char();
    test_c_suppression_only();
    test_c_multiple_reads_flow();
    test_c_interleaved_with_string();
    test_c_symbol_after_newline();
    test_c_width_three_with_spaces();
    test_c_skip_whitespace_directive();
    test_c_suppression_width_combo();
    test_c_ignore_modifiers();

    test_s_basic_word();
    test_s_single_character();
    test_s_skips_leading_spaces();
    test_s_skips_tabs_and_newlines();
    test_s_stops_at_space();
    test_s_stops_at_tab();
    test_s_stops_at_newline();
    test_s_width_basic();
    test_s_width_exact_match();
    test_s_width_smaller_than_word();
    test_s_null_terminated();
    test_s_empty_input_fail();
    test_s_only_whitespace_fail();
    test_s_suppression_consumes_word();
    test_s_suppression_only();
    test_s_multiple_reads();
    test_s_punctuation();
    test_s_number_string();
    test_s_width_then_space();
    test_s_split_word_by_width();
    test_s_ignore_modifiers();
    test_s_suppression_and_width();
    test_s_format_string_space();
    test_s_width_one();
    test_s_literal_match();
    test_s_eof_no_newline();

    test_x_basic_lowercase();
    test_x_basic_uppercase();
    test_x_basic_digit();
    test_x_leading_whitespace();
    test_x_prefix_lowercase();
    test_x_prefix_uppercase();
    test_x_prefix_without_digits_fail();
    test_x_prefix_invalid_digit();
    test_x_negative();
    test_x_positive_sign();
    test_x_sign_only_fail();
    test_x_width_basic();
    test_x_width_with_prefix();
    test_x_width_blocks_prefix();
    test_x_width_sign_only_fail();
    test_x_width_exact_match();
    test_x_stops_at_nondigit();
    test_x_invalid_letter_fail();
    test_x_only_whitespace_fail();
    test_x_leading_zeros();
    test_x_suppression_consumes();
    test_x_suppression_only();
    test_x_multiple_reads();
    test_x_max_uint();
    test_x_sign_and_prefix();
    test_x_mixed_case_prefix();
    test_x_ignore_modifiers();
    test_x_zero_input();

    test_f_basic_integer();
    test_f_basic_fraction();
    test_f_leading_whitespace();
    test_f_negative();
    test_f_plus_sign();
    test_f_leading_dot();
    test_f_trailing_dot();
    test_f_dot_only_fail();
    test_f_sign_only_fail();
    test_f_exponent_positive();
    test_f_exponent_negative();
    test_f_exponent_plus_sign();
    test_f_invalid_exponent_rollback_e();
    test_f_invalid_exponent_rollback_e_plus();
    test_f_width_basic();
    test_f_width_blocks_fraction();
    test_f_width_blocks_exponent();
    test_f_stops_at_letter();
    test_f_multiple_reads();
    test_f_uppercase_E();
    test_f_leading_zeros_scientific();
    test_f_weird_width_cut_mid_exp();
    test_f_suppression();
    test_f_ignore_l_modifier();
    test_f_valid_dot_exponent();

    test_b_basic_zero();
    test_b_basic_one();
    test_b_basic_multi_bits();
    test_b_leading_whitespace();
    test_b_stops_at_letter();
    test_b_stops_at_digit_two();
    test_b_invalid_letter_fail();
    test_b_invalid_digit_fail();
    test_b_only_whitespace_fail();
    test_b_width_basic();
    test_b_width_exact_match();
    test_b_width_blocks_extra_bits();
    test_b_width_eof_fail();
    test_b_leading_zeros();
    test_b_suppression_consumes();
    test_b_suppression_only();
    test_b_multiple_reads();
    test_b_zero_then_letter();
    test_b_max_64bit();
    test_b_negative_binary();
    test_b_suppress_with_width();
    test_b_modifier_ignore();

    test_L_basic_line();
    test_L_skip_leading_whitespace();
    test_L_empty_line_after_ws();
    test_L_newline_left_in_buffer();
    test_L_width_cut();
    test_L_width_exact();
    test_L_width_one();
    test_L_width_newline_preserved();
    test_L_eof_immediate();
    test_L_suppressed();
    test_L_flow_multiple_lines();
    test_L_mixed_with_int();
    test_L_only_spaces();
    test_L_width_zero();
    test_L_ignore_modifier();
    test_L_single_newline();
    test_L_literal_after();
    test_L_long_width_flow();
    test_L_many_calls();

    test_D_basic_slash();
    test_D_basic_dash();
    test_D_leading_whitespace();
    test_D_mixed_separators_fail();
    test_D_missing_separator();
    test_D_invalid_month();
    test_D_day_zero();
    test_D_day_overflow();
    test_D_april_31();
    test_D_feb29_non_leap();
    test_D_feb29_leap();
    test_D_1900_not_leap();
    test_D_2000_leap();
    test_D_short_year();
    test_D_letters_fail();
    test_D_width_cut_month();
    test_D_stopper_left();
    test_D_suppressed();
    test_D_flow_multiple();
    test_D_mixed_with_int();
    test_D_only_separators();
    test_D_padded_year();
    test_D_format_space();
    test_D_complex_suppress();

    test_R_basic();
    test_R_lowercase();
    test_R_mixed_case();
    test_R_black();
    test_R_white();
    test_R_leading_whitespace();
    test_R_missing_hash();
    test_R_only_hash();
    test_R_invalid_first_nibble();
    test_R_invalid_second_nibble();
    test_R_too_short();
    test_R_extra_digits();
    test_R_width_exact();
    test_R_width_too_small();
    test_R_width_mid_component();
    test_R_eof_after_hash();
    test_R_suppressed();
    test_R_flow();
    test_R_mixed_with_int();
    test_R_space_after_hash();
    test_R_literal_match_prefix();
    test_R_suppress_complex();

    printf("\n========================================\n");
    printf("TESTS RUN: %d\n", tests_run);

    if (tests_failed == 0) {
        printf("RESULT: ALL CLEAR! (100%%) \n");
    } else {
        printf("RESULT: %d TESTS FAILED \n", tests_failed);
    }
    printf("========================================\n");

    remove("test_input.tmp");
    return tests_failed != 0;
}