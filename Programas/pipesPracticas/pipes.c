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

// Función para leer una cadena desde stdin usando read
void leerCadena(char *buffer, int longitud) {
    read(STDIN_FILENO, buffer, longitud);
    buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar el salto de línea
}

// Función para leer un entero desde stdin usando read
int leerEntero() {
    char buffer[20];
    read(STDIN_FILENO, buffer, sizeof(buffer));
    return atoi(buffer); // Convertir la cadena a entero
}

int main() {
    int fd[2];
    pid_t pid;
    struct persona personas[5];

    //printf("Ingrese los datos de 5 personas:\n");
        // Mensaje inicial
        write(STDOUT_FILENO, "Ingrese los datos de 5 personas:\n", 34);
    /*
     for (int i = 0; i < 5; i++) {
        printf("\nPersona %d:\n", i+1);
        printf("Nombre: ");
        scanf("%s", personas[i].nombre);
        printf("CURP: ");
        scanf("%s", personas[i].curp);
        printf("Edad: ");
        scanf("%d", &personas[i].edad);
    }
        */
       
    for (int i = 0; i < 5; i++) {
        char mensaje[50];
        snprintf(mensaje, sizeof(mensaje), "\nPersona %d:\n", i + 1);
        write(STDOUT_FILENO, mensaje, strlen(mensaje));

        write(STDOUT_FILENO, "Nombre: ", 8);
        leerCadena(personas[i].nombre, sizeof(personas[i].nombre));

        write(STDOUT_FILENO, "CURP: ", 6);
        leerCadena(personas[i].curp, sizeof(personas[i].curp));

        write(STDOUT_FILENO, "Edad: ", 6);
        personas[i].edad = leerEntero();
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

        char procesoPadre[100];
        snprintf(procesoPadre, sizeof(procesoPadre), "Proceso padre (PID: %d, PPID: %d) enviando datos...\n", getpid(), getppid());
        write(STDOUT_FILENO, procesoPadre, strlen(procesoPadre));

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

        char procesoHijo[100];
        snprintf(procesoHijo, sizeof(procesoHijo), "Proceso hijo (PID: %d, PPID: %d) recibiendo datos...\n", getpid(), getppid());
        write(STDOUT_FILENO, procesoHijo, strlen(procesoHijo));
        struct persona recibidas[5];
        ssize_t bytesLeidos = read(fd[0], recibidas, sizeof(recibidas));
        if (bytesLeidos <= 0) {
            perror("Error al leer del pipe");
            exit(EXIT_FAILURE);
        }
        close(fd[0]);  // Cerrar lectura

        int opcion;
        do {
            /*
            printf("\nMenú:\n");
            printf("1. Suma total de edades\n");
            printf("2. Persona más grande\n");
            printf("3. Persona más pequeña\n");
            printf("4. Salir\n");
            printf("Ingrese una opción: ");
            scanf("%d", &opcion);
            */
            

            write(STDOUT_FILENO, "\nMenú:\n", 9);
            write(STDOUT_FILENO, "1. Suma total de edades\n", 25);
            write(STDOUT_FILENO, "2. Persona más grande\n", 24);
            write(STDOUT_FILENO, "3. Persona más pequeña\n", 26);
            write(STDOUT_FILENO, "4. Salir\n", 12);
            write(STDOUT_FILENO, "Ingrese una opción: ", 22);

            char buffer[20];
            read(STDIN_FILENO, buffer, sizeof(buffer));
            opcion = atoi(buffer);
            
            switch (opcion) {
                case 1: {
                    int suma = 0;
                    for (int i = 0; i < 5; i++) {
                        suma += recibidas[i].edad;
                    }
                    //printf("La suma total de edades es: %d\n", suma);
                    char resultado[50];
                    snprintf(resultado, sizeof(resultado), "La suma total de edades es: %d\n", suma);
                    write(STDOUT_FILENO, resultado, strlen(resultado));
                    break;
                
                }
                case 2: {
                    //Persona mayor
                    int maxIdx = 0;
                    for (int i = 1; i < 5; i++) {
                        if (recibidas[i].edad > recibidas[maxIdx].edad)
                            maxIdx = i;
                    }
                    //printf("La persona más grande es:\n");
                    //printf("Nombre: %s\n", recibidas[maxIdx].nombre);
                    //printf("CURP: %s\n", recibidas[maxIdx].curp);
                    //printf("Edad: %d\n", recibidas[maxIdx].edad);
                    char resultado[100];
                    snprintf(resultado, sizeof(resultado),
                             "La persona más grande es:\nNombre: %s\nCURP: %s\nEdad: %d\n",
                             recibidas[maxIdx].nombre, recibidas[maxIdx].curp, recibidas[maxIdx].edad);
                    write(STDOUT_FILENO, resultado, strlen(resultado));
                    break;
                   
                }
                case 3: {
                    // Menor
                    int minIdx = 0;
                    for (int i = 1; i < 5; i++) {
                        if (recibidas[i].edad < recibidas[minIdx].edad)
                            minIdx = i;
                    }
                    /*
                    printf("La persona más pequeña es:\n");
                    printf("Nombre: %s\n", recibidas[minIdx].nombre);
                    printf("CURP: %s\n", recibidas[minIdx].curp);
                    printf("Edad: %d\n", recibidas[minIdx].edad);
                    break;
                    */
                    
                    char resultado[100];
                    snprintf(resultado, sizeof(resultado),
                             "La persona más pequeña es:\nNombre: %s\nCURP: %s\nEdad: %d\n",
                             recibidas[minIdx].nombre, recibidas[minIdx].curp, recibidas[minIdx].edad);
                    write(STDOUT_FILENO, resultado, strlen(resultado));
                    break;
                }
                case 4:
                    write(STDOUT_FILENO, "Saliendo...\n", 12);
                    break;
                default:
                    write(STDOUT_FILENO, "Opción no válida. Intente de nuevo.\n", 36);
                    break;
            }
        } while (opcion != 4);
    }

    return 0;
}
