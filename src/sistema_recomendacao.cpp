#include "sistema_recomendacao.hpp"
#include <random>     
#include <algorithm>  
#include <limits> 
#include <iterator>

// função principal
void sistema_recomendacao() {
    ifstream arq_saida("datasets/input.dat"); arq_saida.clear(); arq_saida.close();
    cout << "Pre-processamento" << endl;
    loadInput();
}

//Cria a matriz de usuários(linhas são usuários e colunas são os filmes)
pair<matrizUsuarios, pair<vector<int>, unordered_map<int, size_t>>> criarMatrizUsuarios(string &arquivo) {
     matrizUsuarios matriz;
    vector<int> filmesUnicos;
    unordered_map<int, size_t> filme_indice;

    ifstream input(arquivo);
    string linha;
    size_t filmeIndexCounter = 0;

    filmesUnicos.reserve(40000); // Prealoca espaço

    while (getline(input, linha)) {
        const char* str = linha.c_str();
        char* end;

        int usuario = strtol(str, &end, 10); // lê usuário
        unordered_map<int, float>& avaliacoes = matriz[usuario];

        while (*end) {
            while (*end == ' ') ++end; // ignora espaços

            int filme = strtol(end, &end, 10);
            if (*end != ':') break;
            ++end;
            float nota = strtof(end, &end);

            // Novo filme?
            if (!filme_indice.count(filme)) {
                filme_indice[filme] = filmeIndexCounter++;
                filmesUnicos.push_back(filme);
            }

            avaliacoes[filme] = nota;
        }
    }

    normalizarMatriz(matriz);

    return {matriz, {filmesUnicos, filme_indice}};
}

//normaliza os vetores para posterior uso da similaridade por cosseno
void normalizarMatriz(matrizUsuarios& matriz) {
    for (auto& [usuario, avaliacoes] : matriz) {
        // Calcula a norma L2 do vetor de avaliações do usuário
        float soma_quadrados = 0.0f;
        for (const auto& [filme, nota] : avaliacoes) {
            soma_quadrados += nota * nota;
        }
        float norma = sqrt(soma_quadrados);
        
        // Normaliza cada avaliação dividindo pela norma
        if (norma > 0.0f) {  // Evita divisão por zero
            for (auto& [filme, nota] : avaliacoes) {
                nota /= norma;
            }
        }
    }
}

// Calcula a similaridade por cosseno entre dois usuários
float similaridade_cosseno(const unordered_map<int, float>& userA, const unordered_map<int, float>& userB) {
    float soma = 0.0f;

    // Itera sobre os filmes avaliados por userA
    for (const auto& [filmeID, notaA] : userA) {
        // Se userB também avaliou o mesmo filme
        auto itB = userB.find(filmeID);
        if (itB != userB.end()) {
        soma += notaA * itB->second;
}
    }

    return soma;  // Como os vetores estão normalizados, o produto escalar já é a similaridade por cosseno
}

string signatureBandToString(const vector<bool>& band) {
    string s = ""; // Inicializa uma string vazia para armazenar a representação da banda.
    for (bool bit : band) { // Itera sobre cada bit (true/false) no vetor da banda.
        s += (bit ? '1' : '0'); // Se o bit for true, adiciona '1'; caso contrário, adiciona '0' à string.
    }
    return s; // Retorna a string binária que representa a banda da assinatura.
}


