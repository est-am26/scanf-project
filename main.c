#include <stdio.h>
#include "my_scanf.h"

int main() {
    RGBColor color;
    int items;

    printf("--- TEST: RGB Color Extension (%%R) ---\n");
    printf("Rule: Format must be #RRGGBB (e.g., #FF5733)\n\n");

    printf("Enter a color: ");
    items = my_scanf("%R", &color);

    if (items == 1) {
        printf("\n✅ Success!\n");
        printf("R: %d (Red)\n", color.r);
        printf("G: %d (Green)\n", color.g);
        printf("B: %d (Blue)\n", color.b);

        // Bonus: Mostrarlo en formato decimal para CSS
        printf("CSS format: rgb(%d, %d, %d)\n", color.r, color.g, color.b);
    } else {
        printf("\n❌ Matching Failure! The input was not a valid color.\n");
    }

    return 0;
}