
#include "config.hpp"

namespace config {
    Posicoes input_pos;

    const int minUsers = 1; // Ajustado para 1 para incluir mais usuários
    const int minAval = 1; // Ajustado para 1 para incluir mais usuários
    const int K = 10; // Número de vizinhos mais próximos a considerar
    const int N = 10; // Número de recomendações a gerar

    // Constantes para LSH
    const int numHiperplanos = 128; // Número de hiperplanos para gerar a assinatura (ajustado para 16 * 8)
    const int numBandas = 16;     // Número de bandas para LSH
    const int bitsPorBanda = 8;   // Bits por banda (numHiperplanos / numBandas)
}




