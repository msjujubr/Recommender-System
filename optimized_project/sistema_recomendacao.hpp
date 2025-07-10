
#ifndef SISTEMA_RECOMENDACAO_HPP
#define SISTEMA_RECOMENDACAO_HPP

#include "config.hpp"

// Forward declarations
class PreProcessamento;

// Tipo para a assinatura LSH de um usuário (vetor de bits)
using LSHSignature = vector<bool>;
using LSHBuckets = vector<unordered_map<string, vector<int>>>; 

struct LSHIndex {
    vector<vector<float>> random_hyperplanes;
    unordered_map<int, LSHSignature> user_signatures;
    LSHBuckets lsh_buckets;
    unordered_map<int, size_t> filme_id_to_index;
    vector<int> unique_movies_ordered;
};

void sistema_recomendacao();

pair<matrizUsuarios, pair<vector<int>, std::unordered_map<int, size_t>>> criarMatrizUsuarios(string &arquivo);
void normalizarMatriz(matrizUsuarios& matriz);

float similaridade_cosseno(const std::unordered_map<int, float>& usuarioA, const std::unordered_map<int, float>& userB);

// Funções LSH
LSHIndex construirIndiceLSH(const matrizUsuarios& userMatrix, const vector<int>& uniqueMovies, const std::unordered_map<int, size_t>& filme_id_to_index);

std::vector<int> encontrarCandidatosLSH(int query_userID, const LSHIndex& lsh_index);

void gerarRecomendacoesLSH(const matrizUsuarios& userMatrix, const LSHIndex& lsh_index, const string& explore_file, const string& output_file);

#endif


