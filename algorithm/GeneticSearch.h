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

#define POPULATION_SIZE 100
#define MAX_ITERATIONS_NUMBER 1000
#define MAX_STAGNANT_ITERATIONS_NUMBER 20

struct GeneticSolution {
  std::vector<int> path;
  double cost;
};

template<typename Node>
std::vector<int> generate_random_path(int order) {
    std::vector<int> path(order);

    for (int i = 0; i < order; i++) {
        path[i] = i;
    }

    std::shuffle(path.begin(), path.end(), std::mt19937{std::random_device{}()});

    return path;
}

double calculate_path_cost(const std::vector<std::vector<double>>& weights,
    const std::vector<int>& path) {

    double total_cost = 0.0;

    for (size_t i = 0; i < path.size(); i++) {
        int a = path[i];
        int b = path[i + 1 == path.size() ? 0 : i + 1];

        total_cost += weights[a][b];
    }

    return total_cost;
}

template<typename Node>
std::vector<std::vector<int>> generate_population(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, Node start_node) {

    std::vector<std::vector<int>> population;

    std::vector<int> cheapest_insertion_result = cheapest_insertion(graph, weights, start_node);
    std::vector<int> nearest_neighbor_result = nearest_neighbor(graph, weights, start_node);

    population.push_back(cheapest_insertion_result);
    population.push_back(nearest_neighbor_result);

    for (size_t i = 2; i < POPULATION_SIZE; i++) {
        std::vector<int> random_path = generate_random_path(graph.get_order());
        population.push_back(random_path);
    }

    return population;
}

int select_random_parent(const std::vector<double>& fitness_scores) {
    double total = 0;

    for (double score : fitness_scores) {
        total += score;
    }

    std::random_device device;
    std::mt19937 gen(device());
    std::uniform_real_distribution<double> distribution(0.0, total);

    double random = distribution(gen);
    total = 0;

    for (int i = 0; i < fitness_scores.size(); i++) {
        total += fitness_scores[i];

        if (total >= random) {
            return i;
        }
    }

    return fitness_scores.size() - 1;
}

/**
 * @brief Realiza o crossover ordenado entre dois pais para gerar um filho
 * @param first_parent O primeiro pai
 * @param second_parent O segundo pai
 * @param rng O gerador de números aleatórios
 * @return O caminho do filho gerado pelo crossover
 */
std::vector<int> ordered_crossover(const std::vector<int>& first_parent, const std::vector<int>& second_parent) {
    int total_nodes = first_parent.size();
    std::vector<int> offspring(total_nodes, -1);

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
        int node = first_parent[i];
        offspring[i] = node;
        is_node_inserted[node] = true;
    }

    // Indica onde começar a preencher o restante do filho
    int current_index_second_parent = (end_index + 1) % total_nodes;
    int current_index_offspring = (end_index + 1) % total_nodes;

    while (current_index_offspring != start_index) {
        int candidate_node = second_parent[current_index_second_parent];

        // Verifica se o nó já foi inserido
        if (!is_node_inserted[candidate_node]) {
            offspring[current_index_offspring] = candidate_node;
            is_node_inserted[candidate_node] = true;

            current_index_offspring = (current_index_offspring + 1) % total_nodes;
        }

        current_index_second_parent = (current_index_second_parent + 1) % total_nodes;
    }

    return offspring;
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
void apply_mutation(std::vector<int>& individual, double mutation_rate) {

    double random_chance = (double)std::rand() / RAND_MAX;

    if (random_chance < mutation_rate) {
        // Sorteia um número entre 0 e 2 para decidir o tipo
        int mutation_type = std::rand() % 3;

        switch (mutation_type) {
            case 0:
                mutation_swap(individual);
                break;
            case 1:
                mutation_inversion(individual);
                break;
            case 2:
                mutation_scramble(individual);
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
std::vector<std::vector<int>> renovation_elitism(
    const std::vector<std::vector<int>>& current_population,
    const std::vector<std::vector<int>>& offsprings,
    const std::vector<std::vector<double>>& weights
) {
    // Copia da população atual
    std::vector<std::vector<int>> new_population = current_population;

    // Criamos uma tabela para ordenar os indivíduos pelo custo
    std::vector<std::pair<double, int>> leaderboard;

    for (int i = 0; i < new_population.size(); ++i) {
        double cost = calculate_path_cost(weights, new_population[i]);
        leaderboard.push_back({cost, i});
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
    const std::vector<std::vector<double>>& weights, Node start_node) {

    std::vector<std::vector<int>> population = generate_population(graph, weights, start_node);

    std::vector<double> fitness_scores(population.size(), 0.0);

    for (std::vector<int>& path) {
        fitness_scores[i] = 1 / calculate_path_cost(weights, path);
    }

    int stagnant_count = 0;
    int best_solution_cost = INFINITY;
    std::vector<int>& best_solution;

    for (int i = 0; i < MAX_ITERATIONS_NUMBER; i++) {
        int parent_a_index = select_random_parent(fitness_scores);
        int parent_b_index = select_random_parent(fitness_scores);

        if (parent_a_index == parent_b_index) {
            parent_b_index = (parent_b_index + 1) % population.size();
        }

        // cruzamento, mutação, renovação

        if (stagnant_count > MAX_STAGNANT_ITERATIONS_NUMBER) {
            break;
        }
    }

    return best_solution;
}

#endif
