#ifndef DIRECTEDINCIDENCEMATRIXGRAPH_H
#define DIRECTEDINCIDENCEMATRIXGRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include "IGraph.h"

/**
 * @class DirectedIncidenceMatrixGraph
 * @brief Implementação de um grafo direcionado usando uma matriz de incidência.
 * @tparam Node O tipo de dado para cada nó do grafo.
 */
template<typename Node>
class DirectedIncidenceMatrixGraph : public IGraph<Node> {
protected:
    // Matriz de incidência (arestas x vértices).
    std::vector<std::vector<int>> matrix;
    // Mapeia nós para seus índices.
    std::map<Node, size_t> node_to_index;
    // Mapeia índices para seus nós.
    std::map<size_t, Node> index_to_node;

public:
    /**
     * @brief Construtor padrão.
     */
    DirectedIncidenceMatrixGraph() = default;

    /**
     * @brief Retorna o número de vértices no grafo.
     * @return A ordem do grafo.
     */
    size_t get_order() const override { return node_to_index.size(); }

    /**
     * @brief Retorna o número de arestas no grafo.
     * @return O tamanho do grafo.
     */
    size_t get_size() const override { return matrix.size(); }

    /**
     * @brief Verifica se um nó existe no grafo.
     * @param node O nó a ser verificado.
     * @return true se o nó existe, false caso contrário.
     */
    bool has_node(const Node& node) const override {
        return node_to_index.find(node) != node_to_index.end();
    }

    /**
     * @brief Adiciona um novo nó ao grafo.
     * @param node O nó a ser adicionado.
     */
    void add_node(const Node& node) override {
        if (!has_node(node)) {
            size_t index = this->get_order();
            node_to_index[node] = index;
            index_to_node[index] = node;
            // Adiciona uma nova coluna (representando o novo nó) a cada aresta existente.
            for (auto& edge_row : matrix) {
                edge_row.push_back(0);
            }
        }
    }

    /**
     * @brief Adiciona uma aresta direcionada de 'from' para 'to'.
     * @param from O nó de origem.
     * @param to O nó de destino.
     */
    void add_edge(const Node& from, const Node& to) override {
        if (!has_node(from)) this->add_node(from);
        if (!has_node(to)) this->add_node(to);

        // Se a aresta já existe, retorna.
        if (this->is_adjacent(from, to)) {
            return;
        }

        size_t source_index = node_to_index[from];
        size_t dest_index = node_to_index[to];

        // Cria uma nova linha na matriz para a nova aresta.
        std::vector<int> new_edge(get_order(), 0);
        new_edge[source_index] = -1; // -1 indica a saída do nó.
        new_edge[dest_index] = 1;    // 1 indica a chegada no nó.

        matrix.push_back(new_edge);
    }

    /**
     * @brief Obtém todos os nós do grafo.
     * @return Um vetor contendo todos os nós.
     */
    std::vector<Node> get_nodes() const override {
        std::vector<Node> nodes;
        nodes.reserve(this->get_order());
        for(size_t i = 0; i < this->get_order(); ++i) {
            nodes.push_back(index_to_node.at(i));
        }
        return nodes;
    }

    /**
     * @brief Obtém o índice interno de um nó.
     * @param node O nó.
     * @return O índice do nó, ou -1 se não existir.
     */
    int get_index(const Node& node) const override {
        if (has_node(node)) {
            return node_to_index.at(node);
        }
        return -1;
    }

    /**
     * @brief Obtém o nó correspondente a um índice.
     * @param index O índice do nó.
     * @return O nó.
     * @throws std::out_of_range se o índice for inválido.
     */
    Node get_node(int index) const override {
        if (index_to_node.count(index)) {
            return index_to_node.at(index);
        }
        throw std::out_of_range("Indice fora do intervalo.");
    }

    /**
     * @brief Obtém os vizinhos de um nó.
     * @param node O nó.
     * @return Um vetor com os nós vizinhos.
     */
    std::vector<Node> get_neighbors(const Node& node) const override {
        std::vector<Node> neighbors;
        if (!has_node(node)) return neighbors;

        auto indices = get_neighbors_indices(get_index(node));
        for(int index : indices) {
            neighbors.push_back(get_node(index));
        }
        return neighbors;
    }

    /**
     * @brief Obtém os índices dos vizinhos de um nó.
     * @param node_index O índice do nó.
     * @return Um vetor com os índices dos vizinhos.
     */
    std::vector<int> get_neighbors_indices(int node_index) const override {
        std::vector<int> neighbors_indices;
        if (node_index < 0 || node_index >= static_cast<int>(this->get_order())) return neighbors_indices;

        // Percorre cada aresta (linha) da matriz.
        for (const auto& edge_row : matrix) {
            // Se o nó atual for a origem da aresta (-1).
            if (edge_row[node_index] == -1) {
                // Procura pelo destino da mesma aresta (1).
                for (size_t i = 0; i < this->get_order(); ++i) {
                    if (edge_row[i] == 1) {
                        neighbors_indices.push_back(i);
                        break; // Passa para a próxima aresta.
                    }
                }
            }
        }
        return neighbors_indices;
    }

