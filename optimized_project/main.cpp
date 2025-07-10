// main.cpp
#include "config.hpp"
#include "PreProcessamento.hpp" 
#include "sistema_recomendacao.hpp"
#include "utils.hpp" 

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    
    auto inicio_total = chrono::high_resolution_clock::now();

    cout << "Pre-processamento" << endl;
    loadInput();
    
    // Criar diretório outcome se não existir
    system("mkdir -p outcome");
    
    // Gerar arquivo explore.dat automaticamente
    cout << "\nGerando arquivo explore.dat..." << endl;
    gerarExploreFile("datasets/input.dat", "datasets/explore.dat", 1000); 

    cout << "\nCriando a matriz de usuários..." << endl;
    auto inicio_matriz = chrono::high_resolution_clock::now();
    
    string inputFile = "datasets/input.dat";
    auto resultado_matriz = criarMatrizUsuarios(inputFile);
    matrizUsuarios userMatrix = resultado_matriz.first;
    vector<int> uniqueMovies = resultado_matriz.second.first;
    std::unordered_map<int, size_t> filme_id_to_index = resultado_matriz.second.second; // Obtenha o mapeamento

    auto fim_matriz = chrono::high_resolution_clock::now();
    auto duracao_matriz = chrono::duration_cast<chrono::milliseconds>(fim_matriz - inicio_matriz);
    cout << "Tempo para criar a matriz de usuários: " << duracao_matriz.count() << " ms" << endl;

    // --- Construção do Índice LSH ---
    cout << "\nConstruindo o índice LSH..." << endl;
    auto inicio_lsh = chrono::high_resolution_clock::now();
    LSHIndex lsh_index = construirIndiceLSH(userMatrix, uniqueMovies, filme_id_to_index);
    auto fim_lsh = chrono::high_resolution_clock::now();
    auto duracao_lsh = chrono::duration_cast<chrono::milliseconds>(fim_lsh - inicio_lsh);
    cout << "Tempo para construir o índice LSH: " << duracao_lsh.count() << " ms" << endl;

    // --- Geração de Recomendações usando LSH ---
    cout << "\nGerando recomendações com LSH..." << endl;
    auto inicio_recomendacao = chrono::high_resolution_clock::now();
    string exploreFile = "datasets/explore.dat"; 
    string outputFile = "outcome/output.dat";   
    gerarRecomendacoesLSH(userMatrix, lsh_index, exploreFile, outputFile);
    auto fim_recomendacao = chrono::high_resolution_clock::now();
    auto duracao_recomendacao = chrono::duration_cast<chrono::milliseconds>(fim_recomendacao - inicio_recomendacao);
    cout << "Tempo para gerar recomendações: " << duracao_recomendacao.count() << " ms" << endl;


    auto fim_total = chrono::high_resolution_clock::now();
    auto duracao_total = chrono::duration_cast<chrono::seconds>(fim_total - inicio_total);

    cout << "\nTempo de execução Final: " << duracao_total.count() << " s" << endl;
    return 0;
}

