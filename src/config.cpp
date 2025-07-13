#include "config.hpp"

namespace config {
    Posicoes input_pos;

    const int minUsers = 50; 
    const int minAval = 50; 
    extern const int Quant_Usuarios = 50; // quantidade de usuários para o explore.dat
    const int K = 10; // Número de vizinhos mais próximos a considerar
    const int N = 10; // Número de recomendações a gerar

    // Constantes para LSH
    const int numHiperplanos = 16; // Número de hiperplanos para gerar a assinatura (ajustado para 16 * 8)
    const int numBandas = 8;     // Número de bandas para LSH
    const int bitsPorBanda = 2;   // Bits por banda (numHiperplanos / numBandas)
}


void gerarExploreFileFromVector(const vector<int>& users, const string& explore_file, const int sample_size) {
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
}