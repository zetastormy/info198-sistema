#!/bin/bash
makefile_progs=( "src/cpp/admin_usuarios" "src/cpp/pgm" "src/cpp/multi" "src/cpp/indice_invertido" "src/cpp/indice_invertido_paralelo" "src/cpp/cache_service" "src/cpp/search" "src/cpp/search_engine" )
cmake_progs=( "src/cpp/juego" )

for makefile_prog in "${makefile_progs[@]}"
do
    echo "---= COMPILANDO $makefile_prog =---"
    make -C $makefile_prog
    echo ""
done

for cmake_prog in "${cmake_progs[@]}"
do
    echo "---= COMPILANDO $cmake_prog =---"
    mkdir -p build
    cmake -S $cmake_prog -B build
    cmake --build build
    echo ""
done

echo "---= COMPILACIÓN TERMINADA =---"
echo "Si no hay errores, los binarios de cada programa estarán en bin/."
echo "Lea README.md para más información."
