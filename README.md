# Sistema de Recomendação 
![C++](https://img.shields.io/badge/Linguagem-C++-green.svg)
![Make](https://img.shields.io/badge/Compilacao-Make-orange)
![VSCode](https://img.shields.io/badge/IDE-VSCode-informational)
![ISO](https://img.shields.io/badge/ISO-Linux-blueviolet)

<div align="center"> <table> <tr> <td>
 
### 📖 Sumário
- [Introdução](#introducao)
  - [O que são Sistemas de Recomendação?](#o-que-sao-sistemas-de-recomendacao)
  - [Objetivos do Trabalho](#objetivos-do-Trabalho)
- [Metodologia](#metodologia)
  - [Organização do Trabalho](#organizacao-do-trabalho)
  - [Processamento dos Dados](#processamento-dos-dados)
  - [Ferramentas Utilizadas](#ferramentas-utilizadas)
- [Análise de Performance](#analise-de-performance)
  - [Comportamento](#comportamento)
  - [Avaliação Geral](#avaliacao-geral)
- [Compilação e Execução](#compilação-entradas-e-saídas)
  - [MakeFile](#makefile)
  - [Input.dat](#input.dat)
  - [Explore.dat](#explore.dat)
  - [Output.dat](#output.dat)
- [Conclusão](#conclusao)
- [Referências](#referências)
- [Autores](#autores)
- [Agradecimentos](#agradecimentos)

</td> <td>

 <img src="imgs/fluxograma.png" width="400">

</td> </tr> </table> </div>

# Introdução
 Este trabalho consiste no desenvolvimento de um sistema de recomendação que, a partir de perfis de usuários e características de itens, seja capaz de sugerir agrupamentos de elementos
 similares. Utilizando ....
 <!--- resumo da metodologia, fazer referêndcia a livros e qualquer material que utilizamos --->
 <!--- deixar os calculos por aqui também? --->
  Cálculos como medida de distância euclidiana, similaridade do cosseno ou Jaccard
 devem ser considerados como alternativas. Avaliação conforme critérios de eficiência computacional, qualidade dos resultados, organização do código e documentação, conforme descrito a seguir.
 
## O que são Sistemas de Recomendação?

## Compilação e Execução
 Pré-processamento: O pré-processamento deverá seguir os seguintes critérios:
 • Utilizar apenas usuários que tenham realizado pelo menos 50 avaliações distintas.
 • Utilizar apenas filmes avaliados por pelo menos 50 usuários.
 • Remover registros duplicados ou inconsistentes.
 • Gerar arquivo de entrada no seguinte formato:
 usuario_id item_id1:nota1 item_id2:nota2 item_id3:nota3 ...
 Exemplo: 123 12:4.0 54:3.5 76:5.0 145:2.0
 Note que, cada linha representa um usuário (usuario_id) e suas respectivas avaliações (item_id:nota)
 
 Para o arquivo que será utilizado como fonte de informação para a execução das recomendações, tem-se como padrões a serem seguidos:
 Trabalho Final
 Page 1
• Nomedoarquivo: input.dat
 • Localização: Diretório datasets/
 • Formato: Texto puro (UTF-8)
 • Necessidade: Arquivo que representará a base de dados para exploração

 Já para os arquivos de exploração e de saída, espera-se que ambos sigam os seguintes
 padrões:
 • Nome do arquivo: explore.dat
 • Localização: Diretório datasets/
 • Formato: Texto puro (UTF-8)
 • Necessidade: Arquivo que conterá os usuários utilizados para exploração das recomen
dações
 • Nome do arquivo: output.dat
 • Localização: Diretório outcome/
 • Formato: Texto puro (UTF-8)
 • Necessidade: Arquivo contendo as K recomendações para cada usuário apresentados
 no arquivo explore.dat

 Para cada usuario_id listado, o procedimento a ser seguido consiste em:
 • Realizar a busca na base de dados para identificar os filmes previamente avaliados por
 este usuário.
 • A partir desse conjunto de avaliações, calcular a similaridade ou afinidade do usuário em
 relação aos demais usuários da base, utilizando a métrica de distância ou similaridade
 definida pelo projeto.
 Trabalho Final
 Page 2
• Selecionar os K usuários mais similares (de maior afinidade) ao usuário em análise.
 • Identificar os filmes avaliados positivamente pelos usuários similares, mas ainda não
 avaliados pelo usuário-alvo, priorizando aqueles com maior grau de sobreposição de interesse.
 • A partir dessa análise, gerar as recomendações a serem atribuídas a cada usuario_id.
 Os resultados deste processo deverão ser armazenados no arquivo output.dat, obedecendo
 o formato estipulado, em que cada linha corresponde a um usuario_id seguido pelos item_ids
 recomendados

 O arquivo output.dat deverá conter o formato abaixo, sendo o número de recomendações
 (Top-N) será definido via constante global no arquivo config.h.
 usuario_id item_id1 item_id2 item_id3 ...
 Exemplo: 123 54 76 145

# Metodologia

## Lógica Utilizada

A lógica de desenvolvimento do sistema de recomendação segue uma ordem de implementação que consiste em:

```mermaid
 flowchart TD
    A[Início do Programa] --> B{Carregar e Pré-processar Dados}
    B --> C[Ler ratings.csv]
    C --> D{Aplicar Filtros:}
    D --> D1[minUsuario = 50 avaliações]
    D --> D2[minFilme = 50 avaliações]
    D1 --> E[Gerar datasets/input.dat]
    D2 --> E
    E --> F[Gerar Matriz Esparsa de Usuários]
    F --> F1[Normalizar Matriz para Similaridade de Cosseno]
    F1 --> G{Configurar LSH com Projeções Aleatórias}
    G --> G1[Definir num_projections e num_bands]
    G1 --> H[Indexar Matriz de Usuários no LSH]
    H --> I[Ler Usuários para Exploração - explore.dat]
    I --> J{Para cada Usuário a Recomendar:}
    J --> J1[Consultar Vizinhos Candidatos via LSH]
    J1 --> J2[Calcular Similaridade de Cosseno]
    J2 --> J3[Selecionar Top-K Usuários Mais Similares]
    J3 --> J4[Gerar Recomendações Top-N com base nos Top-K Vizinhos]
    J4 --> K[Escrever Recomendações em outputs.dat]
    K --> L[Fim do Programa]


```

### Pré-processamento

### Aplicação de Filtros

### Gerar Input

### Criação da Matriz Esparsa

A partir de input.dat, é construída uma matriz de usuários esparsa. Essa matriz é representada internamente por um `unordered_map<int, unordered_map<int, float>>`, onde:

- A primeira chave (int) é o userId.

- O unordered_map interno mapeia movieId `int` para a nota `float`.

A matriz implementada é esparsa e armazena apenas avaliações existentes (não-nulas), economizando memória significativa para um dataset massivo como o MovieLens 25M.

Durante a construção, também é criado um mapeamento de movieId para um índice contínuo `filme_indice`, essencial para o alinhamento com os vetores de projeção aleatória do LSH.

As avaliações de cada usuário são normalizadas (divididas pela norma L2 de seu vetor de avaliações) para que o produto escalar direto possa ser usado para calcular a Similaridade de Cosseno (esta parte é feita pela função `normalizarMatriz`) .

Por fim, a matriz esparsa de usuários fica no seguinte formato:

| Usuário ID | Filme 101 | Filme 205 | Filme 330 | Filme 402 | ... |
|------------|-----------|-----------|-----------|-----------|-----|
| 1001       | 0.25      | 0.43      | ---       | 0.77      | ... |
| 1002       | ---       | 0.58      | 0.64      | ---       | ... |
| 1003       | 0.36      | ---       | 0.49      | 0.61      | ... |
| ...        | ...       | ...       | ...       | ...       | ... |

- `---` indica ausência de avaliação (zero implícito na matriz esparsa).
- Os valores são **notas já normalizadas** (norma L2 = 1 por linha).

### Implementar LSH com Random Projections

A implementação do Locality Sensitive Hashing (LSH) com Projeções Aleatórias no código segue um fluxo bem definido para permitir a busca eficiente de vizinhos aproximados em espaços de alta dimensionalidade. A ideia central é mapear vetores de alta dimensão (as avaliações dos usuários) para assinaturas binárias (hashes) menores, de forma que vetores similares tenham hashes similares.

A estrutura principal que encapsula o índice LSH é `LSHIndex`, definida como:

- `random_hyperplanes`: Um `vector` de `vector<float>`, onde cada `vector<float>` representa um hiperplano aleatório. O tamanho interno de cada hiperplano é igual ao número total de filmes únicos (`uniqueMovies.size()`).

- `user_signatures`: Um `unordered_map` que armazena a assinatura LSH completa (`LSHSignature`, um `vector<bool>`) para cada `userID`.

- `lsh_buckets`: Um vector de `unordered_map<string, vector<int>>`. Cada elemento no vector representa uma banda (`L`). O `unordered_map` interno mapeia a chave da banda (a string binária da porção da assinatura) para um vector de `userID`s que caem naquele balde para aquela banda específica.

- `filme_id_to_index`: Um u`nordered_map` crucial que mapeia o `filmeID` real para seu índice na representação densa dos vetores (necessário para alinhar as avaliações dos usuários com as dimensões dos hiperplanos).

- `unique_movies_ordered`: Um `vector<int>` que mantém os IDs dos filmes únicos em uma ordem consistente, usada para definir as dimensões dos hiperplanos.

  A construção do índice LSH envolve os seguintes passos, realizados pela função construirIndiceLSH:

#### Geração de Hiperplanos Aleatórios:

- Um conjunto de `config::numHiperplanos` vetores aleatórios é gerado. Cada vetor tem `uniqueMovies.size()` dimensões, com seus componentes amostrados de uma distribuição normal padrão (média 0, desvio padrão 1). Estes vetores são armazenados em `random_hyperplanes`.

#### Criação das Assinaturas de Hash para Cada Usuário:

- Para cada usuário na `userMatrix`:
  - Um produto escalar é calculado entre o vetor de avaliações normalizadas do usuário e cada um dos `random_hyperplanes`.
  - O sinal do produto escalar (>= 0 ou < 0) determina um bit (true ou false) na assinatura LSH do usuário.
  - Esses bits são concatenados para formar a `LSHSignature` completa do usuário, que é então armazenada em `user_signatures`.

#### Agrupamento em Baldes (Bucketing):

- As assinaturas LSH de cada usuário são divididas em `config::numBandas` bandas.
- Para cada banda:
  - A porção da assinatura correspondente à banda é extraída.
  - Essa porção é convertida em uma string binária (e.g., "0101").
  - Esta string serve como chave para um balde dentro da `lsh_buckets` para a banda atual. Todos os `userID`s que possuem a mesma porção de assinatura nessa banda são colocados no mesmo balde.

 Este processo resulta em um índice LSH que pode ser eficientemente consultado para encontrar usuários candidatos similares, que serão então refinados por um cálculo de similaridade de cosseno completo.
  

### Definir numHiperplanos e numBandas

As constantes `numHiperplanos` (número total de projeções/bits na assinatura) e `numBandas` (número de bandas para o bucketing LSH) são parâmetros cruciais na configuração do algoritmo LSH. Elas são definidas no arquivo `config.hpp` e diretamente controlam o "trade-off" entre a precisão (capacidade de encontrar vizinhos verdadeiros) e a eficiência (velocidade da busca) do sistema LSH.

- `numHiperplanos`:
  - Finalidade: Define o número de hiperplanos aleatórios que serão gerados. Cada hiperplano contribui com um bit (0 ou 1) para a assinatura LSH completa de um usuário.
  - Impacto: Um número maior de numHiperplanos resulta em assinaturas LSH mais longas, o que geralmente aumenta a precisão (a capacidade de distinguir usuários diferentes) mas também pode aumentar o tempo de construção e consulta do índice. No código, está definido como 24.

- `numBandas`:
  - Finalidade: Determina em quantas partes (`L`) a assinatura LSH completa será dividida. Para cada banda, uma sub-assinatura é formada e usada para o bucketing (agrupamento de usuários).
  - Impacto: Um número maior de `numBandas` significa que a assinatura é dividida em mais partes menores. Isso aumenta a probabilidade de que usuários similares caiam no mesmo balde em pelo menos uma banda, o que pode melhorar o "recall" (a capacidade de encontrar todos os vizinhos verdadeiros). No código, está definido como 6.
 
 - `bitsPorBanda`:
   - Finalidade: Define quantos bits de cada assinatura LSH farão parte de uma única banda.
   - Relação: Esta constante está intrinsecamente ligada a numHiperplanos e `numBandas`. A relação fundamental no LSH com Projeções Aleatórias é que o número total de bits (`numHiperplanos`) deve ser igual ao produto do número de bandas pelo número de bits por banda: `numHiperplanos` = `numBandas` * `bitsPorBanda`

### Ler Usuário Para Recomendação (explore.dat)

A leitura de usuários para recomendação é feita com base em um arquivo `explore.dat`, estes usuários receberão as recomendações. Esta leitura é feita pela função `explorador`.

- A função começa abrindo o arquivo explore.dat, que está localizado dentro da pasta datasets/.

- Um loop while (arquivo >> userId) lê os IDs dos usuários. Este método de leitura é eficiente para arquivos onde cada ID está em uma linha separada ou são separados por espaços. Ele continua lendo enquanto houver inteiros válidos no arquivo.

- Cada userId lido é imediatamente adicionado a `vector<int> usuarios`, que acumula todos os identificadores.

- Após ler todos os IDs, o arquivo é fechado para liberar os recursos.

- Por fim a função retorna o vetor usuarios, que agora contém a lista completa de IDs de usuários para os quais o sistema deve processar as recomendações.

### Definir Vizinhos Candidatos com o LSH



### Calcular Similaridade de Cosseno entre Usuários e Vizinhos

A função `similaridade_cosseno` é implementada para tirar proveito da normalização prévida das notas dos usuários. Ela recebe dois `unordered_map`s que representam os vetores de avaliação esparsos dos usuários (já normalizados).

A função `similaridade_cosseno`:

- Percorre os filmes avaliados por um usuário (userA).

- Para cada filme avaliado por userA, verifica se o outro usuário (userB) também avaliou o mesmo filme (userB.count(filmeID)).

- Se sim, o produto das notas (já normalizadas) desses filmes é somado (notaA * userB.at(filmeID)).

- O resultado final é o produto escalar, que, devido à normalização prévia, é diretamente a similaridade de cosseno.

### Gerar Recomendações com Base nos Melhores Vizinhos



**negrito**
*italico*

| Função            | Descrição                                                  |
|-------------------|------------------------------------------------------------|
| [`nomw`](link)  | resumo da função  |

## Comportamento
Assim como no [Jogo da Vida][1] (J. Conway, 1970), a propagação do fogo na matriz é baseada em *Autômatos Celulares*, um modelo computacional introduzido por John von Neumann e amplamente explorado por Stephen Wolfram em sua obra: [A New Kind of Science (2002)][2]. Autômatos celulares consistem em células organizadas em uma grade, onde cada célula possui um estado (como “árvore”, “fogo” ou “zona segura” dentro da matriz "floresta") e evolui ao longo do tempo de acordo com regras locais e discretas. Essa abordagem é ideal para simular fenômenos naturais como incêndios florestais, ou condições de vida como no jogo do Conway, em que cada célula depende do estado de suas vizinhas.

# Compilação, Entradas e Saídas
## Makefile
O programa é executado por um Makefile, arquivo de texto que automatiza o processo de compilação, que interage com todos os arquivos dentro da pasta "src". 
Este apresenta os seguintes comandos:

&nbsp;&nbsp;&nbsp;&nbsp;**make:** Compila o projeto

&nbsp;&nbsp;&nbsp;&nbsp;**make clean:** Remove os arquivos

&nbsp;&nbsp;&nbsp;&nbsp;**make run:** Compila o projeto (se necessário) e depois executa o programa.

&nbsp;&nbsp;&nbsp;&nbsp;**make c:** make clean + make run

[Script do Makefile utilizado (C++)](Makefile)

# Referências
- [Documento - Trabalho Final](docs/trabalho-final.pdf)
- [1]: https://doi.org/10.1145/2827872
- *F. Maxwell Harper and Joseph A. Konstan. 2015. The MovieLens Datasets: History and Context. ACM Transactions on Interactive Intelligent Systems (TiiS) 5, 4: 19:1–19:19.*

# Autores

### Arthur Santana de Mesquita

<div> 
  <a href="https://www.youtube.com/@msjujubr" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" target="_blank"></a>
  <a href="https://instagram.com/msjujubr" target="_blank"><img src="https://img.shields.io/badge/-Instagram-%23E4405F?style=for-the-badge&logo=instagram&logoColor=white" target="_blank"></a>
 	<a href="https://www.twitch.tv/msjujubr" target="_blank"><img src="https://img.shields.io/badge/Twitch-9146FF?style=for-the-badge&logo=twitch&logoColor=white" target="_blank"></a>
  <a href = "mailto:juliamourasouza10@gmail.com"><img src="https://img.shields.io/badge/-Gmail-%23333?style=for-the-badge&logo=gmail&logoColor=white" target="_blank"></a>
  <a href="https://www.linkedin.com/in/msjujubr/" target="_blank"><img src="https://img.shields.io/badge/-LinkedIn-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white" target="_blank"></a>
</div>

### Arthur de Oliveira Mendonça 

<div> 
  <a href="https://www.youtube.com/@msjujubr" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" target="_blank"></a>
  <a href="https://instagram.com/msjujubr" target="_blank"><img src="https://img.shields.io/badge/-Instagram-%23E4405F?style=for-the-badge&logo=instagram&logoColor=white" target="_blank"></a>
 	<a href="https://www.twitch.tv/msjujubr" target="_blank"><img src="https://img.shields.io/badge/Twitch-9146FF?style=for-the-badge&logo=twitch&logoColor=white" target="_blank"></a>
  <a href = "mailto:juliamourasouza10@gmail.com"><img src="https://img.shields.io/badge/-Gmail-%23333?style=for-the-badge&logo=gmail&logoColor=white" target="_blank"></a>
  <a href="https://www.linkedin.com/in/msjujubr/" target="_blank"><img src="https://img.shields.io/badge/-LinkedIn-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white" target="_blank"></a>
</div>

### João Antônio 

<div> 
  <a href="https://www.youtube.com/@msjujubr" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" target="_blank"></a>
  <a href="https://instagram.com/msjujubr" target="_blank"><img src="https://img.shields.io/badge/-Instagram-%23E4405F?style=for-the-badge&logo=instagram&logoColor=white" target="_blank"></a>
 	<a href="https://www.twitch.tv/msjujubr" target="_blank"><img src="https://img.shields.io/badge/Twitch-9146FF?style=for-the-badge&logo=twitch&logoColor=white" target="_blank"></a>
  <a href = "mailto:juliamourasouza10@gmail.com"><img src="https://img.shields.io/badge/-Gmail-%23333?style=for-the-badge&logo=gmail&logoColor=white" target="_blank"></a>
  <a href="https://www.linkedin.com/in/msjujubr/" target="_blank"><img src="https://img.shields.io/badge/-LinkedIn-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white" target="_blank"></a>
</div>


### Júlia de Moura Souza

<div> 
  <a href="https://www.youtube.com/@msjujubr" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" target="_blank"></a>
  <a href="https://instagram.com/msjujubr" target="_blank"><img src="https://img.shields.io/badge/-Instagram-%23E4405F?style=for-the-badge&logo=instagram&logoColor=white" target="_blank"></a>
 	<a href="https://www.twitch.tv/msjujubr" target="_blank"><img src="https://img.shields.io/badge/Twitch-9146FF?style=for-the-badge&logo=twitch&logoColor=white" target="_blank"></a>
  <a href = "mailto:juliamourasouza10@gmail.com"><img src="https://img.shields.io/badge/-Gmail-%23333?style=for-the-badge&logo=gmail&logoColor=white" target="_blank"></a>
  <a href="https://www.linkedin.com/in/msjujubr/" target="_blank"><img src="https://img.shields.io/badge/-LinkedIn-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white" target="_blank"></a>
</div>

# Agradecimentos
