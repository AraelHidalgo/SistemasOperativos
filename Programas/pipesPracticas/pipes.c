#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

struct persona {
    char nombre[20];
    char curp[18];
    int edad;
};

int main() {
    int fd[2];
    pid_t pid;
    struct persona personas[5];

    printf("Ingrese los datos de 5 personas:\n");
    for (int i = 0; i < 5; i++) {
        printf("\nPersona %d:\n", i+1);
        printf("Nombre: ");
        scanf("%s", personas[i].nombre);
        printf("CURP: ");
        scanf("%s", personas[i].curp);
        printf("Edad: ");
        scanf("%d", &personas[i].edad);
    }

 
    if (pipe(fd) == -1) {
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

    // Proceso hijo
    pid = fork();
    if (pid < 0) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Padre envia el arreglo por el pipe
        close(fd[0]);  //no se usa la lectura en el padre

        if (write(fd[1], personas, sizeof(personas)) == -1) {
            perror("Error al escribir en el pipe");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);  // Envia los datos y cierra el extremo de escritura

        // El hijo termine
        wait(NULL);
    } else {
        //hijo
        close(fd[1]);  // No se usa escritura

        struct persona recibidas[5];
        ssize_t bytesLeidos = read(fd[0], recibidas, sizeof(recibidas));
        if (bytesLeidos <= 0) {
            perror("Error al leer del pipe");
            exit(EXIT_FAILURE);
        }
        close(fd[0]);  // Cerrar lectura

        int opcion;
        do {
            printf("\nMenú:\n");
            printf("1. Suma total de edades\n");
            printf("2. Persona más grande\n");
            printf("3. Persona más pequeña\n");
            printf("4. Salir\n");
            printf("Ingrese una opción: ");
            scanf("%d", &opcion);

            switch (opcion) {
                case 1: {
                    int suma = 0;
                    for (int i = 0; i < 5; i++) {
                        suma += recibidas[i].edad;
                    }
                    printf("La suma total de edades es: %d\n", suma);
                    break;
                }
                case 2: {
                    //Persona mayor
                    int maxIdx = 0;
                    for (int i = 1; i < 5; i++) {
                        if (recibidas[i].edad > recibidas[maxIdx].edad)
                            maxIdx = i;
                    }
                    printf("La persona más grande es:\n");
                    printf("Nombre: %s\n", recibidas[maxIdx].nombre);
                    printf("CURP: %s\n", recibidas[maxIdx].curp);
                    printf("Edad: %d\n", recibidas[maxIdx].edad);
                    break;
                }
                case 3: {
                    // Menor
                    int minIdx = 0;
                    for (int i = 1; i < 5; i++) {
                        if (recibidas[i].edad < recibidas[minIdx].edad)
                            minIdx = i;
                    }
                    printf("La persona más pequeña es:\n");
                    printf("Nombre: %s\n", recibidas[minIdx].nombre);
                    printf("CURP: %s\n", recibidas[minIdx].curp);
                    printf("Edad: %d\n", recibidas[minIdx].edad);
                    break;
                }
                case 4:
                    printf("Saliendo...\n");
                    break;
                default:
                    printf("Opción no válida. Intente de nuevo.\n");
                    break;
            }
        } while (opcion != 4);
    }

    return 0;
}
