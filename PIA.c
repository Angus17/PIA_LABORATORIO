#include "./Cabeceras.h"

struct Datos_Pacientes
{
    char tipo_servicio[15];
    char nombre[100];
    char direccion[200];
    int edad;
    char genero;
    char sintomas[50];
    int numero_consultorio;
    int numero_registro;
};

bool buscar_registro_ya_existente(FILE *, int *);
void alta_pacientes(FILE *, struct Datos_Pacientes *, const char *);
void buscar_editar_paciente(FILE *);


int main(void)
{
    FILE *pacientes, *temporal;
    char *ruta_archivo_pacientes, *ruta_temporal;


    return EXIT_SUCCESS;
}