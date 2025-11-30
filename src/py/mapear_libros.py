import os


def main():
    """
    Script para generar un mapa de libro, asociando una ID a cada título de la carpeta libros
    que especifique el usuario.
    """

    pid = os.getpid()

    print(f"---= GENERADOR DE MAPA DE LIBROS (PID: {pid}) =---")

    books_dir = input("Ingrese la ruta de la carpeta donde tiene los libros: ")
    books_map_name = input("Ingrese nombre de archivo para guardar mapa de libros: ")

    id = 1
    books_map = []

    for book in os.listdir(books_dir):
        books_map.append((id, book.replace(".txt", "")))
        id += 1

    with open(books_map_name, "w") as books_map_file:
        for id, book in books_map:
            books_map_file.write(f"{id};{book}\n")

    print(f"Se ha guardado el mapa de libros en {books_map_name} exitosamente.")


if __name__ == "__main__":
    main()
