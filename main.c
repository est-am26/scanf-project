#include <stdio.h>
#include "my_scanf.h"

int main() {
    int option;

    // Variables for testing
    int i_val;
    unsigned int x_val;
    double f_val;
    char c_val;
    char s_val[100];
    char l_val[200];
    unsigned long long b_val;
    Date d_val;
    RGBColor r_val;

    printf("========================================\n");
    printf("      SUPER SCANF: THE FULL DEMO        \n");
    printf("========================================\n");

    while(1) {
        printf("\n------------- MENU -------------\n");
        printf("STANDARD SPECIFIERS:\n");
        printf("1. Integer (%%d)\n");
        printf("2. Hexadecimal (%%x)\n");
        printf("3. Float (%%lf)\n");
        printf("4. Character (%%c)\n");
        printf("5. String (%%s)\n");
        printf("\nCUSTOM EXTENSIONS:\n");
        printf("6. Binary (%%b)\n");
        printf("7. Full Line (%%L)\n");
        printf("8. Date (%%D)\n");
        printf("9. Color (%%R)\n");
        printf("0. Exit\n");
        printf("--------------------------------\n");
        printf("Choose an option (0-9): ");

        // Read the menu option
        if (my_scanf("%d", &option) != 1) {
            printf("Invalid input. Clearing buffer...\n");
            char garbage;
            my_scanf("%c", &garbage); // simple flush
            continue;
        }

        if (option == 0) break;

        switch(option) {
            case 1: // %d
                printf("Enter a signed integer: ");
                if (my_scanf(" %d", &i_val)) printf("Result: %d\n", i_val);
                else printf("Error reading integer.\n");
                break;

            case 2: // %x
                printf("Enter a hex number (e.g., FF, 1A): ");
                if (my_scanf(" %x", &x_val)) printf("Result (Decimal): %u\n", x_val);
                else printf("Error reading hex.\n");
                break;

            case 3: // %f
                printf("Enter a float (e.g., 3.14, -1.2e5): ");
                if (my_scanf(" %lf", &f_val)) printf("Result: %lf\n", f_val);
                else printf("Error reading float.\n");
                break;

            case 4: // %c
                printf("Enter a single character: ");
                // Space before %c skips previous newline
                if (my_scanf(" %c", &c_val)) printf("Result: '%c'\n", c_val);
                else printf("Error reading char.\n");
                break;

            case 5: // %s
                printf("Enter a word (stops at space): ");
                if (my_scanf(" %s", s_val)) printf("Result: \"%s\"\n", s_val);
                else printf("Error reading string.\n");
                break;

            case 6: // %b (Extension)
                printf("Enter a binary number (e.g., 1010): ");
                if (my_scanf(" %b", &b_val)) printf("Result (Decimal): %llu\n", b_val);
                else printf("Error reading binary.\n");
                break;

            case 7: // %L (Extension)
                printf("Enter a full sentence (with spaces): ");
                // Space before %L is crucial here to consume the menu's newline
                if (my_scanf(" %L", l_val)) printf("Result: \"%s\"\n", l_val);
                else printf("Error reading line.\n");
                break;

            case 8: // %D (Extension)
                printf("Enter a date (DD/MM/YYYY or DD-MM-YYYY): ");
                if (my_scanf(" %D", &d_val)) {
                    printf("Result: %02d / %02d / %04d\n", d_val.day, d_val.month, d_val.year);
                } else {
                    printf("Error reading date.\n");
                }
                break;

            case 9: // %R (Extension)
                printf("Enter a color (format #RRGGBB): ");
                if (my_scanf(" %R", &r_val)) {
                    printf("Result: Red=%d, Green=%d, Blue=%d\n", r_val.r, r_val.g, r_val.b);
                } else {
                    printf("Error reading color.\n");
                }
                break;

            default:
                printf("Unknown option. Please try again.\n");
        }
    }

    printf("\nExiting demo. Great job!\n");
    return 0;
}