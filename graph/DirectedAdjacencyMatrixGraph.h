#ifndef DIRECTEDADJACENCYMATRIXGRAPH_H
#define DIRECTEDADJACENCYMATRIXGRAPH_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>

#include "IGraph.h"

/**
 * @class DirectedAdjacencyMatrixGraph
 * @brief Implementação de um grafo direcionado usando uma matriz de adjacência.
 * @tparam Node O tipo de dado para cada nó do grafo.
 */
template<typename Node>
class DirectedAdjacencyMatrixGraph : public IGraph<Node> {
protected:
    // Matriz que armazena as adjacências (arestas) do grafo.
    std::vector<std::vector<int>> matrix;
    // Vetor para mapear um índice ao seu nó correspondente.
    std::vector<Node> index_to_node;
    // Mapa para mapear um nó ao seu índice correspondente de forma eficiente.
    std::unordered_map<Node, size_t> node_to_index;

public:
    /**
     * @brief Construtor padrão.
     */
    explicit DirectedAdjacencyMatrixGraph() = default;

    /**
     * @brief Construtor que inicializa o grafo com uma lista de nós.
     * @param initial_nodes Um vetor de nós para popular o grafo.
     */
    explicit DirectedAdjacencyMatrixGraph(const std::vector<Node>& initial_nodes) {
        for (const auto& node : initial_nodes) {
            add_node(node);
        }
    }

    /**
     * @brief Retorna o número de vértices (nós) no grafo.
     * @return A ordem do grafo.
     */
    size_t get_order() const override {
        return index_to_node.size();
    }

    /**
     * @brief Retorna o número de arestas no grafo.
     * @return O tamanho do grafo.
     */
    size_t get_size() const override {
        size_t edge_count = 0;
        // Percorre a matriz para contar as arestas (células com valor 1).
        for (const auto& row : matrix) {
            for (int val : row) {
                if (val == 1) {
                    edge_count++;
                }
            }
        }
        return edge_count;
    }

    /**
     * @brief Verifica se um nó existe no grafo.
     * @param node O nó a ser verificado.
     * @return true se o nó existe, false caso contrário.
     */
    bool has_node(const Node& node) const override {
        return node_to_index.count(node) > 0;
    }

    /**
     * @brief Adiciona um novo nó ao grafo.
     * @param node O nó a ser adicionado.
     */
    void add_node(const Node& node) override {
        // Se o nó já existe, não faz nada.
        if (has_node(node)) {
            return;
        }

        // Define o índice para o novo nó.
        size_t new_index = get_order();
        node_to_index[node] = new_index;
        index_to_node.push_back(node);

        matrix.push_back(std::vector<int>(get_order(), 0));

        // Redimensiona todas as linhas para adicionar a nova coluna.
        for (auto& row : matrix) {
            row.resize(get_order(), 0);
        }
    }

    /**
     * @brief Remove um nó do grafo.
     * @param node O nó a ser removido.
     */
    void remove_node(const Node& node) override {
        if (!has_node(node)) {
            throw std::runtime_error("Cannot remove node: node does not exist.");
        }

        size_t index_to_remove = node_to_index.at(node);
        size_t last_index = get_order() - 1;

        // Para evitar redimensionamentos custosos, troca o nó a ser removido pelo último nó.
        if (index_to_remove != last_index) {
            // Troca as linhas e colunas na matriz.
            std::swap(matrix[index_to_remove], matrix[last_index]);
            for (size_t i = 0; i < get_order(); ++i) {
                std::swap(matrix[i][index_to_remove], matrix[i][last_index]);
            }
        }

        // Remove a última linha e coluna da matriz.
        matrix.pop_back();
        for (auto& row : matrix) {
            row.pop_back();
        }

        Node last_node = index_to_node.back();
        node_to_index.erase(node);
        index_to_node.erase(index_to_node.begin() + index_to_remove);

        // Atualiza os mapeamentos do nó que foi movido.
        if (index_to_remove != last_index) {
            index_to_node[index_to_remove] = last_node;
            node_to_index[last_node] = index_to_remove;
        }
    }

    /**
     * @brief Adiciona uma aresta direcionada de um nó de origem para um nó de destino.
     * @param from O nó de origem.
     * @param to O nó de destino.
     */
    void add_edge(const Node& from, const Node& to) override {
        // Adiciona os nós se eles não existirem.
        if (!has_node(from)) add_node(from);
        if (!has_node(to)) add_node(to);

        size_t from_idx = node_to_index.at(from);
        size_t to_idx = node_to_index.at(to);

        matrix[from_idx][to_idx] = 1;
    }

    /**
     * @brief Remove uma aresta direcionada entre dois nós.
     * @param from O nó de origem.
     * @param to O nó de destino.
     */
    void remove_edge(const Node& from, const Node& to) override {
        if (has_node(from) && has_node(to)) {
            size_t from_idx = node_to_index.at(from);
            size_t to_idx = node_to_index.at(to);
            // Remove a marcação da aresta na matriz.
            matrix[from_idx][to_idx] = 0;
        }
    }

