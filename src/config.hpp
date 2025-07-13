
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
#include <iterator>
#include <atomic>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iomanip>
#include <random>
#include <limits>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using Avaliacoes = std::unordered_map<int, std::pair<float, double>>;
using Posicoes = std::unordered_map<int, std::streampos>;
using Usuarios = std::unordered_map<int, Avaliacoes>;
using matrizUsuarios = std::unordered_map<int, std::unordered_map<int, float>>;

using namespace std;


template<typename T, size_t PoolSize> class MemoryPool;

// "Piscina de Memória" para alocação eficiente de objetos
template<typename T, size_t PoolSize = 1024>
class MemoryPool {
private:
    alignas(T) char pool[PoolSize * sizeof(T)];
    std::vector<T *> free_list;
    size_t allocated = 0;

public:
    MemoryPool() {
        free_list.reserve(PoolSize);
        for (size_t i = 0; i < PoolSize; ++i) {
            free_list.push_back(reinterpret_cast<T *>(pool + i * sizeof(T)));
        }
    }

    T *allocate() {
        if (free_list.empty()) {
            return new T(); 
        }
        T *ptr = free_list.back();
        free_list.pop_back();
        return ptr;
    }

    void deallocate(T *ptr) {
        if (ptr >= reinterpret_cast<T *>(pool) &&
            ptr < reinterpret_cast<T *>(pool + PoolSize * sizeof(T))) {
            free_list.push_back(ptr);
        } else {
            delete ptr; 
        }
    }
};

// Structs dos Itens:
struct MovieRating {
    int movie_id;
    float rating;
};

struct UserProfile {
    int user_id;
    std::vector<MovieRating> ratings;
    float norm;
};

namespace config {
    extern Posicoes input_pos;

    extern const int minUsers;
    extern const int minAval;
    extern const int Quant_Usuarios;
    extern const int K; 
    extern const int N; 

    // Constantes para LSH
    extern const int numHiperplanos;
    extern const int numBandas;
    extern const int bitsPorBanda;
}

// Gera arquivo explore.dat com uma amostra de usuários
void gerarExploreFileFromVector(const vector<int>& users, const string& explore_file, int sample_size);

// Enum para tipo de similaridade
enum class SimilarityType {
    COSSENO,
    JACCARD,
    EUCLIDIANA
};