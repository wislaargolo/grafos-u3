#ifndef GENETIC_SEARCH_H
#define GENETIC_SEARCH_H

#include <vector>
#include<random>
#include "../graph/IGraph.h"
#include "CheapestInsertion.h"
#include "NearestNeighbor.h"

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

    for (size_t i = 2; i < 10; i++) {
        std::vector<int> random_path = generate_random_path(graph.get_order());
        population.push_back(random_path);
    }

    return population;
}

template<typename Node>
std::vector<int> genetic_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, Node start_node) {

    std::vector<std::vector<int>> population = generate_population(graph, weights, start_node);

    std::vector<double> fitness_scores(population.size(), 0.0);

    for (size_t i = 0; i < population.size(); i++) {
        fitness_scores[i] = calculate_path_cost(weights, population[i]);
    }

    int first_lowest_index = -1, second_lowest_index = -1;
    double first_lowest_cost = std::numeric_limits<double>::max();
    double second_lowest_cost = std::numeric_limits<double>::max();

    for (size_t i = 0; i < fitness_scores.size(); i++) {
        if (fitness_scores[i] < first_lowest_cost) {
            second_lowest_cost = first_lowest_cost;
            second_lowest_index = first_lowest_index;

            first_lowest_cost = fitness_scores[i];
            first_lowest_index = i;
        } else if (fitness_scores[i] < second_lowest_cost) {
            second_lowest_cost = fitness_scores[i];
            second_lowest_index = i;
        }
    }
}

#endif
