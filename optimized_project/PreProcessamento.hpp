
#ifndef PREPROCESSAMENTO_HPP
#define PREPROCESSAMENTO_HPP

#include "config.hpp"

extern mutex mtx;

void loadInput();

void bloco_leitura(Usuarios &local, const std::unordered_map<int, pair<streampos, streampos>> &intervalos,
vector<int> Users, std::unordered_map<int, unsigned long> &AvalUser, std::unordered_map<int, unsigned long> &Filme_cont);

void bloco_filtro(Usuarios &mapa, const std::unordered_map<int, unsigned long> &User_cont, const std::unordered_map<int, unsigned long> &Filme_cont, vector<string> &bloco);

void UserIntervalos(std::unordered_map<int, pair<streampos, streampos>> &intervalo, streampos inicio, streampos fim);

string formatarNota(float nota);

#endif


