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
    int es_bebe;
};

struct Conjunto_Datos
{
    struct Datos_Pacientes datos;
    FILE *pacientes, *temporal;
    char *ruta_archivo_pacientes, *ruta_archivo_temporal;
    int total_pacientes;
};

void obtener_path(char **, char **);
void alta_pacientes(FILE *, struct Datos_Pacientes *, int *, const char *);
void buscar_editar_paciente(struct Conjunto_Datos *);
void realizar_edicion(struct Conjunto_Datos *, const int *, const int *, const int *, const char *);
void listar_pacientes(FILE *, struct Datos_Pacientes *, const char *);
void baja_pacientes(struct Conjunto_Datos *);


void contar_pacientes(FILE *, struct Datos_Pacientes *, int *, const char *);
bool buscar_numero_nombre(FILE *, struct Datos_Pacientes *, const int *, const char *, const int *);

void convertir_cadena_a_minuscula(char *);

void validar_errores_por_SO();
void limpiar_buffer_STDIN();
void limpiar_terminal();
void pausar_terminal();


int main(void)
{
    int opcion;
    struct Conjunto_Datos todo;

    todo.ruta_archivo_pacientes = NULL;
    todo.ruta_archivo_temporal = NULL;

    setlocale(LC_ALL, "es_MX.UTF-8");

    obtener_path(&todo.ruta_archivo_pacientes, &todo.ruta_archivo_temporal);

    todo.pacientes = fopen(todo.ruta_archivo_pacientes, "a+");

    if ( todo.pacientes == NULL)
    {
        free(todo.ruta_archivo_pacientes);
        free(todo.ruta_archivo_temporal);

        fprintf(stderr, "ERROR DE ARCHIVOS, INTENTE MAS TARDE. . .\n");

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

            contar_pacientes( todo.pacientes, &todo.datos, &todo.total_pacientes, todo.ruta_archivo_pacientes );

        switch ( opcion )
        {
            case 1:

                if (todo.total_pacientes < MAX_PACIENTES)

                    alta_pacientes(todo.pacientes, &todo.datos, &todo.total_pacientes, todo.ruta_archivo_pacientes);

                else

                    puts("EL HOSPITAL ESTA LLENO!. . .");

                break;

            case 2:

                if ( todo.total_pacientes > 0 )

                    buscar_editar_paciente( &todo );

                else

                    puts("NO HAY PACIENTES REGISTRADOS EN EL SISTEMA!. . .");

                break;

            case 3:

                if ( todo.total_pacientes > 0 )

                    listar_pacientes( todo.pacientes, &todo.datos, todo.ruta_archivo_pacientes );

                else

                    puts("NO HAY PACIENTES REGISTRADOS EN EL SISTEMA!. . .");

                break;

            case 4:

                if ( todo.total_pacientes > 0 )

                    baja_pacientes( &todo );

                else

                    puts("NO HAY PACIENTES REGISTRADOS EN EL SISTEMA!. . .");


                break;

            case 5:

                free( todo.ruta_archivo_pacientes );
                free( todo.ruta_archivo_temporal );

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
        fprintf(stderr, "NO HAY SUFICIENTE MEMORIA. . .\n");

        exit(EXIT_FAILURE);
    }

    *dir_temporal = (char *) malloc(sizeof(char) * MAX_BYTES);

    if (*dir_temporal == NULL)
    {
        free(*dir_pacientes);

        fprintf(stderr, "NO HAY SUFICIENTE MEMORIA. . .\n");

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

void alta_pacientes( FILE *file_pacientes, struct Datos_Pacientes *data, int *pacientes_neto, const char *dir_pacientes)
{
    char respuesta[3];
    char expresion[] = "^(externo|emergencia)$";
    char expresion_2[] = "^[A-Za-z ]+$";
    char expresion_3[] = "^([0-9A-Za-z ]+) #([0-9]+) ([A-Za-z ]+)\\, ([A-Za-z ]+)\\, ([A-Za-z ]+)$";
    char expresion_4[] = "^[A-Za-z, ]+$";
    regex_t re_servicio, re_cadenas, re_direccion, re_sintomas;
    bool numero_disponible = true;
    int regular_1, dias, busqueda_id = 1;

    file_pacientes = fopen( dir_pacientes, "a+" );

    if ( file_pacientes == NULL )

        fprintf(stderr, "ERROR DE FICHEROS. . .\n");

    else
    {
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

        while ( strcmp( respuesta, "si" ) == 0 && (*pacientes_neto) < MAX_PACIENTES )
        {
            do
            {
                limpiar_terminal();

                printf("A qué tipo de servicio se da de alta?\n - Externo\n - Emergencia\n: ");
                limpiar_buffer_STDIN();
                fgets( data->tipo_servicio, sizeof( data->tipo_servicio ), stdin );

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
                fgets( data->nombre, sizeof( data->nombre ), stdin );

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
                        "(Calle) #(numero) (Colonia, Fracc...etc.), (Municipio), (Estado)\n: ");
                limpiar_buffer_STDIN();
                fgets( data->direccion, sizeof( data->direccion ) - 1, stdin );

                *( data->direccion + ( strcspn( data->direccion, "\n" ) ) ) = '\0';

                regular_1 = regexec( &re_direccion, data->direccion, 0, NULL, 0 );

                if ( regular_1 != 0 )

                    validar_errores_por_SO();

            } while ( regular_1 != 0 );

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

                        data->es_bebe = 1; // true

                    else

                        data->es_bebe = 0; // false

            } while ( data->edad < 0 );

            if ( data->es_bebe )
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
                            data->es_bebe = 0;

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
                fgets( data->sintomas, sizeof( data->sintomas ), stdin );

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

                    numero_disponible = buscar_numero_nombre(file_pacientes, NULL, &data->numero_registro, NULL, &busqueda_id);

            } while (!numero_disponible);

            (*pacientes_neto)++;

            *(data->tipo_servicio) = toupper( *(data->tipo_servicio) );

            fprintf(file_pacientes, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", data->tipo_servicio, data->numero_registro, data->nombre, data->edad, data->genero, data->sintomas, data->numero_consultorio, data->direccion, data->es_bebe);

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

                if (strcmp( respuesta, "si" ) == 0)
                {
                    fprintf( file_pacientes, "\n" );

                    fflush( file_pacientes );
                }
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
}

void contar_pacientes(FILE *file_pacientes, struct Datos_Pacientes *data, int *pacientes, const char *dir_pacientes)
{
    *pacientes = 0;

    file_pacientes = fopen( dir_pacientes, "r" );

    if ( file_pacientes != NULL )
    {
        rewind(file_pacientes);


        while (fscanf(file_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", data->tipo_servicio, &data->numero_registro, data->nombre, &data->edad, &data->genero, data->sintomas, &data->numero_consultorio, data->direccion, &data->es_bebe) != EOF)

            (*pacientes)++;

        fclose( file_pacientes );
    }


}

void buscar_editar_paciente(struct Conjunto_Datos *a_data)
{
    char respuesta[3], respuesta_2[3], respuesta_busqueda, nombre[100];
    int numero_registro, buscar_id = 2, buscar_nombre = 3, opcion;

    do
    {
        limpiar_terminal();

        printf("Desea buscar datos de algun paciente? Si/No\n: ");
        limpiar_buffer_STDIN();
        fgets( respuesta, sizeof(respuesta), stdin );

        convertir_cadena_a_minuscula( respuesta );

        if ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) )

            validar_errores_por_SO();

    } while ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) );

    if (strcmp( respuesta, "si" ) == 0)
    {
        a_data->pacientes = fopen( a_data->ruta_archivo_pacientes, "r" );

        if ( a_data->pacientes == NULL )

            fprintf(stderr, "ERROR DE LECTURA DE FICHEROS. . .\n");

        else
        {
            do
            {
                limpiar_terminal();

                printf("Desea buscar por\n a) Numero de registro\n b) Nombre\n?: ");
                limpiar_buffer_STDIN();
                scanf(" %c", &respuesta_busqueda);

                respuesta_busqueda = tolower(respuesta_busqueda);

                if ( respuesta_busqueda != 'a' && respuesta_busqueda != 'b' )

                    validar_errores_por_SO();

            } while (respuesta_busqueda != 'a' && respuesta_busqueda != 'b');

            switch (respuesta_busqueda)
            {
                case 'a':

                    do
                    {
                        do
                        {
                            limpiar_terminal();

                            printf("Ingresa el número de registro del paciente: ");
                            limpiar_buffer_STDIN();
                        } while ( scanf("%d", &numero_registro) != 1 );

                        if (numero_registro < 1)

                            validar_errores_por_SO();

                    } while (numero_registro < 1);

                    if ( !buscar_numero_nombre( a_data->pacientes, &a_data->datos, &numero_registro, NULL, &buscar_id ) )
                    {
                        printf("%-15s%-20s%-10s%-25s%-10s%-30s%-50s%-15s\n",
                            "# REGISTRO", "TIPO SERVICIO", "GENERO", "NOMBRE", "EDAD", "SINTOMAS", "DOMICILIO", "# CONSULTORIO");

                        if (a_data->datos.es_bebe)
                        {
                            printf("%-15d%-20s%-10c%-25s%-10d meses%-30s%-50s%-15d\n",
                                a_data->datos.numero_registro, a_data->datos.tipo_servicio, a_data->datos.genero, a_data->datos.nombre,
                                a_data->datos.edad, a_data->datos.sintomas, a_data->datos.direccion, a_data->datos.numero_consultorio);
                        }
                        else
                        {
                            printf("%-15d%-20s%-10c%-25s%-10d años %-30s%-50s%-15d\n",
                                a_data->datos.numero_registro, a_data->datos.tipo_servicio, a_data->datos.genero, a_data->datos.nombre,
                                a_data->datos.edad, a_data->datos.sintomas, a_data->datos.direccion, a_data->datos.numero_consultorio);
                        }

                        pausar_terminal();

                        do
                        {
                            limpiar_terminal();

                            printf("Desea editar campos del paciente? Si/No\n: ");
                            limpiar_buffer_STDIN();
                            fgets( respuesta_2, sizeof(respuesta_2), stdin );

                            convertir_cadena_a_minuscula( respuesta_2 );

                            if ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) )

                                validar_errores_por_SO();

                        } while ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) );

                        while ( strcmp( respuesta_2, "si" ) == 0 )
                        {
                            do
                            {
                                do
                                {
                                    limpiar_terminal();

                                    puts("Selecciona el campo a editar");
                                    puts("1) Nombre");
                                    puts("2) Direccion");
                                    puts("3) Sintomas");
                                    puts("4) Edad");
                                    puts("5) Genero");
                                    puts("6) # Consultorio");
                                    puts("7) Tipo de servicio");
                                    printf(": ");

                                    limpiar_buffer_STDIN();

                                } while ( scanf("%d", &opcion) != 1 );

                                if (opcion < 1 || opcion > 7)

                                    validar_errores_por_SO();

                            } while (opcion < 1 || opcion > 7);

                            realizar_edicion( a_data, &opcion, &buscar_id, &numero_registro, NULL);

                            do
                            {
                                limpiar_terminal();

                                printf("Desea editar otro campo del paciente? Si/No\n: ");
                                limpiar_buffer_STDIN();
                                fgets( respuesta_2, sizeof(respuesta_2), stdin );

                                convertir_cadena_a_minuscula( respuesta_2 );

                                if ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) )

                                    validar_errores_por_SO();

                            } while ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) );
                        }

                    }
                    else
                    {
                        limpiar_terminal();

                        puts("No se encontró al paciente en nuestro sistema. . .");
                    }

                break;

                case 'b':

                    do
                    {
                        limpiar_terminal();

                        printf("Ingresa el nombre del paciente: ");
                        limpiar_buffer_STDIN();
                        fgets(nombre, sizeof(nombre), stdin);

                        *( nombre + ( strcspn( nombre, "\n" ) ) ) = '\0';

                    } while (numero_registro < 1);

                    strcat( nombre, " " );

                    if ( !buscar_numero_nombre( a_data->pacientes, &a_data->datos, NULL, nombre, &buscar_nombre ) )
                    {
                        printf("%-15s%-20s%-10s%-25s%-10s%-30s%-50s%-15s\n",
                            "# REGISTRO", "TIPO SERVICIO", "GENERO", "NOMBRE", "EDAD", "SINTOMAS", "DOMICILIO", "# CONSULTORIO");

                        if (a_data->datos.es_bebe)
                        {
                            printf("%-15d%-20s%-10c%-25s%-10d meses%-30s%-50s%-15d\n",
                                a_data->datos.numero_registro, a_data->datos.tipo_servicio, a_data->datos.genero, a_data->datos.nombre,
                                a_data->datos.edad, a_data->datos.sintomas, a_data->datos.direccion, a_data->datos.numero_consultorio);
                        }
                        else
                        {
                            printf("%-15d%-20s%-10c%-25s%-10d años %-30s%-50s%-15d\n",
                                a_data->datos.numero_registro, a_data->datos.tipo_servicio, a_data->datos.genero, a_data->datos.nombre,
                                a_data->datos.edad, a_data->datos.sintomas, a_data->datos.direccion, a_data->datos.numero_consultorio);
                        }
                        pausar_terminal();

                        do
                        {
                            limpiar_terminal();

                            printf("Desea editar campos del paciente? Si/No\n: ");
                            limpiar_buffer_STDIN();
                            fgets( respuesta_2, sizeof(respuesta_2), stdin );

                            convertir_cadena_a_minuscula( respuesta_2 );

                            if ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) )

                                validar_errores_por_SO();

                        } while ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) );

                        while ( strcmp( respuesta_2, "si" ) == 0 )
                        {
                            do
                            {
                                do
                                {
                                    limpiar_terminal();

                                    puts("Selecciona el campo a editar");
                                    puts("1) Nombre");
                                    puts("2) Direccion");
                                    puts("3) Sintomas");
                                    puts("4) Edad");
                                    puts("5) Genero");
                                    puts("6) # Consultorio");
                                    puts("7) Tipo de servicio");
                                    printf(": ");

                                    limpiar_buffer_STDIN();

                                } while ( scanf("%d", &opcion) != 1 );

                                if (opcion < 1 || opcion > 7)

                                    validar_errores_por_SO();

                            } while (opcion < 1 || opcion > 7);

                            realizar_edicion( a_data, &opcion, &buscar_nombre, NULL, nombre);

                            do
                            {
                                limpiar_terminal();

                                printf("Desea editar otro campo del paciente? Si/No\n: ");
                                limpiar_buffer_STDIN();
                                fgets( respuesta_2, sizeof(respuesta_2), stdin );

                                convertir_cadena_a_minuscula( respuesta_2 );

                                if ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) )

                                    validar_errores_por_SO();

                            } while ( strlen( respuesta_2 ) == 0 || ( strcmp( respuesta_2, "si" ) != 0 && strcmp( respuesta_2, "no" ) != 0) );
                        }

                    }
                    else
                    {
                        limpiar_terminal();

                        puts("No se encontró al paciente en nuestro sistema. . .");
                    }

                break;
            }

            fclose( a_data->pacientes );

        }
    }
}

