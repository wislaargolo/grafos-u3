#ifndef MEMETIC_SEARCH_H
#define MEMETIC_SEARCH_H

#include <vector>
#include "../graph/IGraph.h"
#include "GeneticSearch.h"
#include "LocalSearch.h"

template <typename Node>
void print_population(const IGraph<Node> &graph, const std::vector<std::vector<double>> &weights,
                      std::vector<Individual> &population)
{
    std::cout << "[Population]\n";
    for (size_t i = 0; i < population.size(); i++)
    {
        double cost = population[i].cost;
        std::cout << "Individual " << i + 1 << ": Cost = " << cost << ", Path = ";
        for (const auto &node : population[i].path)
        {
            std::cout << graph.get_node(node) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
};

// (1) Inicio
// Geração da População Inicial
template <typename Node>
std::vector<Individual> generate_initial_population(const IGraph<Node> &graph,
                                                    const std::vector<std::vector<double>> &weights)
{
    return generate_population(graph, weights);
};

// (2) Fitness
void calculate_initial_fitness(std::vector<Individual> &population,
                               const std::vector<std::vector<double>> &weights)
{
    calculate_fitness(population, weights);
};

// (3) Nova Geração
std::vector<Individual> generate_new_individuas(std::vector<Individual> &population, const std::vector<std::vector<double>> &weights, int iteration_count, std::pair<int, int> &last_parents)
{
    std::pair<int, int> parents = select_parents(population, iteration_count, last_parents);

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

    return offsprings;
};

// (4) Busca local
// Função para melhorar cada indivíduo da população usando busca local
// Note: não é template porque o tipo de nó não é necessário aqui
void improve_individuas(
    const std::vector<std::vector<double>> &weights,
    std::vector<Individual> &population,
    LocalSearchMethod method,
    ImprovementType improvement)
{

    for (auto &individual : population)
    {
        LocalSearchResult improved = local_search(weights, individual.path,
                                                  method, improvement);
        individual.path = improved.solution;
        individual.cost = improved.cost;
        individual.fitness = 1 / improved.cost;
    }
};

// (5) Renovar
void renew_population(std::vector<Individual> &population, const std::vector<Individual> offsprings, const std::vector<std::vector<double>> &weights)
{
    population = renovation_elitism(population, offsprings, weights);
};

// (6) Teste
void evaluate_population(
    const std::vector<std::vector<double>> &weights,
    const std::vector<Individual> population,
    Individual &best_solution,
    int &stagnant_count)
{
    int best_index = 0;

    for (int i = 1; i < population.size(); i++)
    {
        if (population[i].cost < population[best_index].cost)
        {
            best_index = i;
        }
    }

    if (population[best_index].cost < best_solution.cost)
    {
        best_solution = population[best_index];
        stagnant_count = 0;
    }
    else
    {
        stagnant_count++;
    }
};

template <typename Node>
TSPResult memetic_search(const IGraph<Node> &graph,
                         const std::vector<std::vector<double>> &weights)
{

    // (1) Inicio
    std::vector<Individual> population = generate_initial_population(graph, weights);

    // (2) Fitness
    calculate_initial_fitness(population, weights);

    // Inicializa variaveis de controle da execução
    int stagnant_count = 0;
    Individual best_solution = population[0];
    std::pair<int, int> last_parents = {-1, -1};

    for (int i = 0; i < MAX_ITERATIONS_NUMBER && stagnant_count < MAX_STAGNANT_ITERATIONS_NUMBER; i++)
    {
        // (3) Nova Geração
        std::vector<Individual> offspring = generate_new_individuas(population, weights, i, last_parents);

        // (4) Busca local
        improve_individuas(weights, offspring, LocalSearchMethod::SWAP, ImprovementType::FIRST_IMPROVEMENT);

        // (5) Renovar
        renew_population(population, offspring, weights);

        // (6) Teste
        evaluate_population(weights, population, best_solution, stagnant_count);
    }

    TSPResult result;
    result.cost = best_solution.cost;
    result.path = best_solution.path;
    return result;
};

#endif // MEMETIC_SEARCH_H
