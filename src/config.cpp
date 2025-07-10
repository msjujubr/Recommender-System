#include "config.hpp"

namespace config {
    Posicoes input_pos;

    const int minUsers = 50;
    const int minAval = 50;
    const int K = 5;
    const int N = 10;

    const int numHiperplanos = 6; //sujeito a alterações
    const int numBandas = 3; //sujeito a alterações
    const int bitsPorBanda = 2; //sujeito a alterações

    //numBandas * bitsPorBanda deve ser igual ao numHiperplanos
}