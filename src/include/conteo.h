#ifndef CONTEO_H
#define CONTEO_H

#include <string>
#include <vector>

wchar_t normalizarVocal(wchar_t c);
std::vector<int> contar(std::string archivoTexto);
void imprimirConteo(std::string archivoTexto);

#endif
