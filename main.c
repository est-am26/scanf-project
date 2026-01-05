#include <stdio.h>
#include "my_scanf.h"

int main(void) {
    int num = 0;
    char letra = ' ';
    char palabra[50] = {0};

    printf("=== DEMO DE MY_SCANF ===\n");

    // 1. Probar Entero
    printf("1. Escribe un numero entero (ej: -123): ");
    if (my_scanf("%d", &num)) {
        printf("   -> Leido: %d\n", num);
    } else {
        printf("   -> Error leyendo numero.\n");
    }

    // Limpiar buffer (como fflush stdin casero) para la siguiente lectura
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // 2. Probar Caracter
    printf("\n2. Escribe una letra (ej: Z): ");
    if (my_scanf("%c", &letra)) {
        printf("   -> Leido: '%c'\n", letra);
    }

    while ((c = getchar()) != '\n' && c != EOF); // Limpiar buffer otra vez

    // 3. Probar String
    printf("\n3. Escribe una palabra (ej: Hola): ");
    if (my_scanf("%s", palabra)) {
        printf("   -> Leido: \"%s\"\n", palabra);
    }

    printf("\n=== FIN DE LA DEMO ===\n");
    return 0;
}