    /**
     * @brief Obtém os vizinhos de um determinado nó.
     * @param node O nó para o qual os vizinhos serão retornados.
     * @return Um vetor contendo os nós vizinhos.
     */
    std::vector<Node> get_neighbors(const Node& node) const override {
        if (!has_node(node)) {
            return {};
        }

        size_t node_idx = node_to_index.at(node);
        std::vector<Node> neighbors;
        // Percorre a linha correspondente ao nó na matriz para encontrar os vizinhos.
        for (size_t j = 0; j < get_order(); ++j) {
            if (matrix[node_idx][j] == 1) {
                neighbors.push_back(index_to_node[j]);
            }
        }
        return neighbors;
    }

    /**
     * @brief Obtém todos os nós do grafo.
     * @return Um vetor contendo todos os nós.
     */
    std::vector<Node> get_nodes() const override {
        return index_to_node;
    }

    /**
     * @brief Imprime uma representação visual do grafo no console.
     */
    void print() const override {
        std::cout  << "Graph (directed, order: " << get_order()
                   << ", size: " << get_size() << "):\n";

        if (get_order() == 0) {
            std::cout << "(Graph is empty)\n\n";
            return;
        }

        std::cout << "\nAdjacency Matrix:\n";
        std::cout << "    ";
        for (const auto& node : index_to_node) {
            std::cout << node << " ";
        }
        std::cout << "\n----";
        for (size_t i = 0; i < get_order(); ++i) {
            std::cout << "--";
        }
        std::cout << "\n";

        for (size_t i = 0; i < get_order(); ++i) {
            std::cout << index_to_node[i] << " | ";
            for (size_t j = 0; j < get_order(); ++j) {
                if (matrix[i][j] == 1) {
                    // Imprime arestas com cor para melhor visualização.
                    std::cout << "\033[1;32m" << matrix[i][j] << "\033[0m ";
                } else {
                    std::cout << matrix[i][j] << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    /**
     * @brief Obtém o índice interno de um nó.
     * @param node O nó cujo índice é desejado.
     * @return O índice do nó, ou -1 se não existir.
     */
    int get_index(const Node& node) const override {
        auto it = node_to_index.find(node);
        if (it != node_to_index.end()) {
            return it->second;
        }
        return -1;
    }

    /**
     * @brief Obtém o nó correspondente a um índice.
     * @param index O índice do nó.
     * @return O nó correspondente.
     * @throws std::out_of_range se o índice for inválido.
     */
    Node get_node(int index) const override {
        if (index >= 0 && static_cast<size_t>(index) < index_to_node.size()) {
            return index_to_node[index];
        }
        throw std::out_of_range("get_node: Index out of range");
    }

    /**
     * @brief Obtém os índices dos vizinhos de um nó.
     * @param index O índice do nó.
     * @return Um vetor de inteiros com os índices dos vizinhos.
     * @throws std::out_of_range se o índice for inválido.
     */
    std::vector<int> get_neighbors_indices(int index) const override {
        if (index < 0 || static_cast<size_t>(index) >= get_order()) {
            throw std::out_of_range("get_neighbors_indices: Index out of range");
        }

        std::vector<int> neighbors_indices;
        // Percorre a linha da matriz para encontrar os índices dos vizinhos.
        for (size_t j = 0; j < get_order(); ++j) {
            if (matrix[index][j] == 1) {
                neighbors_indices.push_back(j);
            }
        }
        return neighbors_indices;
    }

    /**
     * @brief Calcula o grau de entrada de um nó.
     * @param node O nó para o qual o grau de entrada será calculado.
     * @return O grau de entrada do nó.
     */
    size_t get_in_degree(const Node& node) const override {
        if (!has_node(node)) {
            return 0;
        }
        size_t node_idx = node_to_index.at(node);
        size_t in_degree = 0;
        // Percorre a coluna da matriz para contar as arestas que chegam ao nó.
        for (size_t i = 0; i < get_order(); ++i) {
            if (matrix[i][node_idx] == 1) {
                in_degree++;
            }
        }
        return in_degree;
    }

    /**
     * @brief Calcula o grau de saída de um nó.
     * @param node O nó para o qual o grau de saída será calculado.
     * @return O grau de saída do nó.
     */
    size_t get_out_degree(const Node& node) const override {
        if (!has_node(node)) {
            return 0;
        }
        size_t node_idx = node_to_index.at(node);
        size_t out_degree = 0;
        // Percorre a linha da matriz para contar as arestas que saem do nó.
        for (size_t j = 0; j < get_order(); ++j) {
            if (matrix[node_idx][j] == 1) {
                out_degree++;
            }
        }
        return out_degree;
    }

    /**
     * @brief Verifica se um nó 'v' é adjacente a um nó 'u'.
     * @param u O nó de origem.
     * @param v O nó de destino.
     * @return true se existe uma aresta de u para v, false caso contrário.
     */
    bool is_adjacent(const Node& u, const Node& v) const override {
        // Verifica se há uma aresta marcada na matriz.
        return matrix[get_index(u)][get_index(v)] == 1;
    }

    std::vector<EdgeIndex> get_all_edges() const override {
        std::vector<EdgeIndex> edges;
        // Percorre a matriz para coletar todas as arestas.
        for (size_t from_index = 0; from_index < get_order(); from_index++) {
            for( size_t to_index = 0; to_index < get_order(); to_index++) {
                // Se há uma aresta de from_index para to_index, adiciona à lista de arestas
                if (matrix[from_index][to_index] == 1) {
                    edges.push_back(EdgeIndex{static_cast<int>(from_index), static_cast<int>(to_index)});
                }
            }
        }
        return edges;
    }
};

#endif
