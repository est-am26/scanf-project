#include <stdio.h>
#include "my_scanf.h"

int main() {
    int edad = 0;

    printf("Por favor, escribe tu edad: ");

    // Llamamos a TU función my_scanf
    my_scanf("%d", &edad);

    printf("¡Increíble! Tienes %d años.\n", edad);

    return 0;
}