// Constrói o índice LSH
LSHIndex construirIndiceLSH(const matrizUsuarios& userMatrix, const vector<int>& uniqueMovies, const unordered_map<int, size_t>& filme_id_to_index) { 
   LSHIndex index;
    index.filme_id_to_index = filme_id_to_index;
    index.unique_movies_ordered = uniqueMovies;
    
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> d(0, 1);

    // 1. Gerar Hiperplanos Aleatórios (serial)
    index.random_hyperplanes.resize(config::numHiperplanos);
    for (auto& hyperplane : index.random_hyperplanes) {
        hyperplane.resize(uniqueMovies.size());
        for (auto& val : hyperplane) {
            val = d(gen);
        }
    }

    // 2. Criar Assinaturas em paralelo
    mutex signature_mutex;
    index.user_signatures.reserve(userMatrix.size());
    
    auto signature_worker = [&](auto start_it, auto end_it) {
        unordered_map<int, LSHSignature> local_signatures;
        
        for (auto it = start_it; it != end_it; ++it) {
            const auto& [userID, ratings] = *it;
            LSHSignature signature;
            signature.reserve(config::numHiperplanos);
            
            for (const auto& hyperplane : index.random_hyperplanes) {
                float dot = 0.0f;
                for (const auto& [filmeID, rating] : ratings) {
                    auto it_index = index.filme_id_to_index.find(filmeID);
                    if (it_index != index.filme_id_to_index.end()) {
                        dot += rating * hyperplane[it_index->second];
                    }
                }
                signature.push_back(dot >= 0);
            }
            
            local_signatures[userID] = move(signature);
        }
        
        lock_guard<mutex> lock(signature_mutex);
        index.user_signatures.merge(local_signatures);
    };

    // Dividir usuários entre threads
    const unsigned num_threads = thread::hardware_concurrency();
    vector<thread> threads;
    auto it = userMatrix.begin();
    size_t chunk_size = userMatrix.size() / num_threads;

    for (unsigned i = 0; i < num_threads; ++i) {
        auto start_it = it;
        advance(it, chunk_size);
        auto end_it = (i == num_threads - 1) ? userMatrix.end() : it;
        threads.emplace_back(signature_worker, start_it, end_it);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 3. Agrupamento em Baldes em paralelo
    index.lsh_buckets.resize(config::numBandas);
    vector<mutex> bucket_mutexes(config::numBandas);
    
    auto bucket_worker = [&](auto start_it, auto end_it) {
        vector<unordered_map<string, vector<int>>> local_buckets(config::numBandas);
        
        for (auto it = start_it; it != end_it; ++it) {
            const auto& [userID, signature] = *it;
            
            for (int band = 0; band < config::numBandas; ++band) {
                int start = band * config::bitsPorBanda;
                int end = min(start + config::bitsPorBanda, static_cast<int>(signature.size()));
                
                vector<bool> band_bits(signature.begin() + start, signature.begin() + end);
                string band_key = signatureBandToString(band_bits);
                
                local_buckets[band][band_key].push_back(userID);
            }
        }
        
        // Merge com os buckets globais
        for (int band = 0; band < config::numBandas; ++band) {
            lock_guard<mutex> lock(bucket_mutexes[band]);
            for (auto& [key, users] : local_buckets[band]) {
                index.lsh_buckets[band][key].insert(
                    index.lsh_buckets[band][key].end(),
                    users.begin(),
                    users.end()
                );
            }
        }
    };

    // Dividir assinaturas entre threads
    threads.clear();
    auto it_sig = index.user_signatures.begin();
    chunk_size = index.user_signatures.size() / num_threads;

    for (unsigned i = 0; i < num_threads; ++i) {
        auto start_it = it_sig;
        advance(it_sig, chunk_size);
        auto end_it = (i == num_threads - 1) ? index.user_signatures.end() : it_sig;
        threads.emplace_back(bucket_worker, start_it, end_it);
    }

    for (auto& t : threads) {
        t.join();
    }

    return index;
}

// Encontra usuários candidatos similares usando LSH
vector<int> encontrarCandidatosLSH(int query_userID, const LSHIndex& lsh_index) { 
    unordered_set<int> candidate_set; // Usa um unordered_set para armazenar IDs de candidatos únicos e evitar duplicatas.

    // Garante que o usuário alvo tem uma assinatura LSH
    if (lsh_index.user_signatures.find(query_userID) == lsh_index.user_signatures.end()) {
        // Usuário não encontrado no índice LSH, retorna lista vazia
        return {}; // Se o usuário não tem uma assinatura LSH, retorna um vetor vazio.
    }

    const LSHSignature& query_signature = lsh_index.user_signatures.at(query_userID); // Obtém a assinatura LSH do usuário alvo.

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
void gerarRecomendacoesLSH(const matrizUsuarios& userMatrix, const LSHIndex& lsh_index, const string& explore_file, const string& output_file) {
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
    const unsigned num_threads = thread::hardware_concurrency();
    vector<thread> threads;
    
    // Pré-processamento: mapa de filmes avaliados por usuário
    unordered_map<int, unordered_set<int>> user_rated_movies;
    for (const auto& [user, ratings] : userMatrix) {
        for (const auto& [movie, _] : ratings) {
            user_rated_movies[user].insert(movie);
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
            if (it_query == userMatrix.end()) continue;

            const auto& query_ratings = it_query->second;
            const auto& query_movies = user_rated_movies[query_userID];

            // 1. Encontrar candidatos via LSH
            auto candidates = encontrarCandidatosLSH(query_userID, lsh_index);

            // 2. Calcular similaridade com heap top-K
            priority_queue<pair<float, int>> topK; // max-heap

            for (int candidate_id : candidates) {
                auto it_cand = userMatrix.find(candidate_id);
                if (it_cand == userMatrix.end()) continue;

                // Similaridade otimizada
                float sim = 0.0f;
                const auto& cand_ratings = it_cand->second;
                
                // Iterar sobre o vetor menor para melhor performance
                if (query_ratings.size() < cand_ratings.size()) {
                    for (const auto& [movie, rating] : query_ratings) {
                        auto it = cand_ratings.find(movie);
                        if (it != cand_ratings.end()) {
                            sim += rating * it->second;
                        }
                    }
                } else {
                    for (const auto& [movie, rating] : cand_ratings) {
                        auto it = query_ratings.find(movie);
                        if (it != query_ratings.end()) {
                            sim += rating * it->second;
                        }
                    }
                }

                topK.push({sim, candidate_id});
                if (topK.size() > static_cast<size_t>(config::K)) topK.pop();
            }

            // 3. Recomendar filmes não avaliados pelo usuário
            unordered_map<int, pair<float, int>> movie_scores;

            while (!topK.empty()) {
                auto [sim, similar_userID] = topK.top();
                topK.pop();

                for (const auto& [filmeID, nota] : userMatrix.at(similar_userID)) {
                    if (query_movies.count(filmeID) == 0) {
                        movie_scores[filmeID].first += nota;
                        movie_scores[filmeID].second++;
                    }
                }
            }

            // 4. Selecionar top-N filmes
            priority_queue<pair<float, int>> top_movies; // max-heap
            for (const auto& [filmeID, score] : movie_scores) {
                float avg = score.first / score.second;
                top_movies.push({avg, filmeID});
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
    for (unsigned i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    // Aguardar todas as threads terminarem
    for (auto& t : threads) {
        t.join();
    }

    // Escrever resultados no arquivo de saída
    ofstream output_stream(output_file);
    for (const auto& line : results) {
        output_stream << line << '\n';
    }
}