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

#define POPULATION_SIZE 500
#define MAX_ITERATIONS_NUMBER 10000
#define MAX_STAGNANT_ITERATIONS_NUMBER 10000
#define MUTATION_PERCENT 0.5

struct Individual {
  std::vector<int> path;
  double cost;
  double fitness;
};

std::vector<int> generate_random_path(size_t order) {
    std::vector<int> path(order);

    for (size_t i = 0; i < order; i++) {
        path[i] = i;
    }

    std::shuffle(path.begin(), path.end(), std::mt19937{std::random_device{}()});

    return path;
}

template<typename Node>
std::vector<Individual> generate_population(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights) {

    std::vector<Individual> population;

    std::vector<int> cheapest_insertion_result = cheapest_insertion(graph, weights, graph.get_node(0));
    std::vector<int> nearest_neighbor_result = nearest_neighbor(graph, weights, graph.get_node(0));

    population.push_back({cheapest_insertion_result, -1, -1});
    population.push_back({nearest_neighbor_result, -1, -1});

    for (size_t i = 2; i < POPULATION_SIZE; i++) {
        std::vector<int> random_path = generate_random_path(graph.get_order());
        population.push_back({random_path, -1, -1});
    }

    return population;
}

void calculate_fitness(std::vector<Individual>& population,
    const std::vector<std::vector<double>>& weights) {

    for (auto& item : population) {
        item.cost = calculate_path_cost(weights, item.path);
        item.fitness = 1 / item.cost;
    }
}

std::pair<int, int> select_best_parents(const std::vector<Individual>& population, std::pair<int, int> last_parents) {
    int first_better = -1, second_better = -1;

    for (size_t i = 0; i < population.size(); i++) {
        if (i == last_parents.first || i == last_parents.second) {
            continue;
        }

        if (first_better == -1 || population[i].fitness > population[first_better].fitness) {
            second_better = first_better;
            first_better = i;
        } else if (second_better == -1 || population[i].fitness > population[second_better].fitness) {
            second_better = i;
        }
    }

    return std::make_pair(first_better, second_better);;
}

std::pair<int, int> select_random_parents(const std::vector<Individual>& population) {
    int first_parent = std::rand() % population.size();
    int second_parent = (first_parent + std::rand() % (population.size() - 1) + 1) % population.size();

    return std::make_pair(first_parent, second_parent);
}

std::pair<int, int> select_parents(const std::vector<Individual>& population, int iteration_count,
    std::pair<int, int>& last_parents) {

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

// ALGORTIMOS DE MUTAÇÃO

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

template<typename Node>
std::vector<int> genetic_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights) {

    std::srand(std::time(nullptr));

    std::vector<Individual> population = generate_population(graph, weights);
    calculate_fitness(population, weights);

    int stagnant_count = 0;
    Individual best_solution = population[0];

    std::pair<int, int> last_parents = {-1, -1};

    for (int i = 0; i < MAX_ITERATIONS_NUMBER && stagnant_count < MAX_STAGNANT_ITERATIONS_NUMBER; i++) {
        std::pair<int, int> parents = select_parents(population, i, last_parents);

        // Cruzamento por crossover
        std::vector<int> path1 = ordered_crossover(population[parents.first], population[parents.second]);
        Individual child1 = {path1, -1, -1};

        std::vector<int> path2 = ordered_crossover(population[parents.second], population[parents.first]);
        Individual child2 = {path2, -1, -1};

        // Mutação com taxa de 50%
        apply_mutation(child1, MUTATION_PERCENT);
        apply_mutation(child2, MUTATION_PERCENT);

        child1.cost = calculate_path_cost(weights, child1.path);
        child2.cost = calculate_path_cost(weights, child2.path);
        child1.fitness = 1 / child1.cost;
        child2.fitness = 1 / child2.cost;

        std::vector<Individual> offsprings = {child1, child2};

        // Renovação da população com elitismo
        population = renovation_elitism(population, offsprings, weights);

        int best_index = 0;

        for (int i = 1; i < population.size(); i++) {
            if (population[i].cost < population[best_index].cost) {
                best_index = i;
            }
        }

        if (population[best_index].cost < best_solution.cost) {
            best_solution = population[best_index];
            stagnant_count = 0;
        } else {
            stagnant_count++;
        }
    }

    return best_solution.path;
}

#endif
