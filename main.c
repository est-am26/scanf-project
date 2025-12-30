#include <stdio.h>
#include "my_scanf.h"

int main() {
    int edad;
    char inicial;
    char nombre[50];

    printf("Escribe tu nombre: ");
    my_scanf("%s", nombre);

    printf("Escribe tu inicial: ");
    my_scanf("%c", &inicial); // Nota: a veces hay que limpiar el buffer antes

    printf("Escribe tu edad: ");
    my_scanf("%d", &edad);

    printf("\n--- RESULTADOS ---\n");
    printf("Nombre: %s\n", nombre);
    printf("Inicial: %c\n", inicial);
    printf("Edad: %d\n", edad);

    return 0;
}