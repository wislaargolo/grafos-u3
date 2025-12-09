#ifndef TSPRESULT_H
#define TSPRESULT_H

#include <vector>
#include <iostream>
#include "../graph/IGraph.h"

/**
 * @brief Estrutura para armazenar o resultado final de uma solução do problema do caixeiro viajante
 */
struct TSPResult
{
    std::vector<int> path; // Caminho percorrido em índices
    double cost; // Custo total obtido

    TSPResult() : cost(0.0) {}
};

/**
 * @brief Função para imprimir o resultado do TSP
 * @tparam Node O tipo de dado dos nós no caminho
 * @param result O resultado do TSP a ser impresso
 */
template<typename Node>
void print_tsp_result(const IGraph<Node>& graph, const TSPResult& result) {
    std::cout << "TSP Path: ";
    for (const auto& node : result.path) {
        std::cout << graph.get_node(node) << " ";
    }
    std::cout << "\nTotal Cost: " << result.cost << std::endl;
}


#endif