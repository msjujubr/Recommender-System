
#pragma once

#include "config.hpp"

// Gera arquivo explore.dat com uma amostra de usuários
void gerarExploreFile(const string& input_file, const string& explore_file, int sample_size = 1000);
void gerarExploreFileFromVector(const vector<int>& users, const string& explore_file, int sample_size);

// Diferentes medidas de similaridade
float similaridade_jaccard(const std::unordered_map<int, float>& userA, const std::unordered_map<int, float>& userB);
float similaridade_euclidiana(const std::unordered_map<int, float>& userA, const std::unordered_map<int, float>& userB);

// Enum para tipo de similaridade
enum class SimilarityType {
    COSSENO,
    JACCARD,
    EUCLIDIANA
};





