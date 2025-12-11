#ifndef GENETIC_SEARCH_H
#define GENETIC_SEARCH_H

#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "../graph/IGraph.h"
#include "CheapestInsertion.h"
#include "NearestNeighbor.h"
#include "../utils/TSPUtils.h"

// Tamanho da população durante o algoritmo genético
#define POPULATION_SIZE 500
// Número de iterações realizadas pelo algoritmo
#define MAX_ITERATIONS_NUMBER 10000
// Percentual de mutação dos indivíduos
#define MUTATION_PERCENT 0.5

/**
 * @brief Estrutura que armazena um indivíduo da população, com a solução "path referente a ele", seu custo e fitness
 */
struct Individual {
  std::vector<int> path;
  double cost;
  double fitness;
};

/**
 * @brief Gera uma solução aleatória
 * @param order Ordem do grafo a ser considerado, ou seja, número de nós da solução gerada
 * @return Solução gerada aleatoriamente representada por vetor de tamanho igual a order
 */
std::vector<int> generate_random_path(size_t order) {
    std::vector<int> path(order);

    // Inicializa a lista com elementos de 0 até order
    for (size_t i = 0; i < order; i++) {
        path[i] = i;
    }

    // Embaralha a lista
    std::shuffle(path.begin(), path.end(), std::mt19937{std::random_device{}()});

    return path;
}

/**
 * @brief Gera a população inicial do algoritmo genético
 * @param graph Grafo que o algoritmo está utilizando
 * @param weights Matriz de pesos do grafo utilizado
 * @return Vetor de indivíduos representando a população inicial
 */
template<typename Node>
std::vector<Individual> generate_population(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights) {

    std::vector<Individual> population;

    // Primeiramente insere os resultados dos algoritmos vizinho mais próximo e inserção mais barata na população
    std::vector<int> cheapest_insertion_result = cheapest_insertion(graph, weights, graph.get_node(0));
    std::vector<int> nearest_neighbor_result = nearest_neighbor(graph, weights, graph.get_node(0));

    population.push_back({cheapest_insertion_result, -1, -1});
    population.push_back({nearest_neighbor_result, -1, -1});

    // Preenche o restante da população com soluções aleatórias
    for (size_t i = 2; i < POPULATION_SIZE; i++) {
        std::vector<int> random_path = generate_random_path(graph.get_order());
        population.push_back({random_path, -1, -1});
    }

    return population;
}

/**
 * @brief Calcula o custo e o fitness de cada indivíduo da população
 * @param population População utilizada pelo algoritmo a ser atualizada
 * @param weights Matriz de pesos do grafo utilizado
 */
void calculate_fitness(std::vector<Individual>& population,
    const std::vector<std::vector<double>>& weights) {

    // Para cada indivíduo, calcula seu custo e depois seu fitness como sendo o inverso do custo
    for (auto& item : population) {
        item.cost = calculate_path_cost(weights, item.path);
        item.fitness = 1 / item.cost;
    }
}

/**
 * @brief Seleciona os dois indivíduos com melhor fitness para serem pais
 * @param population População para se pegar os dois indivíduos
 * @param last_parents Par com índices dos dois últimos pais selecionados por essa função, para não repetí-los 2 vezes seguidas
 * @return Par com índices dos dois indivíduos de melhor fitness que são diferentes dos dois em last_parents
 */
std::pair<int, int> select_best_parents(const std::vector<Individual>& population, std::pair<int, int> last_parents) {
    int first_better = -1, second_better = -1;

    // Para cada indivíduo que não foi um dos dois últimos pais selecionados
    for (size_t i = 0; i < population.size(); i++) {
        if (i == last_parents.first || i == last_parents.second) {
            continue;
        }

        // Verifica primeiramente se é melhor que o primeiro melhor encontrado até então
        if (first_better == -1 || population[i].fitness > population[first_better].fitness) {
            // Se sim, atualiza o primeiro e o segundo melhores
            second_better = first_better;
            first_better = i;
        }
        // Se não, verifica se é melhor que o segundo melhor encontrado até então
        else if (second_better == -1 || population[i].fitness > population[second_better].fitness) {
            // Se sim, atualiza o segundo melhor
            second_better = i;
        }
    }

    // Retorna um par com os dois melhores fitness diferentes de last_parents
    return std::make_pair(first_better, second_better);;
}

/**
 * @brief Seleciona dois indivíduos aleatórios e distintos da população
 * @param population População para se pegar os dois indivíduos
 * @return Par com os índices de dois indivíduos aleatórios e distintos
 */
