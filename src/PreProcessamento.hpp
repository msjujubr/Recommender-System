
#ifndef PREPROCESSAMENTO_HPP
#define PREPROCESSAMENTO_HPP

#include "config.hpp"

extern mutex mtx;

void loadInput();

char *mmapArquivo(const string &caminho, size_t &tamanho);
void UserIntervalos(unordered_map<int, pair<size_t, size_t>> &intervalo, size_t inicio, size_t fim, char *base);
void bloco_leitura(Usuarios &local, const unordered_map<int, pair<size_t, size_t>> &intervalos,
    const vector<int> &users, unordered_map<int, int> &AvalUser,
    unordered_map<int, int> &UserFilme, char *base);
void bloco_filtro(Usuarios &mapa, const unordered_map<int, int> &User_cont,
    const unordered_map<int, int> &Filme_cont, vector<string> &bloco);

string formatarNota(float nota);
void escreverArquivo(const vector<string> &linhas, const string &caminho);

#endif