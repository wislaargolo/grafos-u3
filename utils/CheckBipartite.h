#ifndef CHECK_BIPARTITION_H
#define CHECK_BIPARTITION_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "../graph/IGraph.h"

/**
 * @brief Dfs da verificação se o grafo é bipartido
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser percorrido.
 * @param discovery O vetor de visitados da dfs.
 * @param node O vértice que está sendo visitado no momento.
 * @return true se o grupo de nós alcançado for bipartido, false caso contrário.
 */
template<typename Node>
bool check_bipartite_dfs(const IGraph<Node>& graph, std::vector<int>& discovery, int node) {
    // Percorre todos os vizinhos do vértice atual
    for (int neighbor_index : graph.get_neighbors_indices(node)) {
        // Se o vizinho não foi descoberto ainda
        if (discovery[neighbor_index] == -1) {
            // "Colore" o vizinho com a cor oposta e visita
            discovery[neighbor_index] = 1 - discovery[node];

            // Se foi encontrado algum conflito de cores, retorna falso
            if (!check_bipartite_dfs(graph, discovery, neighbor_index)) {
                return false;
            }
        }
        // Se o vizinho já foi descoberto e a cor dele é a mesma do vértice atual
        else if (discovery[node] == discovery[neighbor_index]) {
            // Retorna falso, indicando que o grafo não é bipartido
            return false;
        }
    }

    // Se não foi encontrado nenhum conflito, retorna verdareiro
    return true;
}

/**
 * @brief Verifica se o grafo é bipartido.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser verificado.
 * @return true se o grafo for bipartido, false caso contrário.
 */
template<typename Node>
bool is_graph_bipartite(const IGraph<Node>& graph) {
    size_t size = graph.get_nodes().size();

    if (size == 0) {
        throw std::invalid_argument("Graph is empty");
    }

    // Array de descoberta da dfs, -1 representa não descoberto, e 0 e 1 representam as cores
    std::vector<int> discovery(size, -1);

    // Percorre todos os vértices e visita os que não foram visitados
    for (size_t i = 0; i < size; i++) {
        if (discovery[i] == -1) {
            discovery[i] = 0;

            // Se foi encontrado conflito nessa visita, retorna que o grafo não é bipartido
            if (!check_bipartite_dfs(graph, discovery, i)) {
                return false;
            }
        }
    }

    // Se não foi encontrado conflito, retorna que o grafo é bipartido
    return true;
}

#endif