std::pair<int, int> select_random_parents(const std::vector<Individual>& population) {
    // Seleciona aleatoriamente o índice do primeiro indivíduo
    int first_parent = std::rand() % population.size();
    // Seleciona aleatoriamente o índice segundo indivíduo de forma que não seja igual ao primeiro
    int second_parent = (first_parent + std::rand() % (population.size() - 1) + 1) % population.size();

    // Retorna um par com os índices dos dois indivíduos aleatórios
    return std::make_pair(first_parent, second_parent);
}

/**
 * @brief Seleciona os pais que irão reproduzir na iteração atual de maneira híbrida
 * @param population População para se pegar os dois indivíduos
 * @param iteration_count Contagem de iterações do algoritmo
 * @param last_parents Par com os índices dos últimos dois pais selecionados por elitismo
 * @return Par com índices dos pais escolhidos, através do elitismo ou da aleatoriedade dependendo da iteração
 */
std::pair<int, int> select_parents(const std::vector<Individual>& population, int iteration_count,
    std::pair<int, int>& last_parents) {

    // A cada 4 iterações, em 3 a seleção é feita pelo elitismo e em uma a seleção é aleatória
    if (iteration_count % 4 == 3) {
        return select_random_parents(population);
    } else {
        last_parents = select_best_parents(population, last_parents);

        return last_parents;
    }
}

/**
 * @brief Realiza o crossover ordenado entre dois pais para gerar um filho
 * @param first_parent O primeiro pai
 * @param second_parent O segundo pai
 * @param rng O gerador de números aleatórios
 * @return O caminho do filho gerado pelo crossover
 */
std::vector<int> ordered_crossover(const Individual& first_parent, const Individual& second_parent) {
    int total_nodes = first_parent.path.size();
    std::vector<int> path(total_nodes, -1);

    // Pontos de corte aleatórios
    int start_index = std::rand() % total_nodes;
    int end_index = std::rand() % total_nodes;

    if (start_index > end_index) {
        std::swap(start_index, end_index);
    }

    // Mapeia para rastrear quais nós já foram inseridos no filho
    std::vector<bool> is_node_inserted(total_nodes, false);

    // copia fatia
    for (int i = start_index; i <= end_index; ++i) {
        int node = first_parent.path[i];
        path[i] = node;
        is_node_inserted[node] = true;
    }

    // Indica onde começar a preencher o restante do filho
    int current_index_second_parent = (end_index + 1) % total_nodes;
    int current_index_offspring = (end_index + 1) % total_nodes;

    while (current_index_offspring != start_index) {
        int candidate_node = second_parent.path[current_index_second_parent];

        // Verifica se o nó já foi inserido
        if (!is_node_inserted[candidate_node]) {
            path[current_index_offspring] = candidate_node;
            is_node_inserted[candidate_node] = true;

            current_index_offspring = (current_index_offspring + 1) % total_nodes;
        }

        current_index_second_parent = (current_index_second_parent + 1) % total_nodes;
    }

    return path;
}

/**
 * @brief Realiza a mutação por troca entre dois genes
 * @param individual O indivíduo a ser mutado
*/
void mutation_swap(std::vector<int>& individual) {
    int size = individual.size();

    int first_index = std::rand() % size;
    int second_index = std::rand() % size;

    std::swap(individual[first_index], individual[second_index]);
}

/**
 * @brief Realiza a mutação por inversão de um trecho
 * @param individual O indivíduo a ser mutado
 */
void mutation_inversion(std::vector<int>& individual) {
    int size = individual.size();

    int first_random_index = std::rand() % size;
    int second_random_index = std::rand() % size;

    int start = std::min(first_random_index, second_random_index);
    int end = std::max(first_random_index, second_random_index);

    std::reverse(individual.begin() + start, individual.begin() + end + 1);
}

/**
 * @brief Realiza a mutação por embaralhamento de um trecho
 * @param individual O indivíduo a ser mutado
 */
void mutation_scramble(std::vector<int>& individual) {
    int size = individual.size();

    int first_random_index = std::rand() % size;
    int second_random_index = std::rand() % size;

    int start = std::min(first_random_index, second_random_index);
    int end = std::max(first_random_index, second_random_index);

    int range_size = end - start;

    if (range_size > 0) {
        for(int i = 0; i < range_size; i++) {
            // Sorteia dois offsets dentro do intervalo e troca os elementos correspondentes
            int offset_a = std::rand() % (range_size + 1);
            int offset_b = std::rand() % (range_size + 1);

            std::swap(individual[start + offset_a], individual[start + offset_b]);
        }
    }
}

