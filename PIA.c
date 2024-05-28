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
void alta_pacientes(FILE *, struct Datos_Pacientes *, int *);
void buscar_editar_paciente();
void contar_pacientes(FILE *, struct Datos_Pacientes *, int *);
bool buscar_consultorio(FILE *, const int *);
bool buscar_numero(FILE *, int *);

void convertir_cadena_a_minuscula(char *);

void validar_errores_por_SO();
void limpiar_buffer_STDIN();
void limpiar_terminal();
void pausar_terminal();


int main(void)
{
    FILE *pacientes, *temporal;
    char *ruta_archivo_pacientes = NULL, *ruta_archivo_temporal = NULL;
    int opcion, total_pacientes;
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

        if (opcion != 5)

            contar_pacientes( pacientes, &datos, &total_pacientes );        

        switch ( opcion )
        {
            case 1:

                if (total_pacientes < MAX_PACIENTES)

                    alta_pacientes(pacientes, &datos, &total_pacientes);

                else

                    puts("EL HOSPITAL ESTA LLENO!. . .");

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

        if (opcion != 5)

            pausar_terminal();

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

void alta_pacientes( FILE *file_pacientes, struct Datos_Pacientes *data, int *pacientes_neto)
{
    char respuesta[3];
    char expresion[] = "^(externo|emergencia)$";
    char expresion_2[] = "^[A-Za-z ]+$";
    char expresion_3[] = "^([0-9A-Za-z ]+) #([0-9]+) ([A-Za-z ]+)\\, ([A-Za-z ]+)\\, ([A-Za-z ]+)$";
    char expresion_4[] = "^[A-Za-z, ]+$";
    regex_t re_servicio, re_cadenas, re_direccion, re_sintomas;
    bool es_bebe, numero_disponible = true;
    int regular_1, dias;

    regcomp( &re_servicio, expresion, REG_EXTENDED );
    regcomp( &re_cadenas, expresion_2, REG_EXTENDED );
    regcomp( &re_direccion, expresion_3, REG_EXTENDED );
    regcomp( &re_sintomas, expresion_4, REG_EXTENDED );

    do
    {
        limpiar_terminal();

        printf("Desea dar de alta algun paciente? Si/No\n: ");
        limpiar_buffer_STDIN();
        fgets( respuesta, sizeof(respuesta), stdin );

        convertir_cadena_a_minuscula( respuesta );

        if ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) )

            validar_errores_por_SO();

    } while ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) );

    if ( *pacientes_neto != 0 && strcmp( respuesta, "si" ) == 0 )
    {
        fprintf( file_pacientes, "\n" );
        fflush( file_pacientes );
    }

    while ( strcmp( respuesta, "si" ) == 0 )
    {
        do
        {
            limpiar_terminal();

            printf("A qué tipo de servicio se da de alta?\n - Externo\n - Emergencia\n: ");
            limpiar_buffer_STDIN();
            fgets( data->tipo_servicio, sizeof( data->tipo_servicio ) - 1, stdin );

            *( data->tipo_servicio + ( strcspn( data->tipo_servicio, "\n" ) ) ) = '\0';

            convertir_cadena_a_minuscula(data->tipo_servicio);

            regular_1 = regexec( &re_servicio, data->tipo_servicio, 0, NULL, 0 );

            if ( regular_1 != 0 )

                validar_errores_por_SO();

        } while ( regular_1 != 0 );

        do
        {
            limpiar_terminal();

            printf("Nombre del paciente: ");
            limpiar_buffer_STDIN();
            fgets( data->nombre, sizeof( data->nombre ) - 1, stdin );

            *( data->nombre + ( strcspn( data->nombre, "\n" ) ) ) = '\0';

            regular_1 = regexec( &re_cadenas, data->nombre, 0, NULL, 0 );

            if ( regular_1 != 0 )

                validar_errores_por_SO();

        } while ( regular_1 != 0 );

        do
        {
            limpiar_terminal();

            printf("Dirección del paciente\n"
                    "NOTA!: Cambia los campos entre () ignorando todos los parentesis\n"
                    "(Calle) #(numero) (Colonia,Fracc...etc.), (Municipio), (Estado)\n: ");
            limpiar_buffer_STDIN();
            fgets( data->direccion, sizeof( data->direccion ) - 1, stdin );

            *( data->direccion + ( strcspn( data->direccion, "\n" ) ) ) = '\0';

            regular_1 = regexec( &re_direccion, data->direccion, 0, NULL, 0 );

            if ( regular_1 != 0 )

                validar_errores_por_SO();

        } while ( regular_1 != 0 );

        strcat( data->direccion, "." ); // Delimitador

        do
        {
            do
            {
                limpiar_terminal();

                printf("Edad del paciente ( Ingrese 0 si es un bebé ): ");
                limpiar_buffer_STDIN();

            } while ( scanf( " %d", &data->edad ) != 1);

            if ( data->edad < 0)

                validar_errores_por_SO();

            else if ( data->edad == 0 )

                    es_bebe = true;

                else

                    es_bebe = false;

        } while ( data->edad < 0 );

        if ( es_bebe )
        {
            do
            {
                do
                {
                    limpiar_terminal();

                    printf("Cuántos días tiene de nacid@: ");
                    limpiar_buffer_STDIN();

                } while ( scanf(" %d", &dias) != 1);

                if ( dias < 1)

                    validar_errores_por_SO();

                else if ( dias > 365 )
                    {
                        es_bebe = false;

                        limpiar_terminal();
                        puts("El/la paciente ya supera el año de nacid@, ya no se considera bebé. . .");
                        pausar_terminal();

                        data->edad = dias / 365; // Edad aproximada en años
                    }
                    else

                        data->edad = dias / 30; // Edad aproximada en meses

            } while ( dias < 1 );
        }

        do
        {
            limpiar_terminal();

            printf("Género del paciente\nM - Masculino\nF - Femenino\n O - Otro\n: ");
            limpiar_buffer_STDIN();
            scanf(" %c", &data->genero);

            data->genero = toupper(data->genero);

            if ( data->genero != 'M' && data->genero != 'F' && data->genero != 'O' )

                validar_errores_por_SO();

        } while ( data->genero != 'M' && data->genero != 'F' && data->genero != 'O' );

        do
        {
            limpiar_terminal();

            printf("Síntomas del paciente: ");
            limpiar_buffer_STDIN();
            fgets( data->sintomas, sizeof( data->sintomas ) - 1, stdin );

            *( data->sintomas + ( strcspn( data->sintomas, "\n" ) ) ) = '\0';

            regular_1 = regexec( &re_sintomas, data->sintomas, 0, NULL, 0 );

            if ( regular_1 != 0 )

                validar_errores_por_SO();

        } while ( regular_1 != 0 );

        do
        {
            do
            {
                limpiar_terminal();

                printf("\n# de Consultorio a asignar ( 1-20 ): ");
                limpiar_buffer_STDIN();

            } while ( scanf(" %d", &data->numero_consultorio) != 1 );

            if ( data->numero_consultorio < 1 || data->numero_consultorio > MAX_CONSULTORIOS )

                validar_errores_por_SO();

        } while ( data->numero_consultorio < 1 || data->numero_consultorio > MAX_CONSULTORIOS );


        srand( time( NULL ) );

        do
        {
            data->numero_registro = rand() + 1;

            if ( *pacientes_neto > 0 )

                numero_disponible = buscar_numero(file_pacientes, &data->numero_registro);

        } while (!numero_disponible);

        (*pacientes_neto)++;

        *(data->tipo_servicio) = toupper( *(data->tipo_servicio) );

        fprintf(file_pacientes, "%s %d %s %d %c %s %d %s", data->tipo_servicio, data->numero_registro, data->nombre, data->edad, data->genero, data->sintomas, data->numero_consultorio, data->direccion);

        fflush( file_pacientes );

        if ( *pacientes_neto < MAX_PACIENTES )
        {
            do
            {
                limpiar_terminal();

                printf("Desea dar de alta algún otro paciente? Si/No\n: ");
                limpiar_buffer_STDIN();
                fgets( respuesta, sizeof(respuesta), stdin );

                convertir_cadena_a_minuscula( respuesta );

                if ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) )

                    validar_errores_por_SO();

            } while ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) );
        }
        else
        {
            limpiar_terminal();

            puts("El Hospital a alcanzado su límite máximo de cupo");
        }
    }

    regfree( &re_cadenas );
    regfree( &re_servicio );
    regfree( &re_sintomas );
    regfree( &re_direccion );

}

