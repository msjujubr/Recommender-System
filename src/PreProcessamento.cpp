#include "PreProcessamento.hpp"

mutex mtx;

// PROCESSO: localizar usuarios no arquivo, divide os usuarios entre as threads
//           cada thread lê seus usuarios, contam Aval/User e User/Filme 
//           threads filtram 


// Pre-carregamento
void loadInput() {
    size_t tamanho_arquivo;
    char *base = mmapArquivo("datasets/ratings.csv", tamanho_arquivo);

    vector<thread> threads;
    int n_threads = thread::hardware_concurrency();

    auto inicio = high_resolution_clock::now();

    unordered_map<int, pair<size_t, size_t>> mapa_intervalos;
    unordered_map<int, vector<int>> UsersInverv;

    // Mapeamento dos usuários
    {
        vector<unordered_map<int, pair<size_t, size_t>>> locais(n_threads);
        for (int i = 0; i < n_threads; i++) {
            size_t ini = (tamanho_arquivo * i) / n_threads;
            size_t fim = (tamanho_arquivo * (i + 1)) / n_threads;
            threads.emplace_back(UserIntervalos, ref(locais[i]), ini, fim, base);
        }

        for (auto &t : threads) t.join();
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
                bloco_leitura(ref(mapas_locais[i]), ref(mapa_intervalos), ref(UsersInverv[i]),
                    ref(AvalUser[i]), ref(UserFilme[i]), base);
            });
        }

        for (auto &t : threads) t.join();
        threads.clear();

        for (const auto &mapa : AvalUser)
            for (const auto &[userID, count] : mapa)
                User_cont[userID] += count;

        for (const auto &mapa : UserFilme)
            for (const auto &[filmeID, count] : mapa)
                Filme_cont[filmeID] += count;
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

        for (auto &t : threads) t.join();

        for (const auto &buf : blocos_finais)
            for (const auto &linha : buf)
                bloco_escrita.push_back(linha);
    }

    fim = high_resolution_clock::now();
    duracao = duration_cast<milliseconds>(fim - inicio);
    cout << "Duracao Filtragem: " << duracao.count() << " ms" << endl;

    // Escrita do arquivo
    escreverArquivo(bloco_escrita, "datasets/input.dat");

    // Libera o mmap
    munmap(base, tamanho_arquivo);
}

// Função de Leitura (evitar stringstream)
void bloco_leitura(Usuarios &local, const unordered_map<int, pair<size_t, size_t>> &intervalos,
    vector<int> Users, unordered_map<int, int> &AvalUser, unordered_map<int, int> &UserFilme,
    char *base) {
    for (int userID : Users) {
        auto it = intervalos.find(userID);
        if (it == intervalos.end()) continue;

        size_t inicio = it->second.first;
        size_t fim = it->second.second;

        size_t pos = inicio;
        while (pos < fim) {
            size_t linha_fim = pos;
            while (linha_fim < fim && base[linha_fim] != '\n') ++linha_fim;

            string_view linha(&base[pos], linha_fim - pos);

            size_t p0 = linha.find(',');
            size_t p1 = linha.find(',', p0 + 1);
            size_t p2 = linha.find(',', p1 + 1);

            if (p0 == string_view::npos || p1 == string_view::npos || p2 == string_view::npos) {
                pos = linha_fim + 1;
                continue;
            }

            int uid = stoi(string(linha.substr(0, p0)));
            int filmeID = stoi(string(linha.substr(p0 + 1, p1 - p0 - 1)));
            float nota = stof(string(linha.substr(p1 + 1, p2 - p1 - 1)));
            int ts = stoi(string(linha.substr(p2 + 1)));

            auto &ref = local[uid][filmeID];
            if (ref.second < ts) {
                ref = { nota, ts };
            }

            AvalUser[uid]++;
            UserFilme[filmeID]++;

            pos = linha_fim + 1;
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
void UserIntervalos(unordered_map<int, pair<size_t, size_t>> &intervalo, size_t inicio, size_t fim, char *base) {
    size_t pos = inicio;

    // Garante que estamos na linha completa
    while (pos < fim && base[pos] != '\n') ++pos;
    ++pos; // Pula '\n'

    while (pos < fim) {
        size_t linha_fim = pos;
        while (linha_fim < fim && base[linha_fim] != '\n') ++linha_fim;

        string_view linha(&base[pos], linha_fim - pos);

        size_t p0 = linha.find(',');
        if (p0 != string_view::npos) {
            int uid = stoi(string(linha.substr(0, p0)));
            auto &local = intervalo[uid];
            if (local.first == local.second) {
                local = { pos, linha_fim + 1 };
            } else {
                local.second = linha_fim + 1;
            }
        }

        pos = linha_fim + 1;
    }
}

string formatarNota(float nota) {
    ostringstream out;
    out << fixed << setprecision(1) << nota;
    return out.str();
}

char *mmapArquivo(const string &caminho, size_t &tamanho) {
    int fd = open(caminho.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir arquivo com open()");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Erro ao obter estatísticas do arquivo");
        exit(EXIT_FAILURE);
    }

    tamanho = sb.st_size;

    char *data = static_cast<char *>(mmap(nullptr, tamanho, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED) {
        perror("Erro ao mapear o arquivo com mmap()");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return data;
}

void escreverArquivo(const vector<string> &linhas, const string &caminho) {
    int fd = open(caminho.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Erro ao abrir arquivo para escrita");
        exit(EXIT_FAILURE);
    }

    for (const auto &linha : linhas) {
        write(fd, linha.c_str(), linha.size());
        write(fd, "\n", 1);
    }

    close(fd);
}