/**
 * @brief Aplica mutação em um indivíduo com base na taxa de mutação
 * @param individual O indivíduo a ser mutado
 * @param mutation_rate A taxa de mutação (entre 0.0 e 1.0)
 */
void apply_mutation(Individual& individual, double mutation_rate) {

    double random_chance = (double)std::rand() / RAND_MAX;

    if (random_chance < mutation_rate) {
        // Sorteia um número entre 0 e 2 para decidir o tipo
        int mutation_type = std::rand() % 3;

        switch (mutation_type) {
            case 0:
                mutation_swap(individual.path);
                break;
            case 1:
                mutation_inversion(individual.path);
                break;
            case 2:
                mutation_scramble(individual.path);
                break;
        }
    }
}

/**
 * @brief Aplica elitismo copiando o melhor indivíduo da população atual para a próxima geração
 * @param current_population A população atual
 * @param offsprings Os filhos gerados para a próxima geração
 * @param weights A matriz de pesos para cálculo do custo
 * @return A nova população após aplicar o elitismo
 */
std::vector<Individual> renovation_elitism(
    const std::vector<Individual> current_population,
    const std::vector<Individual>& offsprings,
    const std::vector<std::vector<double>>& weights
) {
    // Copia da população atual
    std::vector<Individual> new_population = current_population;

    // Criamos uma tabela para ordenar os indivíduos pelo custo
    std::vector<std::pair<double, int>> leaderboard;

    for (int i = 0; i < new_population.size(); ++i) {
        leaderboard.push_back({new_population[i].cost, i});
    }

    // Ordena do menor para o maior custo
    std::sort(leaderboard.begin(), leaderboard.end());

    int num_children = offsprings.size();
    int population_size = new_population.size();

    // Substitui os piores indivíduos pelos filhos gerados
    for (int i = 0; i < num_children; ++i) {

        if (i < population_size) {

            // Pegamos o índice do pior indivíduo e subtituímos pelo filho
            int worst_index = leaderboard[population_size - 1 - i].second;
            new_population[worst_index] = offsprings[i];
        }
    }

    return new_population;
}

/**
 * @brief Função que executa o algoritmo genético
 * @param graph Grafo para ser executado o algoritmo
 * @param weights Matriz de peso do grafo
 * @return Melhor solução encontrada durante toda a execução do algoritmo
 */
template<typename Node>
std::vector<int> genetic_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights) {

    std::srand(std::time(nullptr));

    // Gera e calcula o fitness da população inicial
    std::vector<Individual> population = generate_population(graph, weights);
    calculate_fitness(population, weights);

    // Declara a melhor solução atual como sendo a primeira
    Individual best_solution = population[0];

    // Variável utilizada para armazenar os últimos pais a serem selecionados pelo elitismo
    std::pair<int, int> last_parents = {-1, -1};

    for (int i = 0; i < MAX_ITERATIONS_NUMBER; i++) {
        // Seleção por elitismo ou aleatoriedade
        std::pair<int, int> parents = select_parents(population, i, last_parents);

        // Cruzamento por crossover
        std::vector<int> path1 = ordered_crossover(population[parents.first], population[parents.second]);
        Individual child1 = {path1, -1, -1};

        std::vector<int> path2 = ordered_crossover(population[parents.second], population[parents.first]);
        Individual child2 = {path2, -1, -1};

        // Mutação com taxa de 50%
        apply_mutation(child1, MUTATION_PERCENT);
        apply_mutation(child2, MUTATION_PERCENT);

        // Cálculo do fitness dos filhos
        child1.cost = calculate_path_cost(weights, child1.path);
        child2.cost = calculate_path_cost(weights, child2.path);
        child1.fitness = 1 / child1.cost;
        child2.fitness = 1 / child2.cost;

        std::vector<Individual> offsprings = {child1, child2};

        // Renovação da população com elitismo
        population = renovation_elitism(population, offsprings, weights);

        int best_index = 0;

        // Verificação de qual a melhor solução da população
        for (int i = 1; i < population.size(); i++) {
            if (population[i].cost < population[best_index].cost) {
                best_index = i;
            }
        }

        // Verificação se foi encontrada uma melhor solução do que a melhor até então
        if (population[best_index].cost < best_solution.cost) {
            best_solution = population[best_index];
        }
    }

    return best_solution.path;
}

#endif
