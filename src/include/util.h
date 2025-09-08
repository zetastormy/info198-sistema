#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
using namespace std;

struct usuario {
    int id;
    char nombre[21];
    char username[21];
    char password[21];
    char perfil[21];
};

void esperarTecla();
void cargarUsuarios(vector<usuario>& usuarios, string archivoUsuarios);
bool esEntero(string id);

#endif
