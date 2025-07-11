
#include "utils.hpp"
#include <random>
#include <algorithm>

void gerarExploreFile(const string& input_file, const string& explore_file, int sample_size) {
    std::unordered_map<int, bool> unique_users;
    
    ifstream input(input_file);
    string linha;
    
    // Coletar todos os usuários únicos
    while (getline(input, linha)) {
        istringstream iss(linha);
        int userID;
        if (iss >> userID) {
            unique_users.emplace(userID, true);
        }
    }
    
    // Converter para vector para sampling
    vector<int> all_users;
    all_users.reserve(unique_users.size());
    for (auto const& [key, value] : unique_users) {
        all_users.push_back(key);
    }
    
    // Sample aleatório
    random_device rd;
    mt19937 gen(rd());
    shuffle(all_users.begin(), all_users.end(), gen);
    
    int actual_sample = min(sample_size, static_cast<int>(all_users.size()));
    
    // Escrever explore.dat
    ofstream explore_output(explore_file);
    for (int i = 0; i < actual_sample; ++i) {
        explore_output << all_users[i];
        if (i < actual_sample - 1) explore_output << "\n";
    }
    explore_output.close();
    
    cout << "Arquivo explore.dat criado com " << actual_sample << " usuários" << endl;
}

void gerarExploreFileFromVector(const vector<int>& users, const string& explore_file, int sample_size) {
    vector<int> all_users = users;
    
    // Sample aleatório
    random_device rd;
    mt19937 gen(rd());
    shuffle(all_users.begin(), all_users.end(), gen);
    
    int actual_sample = min(sample_size, static_cast<int>(all_users.size()));
    
    // Escrever explore.dat
    ofstream explore_output(explore_file);
    for (int i = 0; i < actual_sample; ++i) {
        explore_output << all_users[i];
        if (i < actual_sample - 1) explore_output << "\n";
    }
    explore_output.close();
    
    cout << "Arquivo explore.dat criado com " << actual_sample << " usuários" << endl;
}

float similaridade_jaccard(const std::unordered_map<int, float>& userA, const std::unordered_map<int, float>& userB) {
    std::unordered_map<int, bool> intersecao;
    std::unordered_map<int, bool> uniao;
    
    // Adicionar filmes do usuário A
    for (auto const& [key, value] : userA) {
        uniao.emplace(key, true);
    }
    
    // Processar filmes do usuário B
    for (auto const& [key, value] : userB) {
        if (userA.count(key)) {
            intersecao.emplace(key, true);
        }
        uniao.emplace(key, true);
    }
    
    if (uniao.empty()) return 0.0f;
    return static_cast<float>(intersecao.size()) / static_cast<float>(uniao.size());
}

float similaridade_euclidiana(const std::unordered_map<int, float>& userA, const std::unordered_map<int, float>& userB) {
    float soma_quadrados = 0.0f;
    
    // Iterar sobre usuário com menos filmes
    const auto& smaller = (userA.size() < userB.size()) ? userA : userB;
    const auto& larger = (userA.size() < userB.size()) ? userB : userA;
    
    for (auto const& [key, value] : smaller) {
        auto other_rating = larger.find(key);
        if (other_rating != larger.end()) {
            float diff = value - other_rating->second;
            soma_quadrados += diff * diff;
        } else {
            // Assumir rating 0 para filmes não avaliados
            soma_quadrados += value * value;
        }
    }
    
    // Converter distância em similaridade (inverso normalizado)
    return 1.0f / (1.0f + sqrt(soma_quadrados));
}







