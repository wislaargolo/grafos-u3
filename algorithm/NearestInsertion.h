#include <vector>
#include <limits>

#include "../graph/IGraph.h"
#include "TSPResult.h"
#include "LocalSearch.h"

/**
 * @brief Implementa o algoritmo da inserção mais próxima para o problema do caixeiro viajante
 * @param graph_order Número de nós no grafo
 * @param weights A matriz de pesos
 * @param start_index O índice do nó inicial para o percurso
 * @return Um vetor com a ordem dos índices dos nós visitados no percurso
 */
std::vector<int> nearest_insertion(size_t graph_order, const std::vector<std::vector<double>>& weights, int start_index);

/**
 * @brief Combina o algoritmo da inserção mais próxima com busca local
 * @param graph O grafo para aplicação do algoritmo
 * @param weights A matriz de pesos
 */
template<typename Node>
TSPResult nearest_insertion_local_search(const IGraph<Node>& graph,
    const std::vector<std::vector<double>>& weights, int start_index, 
    LocalSearchMethod method, ImprovementType improvement) {

    std::vector<int> initial_path = nearest_insertion(graph.get_order(), weights, start_index);

    LocalSearchResult local_search_result = local_search(weights, initial_path, method, improvement);

    TSPResult result;
    result.cost = local_search_result.cost;
    result.path = local_search_result.solution;

    return result;
}