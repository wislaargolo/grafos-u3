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
std::vector<int> nearest_neighbor(const size_t graph_order, 
    const std::vector<std::vector<double>>& weights, int start_index);

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
TSPResult nearest_neighbor_local_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, int start_index, 
    LocalSearchMethod method, ImprovementType improvement) {

    std::vector<int> initial_path = nearest_neighbor(graph.get_order(), weights, start_index);

    LocalSearchResult local_search_result = local_search(weights, initial_path, method, improvement);

    TSPResult result;
    result.cost = local_search_result.cost;
    result.path = std::move(local_search_result.solution);

    return result;
}



#endif