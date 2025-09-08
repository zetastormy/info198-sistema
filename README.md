# Administrador de usuarios

## Propósito de la aplicación
Este es un sistema multipropósito el cual está diseñado para facilitar la gestión y control de usuarios
en una organización o sistema informático, así como también realizar otras funciones que puedan ser de interés para los usuarios de este mismo. Permite crear cuentas de usuario con diferentes perfiles, especificando
atributos como nombre, nombre de usuario, contraseña, así como también permite eliminarlos y listar los distintos
usuarios almacenados.

### Funcionalidades principales
- Creación de usuarios con diferentes perfiles.
- Listado de usuarios.
- Eliminación de usuarios.
- Multiplicación de matrices.
- Verificación de palíndromo.
- Calcular función `f(x) = x^2 + 2x + 8` con parámetro personalizado.

## ¿Cómo usar la aplicación?

### Requisitos

- Distribución de Linux moderna
- Bash (>=5.3)
- GNU Make (>=4.4)
- Lenguaje `es_CL.UTF-8` ([Más información](https://wiki.archlinux.org/title/Locale))

### Compilación

Para facilitar la compilación de los programas, se ha puesto a su disposición el script `build.sh`, este ejecutará
el comando `make` en los directorios de cada uno de los programas donde se encuentra su respectivo Makefile. Simplemente ejecute el script de la siguiente manera:

```bash
./build.sh
```

Posteriormente, podrá encontrar el ejecutable de cada programa en la carpeta `bin/` generada.

### Ejecución

Desde el directorio raíz del programa (donde está ubicado el archivo `.env`), usted podrá ejecutar los programas de la siguientes maneras:

- **Administrador de usuarios:**

  ```bash
  ./bin/admin_usuarios
  ```

- **Programa principal:**

  ```bash
  ./bin/pgm -u <usuario> -p <contraseña> -f <ruta a texto a evaluar>
  ```

- **Multiplicador de matrices:**

  ```bash
  ./bin/multi <ruta archivo matriz A> <ruta archivo matriz B> <separador>
  ```

Este desplegará una interfaz de línea de comandos (CLI) que le permitirá trabajar con las distintas funciones ofrecidas
por el sistema.

### Variables de entorno

El programa dispone de múltiples variables de entorno para facilitar la manipulación de las rutas de los archivos sin necesidad de modificar el código:

- `USER_FILE`: que especifica el archivo donde se almacenan los usuarios. Por defecto, el valor de esta variable es `data/usuarios.txt` que creará un directorio llamado `data` si no existe.

- `PROFILE_FILE`: que especifica el archivo donde se almacenan los perfiles y sus permisos respectivos. Este archivo se encuentra presente en el repositorio en `config/perfiles.txt`, si este no se encuentra presente, el programa principal no manejará permisos.
