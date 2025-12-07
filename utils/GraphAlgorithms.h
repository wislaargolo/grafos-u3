#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include "../graph/UndirectedAdjacencyListGraph.h"
#include "../graph/UndirectedAdjacencyMatrixGraph.h"
#include "../graph/UndirectedIncidenceMatrixGraph.h"
#include "Bfs.h"

/**
 * @brief Verifica se um grafo não-direcionado é conectado.
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser verificado.
 * @return true se o grafo for conectado, false caso contrário.
 */
template<typename Node>
bool is_connected(const IGraph<Node>& graph) {
    if (graph.get_order() <= 1) {
        return true;
    }

    /*Se todos os nós são acessíveis a partir de um nó inicial, o grafo é conectado*/
    std::vector<Node> visited = bfs(graph, graph.get_nodes().front());
    return visited.size() == graph.get_order();
}


/**
 * @brief Copia os nós e as arestas de um grafo de origem para um grafo de destino.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param from O grafo de origem.
 * @param to O grafo de destino que será modificado para conter os mesmos nós e arestas do grafo de origem.
 */
template<typename Node>
void copy_graph(const IGraph<Node>& from, IGraph<Node>& to) {
    for (Node node : from.get_nodes()) {
        to.add_node(node);

        for (Node neighbor : from.get_neighbors(node)) {
            to.add_edge(node, neighbor);
        }
    }
}

#endif
