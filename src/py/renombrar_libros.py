import os


def main():
    """
    Script para renombrar libros, debido a que actualmente nuestros libros no
    poseen su título dentro del nombre del archivo, lo cual puede causar problemas
    al mostrar resultados de búsqueda.
    """

    pid = os.getpid()

    print(f"---= RENOMBRADOR DE LIBROS (PID: {pid}) =---")

    books_dir = input("Ingrese la ruta de la carpeta donde tiene los libros: ")

    for book in os.listdir(books_dir):
        title = ""

        with open(os.path.join(books_dir, book), "r") as book_content:
            for line in book_content:
                if line.startswith("Title:"):
                    title = line[7:].replace("\n", "")
                    break

        os.rename(
            os.path.join(books_dir, book), os.path.join(books_dir, title + ".txt")
        )


if __name__ == "__main__":
    main()
