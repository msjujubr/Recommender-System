#include "pch.hpp"
#include "config.hpp"
#include "PreProcessamento.hpp"
#include "sistema_recomendacao.hpp"
#include "utils.hpp"

using namespace std;
using namespace std::chrono;

int main() {
    system("mkdir -p outcome");

    // Pre-processamento
    auto start_total = high_resolution_clock::now();
    sistema_recomendacao(); // Isso agora apenas carrega e filtra os dados para input.dat

    // Criar a matriz de usuários
    cout << "Criando a matriz de usuários..." << endl;
    auto start_matriz = high_resolution_clock::now();
    pair<matrizUsuarios, pair<vector<int>, std::unordered_map<int, size_t>>> matriz_data = criarMatrizUsuarios("datasets/input.dat");
    matrizUsuarios userMatrix = matriz_data.first;
    vector<int> uniqueMovies = matriz_data.second.first;
    std::unordered_map<int, size_t> filme_id_to_index = matriz_data.second.second;
    auto end_matriz = high_resolution_clock::now();
    auto duracao_matriz = duration_cast<milliseconds>(end_matriz - start_matriz);
    cout << "Tempo para criar a matriz de usuários: " << duracao_matriz.count() << " ms" << endl;

    // Gerar explore.dat com usuários da userMatrix
    cout << "Gerando arquivo explore.dat..." << endl;
    vector<int> users_in_matrix;
    for(auto const& [userID, ratings] : userMatrix) {
        users_in_matrix.push_back(userID);
    }
    gerarExploreFileFromVector(users_in_matrix, "datasets/explore.dat", 50); // Passa o vetor de usuários existentes

    // Construir o índice LSH
    cout << "Construindo o índice LSH..." << endl;
    auto start_lsh = high_resolution_clock::now();
    LSHIndex lsh_index = construirIndiceLSH(userMatrix, uniqueMovies, filme_id_to_index);
    auto end_lsh = high_resolution_clock::now();
    auto duracao_lsh = duration_cast<milliseconds>(end_lsh - start_lsh);
    cout << "Tempo para construir o índice LSH: " << duracao_lsh.count() << " ms" << endl;

    // Gerar recomendações com LSH
    cout << "Gerando recomendações com LSH..." << endl;
    auto start_recomendacoes = high_resolution_clock::now();
    gerarRecomendacoesLSH(userMatrix, lsh_index, "datasets/explore.dat", "outcome/output.dat");
    auto end_recomendacoes = high_resolution_clock::now();
    auto duracao_recomendacoes = duration_cast<milliseconds>(end_recomendacoes - start_recomendacoes);
    cout << "Tempo para gerar recomendações: " << duracao_recomendacoes.count() << " ms" << endl;

    auto end_total = high_resolution_clock::now();
    auto duracao_total = duration_cast<milliseconds>(end_total - start_total);
    cout << "Tempo de execução Final: " << duracao_total.count() << " s" << endl;

    return 0;
}