void realizar_edicion( struct Conjunto_Datos *all, const int *opcion_elegida, const int *tipo_busqueda, const int *id_registro, const char *name)
{
    char auxiliar_nombre[100], auxiliar_tipo_servicio[15], auxiliar_direccion[200], auxiliar_sintomas[50], auxiliar_genero;
    char expresion[] = "^(externo|emergencia)$";
    char expresion_2[] = "^[A-Za-z ]+$";
    char expresion_3[] = "^([0-9A-Za-z ]+) #([0-9]+) ([A-Za-z ]+)\\, ([A-Za-z ]+)\\, ([A-Za-z ]+)$";
    char expresion_4[] = "^[A-Za-z, ]+$";

    regex_t re_servicio, re_cadenas, re_direccion, re_sintomas;
    int regular_1, auxiliar_edad, auxiliar_es_bebe, auxiliar_consultorio, auxiliar_dias;

    all->temporal = fopen( all->ruta_archivo_temporal, "w" );

    if ( all->temporal == NULL )

        fprintf(stderr, "ERROR DE FICHEROS, NO PUDO EDITAR CORRECTAMENTE. . .\n");

    else
    {

        switch ( *opcion_elegida )
        {
            case 1: // Nombre

                regcomp( &re_cadenas, expresion_2, REG_EXTENDED );

                do
                {
                    limpiar_terminal();

                    printf("Nombre del paciente: ");
                    limpiar_buffer_STDIN();
                    fgets( auxiliar_nombre, sizeof( auxiliar_nombre ), stdin );

                    *( auxiliar_nombre + ( strcspn( auxiliar_nombre, "\n" ) ) ) = '\0';

                    regular_1 = regexec( &re_cadenas, auxiliar_nombre, 0, NULL, 0 );

                    if ( regular_1 != 0 )

                        validar_errores_por_SO();

                } while ( regular_1 != 0 );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, auxiliar_nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, auxiliar_nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                    }
                }

                regfree( &re_cadenas );

            break;

            case 2: // Direccion

                regcomp( &re_direccion, expresion_3, REG_EXTENDED );

                do
                {
                    limpiar_terminal();

                    printf("Dirección del paciente\n"
                            "NOTA!: Cambia los campos entre () ignorando todos los parentesis\n"
                            "(Calle) #(numero) (Colonia, Fracc...etc.), (Municipio), (Estado)\n: ");
                    limpiar_buffer_STDIN();
                    fgets( auxiliar_direccion, sizeof( auxiliar_direccion ), stdin );

                    *( auxiliar_direccion + ( strcspn( auxiliar_direccion, "\n" ) ) ) = '\0';

                    regular_1 = regexec( &re_direccion, auxiliar_direccion, 0, NULL, 0 );

                    if ( regular_1 != 0 )

                        validar_errores_por_SO();

                } while ( regular_1 != 0 );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {

                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, auxiliar_direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, auxiliar_nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, auxiliar_direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                    }
                }

                regfree( &re_direccion );

            break;

            case 3: // Sintomas

                regcomp( &re_sintomas, expresion_4, REG_EXTENDED );

                do
                {
                    limpiar_terminal();

                    printf("Síntomas del paciente: ");
                    limpiar_buffer_STDIN();
                    fgets( auxiliar_sintomas, sizeof( auxiliar_sintomas ), stdin );

                    *( auxiliar_sintomas + ( strcspn( auxiliar_sintomas, "\n" ) ) ) = '\0';

                    regular_1 = regexec( &re_sintomas, auxiliar_sintomas, 0, NULL, 0 );

                    if ( regular_1 != 0 )

                        validar_errores_por_SO();

                } while ( regular_1 != 0 );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, auxiliar_sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, auxiliar_sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }

                regfree( &re_sintomas );

            break;

            case 4: // Edad

                do
                {
                    do
                    {
                        limpiar_terminal();

                        printf("Edad del paciente ( Ingrese 0 si es un bebé ): ");
                        limpiar_buffer_STDIN();

                    } while ( scanf( " %d", &auxiliar_edad ) != 1);

                    if ( auxiliar_edad < 0)

                        validar_errores_por_SO();

                    else if ( auxiliar_edad == 0 )

                            auxiliar_es_bebe = 1; // true

                        else

                            auxiliar_es_bebe = 0; // false

                } while ( auxiliar_edad < 0 );

                if ( auxiliar_es_bebe )
                {
                    do
                    {
                        do
                        {
                            limpiar_terminal();

                            printf("Cuántos días tiene de nacid@: ");
                            limpiar_buffer_STDIN();

                        } while ( scanf(" %d", &auxiliar_dias) != 1);

                        if ( auxiliar_dias < 1)

                            validar_errores_por_SO();

                        else if ( auxiliar_dias > 365 )
                            {
                                auxiliar_es_bebe = 0;

                                limpiar_terminal();
                                puts("El/la paciente ya supera el año de nacid@, ya no se considera bebé. . .");
                                pausar_terminal();

                                auxiliar_edad = auxiliar_dias / 365; // Edad aproximada en años
                            }
                            else

                                auxiliar_edad = auxiliar_dias / 30; // Edad aproximada en meses

                    } while ( auxiliar_dias < 1 );
                }

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%s %d %s %d %c %s %d %s %d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, auxiliar_edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, auxiliar_es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, auxiliar_edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, auxiliar_es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }

            break;

            case 5: // Genero

                do
                {
                    limpiar_terminal();

                    printf("Género del paciente\nM - Masculino\nF - Femenino\n O - Otro\n: ");
                    limpiar_buffer_STDIN();
                    scanf(" %c", &auxiliar_genero);

                    auxiliar_genero = toupper(auxiliar_genero);

                    if ( auxiliar_genero != 'M' && auxiliar_genero != 'F' && auxiliar_genero != 'O' )

                        validar_errores_por_SO();

                } while ( auxiliar_genero != 'M' && auxiliar_genero != 'F' && auxiliar_genero != 'O' );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, auxiliar_genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, auxiliar_genero, all->datos.sintomas, all->datos.numero_consultorio, auxiliar_direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }

            break;

            case 6: // # Consultorio

                do
                {
                    do
                    {
                        limpiar_terminal();

                        printf("\n# de Consultorio a asignar ( 1-20 ): ");
                        limpiar_buffer_STDIN();

                    } while ( scanf(" %d", &auxiliar_consultorio) != 1 );

                    if ( auxiliar_consultorio < 1 || auxiliar_consultorio > MAX_CONSULTORIOS )

                        validar_errores_por_SO();

                } while ( auxiliar_consultorio < 1 || auxiliar_consultorio > MAX_CONSULTORIOS );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, auxiliar_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, auxiliar_consultorio, auxiliar_direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }

            break;

            case 7: // Tipo de Servicio

                regcomp( &re_servicio, expresion, REG_EXTENDED );

                do
                {
                    limpiar_terminal();

                    printf("A qué tipo de servicio se da de alta?\n - Externo\n - Emergencia\n: ");
                    limpiar_buffer_STDIN();
                    fgets( auxiliar_tipo_servicio, sizeof( auxiliar_tipo_servicio ), stdin );

                    *( auxiliar_tipo_servicio + ( strcspn( auxiliar_tipo_servicio, "\n" ) ) ) = '\0';

                    convertir_cadena_a_minuscula(auxiliar_tipo_servicio);

                    regular_1 = regexec( &re_servicio, auxiliar_tipo_servicio, 0, NULL, 0 );

                    if ( regular_1 != 0 )

                        validar_errores_por_SO();

                } while ( regular_1 != 0 );

                *( auxiliar_tipo_servicio ) = toupper( *( auxiliar_tipo_servicio ) );

                rewind(all->pacientes);

                if ( *tipo_busqueda == 2) // Busqueda por ID
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1]%d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {
                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( *id_registro == all->datos.numero_registro )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", auxiliar_tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }
                else // Busqueda por nombre
                {
                    while (fscanf(all->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", all->datos.tipo_servicio, &all->datos.numero_registro, all->datos.nombre, &all->datos.edad, &all->datos.genero, all->datos.sintomas, &all->datos.numero_consultorio, all->datos.direccion, &all->datos.es_bebe) != EOF)
                    {

                        if ( ftell( all->temporal ) != 0 )
                        {
                            fprintf( all->temporal, "\n" );
                            fflush( all->temporal );
                        }

                        *(all->datos.tipo_servicio) = toupper( *(all->datos.tipo_servicio) );

                        if ( strcmp( name, all->datos.nombre ) == 0 )
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", auxiliar_tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, auxiliar_direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }
                        else
                        {
                            fprintf( all->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", all->datos.tipo_servicio, all->datos.numero_registro, all->datos.nombre, all->datos.edad, all->datos.genero, all->datos.sintomas, all->datos.numero_consultorio, all->datos.direccion, all->datos.es_bebe);
                            fflush(all->temporal);
                        }

                    }
                }

                regfree( &re_servicio );

            break;
        }

        remove( all->ruta_archivo_pacientes );
        rename( all->ruta_archivo_temporal, all->ruta_archivo_pacientes );

        fclose( all->temporal );

        limpiar_terminal();
        puts(" EDICION REALIZADA CORRECTAMENTE! ");
        pausar_terminal();
    }
}

void listar_pacientes(FILE *file_pacientes, struct Datos_Pacientes *data, const char *dir_pacientes)
{
    file_pacientes = fopen( dir_pacientes, "r" );

    if ( file_pacientes == NULL )

        fprintf(stderr, "ERROR DE FICHEROS, NO SE LEYERON CORRECTAMENTE. . .\n");

    else
    {
        rewind( file_pacientes );

        printf("%-15s%-20s%-10s%-25s%-10s%-30s%-50s%-15s\n\n",
                "# REGISTRO", "TIPO SERVICIO", "GENERO", "NOMBRE", "EDAD", "SINTOMAS", "DOMICILIO", "# CONSULTORIO");

        while ( fscanf(file_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", data->tipo_servicio, &data->numero_registro, data->nombre, &data->edad, &data->genero, data->sintomas, &data->numero_consultorio, data->direccion, &data->es_bebe) != EOF )
        {
            if (data->es_bebe)
            {
                printf("%-15d%-10s%-10c%-10s%-5d meses%-15s%-30s%-5d\n",
                    data->numero_registro, data->tipo_servicio, data->genero, data->nombre,
                    data->edad, data->sintomas, data->direccion, data->numero_consultorio);
            }
            else
            {
                printf("%-15d%-10s%-10c%-10s%-5d años %-15s%-30s%-5d\n",
                    data->numero_registro, data->tipo_servicio, data->genero, data->nombre,
                    data->edad, data->sintomas, data->direccion, data->numero_consultorio);
            }
        }

        fclose( file_pacientes );
    }
}

void baja_pacientes(struct Conjunto_Datos *a_data)
{
    int numero_registro, buscar = 2;
    char respuesta[3];

    do
    {
        limpiar_terminal();

        printf("Desea dar de baja a algun paciente? Si/No\n: ");
        limpiar_buffer_STDIN();
        fgets( respuesta, sizeof(respuesta), stdin );

        convertir_cadena_a_minuscula( respuesta );

        if ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) )

            validar_errores_por_SO();

    } while ( strlen( respuesta ) == 0 || ( strcmp( respuesta, "si" ) != 0 && strcmp( respuesta, "no" ) != 0) );

    if ( strcmp( respuesta, "si") == 0 )
    {
        a_data->pacientes = fopen( a_data->ruta_archivo_pacientes, "r" );

        if (a_data->pacientes == NULL)

            fprintf(stderr, "ERROR DE LECTURA DE FICHEROS. . .\n");

        else
        {
            do
            {
                do
                {
                    limpiar_terminal();

                    printf("Digita el # de registro del paciente a dar de baja: ");
                    limpiar_buffer_STDIN();
                } while ( scanf( "%d", &numero_registro ) != 1 );

                if ( numero_registro < 1 )

                    validar_errores_por_SO();

            } while ( numero_registro < 1 );

            if ( !buscar_numero_nombre(a_data->pacientes, &a_data->datos, &numero_registro, NULL, &buscar) )
            {
                a_data->temporal = fopen( a_data->ruta_archivo_temporal, "w" );

                if ( a_data->temporal == NULL )

                    fprintf(stderr, "ERROR DE FICHEROS, INTENTE MAS TARDE. . .\n");

                else
                {
                    rewind( a_data->pacientes );

                    while (fscanf(a_data->pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d\n", a_data->datos.tipo_servicio, &a_data->datos.numero_registro, a_data->datos.nombre, &a_data->datos.edad, &a_data->datos.genero, a_data->datos.sintomas, &a_data->datos.numero_consultorio, a_data->datos.direccion, &a_data->datos.es_bebe) != EOF)
                    {
                        if ( ftell( a_data->temporal ) != 0  )
                        {
                            fprintf( a_data->temporal, "\n" );
                            fflush( a_data->temporal );
                        }

                        if ( a_data->datos.numero_registro != numero_registro )
                        {
                            fprintf( a_data->temporal, "%-s %-d %-s %-d %-c %-s %-d %-s %-d", a_data->datos.tipo_servicio, a_data->datos.numero_registro, a_data->datos.nombre, a_data->datos.edad, a_data->datos.genero, a_data->datos.sintomas, a_data->datos.numero_consultorio, a_data->datos.direccion, a_data->datos.es_bebe);
                            fflush( a_data->temporal );
                        }
                    }

                    remove( a_data->ruta_archivo_pacientes );
                    rename( a_data->ruta_archivo_temporal, a_data->ruta_archivo_pacientes );

                    fclose( a_data->temporal );

                    limpiar_terminal();
                    puts("  EL PACIENTE HA SIDO DADO DE BAJA! ");
                }
            }
            else
            {
                limpiar_terminal();

                puts("No se encontró al paciente en nuestro sistema. . .");
            }
        }
    }
}

bool buscar_numero_nombre(FILE *f_pacientes, struct Datos_Pacientes *data, const int *numero_ingresado, const char *nombre_ingresado, const int *buscar)
{

    rewind( f_pacientes );

    if ( *buscar == 1 ) // Funcion alta_pacientes
    {
        struct Datos_Pacientes temp;

        while ( fscanf(f_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", temp.tipo_servicio, &temp.numero_registro, temp.nombre, &temp.edad, &temp.genero, temp.sintomas, &temp.numero_consultorio, temp.direccion, &temp.es_bebe) != EOF )
        {
            if ( temp.numero_registro == (*numero_ingresado) )

                return false; // Numero de registro ocupado

        }
    }
    else if ( *buscar == 2 ) // Funcion editar_buscar (id) - Baja Pacientes
        {
            while ( fscanf(f_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", data->tipo_servicio, &data->numero_registro, data->nombre, &data->edad, &data->genero, data->sintomas, &data->numero_consultorio, data->direccion, &data->es_bebe) != EOF )
            {
                if ( data->numero_registro == (*numero_ingresado) )

                    return false; // Numero de registro encontrado

            }
        }
        else // Funcion editar_buscar (nombre)
        {
            while ( fscanf(f_pacientes, "%15[^0-9] %d %100[^0-9] %d %c %50[^0-9] %d %200[^0-1] %d", data->tipo_servicio, &data->numero_registro, data->nombre, &data->edad, &data->genero, data->sintomas, &data->numero_consultorio, data->direccion, &data->es_bebe) != EOF )
            {

                if ( strcmp( data->nombre, nombre_ingresado ) == 0 )

                    return false; // Nombre registrado

            }
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