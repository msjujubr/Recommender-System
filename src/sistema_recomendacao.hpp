#ifndef SISTEMA_RECOMENDACAO_HPP
#define SISTEMA_RECOMENDACAO_HPP

#include "config.hpp"
#include "PreProcessamento.hpp"
#include <random>

// Tipo para a assinatura LSH de um usuário (vetor de bits)
using LSHSignature = vector<bool>;
using LSHBuckets = vector<unordered_map<string, vector<int>>>; 

struct LSHIndex {
    vector<vector<float>> random_hyperplanes;
    unordered_map<int, LSHSignature> user_signatures; // userID -> assinatura completa
    LSHBuckets lsh_buckets; // Os baldes LSH
    unordered_map<int, size_t> filme_id_to_index; // Mapeia filmeID para o índice no vetor de hiperplano
    vector<int> unique_movies_ordered; // Lista ordenada de filmeIDs (para mapear índices)
};

using matrizUsuarios = unordered_map<int, unordered_map<int, float>>; // usuário -> (filme -> nota)

void sistema_recomendacao();

pair<matrizUsuarios, pair<vector<int>, unordered_map<int, size_t>>> criarMatrizUsuarios(string &arquivo);
void normalizarMatriz(matrizUsuarios& matriz);
void imprimirPrimeirosUsuarios(const matrizUsuarios& matriz, const std::vector<int>& filmes_unicos, int limite_usuarios = 10, int limite_filmes = 5);

float similaridadeCosseno(const unordered_map<int, float>& usuarioA, const unordered_map<int, float>& userB);
// Funções LSH
LSHIndex construirIndiceLSH(const matrizUsuarios& userMatrix, const vector<int>& uniqueMovies, const unordered_map<int, size_t>& filme_id_to_index);

std::vector<int> encontrarCandidatosLSH(int query_userID, const LSHIndex& lsh_index);

void gerarRecomendacoesLSH(const matrizUsuarios& userMatrix, const LSHIndex& lsh_index, const string& explore_file, const string& output_file);


#endif