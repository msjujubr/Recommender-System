#ifndef PREPROCESSAMENTO_HPP
#define PREPROCESSAMENTO_HPP

#include "config.hpp"

#include <iomanip>


extern mutex mtx;

void loadInput();

// DÁ PRA MELHORAR
void bloco_leitura(Usuarios &local, const unordered_map<int, pair<streampos, streampos>> &intervalos,
vector<int> Users, unordered_map<int, int> &AvalUser, unordered_map<int, int> &UserFilme);
void bloco_filtro(Usuarios &mapa, const unordered_map<int, int> User_cont, const unordered_map<int, int> Filme_cont, vector<string> &bloco);
void UserIntervalos(unordered_map<int, pair<streampos, streampos>> &intervalo, streampos inicio, streampos fim);

string formatarNota(float nota);

#endif