#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include <vector>
#include <string>

/**
 * @brief Estrutura para armazenar o resultado da busca local
 */
struct LocalSearchResult {
    std::vector<int> solution; // Solução encontrada
    double cost; // Custo total da solução

    LocalSearchResult() : cost(0.0) {}
};


/**
 * @brief Enum para os tipos de métodos de busca local disponíveis
 */
enum class LocalSearchMethod {
    SWAP,
    SHIFT,
    INVERT
};

/**
 * @brief Enum para os tipos de estratégias de melhoria disponíveis
 */
enum class ImprovementType {
    FIRST_IMPROVEMENT,
    BEST_IMPROVEMENT
};

LocalSearchResult local_search(const std::vector<std::vector<double>>& weights,
                               const std::vector<int>& initial_path,
                               LocalSearchMethod method,
                               ImprovementType improvement);

void apply_shift(std::vector<int>& path, size_t i, size_t j);
void apply_swap(std::vector<int>& path, size_t i, size_t j);
void apply_invert(std::vector<int>& path, size_t i, size_t j);


#endif