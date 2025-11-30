# Sistema multipropósito

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
- Conteo de palabras en un archivo de texto.
- Creador de índices invertidos (con o sin paralelización).
- Juego de dados multijugador.

## Primeros pasos

### Requisitos

- Distribución de Linux moderna
- Bash (>=5.3)
- GNU Make (>=4.4)
- CMake (>=4.1.2)
- Lenguaje `es_CL.UTF-8` ([Más información](https://wiki.archlinux.org/title/Locale))

### Compilación

Para facilitar la compilación de todos los componentes del sistema, se ha puesto a su disposición el script `build.sh`. Este script automatiza el proceso de construcción de la siguiente manera:

1. **Herramientas de consola:** Ejecuta `make` en los directorios correspondientes (`admin_usuarios`, `pgm`, `multi`, `indice_invertido`, `cache_service`, etc.).
2. **Juego Multijugador:** Configura y compila el proyecto utilizando `CMake` (generando una carpeta temporal `build`).

Simplemente ejecute el script de la siguiente manera:

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

- **Programa principal (contiene todo menos el administrador de usuarios):**

  ```bash
  ./bin/pgm -u <usuario> -p <contraseña> -f <ruta a texto a evaluar>
  ```

- **Multiplicador de matrices:**

  ```bash
  ./bin/multi "<ruta archivo matriz A>" "<ruta archivo matriz B>" "<separador>"
  ```

- **Creador de índices invertidos:**

  ```bash
  ./bin/indice_invertido <nombre archivo> "<ruta directorio libros>"
  ```

- **Creador de índices invertidos (paralelo):**

  ```bash
  ./bin/indice_invertido_paralelo <nombre archivo> "<ruta directorio libros>"
  ```

- **Juego de dados multijugador:**

  ```bash
  ./bin/juego
  ```

### Variables de entorno

El programa dispone de múltiples variables de entorno para facilitar la manipulación de las rutas de los archivos sin necesidad de modificar el código:

- `USER_FILE`: especifica el archivo donde se almacenan los usuarios. Por defecto, el valor de esta variable es `data/usuarios.txt` que creará un directorio llamado `data` si no existe.

- `PROFILE_FILE`: especifica el archivo donde se almacenan los perfiles y sus permisos respectivos. Este archivo se encuentra presente en el repositorio en `config/perfiles.txt`, si este no se encuentra presente, el programa principal no manejará permisos.

- `ADMIN_SYS`: especifica el lugar donde está alojado el binario al sistema  administrador de usuarios.

- `MULTI_M`: especifica el lugar donde está alojado el binario al sistema multiplicador de matrices.

- `CREATE_INDEX`: especifica el lugar donde está alojado el binario al sistema creador de índices invertidos.

- `INDICE_INVET_PARALELO`: especifica el lugar donde está alojado el binario al sistema creador de índices invertidos paralelo.

- `GAME`: especifica el lugar donde está alojado el binario del juego multijugador.

- `CACHE`: especifica el lugar donde está alojado el binario del servicio de caché.

- `BUSCADOR`: especifica el lugar donde está alojado el binario del servicio de búsqueda.

- `N_THREADS`: especifica el número de hilos a utilizar en la creación de índice invertido de forma paralela.

- `N_LOTE`: especifica el tamaño del lote de libros que se mantendrán en memoria al crear el índice invertido de forma paralela.

- `MAPA_LIBROS`: especifica la ubicación del archivo donde se almacenan los ID asociados a cada libro.

- `WIN_THRESHOLD`: especifica el número de puntos necesarios para ganar el juego.

- `CACHE_SIZE`: determina el tamaño máximo que puede almacenar el servicio de caché.

- `CACHE_PORT`: define el puerto TCP donde se expone el servicio de caché.

- `SEARCH_PORT`: define el puerto TCP donde se expone el servicio de búsqueda.

- `GAME_PORT`: define el puerto TCP donde está corriendo el servidor del juego.

- `TOPK`: especifica la cantidad máxima de resultados más relevantes que entregará el motor de búsqueda al realizar una consulta.

- `CARPETA_GRAFICOS`: especifica la carpeta donde se guardarán los gráficos generados por los análisis de rendimiento.

- `LOG_RENDIMIENTO`: indica la ruta del archivo donde se registrará la información de rendimiento.

- `LOG_JUEGO`: indica la ruta del archivo donde se registrará la actividad del juego.

- `LOG_INDICE`: indica la ruta del archivo donde se registrará estadísticas de la creación de índices en paralelo.

## Uso del sistema

### Programa principal

El programa principal es ejecutado usando el comando que se específico en la sección "Ejecución". Este recibe 3 parámetros, un usuario y contraseña (deben ser creados en el administrador de usuarios) y un archivo de texto el cual podrá analizar dentro del programa. Posterior a ello, se mostrará un menú en el cuál usted tendrá que seleccionar una opción usando su respectivo número. A continuación, se detallan las opciones disponibles:

- **Salir (`0`):** Salir del sistema.
- **Administrador de usuarios (`1`):** Abre el administrador de usuarios.
- **Multiplicación de matrices cuadradas (`2`):** Abre el multiplicador de matrices cuadradas, el cual pedirá 2 archivos de matrices para multiplicar.
- **Juego (`3`):** Abre el juego multijugador para tirar dados.
- **¿Es palíndromo? (`4`):** Recibe una palabra y verifica si esta es palíndromo o no.
- **Calcula `f(x) = x² + 2x + 8` (`5`):** Recibe un número y calcula el resultado de la función respecto a este.
- **Conteo sobre texto (`6`):** Utiliza el archivo de texto pasado por parámetros para contar palabras, vocales, consonantes y caracteres especiales.
- **Crea índice invertido (`7`):** Genera un índice invertido de las palabras contenidas en una carpeta de libros (en formato de texto).
- **Crea índice invertido paralelo (`8`):** Genera un índice invertido de las palabras contenidas en una carpeta de libros (en formato de texto) de forma paralela.

### Juego multijugador

Este es un juego de dados por equipos donde al menos **2 equipos**, cada uno compuesto por al menos **2 jugadores**, competirán por ser los primeros en alcanzar un total de **puntos** definidos en `.env` por la variable `WIN_THRESHOLD`. En cada turno, un jugador lanzará un "dado" virtual, obteniendo un resultado aleatorio del 1 al 6 que se sumará a la puntuación de su equipo.

#### Preparación (Jugador Anfitrión)

1. El jugador que creará la partida (anfitrión) necesita conocer su dirección IP local para que los demás puedan conectarse.

2. En Linux, puede encontrarla abriendo una terminal y ejecutando el comando:

  ```bash
  ip addr show wlan0
  ```

  (O el nombre de tu interfaz de red activa, como `eth0` si usas cable).

3. Busca la línea que empieza con `inet` y anota la dirección IP que aparece (por ejemplo, `192.168.1.10`).

4. Comparte esta dirección IP con los otros 3 jugadores.

5. Ingresala en el campo correspondiente de tu cliente.

6. Ingresa un nombre de usuario para identificarte en la partida.

7. Apreta el botón de "Crear partida".

8. Ingresa el nombre del equipo al que deseas unirte o crea uno nuevo.

9. Espera que ingrese el resto de los jugadores y luego presiona el botón de "Empezar juego".

#### Conexión (resto de jugadores)

1. Cada jugador deberá iniciar el cliente del juego, ingresando la dirección IP proporcionada por el anfitrión cuando se le solicite.

2. Ingresa un nombre de usuario para identificarte en la partida.

3. Apreta el botón de "Unirse a partida".

4. Ingresa el nombre del equipo al que deseas unirte o crea uno nuevo.

5. Espera a que el anfitrión inicie la partida.

#### Desarrollo de la partida

1. Una vez que al menos 4 jugadores estén conectados en mínimo 2 equipos, el juego podrá ser iniciado por el anfitrión.

2. **Condición de inicio:** Mínimo 2 equipos de mínimo 2 jugadores cada uno.

3. El equipo que inicia la partida se decide de forma aleatoria.

4. Los jugadores se turnan para lanzar el dado virtual. El resultado se suma a la puntuación de su equipo.

5. **Condición de término:** El juego finaliza inmediatamente cuando uno de los equipos alcanza o supera los `WIN_THRESHOLD` puntos. ¡Ese equipo es el ganador!
