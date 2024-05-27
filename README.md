## PIA DE LABORATORIO DE PROGRAMACIÓN ESTRUCTURADA

### Instrucciones: 

Desarrollar un programa para llevar a cabo los procesos básicos de un hospital, tales como el alta, la modificación y eliminación del expediente médico de un paciente utilizando archivos. Al usuario se le presentará el siguiente menú: 

*1. Alta de paciente.* 

*2. Búsqueda o edición de paciente.*

*3. Listado de pacientes.* 

*4. Baja de paciente.* 

*5. Salir*



 **Alta de paciente** 

El usuario podrá registrar un nuevo paciente indicando el tipo de servicio (consulta externa o emergencia), nombre, dirección, edad, género, descripción de los síntomas y el número del consultorio del doctor disponible que lo atenderá. La información proporcionada se almacenará en un archivo, se le asignará un número al registro y se le deberá mostrar al usuario este número de registro. 



**Búsqueda o edición de paciente**

El usuario puede buscar o editar a través del número de registro del paciente o su nombre completo. Se le mostrará toda la información correspondiente número de registro proporcionado. Si el número de registro está equivocado, el programa muestra un mensaje que indica que no hay registros disponibles. 



**Listado de pacientes** 

Este listado se deberá obtener directamente del archivo de texto y se mostrarán los datos de todos los pacientes. 



**Baja de pacientes** 

Esta opción servirá para eliminar el registro de un paciente.


**Salir**

Terminar la ejecución del programa


**_NOTA: Antes de seguir a la compilacion y ejecucion, asegurarse de tener instalado GIT y el compilador GCC_**

En caso de no tener GIT y/o GCC, abra cualquiera de las dos pestañas segun su sistema, por el contrario, hacer caso omiso:

[Instalar GIT en Windows](https://git-scm.com/download/win)

[Instalar compilador GCC en Windows](https://www.msys2.org/#installation)

[Instalar GIT en Linux](https://git-scm.com/download/linux)

[Instalar compilador GCC en Linux](https://redessy.com/como-instalar-y-usar-el-compilador-gcc-en-un-sistema-linux/)

### Para compilar y ejecutar en UNIX/Linux

```bash
git clone https://github.com/Angus17/PIA_LABORATORIO.git
cd PIA_LABORATORIO
gcc PIA.c -o PIA # Compila y crea el ejecutable
./PIA #Iniciar el ejecutable del programa
```
### Para compilar y ejecutar en Windows

```powershell
git clone https://github.com/Angus17/PIA_LABORATORIO.git
cd PIA_LABORATORIO
gcc PIA.c -o PIA.exe -lregex # Compila y crea el ejecutable
./PIA.exe #Iniciar el ejecutable del programa

```
