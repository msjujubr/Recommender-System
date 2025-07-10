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
  int n_threads = min(static_cast<int>(thread::hardware_concurrency()),
                      8); // Limit to 8 threads max

  auto inicio = high_resolution_clock::now();

  std::unordered_map<int, pair<streampos, streampos>> mapa_intervalos;
  std::unordered_map<int, vector<int>> UsersInverv;

  // Mapeamento dos usuários
  {
    vector<std::unordered_map<int, pair<streampos, streampos>>> locais(n_threads);
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
      for (auto const& [uid, intervalo] : local) {
        auto existing = mapa_intervalos.find(uid);
        if (existing == mapa_intervalos.end()) {
          mapa_intervalos.emplace(uid, intervalo);
        } else {
          auto &entry = existing->second;
          entry.first = min(entry.first, intervalo.first);
          entry.second = max(entry.second, intervalo.second);
        }
      }
    }

    int idx = 0;
    for (auto const& [userID, intervalo] : mapa_intervalos) {
      UsersInverv[idx % n_threads].push_back(userID);
      ++idx;
    }
  }
  auto fim = high_resolution_clock::now();
  auto duracao = duration_cast<milliseconds>(fim - inicio);
  cout << "Duracao mapeamento: " << duracao.count() << " ms" << endl;

  inicio = high_resolution_clock::now();

  std::unordered_map<int, unsigned long> User_cont, Filme_cont;
  vector<Usuarios> mapas_locais(n_threads);

  // Leitura dos usuários em blocos
  {
    vector<std::unordered_map<int, unsigned long>> AvalUser(n_threads),
        UserFilme(n_threads);
    for (unsigned long i = 0; i < static_cast<unsigned long>(n_threads); ++i) {
      threads.emplace_back([&, i]() {
        bloco_leitura(ref(mapas_locais[i]), ref(mapa_intervalos),
                      UsersInverv[i], ref(AvalUser[i]), ref(UserFilme[i]));
      });
    }

    for (auto &t : threads) {
      t.join();
    }
    threads.clear();

    // Contagem de avaliações por usuário e filmes
    for (const auto &mapa : AvalUser) {
      for (auto const& [uid, count] : mapa) {
        User_cont[uid] += count;
      }
    }

    // Unifica as contagens de avaliações por filme
    for (const auto &mapa : UserFilme) {
      for (auto const& [filmeID, count] : mapa) {
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

  for (const auto &linha : bloco_escrita) {
    arq_saida << linha << '\n';
  }
}

// Função de Leitura ultra-otimizada
void bloco_leitura(
    Usuarios &local,
    const std::unordered_map<int, pair<streampos, streampos>> &intervalos,
    vector<int> Users, std::unordered_map<int, unsigned long> &AvalUser,
    std::unordered_map<int, unsigned long> &UserFilme) {

  FILE *file = fopen("datasets/ratings.csv", "rb");
  if (!file) {
    cerr << "ratings.csv não encontrado. Abortando.\n";
    exit(EXIT_FAILURE);
  }

  // Use large buffer for faster reading
  constexpr size_t BUFFER_SIZE = 1024 * 1024; // 1MB buffer
  static thread_local char *file_buffer = new char[BUFFER_SIZE];
  setvbuf(file, file_buffer, _IOFBF, BUFFER_SIZE);

  for (int userID : Users) {
    auto it = intervalos.find(userID);
    if (it == intervalos.end())
      continue;

    fseek(file, it->second.first, SEEK_SET); 
    long fim = it->second.second;            

    char line[256];
    while (ftell(file) < fim && fgets(line, sizeof(line), file)) {
      char *ptr = line;
      int uid = 0;

      while (*ptr >= '0' && *ptr <= '9') {
        uid = uid * 10 + (*ptr - '0');
        ptr++;
      }
      if (*ptr != ',')
        continue;
      ptr++;

      int filmeID = 0;
      while (*ptr >= '0' && *ptr <= '9') {
        filmeID = filmeID * 10 + (*ptr - '0');
        ptr++;
      }
      if (*ptr != ',')
        continue;
      ptr++;

      float nota = 0.0f;
      float decimal = 0.1f;
      bool in_decimal = false;
      while ((*ptr >= '0' && *ptr <= '9') || *ptr == '.') {
        if (*ptr == '.') {
          in_decimal = true;
        } else if (!in_decimal) {
          nota = nota * 10.0f + (*ptr - '0');
        } else {
          nota += (*ptr - '0') * decimal;
          decimal *= 0.1f;
        }
        ptr++;
      }
      if (*ptr != ',')
        continue;
      ptr++;

      int ts = 0;
      while (*ptr >= '0' && *ptr <= '9') {
        ts = ts * 10 + (*ptr - '0');
        ptr++;
      }

      // Usar find/emplace para evitar criação desnecessária de objetos
      auto& user_ratings = local[uid];
      auto rating_it = user_ratings.find(filmeID);
      if (rating_it == user_ratings.end()) {
          user_ratings.emplace(filmeID, make_pair(nota, ts));
      } else {
          if (rating_it->second.second < ts) {
              rating_it->second = {nota, ts};
          }
      }

      AvalUser[uid]++;
      UserFilme[filmeID]++;
    }
  }

  fclose(file);
}

// Função de Filtragem
void bloco_filtro(Usuarios &mapa,
                  const std::unordered_map<int, unsigned long> &User_cont,
                  const std::unordered_map<int, unsigned long> &Filme_cont,
                  vector<string> &bloco) {
  for (auto it = mapa.begin(); it != mapa.end();) {
    int userID = it->first;

    // Remove usuários com avaliações insuficientes
    auto user_count = User_cont.find(userID);
    if (user_count == User_cont.end() ||
        user_count->second < static_cast<unsigned long>(config::minAval)) {
      it++;
      continue;
    }

    auto &filmes = it->second;

    // Filtra os filmes do usuário
    for (auto it_f = filmes.begin(); it_f != filmes.end();) {
      auto filme_count = Filme_cont.find(it_f->first);
      if (filme_count == Filme_cont.end() ||
          filme_count->second < static_cast<unsigned long>(config::minUsers)) {
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
void UserIntervalos(std::unordered_map<int, pair<streampos, streampos>> &intervalo,
                    streampos inicio, streampos fim) {
  FILE *arquivo = fopen("datasets/ratings.csv", "rb");
  if (!arquivo) {
    cerr << "ratings.csv não encontrado. Abortando.\n";
    exit(EXIT_FAILURE);
  }

  // Buffer otimizado
  static thread_local char buffer[16384]; // 16KB buffer
  setvbuf(arquivo, buffer, _IOFBF, sizeof(buffer));

  fseek(arquivo, inicio, SEEK_SET);
  // Ignorar a primeira linha se não for o início do arquivo
  if (inicio != 0) {
      char temp_line[256];
      if (fgets(temp_line, sizeof(temp_line), arquivo) == NULL) {
          // Handle error or EOF
      }
  }

  streampos pos_linha_ini = ftell(arquivo);
  char linha[256];

  while (pos_linha_ini < fim && fgets(linha, sizeof(linha), arquivo)) {
    streampos pos_linha_fim = ftell(arquivo);
    char *end;
    int userID = strtol(linha, &end, 10);
    if (*end != ',') {
      pos_linha_ini = pos_linha_fim;
      continue;
    }

    auto &local = intervalo[userID];
    if (local.first == local.second) {
      local = {pos_linha_ini, pos_linha_fim};
    } else {
      local.second = pos_linha_fim;
    }

    pos_linha_ini = pos_linha_fim;
  }
  fclose(arquivo);
}

string formatarNota(float nota) {
  ostringstream out;
  out << fixed << setprecision(1) << nota;
  return out.str();
}

