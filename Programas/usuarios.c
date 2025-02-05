#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct usuario {
    char nombre[50];
    char apellidoPaterno[50];
    char apellidoMaterno[50];
    int edad;
    char direccion[100];
};

int main (int argc, char *argv[]) {

    struct usuario Usuario1 = {
        .edad = atoi(argv[4])
    };

    if (argc != 6){  
        printf("Uso: %s <Nombre> <ApellidoPaterno> <ApellidoMaterno> <Edad> <Direccion>\n", argv[0]);
        return 1;
    }

    strcpy(Usuario1.nombre, argv[1]);
    strcpy(Usuario1.apellidoPaterno, argv[2]);
    strcpy(Usuario1.apellidoMaterno, argv[3]);
    strcpy(Usuario1.direccion, argv[5]);

    printf("Datos del Alumno:\n");
    printf("Nombre: %s\n", Usuario1.nombre);
    printf("Apellido Paterno: %s\n", Usuario1.apellidoPaterno);
    printf("Apellido Materno: %s\n", Usuario1.apellidoMaterno);
    printf("Edad: %d\n", Usuario1.edad);
    printf("Direccion: %s\n", Usuario1.direccion);

    //Pedir un numero
    int numero;
    printf("Introduce un numero: ");
    scanf("%d", &numero);
    printf("El numero introducido es: %d\n", numero);


    return 0;
}