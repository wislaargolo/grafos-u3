#ifndef MEMETIC_SEARCH_H
#define MEMETIC_SEARCH_H

#include <vector>
#include "../graph/IGraph.h"
#include "GeneticSearch.h"
#include "LocalSearch.h"

struct MemeticSolution
{
    std::vector<int> path;
    double cost;
};

template <typename Node>
void print_population(const IGraph<Node> &graph, const std::vector<std::vector<double>> &weights,
                      std::vector<std::vector<int>> &population)
{
    std::cout << "[Population]\n";
    for (size_t i = 0; i < population.size(); i++)
    {
        double cost = calculate_path_cost(weights, population[i]);
        std::cout << "Individual " << i + 1 << ": Cost = " << cost << ", Path = ";
        for (const auto &node : population[i])
        {
            std::cout << graph.get_node(node) << " ";
        }
        std::cout << "\n";
    }
};

// (1) Inicio
// Geração da População Inicial
// GeneticSearch::generate_population()
template <typename Node>
std::vector<std::vector<int>> generate_initial_population(const IGraph<Node> &graph,
                                                          const std::vector<std::vector<double>> &weights, Node start_node)
{

    return generate_inital_population(graph, weights, start_node);
};

// (2) Fitness

// (3) Nova Geração

// (4) Busca local
// Função para melhorar cada indivíduo da população usando busca local
// Note: não é template porque o tipo de nó não é necessário aqui
void improve_population(
    const std::vector<std::vector<double>> &weights,
    std::vector<std::vector<int>> &population,
    LocalSearchMethod method,
    ImprovementType improvement)
{

    for (auto &individual : population)
    {
        LocalSearchResult improved = local_search(weights, individual,
                                                  method, improvement);
        individual = improved.solution;
    }
};

// (5) Renovar

// (6) Teste
bool end_search(
    const std::vector<std::vector<double>> &weights,
    std::vector<std::vector<int>> &population)
{
    return false;
};

template <typename Node>
MemeticSolution memetic_search(const IGraph<Node> &graph,
                               const std::vector<std::vector<double>> &weights,
                               Node start_node)
{

    // Gera população inicial usando heurísticas
    std::vector<std::vector<int>> population = generate_initial_population(graph, weights, start_node);

    improve_population(weights, population, LocalSearchMethod::INVERT, ImprovementType::BEST_IMPROVEMENT);
}

#endif // MEMETIC_SEARCH_H
