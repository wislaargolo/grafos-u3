#ifndef UNDIRECTEDADJACENCYMATRIXGRAPH_H
#define UNDIRECTEDADJACENCYMATRIXGRAPH_H

#include "DirectedAdjacencyMatrixGraph.h"

/**
 * @class UndirectedAdjacencyMatrixGraph
 * @brief Implementação de um grafo não-direcionado usando uma matriz de adjacência.
 * @tparam Node O tipo de dado para cada nó do grafo.
 *
 * Herda de DirectedAdjacencyMatrixGraph para reutilizar a estrutura básica,
 * mas garante a simetria das arestas.
 */
template<typename Node>
class UndirectedAdjacencyMatrixGraph : public DirectedAdjacencyMatrixGraph<Node> {
public:

    /**
     * @brief Retorna o número de arestas não-direcionadas.
     * @return O tamanho do grafo.
     */
    size_t get_size() const override {
        // Como cada aresta é representada duas vezes (u,v) e (v,u), dividimos por 2.
        return DirectedAdjacencyMatrixGraph<Node>::get_size() / 2;
    }

    /**
     * @brief Adiciona uma aresta não-direcionada entre dois nós.
     * @param from O primeiro nó.
     * @param to O segundo nó.
     */
    void add_edge(const Node& from, const Node& to) override {
        // Adiciona a aresta nos dois sentidos para simular a não-direcionalidade.
        DirectedAdjacencyMatrixGraph<Node>::add_edge(from, to);
        DirectedAdjacencyMatrixGraph<Node>::add_edge(to, from);
    }

    /**
     * @brief Remove uma aresta não-direcionada entre dois nós.
     * @param from O primeiro nó.
     * @param to O segundo nó.
     */
    void remove_edge(const Node& from, const Node& to) override {
        // Remove a aresta nos dois sentidos.
        DirectedAdjacencyMatrixGraph<Node>::remove_edge(from, to);
        DirectedAdjacencyMatrixGraph<Node>::remove_edge(to, from);
    }

    /**
     * @brief Imprime uma representação visual do grafo não-direcionado.
     */
    void print() const override {
        std::cout  << "Graph (undirected, order: " << this->get_order()
                   << ", size: " << this->get_size() << "):\n";

        if (this->get_order() == 0) {
            std::cout << "(Graph is empty)\n\n";
            return;
        }

        std::cout << "\nAdjacency Matrix:\n";
        std::cout << "    ";

        auto nodes = this->get_nodes();
        for (const auto& node : nodes) {
            std::cout << node << " ";
        }
        std::cout << "\n----";
        for (size_t i = 0; i < this->get_order(); ++i) {
            std::cout << "--";
        }
        std::cout << "\n";

        for (const auto& node_from : nodes) {
            std::cout << node_from << " | ";
            auto neighbors = this->get_neighbors(node_from);
            for (const auto& node_to : nodes) {
                bool is_neighbor = false;
                // Verifica se node_to está na lista de vizinhos de node_from.
                for(const auto& n : neighbors) {
                    if (n == node_to) {
                        is_neighbor = true;
                        break;
                    }
                }
                if (is_neighbor) {
                    // Imprime com cor para destacar a aresta.
                    std::cout << "\033[1;32m" << 1 << "\033[0m ";
                } else {
                    std::cout << 0 << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    /**
     * @brief Calcula o grau de um nó.
     * @param node O nó.
     * @return O grau do nó.
     */
    size_t get_in_degree(const Node& node) const override {
        // Em grafos não-direcionados, grau de entrada e saída são iguais.
        return this->get_out_degree(node);
    }

    std::vector<EdgeIndex> get_all_edges() const override {
        std::vector<EdgeIndex> edges;

        // Percorre a matriz para coletar todas as arestas.
        for (size_t from_index = 0; from_index < this->get_order(); from_index++) {
            for (size_t to_index = 0; to_index < this->get_order(); to_index++) {
                // Para evitar duplicatas, adiciona a aresta apenas uma vez, quando from_index <= to_index
                if (from_index <= to_index && this->matrix[from_index][to_index] == 1) {
                    edges.push_back(EdgeIndex{static_cast<int>(from_index), static_cast<int>(to_index)});
                }
            }
        }
        return edges;
    }
};

#endif
