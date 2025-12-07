#ifndef TSPUTILS_H
#define TSPUTILS_H

#include <vector>
#include <string>
#include "../algorithm/LocalSearch.h"

/**
 * @brief Calcula o custo total de um caminho baseado na matriz de pesos fornecida.
 * @param weights A matriz de pesos entre os nós do grafo
 * @param path o caminho com a ordem dos índices dos nós visitados
 * @return O custo total do caminho
 */
double calculate_path_cost(const std::vector<std::vector<double>>& weights, const std::vector<int>& path);

/**
 * @brief Converte o método de busca local para string
 * @param m O método de busca local
 */
std::string method_to_string(LocalSearchMethod m);

/**
 * @brief Converte o tipo de melhoria para string
 * @param t O tipo de melhoria
 */
std::string improvement_to_string(ImprovementType t);

#endif