#pragma once

#include <iostream>
#include <string>
#include <sstream>  
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <cmath>
#include <condition_variable>
#include <iterator>  // Add this include
#include <atomic>

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iomanip>  

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>

using Avaliacoes = std::unordered_map<int, std::pair<float, double>>;
using Posicoes  = std::unordered_map<int, std::streampos>;
using Usuarios  = std::unordered_map<int, Avaliacoes>;
using matrizUsuarios = std::unordered_map<int, std::unordered_map<int, float>>;

using namespace std;
using namespace chrono;

namespace config {
    extern Posicoes input_pos;

    extern const int minUsers;
    extern const int minAval;
    extern const int K;
    extern const int N;

    // Constantes para LSH
    extern const int numHiperplanos; // Total de hiperplanos (bits na assinatura)
    extern const int numBandas;       // Número de bandas (L)
    extern const int bitsPorBanda;   // Bits por banda (r)

}


