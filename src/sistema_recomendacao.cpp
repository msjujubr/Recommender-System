#include "sistema_recomendacao.hpp"

// função principal
void sistema_recomendacao() {
    system("mkdir -p outcome");
    loadInput();

    // Criar a matriz de usuários
    pair<matrizUsuarios, pair<vector<int>, std::unordered_map<int, size_t>>> matriz_data = criarMatrizUsuarios("datasets/input.dat");
    matrizUsuarios userMatrix = matriz_data.first;
    vector<int> uniqueMovies = matriz_data.second.first;
    std::unordered_map<int, size_t> filme_id_to_index = matriz_data.second.second;

    // Gerar explore.dat com usuários da userMatrix
    vector<int> users_in_matrix;
    for (auto const &[userID, ratings] : userMatrix) {
        users_in_matrix.push_back(userID);
    }
    gerarExploreFileFromVector(users_in_matrix, "datasets/explore.dat", config::Quant_Usuarios); // Passa o vetor de usuários existentes

    // Construir o índice LSH
    LSHIndex lsh_index = construirIndiceLSH(userMatrix, uniqueMovies, filme_id_to_index);

    // Gerar recomendações com LSH
    gerarRecomendacoesLSH(userMatrix, lsh_index, "datasets/explore.dat", "outcome/output.dat");

}

//Cria a matriz de usuários(linhas são usuários e colunas são os filmes)
pair<matrizUsuarios, pair<vector<int>, std::unordered_map<int, size_t>>> criarMatrizUsuarios(const string &arquivo) {
    matrizUsuarios matriz;
    vector<int> filmesUnicos;
    std::unordered_map<int, size_t> filme_indice;

    ifstream input(arquivo);
    static thread_local char buf[65536]; // 64KB buffer
    unsigned long filmeIndexCounter = 0;

    while (input.getline(buf, sizeof(buf))) {
        char *str = buf;
        char *end;

        int usuario = strtol(str, &end, 10);
        std::unordered_map<int, float> &avaliacoes = matriz[usuario];

        while (*end) {
            while (*end == ' ') ++end;

            int filme = strtol(end, &end, 10);
            if (*end != ':') break;
            ++end;
            float nota = strtof(end, &end);

            // Optimize movie indexing with std::unordered_map
            auto existing = filme_indice.find(filme);
            if (existing == filme_indice.end()) {
                filme_indice.emplace(filme, filmeIndexCounter);
                filmeIndexCounter++;
                filmesUnicos.push_back(filme);
            }

            avaliacoes.emplace(filme, nota);
        }
    }

    normalizarMatriz(matriz);

    return { matriz, {filmesUnicos, filme_indice} };
}

//normaliza os vetores para posterior uso da similaridade por cosseno
void normalizarMatriz(matrizUsuarios &matriz) {
    for (auto it = matriz.begin(); it != matriz.end(); ++it) {
        auto &avaliacoes = it->second;
        // Calcula a norma L2 do vetor de avaliações do usuário
        float soma_quadrados = 0.0f;
        for (auto rating_it = avaliacoes.begin(); rating_it != avaliacoes.end(); ++rating_it) {
            soma_quadrados += rating_it->second * rating_it->second;
        }
        float norma = sqrt(soma_quadrados);

        // Normaliza cada avaliação dividindo pela norma
        if (norma > 0.0f) {  // Evita divisão por zero
            for (auto rating_it = avaliacoes.begin(); rating_it != avaliacoes.end(); ++rating_it) {
                rating_it->second /= norma;
            }
        }
    }
}

string signatureBandToString(const vector<bool> &band) {
    string s = ""; // Inicializa uma string vazia para armazenar a representação da banda.
    for (bool bit : band) { // Itera sobre cada bit (true/false) no vetor da banda.
        s += (bit ? '1' : '0'); // Se o bit for true, adiciona '1'; caso contrário, adiciona '0' à string.
    }
    return s; // Retorna a string binária que representa a banda da assinatura.
}

