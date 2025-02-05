#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

int fibonacciRecursivo(int numeroFibonacci) {
    if (numeroFibonacci <= 1) {
        return numeroFibonacci;
    }
    return fibonacciRecursivo(numeroFibonacci - 1) + fibonacciRecursivo(numeroFibonacci - 2);
}

int esCadena(const char *str) {
    while (*str) {
        if (!isalpha(*str) && !isdigit(*str)) {
            return 0; // No es una cadena
        } 
        str++;
    }
    return 1; // Es una cadena
}

// (7-1) + (7-2) = 6 + 5 = 11
// (6-1) + (6-2) = 5 + 4 = 9
// (5-1) + (5-2) = 4 + 3 = 7
// (4-1) + (4-2) = 3 + 2 = 5
// (3-1) + (3-2) = 2 + 1 = 3
// (2-1) + (2-2) = 1 + 0 = 1
// (1-1) + (1-2) = 0 + 1 = 1
// (0) + (1) = 1
int main(int argc, char *argv[]) {

    char mensaje[50];
    int numeroFibonacci;
    int numeroRepeticiones;

    pid_t id = fork();

    if (argc != 4){  
        printf("Uso: %s <Saludo> <numerofibo> <numeroRepeticiones>\n", argv[0]);
        return 1;
    } else if(!esCadena(argv[1])) {
        printf("El primer argumento debe ser una cadena de texto\n");
        return 1;
    }

    strcpy(mensaje, argv[1]);
    numeroRepeticiones = atoi(argv[2]);
    numeroFibonacci = atoi(argv[3]);

    if (id == -1) {
        printf("Error al crear el proceso");
        return 1;
    } else if (id == 0) {
        for (int i = 0; i < numeroRepeticiones; i++) {
            printf("%s\n", mensaje);
        }
        printf("Soy el proceso hijo. Mi PID es: %d\n", getppid());
        sleep(5);
        printf("Hijo terminado.\n");

    } else {
         for (int i = 0; i < numeroFibonacci; i++) {
            printf("%d ", fibonacciRecursivo(i));
        }
        printf("Soy el proceso padre. Mi PID es: %d\n", getpid());
        printf("Esperando a que el hijo termine...\n");
        wait(NULL);
        printf("Padre terminado.\n");
    }

    printf("Saludo: %s\n", mensaje);
    
}