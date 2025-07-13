#include "PreProcessamento.hpp"
#include <charconv>


void loadInput() {
  size_t tamanho_arquivo;
  char *base = mmapArquivo("datasets/ratings.csv", tamanho_arquivo);

  int n_threads = thread::hardware_concurrency();
  vector<thread> threads;
  unordered_map<int, pair<size_t, size_t>> mapa_intervalos;
  unordered_map<int, vector<int>> usersDistribuidos(n_threads);

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

    vector<pair<int, size_t>> users;
    for (const auto &[uid, intervalo] : mapa_intervalos) {
      users.emplace_back(uid, intervalo.second - intervalo.first);
    }
    sort(users.begin(), users.end(), [](auto &a, auto &b) {
      return a.second > b.second;
    });

    vector<size_t> carga(n_threads, 0);
    for (const auto &[uid, peso] : users) {
      int idx = min_element(carga.begin(), carga.end()) - carga.begin();
      usersDistribuidos[idx].push_back(uid);
      carga[idx] += peso;
    }
  }

  unordered_map<int, int> User_cont, Filme_cont;
  vector<Usuarios> mapas_locais(n_threads);
  {
    vector<unordered_map<int, int>> AvalUser(n_threads), UserFilme(n_threads);
    for (int i = 0; i < n_threads; ++i) {
      threads.emplace_back([&, i]() {
        bloco_leitura(ref(mapas_locais[i]), ref(mapa_intervalos), ref(usersDistribuidos[i]),
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

  vector<string> bloco_escrita;
  {
    vector<vector<string>> blocos_finais(n_threads);
    for (int i = 0; i < n_threads; ++i) {
      threads.emplace_back([&, i]() {
        bloco_filtro(mapas_locais[i], User_cont, Filme_cont, blocos_finais[i]);
      });
    }
    for (auto &t : threads) t.join();

    for (const auto &buf : blocos_finais)
      bloco_escrita.insert(bloco_escrita.end(), buf.begin(), buf.end());
  }

  escreverArquivo(bloco_escrita, "datasets/input.dat");
  munmap(base, tamanho_arquivo);
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

void UserIntervalos(unordered_map<int, pair<size_t, size_t>> &intervalo, size_t inicio, size_t fim, char *base) {
  size_t pos = inicio;
  while (pos < fim && base[pos] != '\n') ++pos;
  ++pos;

  while (pos < fim) {
    size_t linha_fim = pos;
    while (linha_fim < fim && base[linha_fim] != '\n') ++linha_fim;

    string_view linha(&base[pos], linha_fim - pos);
    size_t p0 = linha.find(',');

    if (p0 != string_view::npos) {
      int uid;
      std::from_chars(&linha[0], &linha[0] + p0, uid);
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

void bloco_leitura(Usuarios &local, const unordered_map<int, pair<size_t, size_t>> &intervalos,
  const vector<int> &users, unordered_map<int, int> &AvalUser,
  unordered_map<int, int> &UserFilme, char *base) {
  for (int userID : users) {
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

      int uid, filmeID, ts;
      float nota;
      std::from_chars(&linha[0], &linha[p0], uid);
      std::from_chars(&linha[p0 + 1], &linha[p1], filmeID);
      std::from_chars(&linha[p1 + 1], &linha[p2], nota);
      std::from_chars(&linha[p2 + 1], &linha[linha.size()], ts);

      auto &filmes = local[uid];
      auto it_f = filmes.find(filmeID);
      if (it_f == filmes.end() || it_f->second.second < ts) {
        filmes[filmeID] = { nota, ts };
      }

      AvalUser[uid]++;
      UserFilme[filmeID]++;

      pos = linha_fim + 1;
    }
  }
}

void bloco_filtro(Usuarios &mapa, const unordered_map<int, int> &User_cont,
  const unordered_map<int, int> &Filme_cont, vector<string> &bloco) {
  for (auto it = mapa.begin(); it != mapa.end();) {
    int userID = it->first;

    if (User_cont.at(userID) < config::minAval) {
      ++it;
      continue;
    }

    auto &filmes = it->second;
    for (auto it_f = filmes.begin(); it_f != filmes.end();) {
      if (Filme_cont.at(it_f->first) < config::minUsers) {
        it_f = filmes.erase(it_f);
      } else {
        ++it_f;
      }
    }

    if (filmes.size() < static_cast<size_t>(config::minAval)) {
      it = mapa.erase(it);
    } else {
      ostringstream linha;
      linha << userID;
      for (const auto &[filmeID, nota] : filmes) {
        linha << " " << filmeID << ":" << formatarNota(nota.first);
      }
      bloco.push_back(linha.str());
      ++it;
    }
  }
}

string formatarNota(float nota) {
  ostringstream out;
  out << fixed << setprecision(1) << nota;
  return out.str();
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
