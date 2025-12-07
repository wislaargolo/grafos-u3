#ifndef UNDIRECTEDINCIDENCEMATRIXGRAPH_H
#define UNDIRECTEDINCIDENCEMATRIXGRAPH_H

#include "DirectedIncidenceMatrixGraph.h"
#include <vector>
#include <string>

/**
 * @class UndirectedIncidenceMatrixGraph
 * @brief Implementação de um grafo não-direcionado usando uma matriz de incidência.
 * @tparam Node O tipo de dado para cada nó.
 *
 * Herda de DirectedIncidenceMatrixGraph e adapta os métodos para um contexto não-direcionado.
 */
template<typename Node>
class UndirectedIncidenceMatrixGraph : public DirectedIncidenceMatrixGraph<Node> {
public:
    /**
     * @brief Construtor padrão.
     */
    UndirectedIncidenceMatrixGraph() = default;

    /**
     * @brief Adiciona uma aresta não-direcionada entre dois nós.
     * @param node1 O primeiro nó.
     * @param node2 O segundo nó.
     */
    void add_edge(const Node& node1, const Node& node2) override {
        if (!this->has_node(node1)) this->add_node(node1);
        if (!this->has_node(node2)) this->add_node(node2);

        size_t index1 = this->node_to_index[node1];
        size_t index2 = this->node_to_index[node2];

        // Verifica se a aresta já existe.
        for (const auto& edge_row : this->matrix) {
            // Se encontrar uma linha que já conecta os dois nós, retorna.
            if (edge_row[index1] == 1 && edge_row[index2] == 1) {
                return;
            }
        }

        // Cria uma nova linha para a aresta.
        std::vector<int> new_edge(this->get_order(), 0);
        // Em grafos não-direcionados, ambos os nós incidentes à aresta são marcados com 1.
        new_edge[index1] = 1;
        new_edge[index2] = 1;

        this->matrix.push_back(new_edge);
    }

    /**
     * @brief Obtém os índices dos vizinhos de um nó.
     * @param node_index O índice do nó.
     * @return Um vetor com os índices dos vizinhos.
     */
    std::vector<int> get_neighbors_indices(int node_index) const override {
        std::vector<int> neighbors_indices;
        if (node_index < 0 || node_index >= static_cast<int>(this->get_order())) return neighbors_indices;

        // Percorre cada aresta.
        for (const auto& edge_row : this->matrix) {
            // Se o nó faz parte da aresta (valor 1).
            if (edge_row[node_index] == 1) {
                // Procura pelo outro nó na mesma aresta.
                for (size_t i = 0; i < this->get_order(); ++i) {
                    if (i != static_cast<size_t>(node_index) && edge_row[i] == 1) {
                        neighbors_indices.push_back(i);
                        break;
                    }
                }
            }
        }
        return neighbors_indices;
    }

    /**
     * @brief Remove uma aresta não-direcionada entre dois nós.
     * @param node1 O primeiro nó.
     * @param node2 O segundo nó.
     */
    void remove_edge(const Node& node1, const Node& node2) override {
        if (!this->has_node(node1) || !this->has_node(node2)) return;

        size_t index1 = this->get_index(node1);
        size_t index2 = this->get_index(node2);

        // Remove a aresta (linha) onde ambos os nós são marcados com 1.
        this->matrix.erase(std::remove_if(this->matrix.begin(), this->matrix.end(),
            [index1, index2](const std::vector<int>& edge_row) {
                return edge_row[index1] == 1 && edge_row[index2] == 1;
            }), this->matrix.end());
    }

    /**
     * @brief Imprime uma representação visual do grafo.
     */
    void print() const override {
        std::cout << "Grafo (nao direcionado, ordem: " << this->get_order()
                  << ", tamanho: " << this->get_size() << "):\n";
        if (this->get_order() == 0) {
            std::cout << "(Grafo esta vazio)\n\n";
            return;
        }
        std::cout << "\nMatriz de Incidencia (Arestas x Vertices):\n";

        std::cout << "    |";
        for (size_t i = 0; i < this->get_order(); ++i) {
            std::cout << " " << this->index_to_node.at(i) << " ";
        }
        std::cout << "\n----|";
        for (size_t i = 0; i < this->get_order(); ++i) {
            std::cout << "---";
        }
        std::cout << "\n";

        for (size_t i = 0; i < this->get_size(); ++i) {
            std::cout << " a" << i + 1 << " |";
            for (int value : this->matrix[i]) {
                if (value == 1) {
                    std::cout << "\033[1;32m" << " " << value << "\033[0m ";
                } else {
                    std::cout << " " << value << " ";
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
        if(!this->has_node(node)) return 0;
        size_t node_index = this->get_index(node);
        size_t degree = 0;
        // O grau é o número de arestas que incidem no nó (valor 1 na coluna).
        for (const auto& edge_row : this->matrix) {
            if (edge_row[node_index] == 1) {
                degree++;
            }
        }
        return degree;
    }

    /**
     * @brief Calcula o grau de um nó (sinônimo de get_in_degree).
     * @param node O nó.
     * @return O grau do nó.
     */
    size_t get_out_degree(const Node& node) const override {
        return get_in_degree(node);
    }

    std::vector<EdgeIndex> get_all_edges() const override {
        std::vector<EdgeIndex> edges;

        // Percorre a matriz para coletar todas as arestas.
        for (size_t edge_index = 0; edge_index < this->get_size(); edge_index++) {
            const auto& edge_row = this->matrix[edge_index];
            std::vector<int> incident_nodes;
            // Coleta os nós incidentes à aresta.
            for (size_t node_index = 0; node_index < this->get_order(); node_index++) {
                if (edge_row[node_index] == 1) {
                    incident_nodes.push_back(static_cast<int>(node_index));
                }
            }

            //Quando a aresta conecta exatamente dois nós, adiciona à lista de arestas
            if (incident_nodes.size() == 2) {
                edges.push_back(EdgeIndex{incident_nodes[0], incident_nodes[1]});
            }
        }
        return edges;
    }
};

#endif
