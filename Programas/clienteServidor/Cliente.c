#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void menu_consultas(int sock) {
    int opc;
    char buffer[MAX_BUFFER] = {0};
    
    do {
        printf("\n--- Menú de Consultas ---\n");
        printf("1. Consultar tabla accidente\n");
        printf("2. Consultar tabla auto\n");
        printf("3. Consultar tabla propietario\n");
        printf("4. Consultar tabla telefono\n");
        printf("0. Volver al menú principal\n");
        printf("_Dame la opcion: ");
        scanf("%d", &opc);
        limpiar_buffer();

        switch (opc) {
            case 1:
                send(sock, "CONSULTA|accidente", 18, 0);
                break;
            case 2:
                send(sock, "CONSULTA|auto", 13, 0);
                break;
            case 3:
                send(sock, "CONSULTA|propietario", 21, 0);
                break;
            case 4:
                send(sock, "CONSULTA|telefono", 17, 0);
                break;
            case 0:
                return;
            default:
                printf("Opción no válida\n");
                continue;
        }

        // Recibir y mostrar resultados
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            read(sock, buffer, MAX_BUFFER);
            if (strcmp(buffer, "END_RESULTS") == 0) break;
            printf("%s", buffer);
        }

        printf("Presiona Enter para continuar...\n");
        getchar();
    } while (opc != 0);
}

void menu_altas(int sock) {
    int opc;
    char buffer[MAX_BUFFER] = {0};
    char nombre[100], ape_paterno[100], ape_materno[100], direccion[200];
    char fecha_nacimiento[11], rfc[20], tipo[20], numero[20];
    char placas[20], color[50], auto_tipo[50], cns[20], fecha[11], hora[9], lugar[200];
    int propietario_id, modelo, auto_id;
    
    do {
        printf("\n--- Menú de Altas ---\n");
        printf("1. Alta de Propietario\n");
        printf("2. Alta de Teléfono\n");
        printf("3. Alta de Auto\n");
        printf("4. Alta de Accidente\n");
        printf("0. Volver al menú principal\n");
        printf("_Dame la opcion: ");
        scanf("%d", &opc);
        limpiar_buffer();

        switch (opc) {
            case 1: // Alta Propietario
                printf("\n--- Alta de Propietario ---\n");
                printf("Nombre: ");
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';

                printf("Apellido Paterno: ");
                fgets(ape_paterno, sizeof(ape_paterno), stdin);
                ape_paterno[strcspn(ape_paterno, "\n")] = '\0';

                printf("Apellido Materno: ");
                fgets(ape_materno, sizeof(ape_materno), stdin);
                ape_materno[strcspn(ape_materno, "\n")] = '\0';

                printf("Dirección: ");
                fgets(direccion, sizeof(direccion), stdin);
                direccion[strcspn(direccion, "\n")] = '\0';

                printf("Fecha de Nacimiento (YYYY-MM-DD): ");
                scanf("%10s", fecha_nacimiento);
                limpiar_buffer();

                printf("RFC: ");
                fgets(rfc, sizeof(rfc), stdin);
                rfc[strcspn(rfc, "\n")] = '\0';

                snprintf(buffer, sizeof(buffer), "ALTA_PROPIETARIO|%s|%s|%s|%s|%s|%s", 
                        nombre, ape_paterno, ape_materno, direccion, fecha_nacimiento, rfc);
                send(sock, buffer, strlen(buffer), 0);
                read(sock, buffer, MAX_BUFFER);
                printf("%s\n", buffer);
                break;

            case 2: // Alta Teléfono
                printf("\n--- Alta de Teléfono ---\n");
                printf("ID del Propietario: ");
                scanf("%d", &propietario_id);
                limpiar_buffer();

                printf("Tipo (Casa/Oficina/Celular): ");
                fgets(tipo, sizeof(tipo), stdin);
                tipo[strcspn(tipo, "\n")] = '\0';

                printf("Número: ");
                fgets(numero, sizeof(numero), stdin);
                numero[strcspn(numero, "\n")] = '\0';

                snprintf(buffer, sizeof(buffer), "ALTA_TELEFONO|%d|%s|%s", propietario_id, tipo, numero);
                send(sock, buffer, strlen(buffer), 0);
                read(sock, buffer, MAX_BUFFER);
                printf("%s\n", buffer);
                break;

            case 3: // Alta Auto
                printf("\n--- Alta de Auto ---\n");
                printf("ID del Propietario: ");
                scanf("%d", &propietario_id);
                limpiar_buffer();

                printf("Placas: ");
                fgets(placas, sizeof(placas), stdin);
                placas[strcspn(placas, "\n")] = '\0';

                printf("Color: ");
                fgets(color, sizeof(color), stdin);
                color[strcspn(color, "\n")] = '\0';

                printf("Modelo (año > 2010): ");
                scanf("%d", &modelo);
                limpiar_buffer();

                printf("Tipo: ");
                fgets(auto_tipo, sizeof(auto_tipo), stdin);
                auto_tipo[strcspn(auto_tipo, "\n")] = '\0';

                snprintf(buffer, sizeof(buffer), "ALTA_AUTO|%d|%s|%s|%d|%s", 
                        propietario_id, placas, color, modelo, auto_tipo);
                send(sock, buffer, strlen(buffer), 0);
                read(sock, buffer, MAX_BUFFER);
                printf("%s\n", buffer);
                break;

            case 4: // Alta Accidente
                printf("\n--- Alta de Accidente ---\n");
                printf("ID del Auto: ");
                scanf("%d", &auto_id);
                limpiar_buffer();

                printf("CNS: ");
                fgets(cns, sizeof(cns), stdin);
                cns[strcspn(cns, "\n")] = '\0';

                printf("Lugar (default: Tapachula): ");
                fgets(lugar, sizeof(lugar), stdin);
                lugar[strcspn(lugar, "\n")] = '\0';
                if (strlen(lugar) == 0) strcpy(lugar, "Tapachula");

                printf("Fecha (YYYY-MM-DD): ");
                scanf("%10s", fecha);
                limpiar_buffer();

                printf("Hora (HH:MM:SS): ");
                scanf("%8s", hora);
                limpiar_buffer();

                snprintf(buffer, sizeof(buffer), "ALTA_ACCIDENTE|%d|%s|%s|%s|%s", 
                        auto_id, cns, lugar, fecha, hora);
                send(sock, buffer, strlen(buffer), 0);
                read(sock, buffer, MAX_BUFFER);
                printf("%s\n", buffer);
                break;

            case 0:
                return;
            default:
                printf("Opción no válida\n");
                break;
        }
    } while (opc != 0);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int opc;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error al crear socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nDirección inválida / no soportada \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConexión fallida \n");
        return -1;
    }

    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Dar de alta\n");
        printf("2. Realizar consultas\n");
        printf("3. Eliminar registros\n");
        printf("4. Modificar registros\n");
        printf("0. Salir\n");
        printf("_Dame la opcion: ");
        scanf("%d", &opc);
        limpiar_buffer();

        switch (opc) {
            case 1:
                menu_altas(sock);
                break;
            case 2:
                menu_consultas(sock);
                break;
            case 3:
                // Implementar bajas
                break;
            case 4:
                // Implementar modificaciones
                break;
            case 0:
                send(sock, "EXIT", 4, 0);
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida\n");
                break;
        }
    } while (opc != 0);

    close(sock);
    return 0;
}