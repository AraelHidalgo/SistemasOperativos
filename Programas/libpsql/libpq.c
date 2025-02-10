#include <stdlib.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h> // Para fork() y getpid()
#include <sys/wait.h> // Para wait()

// gcc -o conexion conexion.c -I /usr/include/postgresql/ -lpq

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Función para verificar si un propietario existe
int propietario_existe(PGconn *conn, int id_prop) {
    PGresult *res;
    char query[100];
    int existe = 0;

    // Consulta para verificar la existencia del ID
    snprintf(query, sizeof(query), "SELECT 1 FROM Propietario WHERE id_prop = %d", id_prop);
    res = PQexec(conn, query);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        // Si hay al menos una fila, el ID existe
        existe = (PQntuples(res) > 0);
    } else {
        fprintf(stderr, "Error al validar el ID del propietario: %s\n", PQerrorMessage(conn));
    }

    PQclear(res);
    return existe;
}

void ejecutar_consulta(PGconn *conn, const char *tabla) {
    PGresult *resultado;
    int i, j, fila, columna;
    char query[128];

    snprintf(query, sizeof(query), "SELECT * FROM %s", tabla);

    printf("Estoy en Consulta (PID: %d)\n", getpid());
    printf("Consulta: %s\n", query); 

    resultado = PQexec(conn, query);

    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error en la consulta: %s\n", PQerrorMessage(conn));
        PQclear(resultado);
        return;
    }

    fila = PQntuples(resultado);
    columna = PQnfields(resultado);
    printf("Filas: %d, Columnas: %d\n", fila, columna);

    for (i = 0; i < fila; i++) {
        for (j = 0; j < columna; j++) {
            printf("%s\t", PQgetvalue(resultado, i, j));
        }
        printf("\n");
    }

    PQclear(resultado);
}