void contar_pacientes(FILE *file_pacientes, struct Datos_Pacientes *data, int *pacientes)
{
    rewind(file_pacientes);

    *pacientes = 0;

    while (fscanf(file_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^.]", data->tipo_servicio, &data->numero_registro, data->nombre, &data->edad, &data->genero, data->sintomas, &data->numero_consultorio, data->direccion) != EOF)

        (*pacientes)++;

    if (*pacientes > 0)

        (*pacientes)--;

}

bool buscar_consultorio(FILE *f_pacientes, const int *numero_consultorio)
{
    struct Datos_Pacientes data;

    rewind( f_pacientes );

    while ( fscanf(f_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d  %200[^.] ", data.tipo_servicio, &data.numero_registro, data.nombre, &data.edad, &data.genero, data.sintomas, &data.numero_consultorio, data.direccion) != EOF )
    {
        if (data.numero_consultorio == (*numero_consultorio))

            return false; // Consultorio ocupado

    }

    return true;
}

bool buscar_numero(FILE *f_pacientes, int *numero_ingresado)
{
    struct Datos_Pacientes data;

    rewind( f_pacientes );

    while ( fscanf(f_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^.]", data.tipo_servicio, &data.numero_registro, data.nombre, &data.edad, &data.genero, data.sintomas, &data.numero_consultorio, data.direccion) != EOF )
    {
        if (data.numero_registro == (*numero_ingresado))

            return false; // Numero de registro ocupado

    }

    return true;
}

void convertir_cadena_a_minuscula(char *caracter)
{
    while (*caracter != '\0')
    {
        if (!islower(*caracter))

            *caracter = tolower(*caracter);

        caracter++;
    }
    

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