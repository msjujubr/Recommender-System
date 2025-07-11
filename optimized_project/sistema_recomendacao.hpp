
#pragma once

#include "pch.hpp"
#include "config.hpp"

// Definições de tipos
typedef std::unordered_map<int, std::unordered_map<int, float>> matrizUsuarios;
typedef std::vector<bool> LSHSignature;

struct LSHIndex {
    std::vector<std::vector<float>> random_hyperplanes;
    std::unordered_map<int, LSHSignature> user_signatures;
    std::vector<std::unordered_map<std::string, std::vector<int>>> lsh_buckets;
    std::unordered_map<int, size_t> filme_id_to_index;
    std::vector<int> unique_movies_ordered;
};

// Funções do sistema de recomendação
void sistema_recomendacao();
pair<matrizUsuarios, pair<vector<int>, std::unordered_map<int, size_t>>> criarMatrizUsuarios(const string &arquivo);
void normalizarMatriz(matrizUsuarios& matriz);
float similaridade_cosseno(const std::unordered_map<int, float>& userA, const std::unordered_map<int, float>& userB);
LSHIndex construirIndiceLSH(const matrizUsuarios& userMatrix, const vector<int>& uniqueMovies, const std::unordered_map<int, size_t>& filme_id_to_index);
vector<int> encontrarCandidatosLSH(int query_userID, const LSHIndex& lsh_index);
void gerarRecomendacoesLSH(const matrizUsuarios& userMatrix, const LSHIndex& lsh_index, const string& explore_file, const string& output_file);