    /**
     * @brief Remove um nó do grafo.
     * @param node O nó a ser removido.
     */
    void remove_node(const Node& node) override {
        if (!has_node(node)) return;

        size_t index_to_remove = get_index(node);

        // Remove todas as arestas conectadas ao nó (linhas onde a coluna do nó não é 0).
        matrix.erase(std::remove_if(matrix.begin(), matrix.end(),
            [index_to_remove](const std::vector<int>& edge_row) {
                return edge_row[index_to_remove] != 0;
            }), matrix.end());

        // Remove a coluna correspondente ao nó de todas as arestas restantes.
        for (auto& edge_row : matrix) {
            edge_row.erase(edge_row.begin() + index_to_remove);
        }

        // Remove o nó dos mapeamentos.
        index_to_node.erase(index_to_remove);
        node_to_index.erase(node);

        // Reajusta os índices dos nós subsequentes para manter a consistência.
        for (size_t i = index_to_remove; i < get_order(); ++i) {
            Node n = index_to_node.at(i + 1);
            index_to_node[i] = n;
            node_to_index[n] = i;
        }
        index_to_node.erase(get_order());
    }

    /**
     * @brief Remove uma aresta direcionada entre dois nós.
     * @param from O nó de origem.
     * @param to O nó de destino.
     */
    void remove_edge(const Node& from, const Node& to) override {
        if (!has_node(from) || !has_node(to)) return;

        size_t from_index = get_index(from);
        size_t to_index = get_index(to);
        // Remove a linha (aresta) que conecta 'from' (-1) a 'to' (1).
        matrix.erase(std::remove_if(matrix.begin(), matrix.end(),
            [from_index, to_index](const std::vector<int>& edge_row) {
                return edge_row[from_index] == -1 && edge_row[to_index] == 1;
            }), matrix.end());
    }

    /**
     * @brief Imprime uma representação visual do grafo.
     */
    void print() const override {
        std::cout << "Grafo (direcionado, ordem: " << this->get_order()
                  << ", tamanho: " << this->get_size() << "):\n";
        if (this->get_order() == 0) {
            std::cout << "(Grafo esta vazio)\n\n";
            return;
        }
        std::cout << "\nMatriz de Incidencia (Arestas x Vertices):\n";

        std::cout << "    |";
        for (size_t i = 0; i < this->get_order(); ++i) {
            std::cout << " " << index_to_node.at(i) << " ";
        }
        std::cout << "\n----|";
        for (size_t i = 0; i < this->get_order(); ++i) {
            std::cout << "---";
        }
        std::cout << "\n";

        for (size_t i = 0; i < this->get_size(); ++i) {
            std::cout << " a" << i + 1 << " |";
            for (size_t j = 0; j < this->get_order(); ++j) {
                int value = matrix[i][j];
                 if (value == 1) {
                    std::cout << "\033[1;32m" << "+" << value << "\033[0m "; // Verde para chegada
                } else if (value == -1) {
                    std::cout << "\033[1;31m" << value << "\033[0m "; // Vermelho para saída
                } else {
                    std::cout << " " << value << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    /**
     * @brief Calcula o grau de entrada de um nó.
     * @param node O nó.
     * @return O grau de entrada.
     */
    size_t get_in_degree(const Node& node) const override {
        if (!has_node(node)) {
            return 0;
        }
        size_t node_idx = node_to_index.at(node);
        size_t in_degree = 0;
        // Conta quantas arestas têm valor 1 (chegada) na coluna do nó.
        for (const auto& edge_row : matrix) {
            if (edge_row[node_idx] == 1) {
                in_degree++;
            }
        }
        return in_degree;
    }

    /**
     * @brief Calcula o grau de saída de um nó.
     * @param node O nó.
     * @return O grau de saída.
     */
    size_t get_out_degree(const Node& node) const override {
        if (!has_node(node)) {
            return 0;
        }
        size_t node_idx = node_to_index.at(node);
        size_t out_degree = 0;
        // Conta quantas arestas têm valor -1 (saída) na coluna do nó.
        for (const auto& edge_row : matrix) {
            if (edge_row[node_idx] == -1) {
                out_degree++;
            }
        }
        return out_degree;
    }

    /**
     * @brief Verifica se um nó 'v' é adjacente a 'u'.
     * @param u O nó de origem.
     * @param v O nó de destino.
     * @return true se existe aresta de 'u' para 'v', false caso contrário.
     */
    bool is_adjacent(const Node& u, const Node& v) const override {
        int u_idx = get_index(u);
        int v_idx = get_index(v);
        // Procura por uma aresta que saia de 'u' (-1) e chegue em 'v' (1).
        for (const auto& edge_row : matrix) {
            if (edge_row[u_idx] == -1 && edge_row[v_idx] == 1) {
                return true;
            }
        }
        return false;
    }

    std::vector<EdgeIndex> get_all_edges() const override {
        std::vector<EdgeIndex> edges;
        for (size_t edge_index = 0; edge_index < matrix.size(); edge_index++) {
            int from_index = -1;
            int to_index = -1;
            for (size_t node_index = 0; node_index < this->get_order(); node_index++) {
                if (matrix[edge_index][node_index] == -1) {
                    from_index = static_cast<int>(node_index);
                } else if (matrix[edge_index][node_index] == 1) {
                    to_index = static_cast<int>(node_index);
                }
            }
            if (from_index != -1 && to_index != -1) {
                edges.push_back(EdgeIndex{from_index, to_index});
            }
        }
        return edges;
    }
};

#endif
