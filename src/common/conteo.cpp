#include "../include/conteo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <sstream>

wchar_t normalizarVocal(wchar_t c) {
    switch (c) {
        case L'á': case L'Á': return L'a';
        case L'é': case L'É': return L'e';
        case L'í': case L'Í': return L'i';
        case L'ó': case L'Ó': return L'o';
        case L'ú': case L'Ú': return L'u';
        default: return c;
    }
}

std::vector<int> contar(std::string archivoTexto) {

    std::wifstream archivo(archivoTexto);
    std::wstring linea;
    archivo.imbue(std::locale("es_CL.UTF-8"));

    std::vector<wchar_t> vocales = {L'a', L'e', L'i', L'o', L'u'};
    int n_palabras = 0;
    int n_vocales = 0;
    int n_consonantes = 0;
    int n_especiales = 0;

    while (getline(archivo, linea)) {

        std::wistringstream iss(linea);
        std::wstring palabra;
        while (iss >> palabra)
            n_palabras++;

        for (wchar_t caracter : linea) {
            caracter = normalizarVocal(caracter);
            if (std::iswalpha(caracter)) {
                wchar_t c = towlower(caracter);
                if (find(vocales.begin(), vocales.end(), c) != vocales.end())
                    n_vocales++;
                else
                    n_consonantes++;
            } else if (!iswspace(caracter) && !iswdigit(caracter)) {
                n_especiales++;
            }
        }
    }

    archivo.close();
    return {n_vocales, n_consonantes, n_especiales, n_palabras};
}

void imprimirConteo(std::string archivoTexto){
    if (!std::filesystem::exists(archivoTexto)) {
        std::cout << "(ERROR) El archivo " << archivoTexto << " no existe"<< std::endl;
        return;
    }
    std::vector <int> conteo = contar(archivoTexto);
    std::cout << "El texto tiene " << std::endl;
    std::cout << "• " << conteo[0] << " vocales" << std::endl;
    std::cout << "• " << conteo[1] <<" consonantes" <<std::endl;
    std::cout << "• " << conteo[2] <<" caracteres especiales" <<std::endl;
    std::cout << "• " << conteo[3] <<" palabras" <<std::endl;
}
