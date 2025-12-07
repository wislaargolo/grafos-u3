#include <vector>
#include <limits>
#include <cstddef>

std::vector<int> nearest_insertion(const size_t graph_order, 
    const std::vector<std::vector<double>>& weights, int start_index) {

    std::vector<bool> in_path(graph_order, false);
    std::vector<int> path;

    // Adiciona o nó inicial ao caminho
    path.push_back(start_index);
    in_path[start_index] = true;

    // Adiciona o nó mais próximo do nó inicial ao caminho
    double min_distance = std::numeric_limits<double>::infinity();
    int nearest_index = -1;

    // Procura o nó mais próximo do nó inicial
    for (size_t i = 0; i < graph_order; i++) {
        if (i != start_index && weights[start_index][i] < min_distance) {
            min_distance = weights[start_index][i];
            nearest_index = i;
        }
    }

    if(nearest_index == -1) {
        return path; // Retorna apenas o nó inicial se nenhum outro nó for encontrado
    }
    // Insere o nó mais próximo do inicial no caminho
    path.push_back(nearest_index);
    in_path[nearest_index] = true;

    // Enquanto existirem nós não inseridos no caminho
    while (path.size() < graph_order) {
        double best_increase = std::numeric_limits<double>::infinity();
        int best_node = -1;
        size_t best_position = 0;

        for (size_t node = 0; node < graph_order; node++) {
            //Para cada nó não inserido no caminho
            if (in_path[node]) continue;

            for (size_t position = 0; position < path.size(); position++) {
                size_t next_position = (position + 1) % path.size();
                double increase = weights[path[position]][node] + 
                                  weights[node][path[next_position]] - 
                                  weights[path[position]][path[next_position]];

                if (increase < best_increase) {
                    best_increase = increase;
                    best_node = node;
                    best_position = next_position;
                }
            }
        }

        // Insere o melhor nó encontrado na melhor posição
        path.insert(path.begin() + best_position, best_node);
        in_path[best_node] = true;
    }

    return path;
}
