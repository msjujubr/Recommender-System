#include "PreProcessamento.hpp"

mutex mtx;

// PROCESSO: localizar usuarios no arquivo, divide os usuarios entre as threads
//           cada thread lê seus usuarios, contam Aval/User e User/Filme 
//           threads filtram 


// Pre-carregamento
void loadInput() {
    ifstream entrada("datasets/ratings.csv");
    string linha;
    getline(entrada, linha); // ignora cabeçalho se existir

    entrada.seekg(0, ios::end);
    streampos tamanho_total = entrada.tellg();
    entrada.close();

    vector<thread> threads;
    int n_threads = thread::hardware_concurrency();

    auto inicio = high_resolution_clock::now();

    unordered_map<int, pair<streampos, streampos>> mapa_intervalos;
    unordered_map<int, vector<int>> UsersInverv;

    // Mapeamento dos usuários
    {
        vector<unordered_map<int, pair<streampos, streampos>>> locais(n_threads);
        for (int i = 0; i < n_threads; i++) {
            streampos inicio = (tamanho_total * i) / n_threads;
            streampos fim = (tamanho_total * (i + 1)) / n_threads;
            threads.emplace_back(UserIntervalos, ref(locais[i]), inicio, fim);
        }

        for (auto &t : threads) {
            t.join();
        }
        threads.clear();

        for (const auto &local : locais) {
            for (const auto &[uid, intervalo] : local) {
                auto &entry = mapa_intervalos[uid];
                if (entry.first == entry.second) {
                    entry = intervalo;
                } else {
                    entry.first = min(entry.first, intervalo.first);
                    entry.second = max(entry.second, intervalo.second);
                }
            }
        }

        int idx = 0;
        for (const auto &[userID, _] : mapa_intervalos) {
            UsersInverv[idx % n_threads].push_back(userID);
            ++idx;
        }
    }
    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<milliseconds>(fim - inicio);
    cout << "Duracao mapeamento: " << duracao.count() << " ms" << endl;

    inicio = high_resolution_clock::now();

    unordered_map<int, int> User_cont, Filme_cont;
    vector<Usuarios> mapas_locais(n_threads);

    // Leitura dos usuários em blocos
    {
        vector<unordered_map<int, int>> AvalUser(n_threads), UserFilme(n_threads);
        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back([&, i]() {
                bloco_leitura(ref(mapas_locais[i]), ref(mapa_intervalos), ref(UsersInverv[i]), ref(AvalUser[i]), ref(UserFilme[i]));
            });
        }

        for (auto &t : threads) {
            t.join();
        }
        threads.clear();

        // Contagem de avaliações por usuário e filmes
        for (const auto &mapa : AvalUser) {
            for (const auto &[userID, count] : mapa) {
                User_cont[userID] += count;
            }
        }

        // Unifica as contagens de avaliações por filme
        for (const auto &mapa : UserFilme) {
            for (const auto &[filmeID, count] : mapa) {
                Filme_cont[filmeID] += count;
            }
        }
    }
    fim = high_resolution_clock::now();
    duracao = duration_cast<milliseconds>(fim - inicio);
    cout << "Duracao Leitura: " << duracao.count() << " ms" << endl;

    inicio = high_resolution_clock::now();

    vector<string> bloco_escrita;

    // Filtragem em blocos
    {
        vector<vector<string>> blocos_finais(n_threads);
        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back([&, i]() {
                bloco_filtro(mapas_locais[i], User_cont, Filme_cont, blocos_finais[i]);
            });
        }

        for (auto &t : threads) {
            t.join();
        }

        for (const auto &buf : blocos_finais) {
            for (string linhas : buf) {
                bloco_escrita.push_back(linhas);
            }
        }
    }

    fim = high_resolution_clock::now();
    duracao = duration_cast<milliseconds>(fim - inicio);
    cout << "Duracao Filtragem: " << duracao.count() << " ms" << endl;

    ofstream arq_saida("datasets/input.dat");

    for (const auto& linha : bloco_escrita) {
        arq_saida << linha << '\n';
    }
}

