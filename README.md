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
Este trabalho foi realizado por:

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
