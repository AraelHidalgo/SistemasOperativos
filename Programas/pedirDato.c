#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {

    char mensaje[50];
    char nombre[50];

    pid_t pid = getpid();
    pid_t ppid = getppid();

    if (argc != 3){  
        printf("Uso: %s <Saludo> <nombre>\n", argv[0]);
        return 1;
    }

    strcpy(mensaje, argv[1]);
    strcpy(nombre, argv[2]);

    printf("PID: %d\n", pid);
    printf("PPID: %d\n", ppid);

    printf("Saludo: %s\n", mensaje);
    printf("Nombre: %s\n", nombre);
    
    int edad;
    printf("Dame tu edad: ");
    scanf("%d", &edad);
    printf("El numero introducido es: %d\n", edad);
    return 0;
}