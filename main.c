#include <stdio.h>
#include "my_scanf.h"

int main(void) {
    int x;
    printf("Enter a number: ");
    my_scanf("%d", &x);
    printf("You entered: %d\n", x);
    return 0;
}
