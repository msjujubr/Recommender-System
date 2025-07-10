
#pragma once

#include "pch.hpp"
#include <unordered_map>
#include "utils.hpp"

// Forward declarations
template<typename T, size_t PoolSize> class MemoryPool;

// Simple memory pool for frequent allocations
template<typename T, size_t PoolSize = 1024>
class MemoryPool {
private:
    alignas(T) char pool[PoolSize * sizeof(T)];
    std::vector<T*> free_list;
    size_t allocated = 0;

public:
    MemoryPool() {
        free_list.reserve(PoolSize);
        for (size_t i = 0; i < PoolSize; ++i) {
            free_list.push_back(reinterpret_cast<T*>(pool + i * sizeof(T)));
        }
    }

    T* allocate() {
        if (free_list.empty()) {
            return new T(); // Fallback to heap
        }
        T* ptr = free_list.back();
        free_list.pop_back();
        return ptr;
    }

    void deallocate(T* ptr) {
        if (ptr >= reinterpret_cast<T*>(pool) && 
            ptr < reinterpret_cast<T*>(pool + PoolSize * sizeof(T))) {
            free_list.push_back(ptr);
        } else {
            delete ptr; // Heap allocated
        }
    }
};

// Flat structures for better cache locality
struct MovieRating {
    int movie_id;
    float rating;
};

struct UserProfile {
    int user_id;
    std::vector<MovieRating> ratings;
    float norm;
};

using Avaliacoes = std::unordered_map<int, std::pair<float, double>>;
using Posicoes  = std::unordered_map<int, std::streampos>;
using Usuarios  = std::unordered_map<int, Avaliacoes>;
using matrizUsuarios = std::unordered_map<int, std::unordered_map<int, float>>;

namespace config {
    extern Posicoes input_pos;

    extern const int minUsers;
    extern const int minAval;
    extern const int K;
    extern const int N;

    // Constantes para LSH
    extern const int numHiperplanos;
    extern const int numBandas;
    extern const int bitsPorBanda;
}


