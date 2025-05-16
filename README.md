# Sistema de Recomendação 
![C++](https://img.shields.io/badge/Linguagem-C++-green.svg)
![Make](https://img.shields.io/badge/Compilacao-Make-orange)
![VSCode](https://img.shields.io/badge/IDE-VSCode-informational)
![ISO](https://img.shields.io/badge/ISO-Linux-blueviolet)

 Este trabalho consiste no desenvolvimento de um sistema de recomendação que, a partir de perfis de usuários e características de itens, seja capaz de sugerir agrupamentos de elementos
 similares. Utilizando ....
 <!--- resumo da metodologia, fazer referêndcia a livros e qualquer material que utilizamos --->
 <!--- deixar os calculos por aqui também? --->
 


 Cálculos como medida de distância euclidiana, similaridade do cosseno ou Jaccard
 devem ser considerados como alternativas. Avaliação conforme critérios de eficiência computacional, qualidade dos resultados, organização do código e documentação, conforme descrito a seguir.
  
<div align="center"> <table> <tr> <td>
 
### 📖 Sumário
- [O que são Sistemas de Recomendação?]()
- [Metodologia]()
  - [Lógica Utilizada](#lógica-utilizada)
  - [Comportamento](#comportamento)
- [Análise de Performance]()
- [Compilação, Entradas e Saídas](#compilação-entradas-e-saídas)
  - [MakeFile](#makefile)
  - [Input.dat](#inputdat)
  - [Output.dat](#outputdat)
- [Referências](#referências)

</td> <td>

 <img src="imgs/fluxograma.png" width="400">

</td> </tr> </table> </div>

# O que são Sistemas de Recomendação?
 

# Metodologia

## Lógica Utilizada

***É possível visualizar o processo baixando o [fluxograma](docs/imgs/fluxograma.png) contido neste repositório.***

| Função            | Descrição                                                  |
|-------------------|------------------------------------------------------------|
| [`bool atividade_fogo()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L260)  | Retorna true (1) caso ainda há árvores para serem queimadas.  |
| [`void configuracoes()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L28) | Processa o arquivo Input.dat e armazena as informações nas variáveis globais; Inicializa o animal |
| [`int defVento()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L8)     | Retorna um número de acordo com o caso de vento (tabela abaixo) |
| [`void inicio_animal()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L265)    | Inicializa as variáveis do animal; Gera uma coordenada aleatória entre os 0 e 1 disponíveis na matriz para ser a posição inicial do animal na simulação |
| [`void prop(int x, int y, vector<pair<int,int>>& auxiliar)`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L286) | Confere se na posição (x, y) tem uma árvore saudável (1), se caso afirmativo, queima (2) e armazena no vetor |
| [`void propagacao()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L67)     | Espalha o fogo de acordo com o caso de vento; Para todas as árvores que queimaram na iteração anterior (arv_1_2), pega os vizinhos possíveis e chama a função prop() para cada um; As árvores em arv_1_2 vão para o vetor arv_2_3 e as novas árvores queimadas são armazenadas em arv_1_2. |
| [`void queimada()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L295)  | Define como queimadas (3) todas as árvores do vetor arv_2_3 |
| [`void salvar()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L241)    |  Salva a matriz e as coordenadas do animal |
| [`void relatorio()`](https://github.com/msjujubr/Atividade01/blob/main/src/config.cpp#L249) | Salva a quantidade de passos do animal e o estado dele ao final do programa |

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
- [Documento Prática](docs/documento_atividade01.pdf)
- [1]: https://youtu.be/NqUSJWec3pM?si=C33oaYJOJ01Xs7y5  
  *Vídeo do Cosmopolita explicando o Jogo da Vida*
- [2]: https://archive.org/details/a-new-kind-of-science-stephen-wolfram-z-lib.org/mode/2up
  *Livro completo: A New Kind of Science – Stephen Wolfram (2002)*
- [3]: https://github.com/MasterGos/magisterka/blob/master/Materialy%20z%20sieci/AOP/Wiley%20-%20Wooldridge,%20An%20Introduction%20to%20Multi%20Agent%20Systems%20(OCR%20guaranteed%20on%20full%20book).pdf
  *Link do GitHub do magisterka com o livro: An Introduction to Multi Agent Systems - Michael Wooldridge*
- [4]: https://direct.mit.edu/books/monograph/2503/Growing-Artificial-SocietiesSocial-Science-from
  *Livro no MIT Press: Growing Artificial Societies: Social Science from the Bottom Up (1996)*

<div> 
  <a href="https://www.youtube.com/@msjujubr" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" target="_blank"></a>
  <a href="https://instagram.com/msjujubr" target="_blank"><img src="https://img.shields.io/badge/-Instagram-%23E4405F?style=for-the-badge&logo=instagram&logoColor=white" target="_blank"></a>
 	<a href="https://www.twitch.tv/msjujubr" target="_blank"><img src="https://img.shields.io/badge/Twitch-9146FF?style=for-the-badge&logo=twitch&logoColor=white" target="_blank"></a>
  <a href = "mailto:juliamourasouza10@gmail.com"><img src="https://img.shields.io/badge/-Gmail-%23333?style=for-the-badge&logo=gmail&logoColor=white" target="_blank"></a>
  <a href="https://www.linkedin.com/in/msjujubr/" target="_blank"><img src="https://img.shields.io/badge/-LinkedIn-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white" target="_blank"></a>
</div>
