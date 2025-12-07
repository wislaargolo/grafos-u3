#ifndef NEARESTNEIGHBOR_H
#define NEARESTNEIGHBOR_H

#include <vector>
#include <limits>
#include <algorithm>

#include "../graph/IGraph.h"
#include "TSPResult.h"
#include "LocalSearch.h"

/**
 * @brief Implementa o algoritmo do vizinho mais próximo para o problema do caixeiro viajante
 *
 * @tparam Node O tipo de dado dos nós no grafo
 * @param graph O grafo para aplicação do algoritmo
 * @param weights A matriz de pesos
 * @param start_index O índice do nó inicial para o percurso
 * @return Um vetor de inteiros representando a ordem dos índices dos nós visitados no percurso.
 */
template<typename Node>
std::vector<int> nearest_neighbor(const IGraph<Node>& graph, 
    const std::vector<std::vector<double>>& weights, int start_index) {

    std::vector<int> path;
    size_t order = graph.get_order();

    std::vector<bool> visited(order, false);

    int current_index = start_index;
    visited[current_index] = true;
    path.push_back(current_index);

    while(path.size() < order) {
        double min_distance = std::numeric_limits<double>::infinity();
        int next_index = -1;

        for (size_t i = 0; i < order; i++) {
            if (!visited[i] && weights[current_index][i] < min_distance) {
                min_distance = weights[current_index][i];
                next_index = i;
            }
        }

        if(next_index == -1) {
            break;
        }

        visited[next_index] = true;
        path.push_back(next_index);
        current_index = next_index;

    }

    return path;
}

/**
 * @brief Combina o algoritmo do vizinho mais próximo com busca local
 * @tparam Node O tipo de dado dos nós no grafo
 * @param graph O grafo para aplicação do algoritmo
 * @param weights A matriz de pesos
 * @param start_index O índice do nó inicial
 * @param method O método de busca local a ser utilizado
 * @param improvement O tipo de estratégia de melhoria a ser utilizada
 */
template<typename Node>
TSPResult<Node> nearest_neighbor_local_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, int start_index, 
    LocalSearchMethod method, ImprovementType improvement) {

    std::vector<int> initial_path = nearest_neighbor(graph, weights, start_index);

    LocalSearchResult local_search_result = local_search(weights, initial_path, method, improvement);

    TSPResult<Node> result;
    result.cost = local_search_result.cost;

    for(int index : local_search_result.solution) {
        result.path.push_back(graph.get_node(index));
    }

    return result;
}

template<typename Node>
void print_local_search_result(const TSPResult<Node>& result) {
    std::cout << "Path: ";
    for (const auto& node : result.path) {
        std::cout << node << " ";
    }
    std::cout << "\nTotal Cost: " << result.cost << std::endl;
}


#endif