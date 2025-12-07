#include <vector>
#include <limits>
#include <cstddef>

std::vector<int> nearest_neighbor(const size_t graph_order, 
    const std::vector<std::vector<double>>& weights, int start_index) {

    std::vector<int> path;
    std::vector<bool> visited(graph_order, false);

    // Configura o nó inicial, marca como visitado e adiciona ao caminho
    int current_index = start_index;
    visited[current_index] = true;
    path.push_back(current_index);

    while(path.size() < graph_order) {
        double min_distance = std::numeric_limits<double>::infinity();
        int next_index = -1;

        // Procura o nó mais próximo não visitado
        for (size_t i = 0; i < graph_order; i++) {
            // Verifica se o nó já foi visitado e se a distância do nó atual para ele é menor que a mínima encontrada até agora
            if (!visited[i] && weights[current_index][i] < min_distance) {
                min_distance = weights[current_index][i];
                next_index = i;
            }
        }

        // Se não houver mais nós não visitados, encerra o loop
        if(next_index == -1) {
            break;
        }

        // Marca o próximo nó como visitado e adiciona ao caminho
        visited[next_index] = true;
        path.push_back(next_index);
        current_index = next_index;

    }

    return path;
}