//Tabla propietario
void alta_propietario(PGconn *conn) {
    char nombre[100], ape_paterno[100], ape_materno[100], direccion[200], fecha_nacimiento[11], rfc[20];
    PGresult *res;

    printf("\n--- Alta de Propietario ---\n");
    printf("Nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = '\0'; // Eliminar el salto de línea

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

    // Usar PQexecParams para evitar inyección SQL
    const char *query = "INSERT INTO Propietario (nombre, ape_paterno, ape_materno, direccion, fecha_nacimiento, rfc) VALUES ($1, $2, $3, $4, $5, $6)";
    const char *params[] = {nombre, ape_paterno, ape_materno, direccion, fecha_nacimiento, rfc};
    int paramLengths[] = {strlen(nombre), strlen(ape_paterno), strlen(ape_materno), strlen(direccion), strlen(fecha_nacimiento), strlen(rfc)};
    int paramFormats[] = {0, 0, 0, 0, 0, 0}; // 0 = texto

    res = PQexecParams(conn, query, 6, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error al insertar en Propietario: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Propietario registrado correctamente!\n");
    PQclear(res);
}

// Tabla Teléfono
void alta_telefono(PGconn *conn) {
    int propietario_id;
    char tipo[20], numero[20];
    char prop_id_str[20]; // Buffer para convertir el entero a cadena
    PGresult *res;

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

    // Convertir el entero a cadena
    snprintf(prop_id_str, sizeof(prop_id_str), "%d", propietario_id);

    const char *query = "INSERT INTO Telefono (propietario_id, tipo, numero) VALUES ($1::INTEGER, $2, $3)";
    const char *params[] = {prop_id_str, tipo, numero}; // Usar la cadena convertida
    int paramLengths[] = {strlen(prop_id_str), strlen(tipo), strlen(numero)};
    int paramFormats[] = {0, 0, 0}; // Todos son texto

    res = PQexecParams(conn, query, 3, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error al insertar en Teléfono: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Teléfono registrado correctamente!\n");
    PQclear(res);
}

// Tabla Auto
void alta_auto(PGconn *conn) {
    int propietario_id, modelo;
    char placas[20], color[50], tipo[50];
    char prop_id_str[20], modelo_str[20]; // Buffers para enteros
    PGresult *res;

    printf("\n--- Alta de Auto ---\n");
    do {
        printf("ID del Propietario: ");
        scanf("%d", &propietario_id);
        limpiar_buffer();

        if (propietario_existe(conn, propietario_id)) {
            break; // ID válido, salir del bucle
        } else {
            printf("Error: El ID del propietario no existe. Intente de nuevo.\n");
        }
    } while (1);

    printf("Placas: ");
    fgets(placas, sizeof(placas), stdin);
    placas[strcspn(placas, "\n")] = '\0';

    printf("Color: ");
    fgets(color, sizeof(color), stdin);
    color[strcspn(color, "\n")] = '\0';

        do {
        printf("Modelo (año > 2010): ");
        if (scanf("%d", &modelo) != 1) {
            printf("Error: Debe ingresar un número.\n");
            limpiar_buffer();
            continue;
        }
        limpiar_buffer();

        if (modelo > 2010) {
            break; // Modelo válido
        } else {
            printf("Error: El modelo debe ser mayor a 2010.\n");
        }
    } while (1);


    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = '\0';

    // Convertir enteros a cadenas
    snprintf(prop_id_str, sizeof(prop_id_str), "%d", propietario_id);
    snprintf(modelo_str, sizeof(modelo_str), "%d", modelo);

    const char *query = "INSERT INTO Auto (propietario_id, placas, color, modelo, tipo) VALUES ($1::INTEGER, $2, $3, $4::INTEGER, $5)";
    const char *params[] = {prop_id_str, placas, color, modelo_str, tipo};
    int paramLengths[] = {strlen(prop_id_str), strlen(placas), strlen(color), strlen(modelo_str), strlen(tipo)};
    int paramFormats[] = {0, 0, 0, 0, 0}; // Todos son texto

    res = PQexecParams(conn, query, 5, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error al insertar en Auto: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Auto registrado correctamente!\n");
    PQclear(res);
}

// Tabla Accidente
void alta_accidente(PGconn *conn) {
    int auto_id;
    char cns[20], fecha[11], hora[9], lugar[200];
    char auto_id_str[20]; // Buffer para el entero
    PGresult *res;

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

    //Convirtiendo entero a cadena
    snprintf(auto_id_str, sizeof(auto_id_str), "%d", auto_id);

    const char *query = "INSERT INTO Accidente (auto_id, cns, lugar, fecha, hora) VALUES ($1::INTEGER, $2, $3, $4, $5)";
    const char *params[] = {auto_id_str, cns, lugar, fecha, hora};
    int paramLengths[] = {strlen(auto_id_str), strlen(cns), strlen(lugar), strlen(fecha), strlen(hora)};
    int paramFormats[] = {0, 0, 0, 0, 0}; // Todos son texto

    res = PQexecParams(conn, query, 5, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error al insertar en Accidente: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Accidente registrado correctamente!\n");
    PQclear(res);
}

// Menú de altas
void menu_altas(PGconn *conn) {
    int opc;
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
            case 1:
                alta_propietario(conn);
                break;
            case 2:
                alta_telefono(conn);
                break;
            case 3:
                alta_auto(conn);
                break;
            case 4:
                alta_accidente(conn);
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                break;
            default:
                printf("Opción no válida\n");
                break;
        }
    } while (opc != 0);
}


void ejecutar_baja(PGconn *conn) {
    printf("Estoy en Baja (PID: %d)\n", getpid());
}

void ejecutar_modificacion(PGconn *conn) {
    printf("Estoy en Modificación (PID: %d)\n", getpid());
}

void menu_consultas(PGconn *conn) {
    int opc;
    do {
        printf("\n--- Menú de Consultas ---\n");
        printf("1. Consultar tabla accidente\n");
        printf("2. Consultar tabla auto\n");
        printf("3. Consultar tabla propietario\n");
        printf("4. Consultar tabla telefono\n");
        printf("0. Volver al menú principal\n");
        printf("_Dame la opcion: ");
        scanf("%d", &opc);

        switch (opc) {
            case 1:
                ejecutar_consulta(conn, "accidente");
                printf("Presiona Enter para continuar...\n");
                getchar();
                getchar(); 
                break;
            case 2:
                ejecutar_consulta(conn, "auto");
                printf("Presiona Enter para continuar...\n");
                getchar();
                getchar(); 
                break;
            case 3:
                ejecutar_consulta(conn, "propietario");
                printf("Presiona Enter para continuar...\n");
                getchar(); 
                getchar(); 
                break;
            case 4:
                ejecutar_consulta(conn, "telefono");
                printf("Presiona Enter para continuar...\n");
                getchar(); 
                getchar(); 
                break;
            case 0:
                printf("Volviendo al menú principal...\n");
                sleep(2); 
                break;
            default:
                printf("Opción no válida\n");
                break;
        }
    } while (opc != 0);
}

int main(int argc, char *argv[]) {
    PGconn *conn;
    int opc;

    do {
        printf("\n--- Menú Principal ---\n");
        printf("1. Dar de alta\n");
        printf("2. Realizar consultas\n");
        printf("3. Eliminar registros\n");
        printf("4. Modificar registros\n");
        printf("0. Salir\n");
        printf("_Dame la opcion: ");
        scanf("%d", &opc);

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Error al crear el proceso hijo\n");
            return 1;
        } else if (pid == 0) {
            // Proceso hijo: Crear una nueva conexión
            PGconn *hijo_conn = PQsetdbLogin("localhost", "5432", NULL, NULL, "aseguradora", "postgres", "159357");

            if (PQstatus(hijo_conn) == CONNECTION_BAD) {
                fprintf(stderr, "Error de conexión en el hijo: %s\n", PQerrorMessage(hijo_conn));
                exit(1);
            }

            switch (opc) {
                case 1:
                    menu_altas(hijo_conn);
                    break;
                case 2:
                    menu_consultas(hijo_conn);
                    break;
                case 3:
                    
                    break;
                case 4:
                    
                    break;
                case 0:
                    printf("Saliendo...\n");
                    break;
                default:
                    printf("Opción no válida\n");
                    break;
            }

            PQfinish(hijo_conn); // Cerrar la conexión del hijo
            exit(0);
        } else {
            // Proceso padre: Esperar al hijo
            wait(NULL);
            printf("Proceso hijo terminado (PID: %d)\n", pid);
        }
    } while (opc != 0);

    // Cerrar la conexión del padre al salir
    return 0;
}