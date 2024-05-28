#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <regex.h>
#include <ctype.h>
#include <string.h>

#if defined(__linux__) || defined(unix)

    #include <stdio_ext.h>
    #include <unistd.h>

#elif defined(_WIN32) || defined(_WIN64)

    #include <windows.h>

#endif

#define MAX_BYTES 1000
#define MAX_PACIENTES 200
#define MAX_CONSULTORIOS 20