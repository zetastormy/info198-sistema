# Administrador de usuarios

## Propósito de la aplicación
Este sistema de administración de usuarios es una herramienta diseñada para facilitar la gestión y control de usuarios
en una organización o sistema informático. Permite crear cuentas de usuario con diferentes perfiles, especificando
atributos como nombre, nombre de usuario, contraseña, así como también permite eliminarlos y listar los distintos
usuarios almacenados.

### Funcionalidades principales
- Creación de usuarios con diferentes perfiles.
- Listado de usuarios.
- Eliminación de usuarios.

## ¿Cómo usar la aplicación?

### Compilación

Para ejecutar la aplicación, primero debe compilar el código fuente utilizando el siguiente comando:

```bash
make
```

Este comando creará un directorio llamado `bin` donde se almacenará el ejecutable compilado llamado `prog`. En caso de
que desee eliminar este directorio, puede utilizar el siguiente comando:

```bash
make clean
```

### Ejecución

Después de compilar el código, puede ejecutar la aplicación utilizando el siguiente comando:

```bash
./prog
```

Este desplegará una interfaz de línea de comandos (CLI) que le permitirá trabajar con las distintas funciones ofrecidas
por el sistema.

### Variables de entorno

El programa dispone de una variable de entorno llamada `USER_FILE` que especifica el archivo donde se almacenan los usuarios. Esta puede ser modificada de acuerdo a las necesidades que se requieran. Por defecto, el valor de esta variable es `data/usuarios.txt` que creará un directorio llamado `data` si no existe.
