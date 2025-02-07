#include <stdlib.h>
#include <libpq-fe.h>
#include <stdio.h>
#include<fcntl.h>
#include<string.h>
// gcc -o conexion.c con -I /usr/include/postgresql/ -lpq
//gcc -o conexion.c conex -lpq
char tab[50], cad[100];
int opc;

int main()
{
	PGconn *conn;
	PGresult *resultado;
	PGresult *ress;
	int i,j,opc,fila,columna;
    char cad[128];
	conn=PQsetdbLogin("localhost","5432",NULL,NULL,"aseguradora","postgres","159357");

	if(PQstatus(conn) != CONNECTION_BAD)
      {puts("Estamos conectados");
          printf("_Dame la opcion:");
          scanf("%d",&opc);
          switch (opc) {
              case 1:
                  //alta
                  printf("Estoy en alta\n");

                  break;
                  
              case 2:
                        // Consulta
                        printf("Estoy en Consulta\n");
                        resultado = PQexec(conn, "SELECT * FROM auto");
            
                        if (PQresultStatus(resultado) != PGRES_TUPLES_OK) {
                            fprintf(stderr, "Error en la consulta: %s\n", PQerrorMessage(conn));
                            PQclear(resultado);
                            PQfinish(conn);
                            return 1;
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
                        break;
            
                    default:
                        printf("Opción no válida\n");
                        break;
          }
          
    }
            return 0;
}
//-L/usr/local/include -lpq
//-L/usr/local/include -lpq
