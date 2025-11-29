#!/bin/bash
makefile_progs=( "src/admin_usuarios" "src/pgm" "src/multi" "src/indice_invertido" "src/indice_invertido_paralelo" "src/cache_service" )
cmake_progs=( "src/juego" )

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
