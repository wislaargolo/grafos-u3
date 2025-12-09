#include <vector>
#include <algorithm>
#include <utility>

#include "LocalSearch.h"
#include "TSPResult.h"
#include "../utils/TSPUtils.h"


/**
 * @brief Aplica a operação de swap em dois índices do caminho
 * @param path O caminho onde a operação será aplicada
 * @param i O primeiro índice
 * @param j O segundo índice
 */
void apply_swap(std::vector<int>& path, size_t i, size_t j) {
    std::swap(path[i], path[j]);
}

/**
 * @brief Aplica a operação de shift em dois índices do caminho
 * @param path O caminho onde a operação será aplicada
 * @param i O índice do nó a ser movido
 * @param j O índice para onde o nó será movido
 */
void apply_shift(std::vector<int>& path, size_t i, size_t j) {

    // Se i é menor que j, move o elemento de i para j deslocando os outros para a esquerda
    if(i < j) {
        int temp = path[i];
        for(size_t k = i; k < j; ++k) {
            path[k] = path[k + 1];
        }
        path[j] = temp;
    } else { 
        // Se i é maior que j, move o elemento de i para j deslocando os outros para a direita
        int temp = path[i];
        for(size_t k = i; k > j; --k) {
            path[k] = path[k - 1];
        }
        path[j] = temp;
    }
}

/**
 * @brief Aplica a operação de inversão em dois índices do caminho
 * @param path O caminho onde a operação será aplicada
 * @param i O índice inicial da sublista a ser invertida
 * @param j O índice final da sublista a ser invertida
 */
void apply_invert(std::vector<int>& path, size_t i, size_t j) {
    // Garante que i é menor que j
    if(i > j) {
        std::swap(i, j);
    }

    // Inverte a sublista entre os índices i e j
    std::reverse(path.begin() + i, path.begin() + j + 1);
}

/**
 * @brief Aplica o método de modificação selecionado no caminho
 */
void apply_move(LocalSearchMethod method, std::vector<int>& path,
                std::size_t i, std::size_t j) {
    switch (method) {
        case LocalSearchMethod::SWAP:
            apply_swap(path, i, j);
            break;
        case LocalSearchMethod::SHIFT:
            apply_shift(path, i, j);
            break;
        case LocalSearchMethod::INVERT:
            apply_invert(path, i, j);
            break;
    }
} 

/**
 * @brief Desfaz o método de modificação selecionado no caminho -- ainda nao sei se usamos
 */
void undo_move(LocalSearchMethod method, std::vector<int>& path,
                std::size_t i, std::size_t j) {
    // Para swap e invert, a operação é sua própria inversa
    if(method == LocalSearchMethod::SWAP || method == LocalSearchMethod::INVERT) {
        apply_move(method, path, i, j);
    } else if(method == LocalSearchMethod::SHIFT) {
        // Para shift, inverte os índices para desfazer a operação
        apply_shift(path, j, i);
    }
}


/**
 * @brief Aplica estratégia de primeira melhoria para obter uma solução melhor
 * @param weights A matriz de pesos entre os nós do grafo.
 * @param current_path O caminho atual
 * @param current_cost O custo atual do caminho.
 * @param apply_method A função que aplica o método de modificação no caminho
 * @param is_full Indica se a vizinhança completa deve ser considerada
 * @return true se uma melhoria foi encontrada, false caso contrário
 */
template<typename ApplyMethodFunc>
bool first_improvement_step(const std::vector<std::vector<double>>& weights, std::vector<int>& current_path,
    double& current_cost, ApplyMethodFunc apply_method, bool is_full = false) {

    size_t path_size = current_path.size();

    // Percorre a vizinhança em busca da primeira melhoria
    for(size_t i = 1; i < path_size; ++i) {
        size_t j_start = is_full ? 1 : i + 1;
        for(size_t j = j_start; j < path_size; ++j) {
            if(i == j) continue;
        
            std::vector<int> temp_path = current_path;
            //Aplica a modificação
            apply_method(temp_path, i, j);
            double new_cost = calculate_path_cost(weights, temp_path);

            // Se é observada uma melhoria, atualiza o caminho e o custo e retorna true
            if(new_cost < current_cost) {
                current_path = temp_path;
                current_cost = new_cost;
                return true;
            }
        }
    }
    return false;
}


/**
 * @brief Aplica estratégia de melhor melhoria para obter melhor solução
 * @param weights A matriz de pesos entre os nós do grafo
 * @param current_path O caminho atual
 * @param current_cost O custo atual do caminho
 * @param apply_method A função que aplica o método de modificação no caminho
 * @return true se uma melhoria foi encontrada, false caso contrário
 */
template<typename ApplyMethodFunc>
bool best_improvement_step(const std::vector<std::vector<double>>& weights, std::vector<int>& current_path,
    double& current_cost, ApplyMethodFunc apply_method, bool is_full = false) {

    size_t path_size = current_path.size();
    bool improvement_found = false;
    std::vector<int> best_path = current_path;
    double best_cost = current_cost;

    // Percorre toda a vizinhança para encontrar a melhor melhoria
    for(size_t i = 1; i < path_size; ++i) {
        size_t j_start = is_full ? 1 : i + 1;
        for(size_t j = j_start; j < path_size; ++j) {
            if(i == j) continue;

            std::vector<int> temp_path = current_path;
            //Aplica a modificação
            apply_method(temp_path, i, j);
            double new_cost = calculate_path_cost(weights, temp_path);

            // Se é observada uma melhoria, atualiza a melhor solução encontrada
            if(new_cost < best_cost) {
                best_path = temp_path;
                best_cost = new_cost;
                improvement_found = true;
            }
        }
    }

    // Se uma melhoria foi encontrada, atualiza o caminho e o custo atuais
    if(improvement_found) {
        current_path = best_path;
        current_cost = best_cost;
    }

    return improvement_found;
}

LocalSearchResult local_search(const std::vector<std::vector<double>>& weights,
    const std::vector<int>& initial_path, LocalSearchMethod method, ImprovementType improvement) {

    std::vector<int> current_path = initial_path;
    double current_cost = calculate_path_cost(weights, current_path);
    bool improvement_found = true;
    bool is_full = (method == LocalSearchMethod::SHIFT);

    // Define a função de aplicação do método baseado no tipo selecionado
    auto apply_method = [&](std::vector<int>& path, size_t i, size_t j) {
        apply_move(method, path, i, j);
    };

    // Executa a busca local até que nenhuma melhoria seja encontrada
    while(improvement_found) {
        if(improvement == ImprovementType::FIRST_IMPROVEMENT) {
            improvement_found = first_improvement_step(weights, current_path, current_cost, apply_method, is_full);
            break;
        } else { 
            improvement_found = best_improvement_step(weights, current_path, current_cost, apply_method, is_full);
        }
    }

    LocalSearchResult result;
    result.solution = std::move(current_path);
    result.cost = current_cost;

    return result;
}

