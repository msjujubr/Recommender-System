#include "sistema_recomendacao.hpp"
#include <chrono>

using namespace chrono;

int main() {
    /* 
        Observação para o professor: Não sabemos como irá contabilizar o tempo de execução, 
        então deixamos comentado as linhas aqui na main para a cronometragem de todo o sistema.
        Utilizamos as funções da biblioteca chrono para este fim.
    */
    auto inicio_total = high_resolution_clock::now();

    sistema_recomendacao();

    auto fim_total = high_resolution_clock::now();
    auto duracao_total = duration_cast<seconds>(fim_total - inicio_total);
    cout << "\nTempo de execução Final: " << duracao_total.count() << " s" << endl;

    return 0;
}