// Função de Leitura (evitar stringstream)
void bloco_leitura(Usuarios &local, const unordered_map<int, pair<streampos, streampos>> &intervalos,
    vector<int> Users, unordered_map<int, int> &AvalUser, unordered_map<int, int> &UserFilme) {
    ifstream entrada("datasets/ratings.csv");
    if (!entrada.is_open()) {
        cerr << "ratings.csv não encontrado. Abortando.\n";
        exit(EXIT_FAILURE);
    }

    string linha;

    for (int userID : Users) {
        auto it = intervalos.find(userID);
        if (it == intervalos.end()) continue;

        entrada.clear();
        entrada.seekg(it->second.first);

        streampos fim = it->second.second;

        while (entrada.tellg() < fim && getline(entrada, linha)) {
            size_t p0 = linha.find(',');
            size_t p1 = linha.find(',', p0 + 1);
            size_t p2 = linha.find(',', p1 + 1);

            if (p0 == string::npos || p1 == string::npos || p2 == string::npos) continue;

            int uid = stoi(linha.substr(0, p0));
            int filmeID = stoi(linha.substr(p0 + 1, p1 - p0 - 1));
            float nota = stof(linha.substr(p1 + 1, p2 - p1 - 1));
            int ts = stoi(linha.substr(p2 + 1));

            auto &ref = local[uid][filmeID];
            if (ref.second < ts) {
                ref = { nota, ts };
            }

            AvalUser[uid]++;
            UserFilme[filmeID]++;

        }
    }
}

// Função de Filtragem
void bloco_filtro(Usuarios &mapa, const unordered_map<int, int> User_cont, const unordered_map<int, int> Filme_cont, vector<string> &bloco) {
    for (auto it = mapa.begin(); it != mapa.end();) {
        int userID = it->first;

        // Remove usuários com avaliações insuficientes
        if (User_cont.at(userID) < config::minAval) {
            it++;
            continue;
        }

        auto &filmes = it->second;

        // Filtra os filmes do usuário
        for (auto it_f = filmes.begin(); it_f != filmes.end();) {
            if (Filme_cont.at(it_f->first) < config::minUsers) {
                it_f = filmes.erase(it_f);
            } else {
                ++it_f;
            }
        }

        // Remove usuário se não houver avaliações suficientes após o filtro
        if (filmes.size() < static_cast<size_t>(config::minAval)) {
            it = mapa.erase(it);
        } else {
            string linha = to_string(userID);
            for (const auto &[filmeID, nota] : filmes) {
                linha += " " + to_string(filmeID) + ":" + formatarNota(nota.first);
            }
            bloco.push_back(linha);
            ++it;
        }
    }
}

// Leitura dos Intervalos dos usuários (é necessário?)
void UserIntervalos(unordered_map<int, pair<streampos, streampos>> &intervalo, streampos inicio, streampos fim) {
    ifstream arquivo("datasets/ratings.csv");

    arquivo.seekg(inicio);

    string linha;
    // Ignora a primeira linha (cabeçalho ou parcial)
    getline(arquivo, linha);

    streampos pos_linha_ini = arquivo.tellg();

    while (pos_linha_ini < fim && getline(arquivo, linha)) {
        streampos pos_linha_fim = arquivo.tellg();

        size_t p0 = linha.find(',');
        if (p0 == string::npos) {
            pos_linha_ini = pos_linha_fim;
            continue;
        }

        int userID = stoi(linha.substr(0, p0));

        auto &local = intervalo[userID];
        if (local.first == local.second) {
            local = { pos_linha_ini, pos_linha_fim };
        } else {
            local.second = pos_linha_fim;
        }

        pos_linha_ini = pos_linha_fim;
    }
}
//

string formatarNota(float nota){
    ostringstream out;
    out << fixed << setprecision(1) << nota;
    return out.str();
}