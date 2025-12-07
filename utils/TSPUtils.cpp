#include "TSPUtils.h"

#include <limits>
#include <cstddef>

double calculate_path_cost(const std::vector<std::vector<double>>& weights, const std::vector<int>& path) {
    double total_cost = 0.0;
    size_t path_size = path.size();

    // Se o caminho tiver menos de 2 nós, o custo é zero
    if(path_size < 2) {
        return total_cost;
    }

    // Soma os custos entre os nós consecutivos no caminho
    for (size_t i = 0; i < path_size - 1; ++i) {
        int from = path[i];
        int to = path[i + 1];
        total_cost += weights[from][to];
    }

    // Adiciona o custo de retorno ao nó inicial para completar o ciclo
    total_cost += weights[path[path_size - 1]][path[0]];

    return total_cost;
}