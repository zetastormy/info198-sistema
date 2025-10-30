#!/bin/bash
dir_progs=( "src/admin_usuarios" "src/pgm" "src/multi" "src/indice_invertido" "src/indice_invertido_paralelo")

for dir_prog in "${dir_progs[@]}"
do
    echo "---= COMPILANDO $dir_prog =---"
    make -C $dir_prog
    echo ""
done

echo "---= COMPILACIÓN TERMINADA =---"
echo "Si no hay errores, los binarios de cada programa estarán en bin/."
echo "Lea README.md para más información."
