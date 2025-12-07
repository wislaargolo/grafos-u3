#ifndef TSPUTILS_H
#define TSPUTILS_H

#include <vector>

/**
 * @brief Calcula o custo total de um caminho baseado na matriz de pesos fornecida.
 * @param weights A matriz de pesos entre os nós do grafo
 * @param path o caminho com a ordem dos índices dos nós visitados
 * @return O custo total do caminho
 */
double calculate_path_cost(const std::vector<std::vector<double>>& weights, const std::vector<int>& path);

#endif