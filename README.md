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

### Compilación

Para ejecutar la aplicación, primero debe compilar el código fuente utilizando el siguiente comando:

```bash
make
```

Este comando creará un directorio llamado `bin` donde se almacenarán los ejecutables. En caso de
que desee eliminar este directorio, puede utilizar el siguiente comando:

```bash
make clean
```

### Ejecución

Una vez compilados, puede ejecutar alguno de los siguientes programas disponibles:

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

- `PROFILE_FILE`: que especifica el archivo donde se almacenan los perfiles y sus permisos respectivos. Por defecto, el valor de esta variable es `data/perfiles.txt` que creará un directorio llamado `data` si no existe.
