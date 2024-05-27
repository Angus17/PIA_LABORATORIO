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

void obtener_path(char **, char **);
void contar_pacientes(FILE *, struct Datos_Pacientes *, int *);
bool buscar_registro_ya_existente(FILE *, int *);
void alta_pacientes(FILE *, struct Datos_Pacientes *, const char *);
void buscar_editar_paciente(FILE *);

void validar_errores_por_SO();
void limpiar_buffer_STDIN();
void limpiar_terminal();
void pausar_terminal();


int main(void)
{
    FILE *pacientes, *temporal;
    char *ruta_archivo_pacientes = NULL, *ruta_archivo_temporal = NULL;
    int opcion, total_pacientes = 0;
    struct Datos_Pacientes datos;


    #if defined(_WIN32) || defined(_WIN64)

        setlocale(LC_ALL, "es_MX");

    #elif defined(__linux__) || defined(unix) && !defined(__ANDROID__)

        setlocale(LC_ALL, "es_MX.UTF-8");

    #endif 

    obtener_path(&ruta_archivo_pacientes, &ruta_archivo_temporal);

    pacientes = fopen(ruta_archivo_pacientes, "a+");

    if (pacientes == NULL)
    {
        free(ruta_archivo_pacientes);
        free(ruta_archivo_temporal);

        fprintf(stderr, "ERROR DE ARCHIVOS, INTENTE MAS TARDE. . .");

        return EXIT_FAILURE;
    }

    do
    {
        do
        {
            do
            {
                limpiar_terminal();

                printf( "%88s\n", "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
                printf("\n%16s%45s%28s\n\n", "* *", "MENÚ DE HOSPITAL", "* *");
                printf("%88s\n", "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
                printf("\n%65s\n", "1) Dar de alta a pacientes");
                printf("\n%69s\n", "2) Buscar o editar un paciente");
                printf("\n%62s\n", "3) Listado de pacientes");
                printf("\n%64s\n", "4) Dar de baja a paciente");
                printf("\n%53s\n", "5) Salir");
                printf("\n%31s: ", "Opción");

                limpiar_buffer_STDIN();
            } while (scanf("%d", &opcion) != 1);

            if (opcion < 1 || opcion > 5)

                validar_errores_por_SO();

        } while (opcion < 1 || opcion > 5);

        limpiar_terminal();

        contar_pacientes( pacientes, &datos, &total_pacientes );

        switch ( opcion )
        {
            case 1:
                break;

            case 2:
                break;

            case 3:
                break;

            case 4:
                break;

            case 5:

                free( ruta_archivo_pacientes );
                free( ruta_archivo_temporal );

                fflush( pacientes );
                fclose( pacientes );

                break;
        }

    } while (opcion != 5);


    return EXIT_SUCCESS;
}

void obtener_path( char **dir_pacientes, char **dir_temporal )
{

    *dir_pacientes = (char *) malloc(sizeof(char) * MAX_BYTES);

    if (*dir_pacientes == NULL)
    {
        perror("NO HAY SUFICIENTE MEMORIA. . .");

        exit(EXIT_FAILURE);
    }

    *dir_temporal = (char *) malloc(sizeof(char) * MAX_BYTES);

    if (*dir_temporal == NULL)
    {
        free(*dir_pacientes);

        perror("NO HAY SUFICIENTE MEMORIA. . .");

        exit(EXIT_FAILURE);
    }

    #if defined(_WIN32) || defined(_WIN64)

        system("mkdir Pacientes");
        limpiar_terminal();

        GetCurrentDirectory(sizeof(char) * MAX_BYTES, *dir_pacientes);
        strcat(*dir_pacientes, "\\Pacientes\\Datos_Pacientes.txt");

        GetCurrentDirectory(sizeof(char *) * MAX_BYTES, *dir_temporal);
        strcat(*dir_temporal, "\\Pacientes\\Temporal.txt");

    #elif defined(__linux__) || defined(unix) && !defined(__ANDROID__)

        system("mkdir Pacientes");
        limpiar_terminal();

        getcwd(*dir_pacientes, sizeof(char) * MAX_BYTES);
        strcat(*dir_pacientes, "/Pacientes/Datos_Pacientes.txt");

        getcwd(*dir_temporal, sizeof(char) * MAX_BYTES);
        strcat(*dir_temporal, "/Pacientes/Temporal.txt");

    #endif
}

void contar_pacientes(FILE *file_pacientes, struct Datos_Pacientes *data, int *pacientes)
{
    rewind(file_pacientes);

    while ( fscanf(file_pacientes, "%d %15[^.] %100[^.] %d %c %d %50[^.] %200[^.]  ", &data->numero_registro, data->tipo_servicio, data->nombre, &data->edad, &data->genero, &data->numero_consultorio, data->sintomas, data->direccion) != EOF )

        pacientes++;

}


// Limpia buffer STDIN tanto para sistemas Windows como para UNIX/Linux
void limpiar_buffer_STDIN()
{
    #if defined(_WIN32) || defined(_WIN64)
        rewind(stdin);
    #elif __linux__
        __fpurge(stdin);
    #endif
}

// Limpia terminal tanto para sistemas Windows como para UNIX/Linux
void limpiar_terminal()
{
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #elif __linux__
        system("clear");
    #endif
}

// Pausa la terminal tanto para sistemas Windows como para UNIX/Linux
void pausar_terminal()
{
    #if defined(_WIN32) || defined(_WIN64)
        system("pause");
    #elif __linux__
        printf("Presiona ENTER para continuar. . .");
        fflush(stdout);
        limpiar_buffer_STDIN();
        getchar();
    #endif
}

// Clasifica los mensajes de error tanto para Windows como para UNIX/Linux, cuando se ingresan datos incorrectos
void validar_errores_por_SO()
{

    limpiar_terminal();

    printf("Dato/s ingresado/s no válido/s, verificar dato/s\n");
    fflush(stdout);
    pausar_terminal();
    limpiar_terminal();
}