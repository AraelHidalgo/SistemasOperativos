#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libpq-fe.h>

#define PORT 8080
#define MAX_BUFFER 1024

PGconn *conn;

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int propietario_existe(int id_prop) {
    PGresult *res;
    char query[100];
    int existe = 0;

    snprintf(query, sizeof(query), "SELECT 1 FROM Propietario WHERE id_prop = %d", id_prop);
    res = PQexec(conn, query);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        existe = (PQntuples(res) > 0);
    } else {
        fprintf(stderr, "Error al validar el ID del propietario: %s\n", PQerrorMessage(conn));
    }

    PQclear(res);
    return existe;
}

void ejecutar_consulta(int client_socket, const char *tabla) {
    PGresult *resultado;
    char query[128];
    char buffer[MAX_BUFFER] = {0};
    int i, j, fila, columna;

    snprintf(query, sizeof(query), "SELECT * FROM %s", tabla);
    resultado = PQexec(conn, query);

    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) {
        snprintf(buffer, sizeof(buffer), "Error en la consulta: %s", PQerrorMessage(conn));
        send(client_socket, buffer, strlen(buffer), 0);
        PQclear(resultado);
        return;
    }

    fila = PQntuples(resultado);
    columna = PQnfields(resultado);
    
    // Enviar encabezados
    for (j = 0; j < columna; j++) {
        strcat(buffer, PQfname(resultado, j));
        strcat(buffer, "\t");
    }
    strcat(buffer, "\n");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));

    // Enviar datos
    for (i = 0; i < fila; i++) {
        for (j = 0; j < columna; j++) {
            strcat(buffer, PQgetvalue(resultado, i, j));
            strcat(buffer, "\t");
        }
        strcat(buffer, "\n");
        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    PQclear(resultado);
    send(client_socket, "END_RESULTS", 11, 0);
}

void alta_propietario(int client_socket, char *params[]) {
    PGresult *res;
    const char *query = "INSERT INTO Propietario (nombre, ape_paterno, ape_materno, direccion, fecha_nacimiento, rfc) VALUES ($1, $2, $3, $4, $5, $6)";
    int paramLengths[] = {strlen(params[0]), strlen(params[1]), strlen(params[2]), strlen(params[3]), strlen(params[4]), strlen(params[5])};
    int paramFormats[] = {0, 0, 0, 0, 0, 0};

    res = PQexecParams(conn, query, 6, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send(client_socket, "Error al insertar en Propietario", 30, 0);
    } else {
        send(client_socket, "Propietario registrado correctamente!", 37, 0);
    }
    PQclear(res);
}

void alta_telefono(int client_socket, char *params[]) {
    PGresult *res;
    const char *query = "INSERT INTO Telefono (propietario_id, tipo, numero) VALUES ($1::INTEGER, $2, $3)";
    int paramLengths[] = {strlen(params[0]), strlen(params[1]), strlen(params[2])};
    int paramFormats[] = {0, 0, 0};

    res = PQexecParams(conn, query, 3, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send(client_socket, "Error al insertar en Teléfono", 27, 0);
    } else {
        send(client_socket, "Teléfono registrado correctamente!", 32, 0);
    }
    PQclear(res);
}

void alta_auto(int client_socket, char *params[]) {
    PGresult *res;
    const char *query = "INSERT INTO Auto (propietario_id, placas, color, modelo, tipo) VALUES ($1::INTEGER, $2, $3, $4::INTEGER, $5)";
    int paramLengths[] = {strlen(params[0]), strlen(params[1]), strlen(params[2]), strlen(params[3]), strlen(params[4])};
    int paramFormats[] = {0, 0, 0, 0, 0};

    res = PQexecParams(conn, query, 5, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send(client_socket, "Error al insertar en Auto", 24, 0);
    } else {
        send(client_socket, "Auto registrado correctamente!", 29, 0);
    }
    PQclear(res);
}

void alta_accidente(int client_socket, char *params[]) {
    PGresult *res;
    const char *query = "INSERT INTO Accidente (auto_id, cns, lugar, fecha, hora) VALUES ($1::INTEGER, $2, $3, $4, $5)";
    int paramLengths[] = {strlen(params[0]), strlen(params[1]), strlen(params[2]), strlen(params[3]), strlen(params[4])};
    int paramFormats[] = {0, 0, 0, 0, 0};

    res = PQexecParams(conn, query, 5, NULL, params, paramLengths, paramFormats, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        send(client_socket, "Error al insertar en Accidente", 28, 0);
    } else {
        send(client_socket, "Accidente registrado correctamente!", 33, 0);
    }
    PQclear(res);
}

void manejar_cliente(int client_socket) {
    char buffer[MAX_BUFFER] = {0};
    int valread;
    char *token;
    char *params[6];
    int i;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        valread = read(client_socket, buffer, MAX_BUFFER);
        
        if (valread <= 0) break;

        printf("Comando recibido: %s\n", buffer);
        
        token = strtok(buffer, "|");
        
        if (strcmp(token, "CONSULTA") == 0) {
            token = strtok(NULL, "|");
            ejecutar_consulta(client_socket, token);
        }
        else if (strcmp(token, "ALTA_PROPIETARIO") == 0) {
            for (i = 0; i < 6; i++) {
                params[i] = strtok(NULL, "|");
            }
            alta_propietario(client_socket, params);
        }
        else if (strcmp(token, "ALTA_TELEFONO") == 0) {
            for (i = 0; i < 3; i++) {
                params[i] = strtok(NULL, "|");
            }
            alta_telefono(client_socket, params);
        }
        else if (strcmp(token, "ALTA_AUTO") == 0) {
            for (i = 0; i < 5; i++) {
                params[i] = strtok(NULL, "|");
            }
            alta_auto(client_socket, params);
        }
        else if (strcmp(token, "ALTA_ACCIDENTE") == 0) {
            for (i = 0; i < 5; i++) {
                params[i] = strtok(NULL, "|");
            }
            alta_accidente(client_socket, params);
        }
        else if (strcmp(token, "EXIT") == 0) {
            break;
        }
    }
    
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Conexión a PostgreSQL
    conn = PQsetdbLogin("localhost", "5432", NULL, NULL, "aseguradora", "postgres", "159357");
    
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Error de conexión: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar socket
    // Configurar SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEADDR");
        // No salimos del programa, intentamos con el siguiente
    }
    
    // Configurar SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEPORT");
        // No salimos del programa, continuamos a pesar del error
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Escuchar
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Nueva conexión aceptada\n");
        manejar_cliente(client_socket);
    }

    PQfinish(conn);
    return 0;
}