// Constrói o índice LSH
LSHIndex construirIndiceLSH(const matrizUsuarios &userMatrix, const vector<int> &uniqueMovies, const std::unordered_map<int, size_t> &filme_id_to_index) {
    LSHIndex index;
    index.filme_id_to_index = filme_id_to_index;
    index.unique_movies_ordered = uniqueMovies;

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> d(0, 1);

    // 1. Gerar Hiperplanos Aleatórios (serial)
    index.random_hyperplanes.resize(config::numHiperplanos);
    for (auto &hyperplane : index.random_hyperplanes) {
        hyperplane.resize(uniqueMovies.size());
        for (auto &val : hyperplane) {
            val = d(gen);
        }
    }

    // 2. Criar Assinaturas em paralelo
    const unsigned long num_threads = thread::hardware_concurrency();
    vector<thread> threads;
    vector<unordered_map<int, LSHSignature>> all_local_signatures(num_threads);

    // Distribuir usuários entre threads
    vector<pair<matrizUsuarios::const_iterator, matrizUsuarios::const_iterator>> chunks(num_threads);
    size_t chunk_size = userMatrix.size() / num_threads;
    auto it_matrix = userMatrix.begin();

    for (unsigned long i = 0; i < num_threads; ++i) {
        chunks[i].first = it_matrix;
        if (i == num_threads - 1) {
            chunks[i].second = userMatrix.end();
        } else {
            for (size_t j = 0; j < chunk_size; ++j) {
                ++it_matrix;
            }
            chunks[i].second = it_matrix;
        }
    }

    for (unsigned long i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            for (auto it = chunks[i].first; it != chunks[i].second; ++it) {
                const int userID = it->first;
                const auto &ratings = it->second;
                LSHSignature signature;
                signature.reserve(config::numHiperplanos);

                for (unsigned long h = 0; h < static_cast<unsigned long>(index.random_hyperplanes.size()); ++h) {
                    const auto &hyperplane = index.random_hyperplanes[h];
                    float dot = 0.0f;
                    for (auto rating_it = ratings.begin(); rating_it != ratings.end(); ++rating_it) {
                        auto it_index = index.filme_id_to_index.find(rating_it->first);
                        if (it_index != index.filme_id_to_index.end()) {
                            dot += rating_it->second * hyperplane[it_index->second];
                        }
                    }
                    signature.push_back(dot >= 0);
                }
                all_local_signatures[i][userID] = move(signature);
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }
    threads.clear();

    // Merge all local signatures into the main index.user_signatures
    for (auto &local_map : all_local_signatures) {
        for (auto &pair : local_map) {
            index.user_signatures.insert(pair);
        }
    }

    // 3. Agrupamento em Baldes em paralelo
    index.lsh_buckets.resize(config::numBandas);
    vector<unordered_map<string, vector<int>>> all_local_buckets(num_threads * config::numBandas);

    // Distribuir assinaturas entre threads
    vector<pair<unordered_map<int, LSHSignature>::const_iterator, unordered_map<int, LSHSignature>::const_iterator>> sig_chunks(num_threads);
    size_t sig_chunk_size = index.user_signatures.size() / num_threads;
    auto it_sig_start = index.user_signatures.begin();

    for (unsigned long i = 0; i < num_threads; ++i) {
        sig_chunks[i].first = it_sig_start;
        if (i == num_threads - 1) {
            sig_chunks[i].second = index.user_signatures.end();
        } else {
            for (size_t j = 0; j < sig_chunk_size; ++j) {
                ++it_sig_start;
            }
            sig_chunks[i].second = it_sig_start;
        }
    }

    for (unsigned long i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            for (auto it = sig_chunks[i].first; it != sig_chunks[i].second; ++it) {
                const auto &[userID, signature] = *it;

                for (int band = 0; band < config::numBandas; ++band) {
                    int start = band * config::bitsPorBanda;
                    int end = min(start + config::bitsPorBanda, static_cast<int>(signature.size()));

                    vector<bool> band_bits(signature.begin() + start, signature.begin() + end);
                    string band_key = signatureBandToString(band_bits);

                    all_local_buckets[i * config::numBandas + band][band_key].push_back(userID);
                }
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    // Merge all local buckets into the main index.lsh_buckets
    for (int band = 0; band < config::numBandas; ++band) {
        for (unsigned long i = 0; i < num_threads; ++i) {
            for (auto &[key, users] : all_local_buckets[i * config::numBandas + band]) {
                index.lsh_buckets[band][key].insert(index.lsh_buckets[band][key].end(), users.begin(), users.end());
            }
        }
    }

    return index;
}

// Encontra usuários candidatos similares usando LSH
vector<int> encontrarCandidatosLSH(int query_userID, const LSHIndex &lsh_index) {
    unordered_set<int> candidate_set; // Usa um unordered_set para armazenar IDs de candidatos únicos e evitar duplicatas.

    // Garante que o usuário alvo tem uma assinatura LSH
    if (lsh_index.user_signatures.find(query_userID) == lsh_index.user_signatures.end()) {
        // Usuário não encontrado no índice LSH, retorna lista vazia
        return {}; // Se o usuário não tem uma assinatura LSH, retorna um vetor vazio.
    }

    const LSHSignature &query_signature = lsh_index.user_signatures.at(query_userID); // Obtém a assinatura LSH do usuário alvo.

    for (int i = 0; i < config::numBandas; ++i) { // Itera sobre cada banda.
        // Extrai a porção da assinatura para a banda atual
        auto start_it = query_signature.begin() + (i * config::bitsPorBanda); // Calcula o início da porção da assinatura.
        auto end_it = start_it + config::bitsPorBanda; // Calcula o final da porção.
        if (end_it > query_signature.end()) { // Ajusta o final se a última banda for parcial.
            end_it = query_signature.end();
        }
        vector<bool> band_signature_vec(start_it, end_it); // Cria o vetor de bits da banda.
        string band_signature_str = signatureBandToString(band_signature_vec); // Converte para string.

        // Adiciona todos os usuários do balde à lista de candidatos
        if (lsh_index.lsh_buckets[i].count(band_signature_str)) { // Verifica se a banda de assinatura existe em algum balde.
            for (int candidate_id : lsh_index.lsh_buckets[i].at(band_signature_str)) { // Itera sobre todos os usuários neste balde.
                if (candidate_id != query_userID) {
                    candidate_set.insert(candidate_id); // Adiciona o usuário ao conjunto de candidatos.
                }
            }
        }
    }

    // Converte o set de candidatos para vector
    vector<int> candidates(candidate_set.begin(), candidate_set.end()); // Converte o conjunto (unordered_set) de IDs de candidatos para um vetor.
    return candidates; // Retorna o vetor de IDs de usuários candidatos.
}

// Gera recomendações para usuários do explore.dat usando LSH
void gerarRecomendacoesLSH(const matrizUsuarios &userMatrix, const LSHIndex &lsh_index, const string &explore_file, const string &output_file) {
    // Leitura dos usuários a serem processados
    ifstream explore_input(explore_file);
    vector<int> explore_users((istream_iterator<int>(explore_input)),
        istream_iterator<int>());

    // Fila de trabalho e mutex
    queue<int> work_queue;
    mutex queue_mutex;
    for (int user : explore_users) work_queue.push(user);

    // Resultados e mutex
    vector<string> results;
    mutex results_mutex;

    // Número de threads baseado no hardware
    const unsigned long num_threads = thread::hardware_concurrency();
    vector<thread> threads;

    // Pré-processamento: mapa de filmes avaliados por usuário
    unordered_map<int, unordered_set<int>> user_rated_movies;
    for (auto it = userMatrix.begin(); it != userMatrix.end(); ++it) {
        const int user = it->first;
        const auto &ratings = it->second;
        for (auto rating_it = ratings.begin(); rating_it != ratings.end(); ++rating_it) {
            user_rated_movies[user].insert(rating_it->first);
        }
    }

    // Função de trabalho para cada thread
    auto worker = [&]() {
        while (true) {
            int query_userID;

            // Obter próximo usuário da fila
            {
                lock_guard<mutex> lock(queue_mutex);
                if (work_queue.empty()) return;
                query_userID = work_queue.front();
                work_queue.pop();
            }

            // Verificar se usuário existe na matriz
            auto it_query = userMatrix.find(query_userID);
            if (it_query == userMatrix.end()) {
                continue;
            }

            const auto &query_ratings = it_query->second;
            const auto &query_movies = user_rated_movies[query_userID];

            // 1. Encontrar candidatos via LSH
            auto candidates = encontrarCandidatosLSH(query_userID, lsh_index);
            if (candidates.empty()) {
                continue;
            }

            // 2. Calcular similaridade com heap top-K
            priority_queue<pair<float, int>> topK; // max-heap

            for (int candidate_id : candidates) {
                auto it_cand = userMatrix.find(candidate_id);
                if (it_cand == userMatrix.end()) continue;

                // Similaridade otimizada
                float sim = 0.0f;
                const auto &cand_ratings = it_cand->second;

                // Iterar sobre o vetor menor para melhor performance
                if (query_ratings.size() < cand_ratings.size()) {
                    for (auto rating_it = query_ratings.begin(); rating_it != query_ratings.end(); ++rating_it) {
                        auto it = cand_ratings.find(rating_it->first);
                        if (it != cand_ratings.end()) {
                            sim += rating_it->second * it->second;
                        }
                    }
                } else {
                    for (auto rating_it = cand_ratings.begin(); rating_it != cand_ratings.end(); ++rating_it) {
                        auto it = query_ratings.find(rating_it->first);
                        if (it != query_ratings.end()) {
                            sim += rating_it->second * it->second;
                        }
                    }
                }

                topK.push({ sim, candidate_id });
                if (topK.size() > static_cast<unsigned long>(config::K)) topK.pop();
            }

            if (topK.empty()) {
                continue;
            }

            // 3. Recomendar filmes não avaliados pelo usuário
            unordered_map<int, pair<float, int>> movie_scores;

            while (!topK.empty()) {
                auto [sim, similar_userID] = topK.top();
                topK.pop();

                auto similar_user_it = userMatrix.find(similar_userID);
                if (similar_user_it != userMatrix.end()) {
                    for (auto movie_it = similar_user_it->second.begin(); movie_it != similar_user_it->second.end(); ++movie_it) {
                        const int filmeID = movie_it->first;
                        const float nota = movie_it->second;
                        if (query_movies.count(filmeID) == 0) {
                            movie_scores[filmeID].first += nota;
                            movie_scores[filmeID].second++;
                        }
                    }
                }
            }

            if (movie_scores.empty()) {
                continue;
            }

            // 4. Selecionar top-N filmes
            priority_queue<pair<float, int>> top_movies; // max-heap
            for (const auto &[filmeID, score] : movie_scores) {
                float avg = score.first / score.second;
                top_movies.push({ avg, filmeID });
            }

            // 5. Gerar linha de saída
            stringstream ss;
            ss << query_userID;

            vector<int> recommended_ids;
            for (int i = 0; i < config::N && !top_movies.empty(); ++i) {
                recommended_ids.push_back(top_movies.top().second);
                top_movies.pop();
            }

            for (int movie_id : recommended_ids) {
                ss << " " << movie_id;
            }

            // Adicionar resultado com lock
            lock_guard<mutex> lock(results_mutex);
            results.push_back(ss.str());
        }
    };

    // Criar pool de threads
    for (unsigned long i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    // Aguardar todas as threads terminarem
    for (auto &t : threads) {
        t.join();
    }


    // Escrever resultados no arquivo de saída
    ofstream output_stream(output_file);
    for (const auto &line : results) {
        output_stream << line << '\n';
    }
}