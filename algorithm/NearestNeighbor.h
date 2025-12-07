#ifndef NEARESTNEIGHBOR_H
#define NEARESTNEIGHBOR_H

#include <vector>
#include <limits>
#include <algorithm>
#include <utility>
#include <cstddef>

#include "../graph/IGraph.h"
#include "TSPResult.h"
#include "LocalSearch.h"

/**
 * @brief Implementa o algoritmo do vizinho mais próximo para o problema do caixeiro viajante
 *
 * @param graph_order Número de nós no grafo
 * @param weights A matriz de pesos
 * @param start_index O índice do nó inicial para o percurso
 * @return Um vetor de inteiros representando a ordem dos índices dos nós visitados no percurso.
 */
template<typename Node>
std::vector<int> nearest_neighbor(const IGraph<Node>& graph, 
    const std::vector<std::vector<double>>& weights, Node start_node) {

    std::vector<int> path;
    size_t graph_order = graph.get_order();
    std::vector<bool> visited(graph_order, false);

    // Configura o nó inicial, marca como visitado e adiciona ao caminho
    int current_index = graph.get_index(start_node);
    visited[current_index] = true;
    path.push_back(current_index);

    while(path.size() < graph_order) {
        double min_distance = std::numeric_limits<double>::infinity();
        int next_index = -1;

        // Procura o nó mais próximo não visitado
        for (size_t i = 0; i < graph_order; i++) {
            // Verifica se o nó já foi visitado e se a distância do nó atual para ele é menor que a mínima encontrada até agora
            if (!visited[i] && weights[current_index][i] < min_distance) {
                min_distance = weights[current_index][i];
                next_index = i;
            }
        }

        // Se não houver mais nós não visitados, encerra o loop
        if(next_index == -1) {
            break;
        }

        // Marca o próximo nó como visitado e adiciona ao caminho
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
 * @param start_node O nó inicial para o percurso
 * @param method O método de busca local a ser utilizado
 * @param improvement O tipo de estratégia de melhoria a ser utilizada
 */
template<typename Node>
TSPResult nearest_neighbor_local_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, Node start_node, 
    LocalSearchMethod method, ImprovementType improvement) {

    std::vector<int> initial_path = nearest_neighbor(graph, weights, start_node);

    LocalSearchResult local_search_result = local_search(weights, initial_path, method, improvement);

    TSPResult result;
    result.cost = local_search_result.cost;
    result.path = std::move(local_search_result.solution);

    return result;
}



#endif