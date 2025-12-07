#ifndef TSPRESULT_H
#define TSPRESULT_H

#include <vector>
#include <iostream>

/**
 * @brief Estrutura para armazenar o resultado final de uma solução do problema do caixeiro viajante
 * @tparam Node O tipo de dado dos nós no caminho
 */
template<typename Node>
struct TSPResult
{
    std::vector<Node> path; // Caminho percorrido
    double cost; // Custo total do caminho

    TSPResult() : cost(0.0) {}
};

/**
 * @brief Função para imprimir o resultado do TSP
 * @tparam Node O tipo de dado dos nós no caminho
 * @param result O resultado do TSP a ser impresso
 */
template<typename Node>
void print_tsp_result(const TSPResult<Node>& result) {
    std::cout << "TSP Path: ";
    for (const auto& node : result.path) {
        std::cout << node << " ";
    }
    std::cout << "\nTotal Cost: " << result.cost << std::endl;
}


#endif