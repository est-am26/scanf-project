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
    prepare_input("-");
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
    prepare_input("-5");
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

/* =========================================================================
 * MODIFIER TESTS (%lld, %hd)
 * ========================================================================= */

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
    // FIX: Use width limit %5s so it stops at "hello" and leaves "X" for %c.
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
    char c = 'Z'; // Valor basura inicial
    prepare_input("   A");

    // Notar el espacio antes del %c
    ASSERT_EQ(1, my_scanf(" %c", &c), "C_SkipSpace_Ret");
    ASSERT_EQ('A', c, "C_SkipSpace_Val");
}

void test_c_suppression_width_combo(void) {
    // Fire Test: Consume exactly 3 chars, store nothing.
    char c = 0;
    prepare_input("123X");

    // %*3c se come "123". El siguiente %c debe leer 'X'.
    // Retorna 1 porque solo %c cuenta (la supresión no).
    ASSERT_EQ(1, my_scanf("%*3c%c", &c), "C_SuppressWidth_Ret");
    ASSERT_EQ('X', c, "C_SuppressWidth_Val");
}

void test_c_ignore_modifiers(void) {
    // Fire Test: Modifiers like 'h' or 'l' on %c are usually ignored
    // in simple implementations (or denote wide chars), but shouldn't crash.
    char c = 0;
    prepare_input("M");

    // Tu parser leerá el 'h', verá que es modificador, y luego leerá 'c'.
    // Debe funcionar igual que %c normal.
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
    ASSERT_EQ(0, my_scanf("%s", buf), "S15_EmptyFail");
}

void test_s_only_whitespace_fail(void) {
    // Test: Fails if input is only whitespace (and then EOF).
    char buf[10];
    prepare_input("   \t\n");
    ASSERT_EQ(0, my_scanf("%s", buf), "S16_WhitespaceFail");
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

    printf("\n========================================\n");
    printf("TESTS RUN: %d\n", tests_run);

    if (tests_failed == 0) {
        printf("RESULT: ALL CLEAR! (100%%) 🏆\n");
    } else {
        printf("RESULT: %d TESTS FAILED ⚠️\n", tests_failed);
    }
    printf("========================================\n");

    remove("test_input.tmp");
    return tests_failed != 0;
}