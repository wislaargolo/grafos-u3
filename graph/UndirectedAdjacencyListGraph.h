#ifndef UNDIRECTEDADJACENCYLISTGRAPH_H
#define UNDIRECTEDADJACENCYLISTGRAPH_H
#include <algorithm> 

#include "DirectedAdjacencyListGraph.h" 

/**
 * @class UndirectedAdjacencyListGraph
 * @brief Classe que implementa uma representação de grafo não-direcionado usando listas de adjacência.
 * @tparam Node O tipo de dado que será armazenado em cada vértice do grafo.
 *
 * Esta classe herda de um grafo direcionado (`DirectedAdjacencyListGraph`) e
 * sobrescreve os métodos de manipulação de arestas para garantir a natureza
 * bidirecional.
 */
template<typename Node>
class UndirectedAdjacencyListGraph : public DirectedAdjacencyListGraph<Node> {
    
    public:
        /**
         * @brief Adiciona uma aresta não-direcionada entre os nós 'u' e 'v'.
         *
         * Para simular uma aresta não-direcionada,
         * este método adiciona duas arestas direcionadas: uma de 'u' para 'v' e outra
         * de 'v' para 'u'.
         *
         * @param u O primeiro nó da aresta.
         * @param v O segundo nó da aresta.
         */
        void add_edge(const Node& u, const Node& v) override {
            DirectedAdjacencyListGraph<Node>::add_edge(u, v);
            DirectedAdjacencyListGraph<Node>::add_edge(v, u);
        }

         /**
         * @brief Remove uma aresta não-direcionada entre os nós 'u' e 'v'.
         *
         * Para remover completamente a conexão, remove-se tanto a aresta u -> v quanto a aresta v -> u.
         *
         * @param u O primeiro nó da aresta a ser removida.
         * @param v O segundo nó da aresta a ser removida.
         */
        void remove_edge(const Node& u, const Node& v) override {
            DirectedAdjacencyListGraph<Node>::remove_edge(u, v);
            DirectedAdjacencyListGraph<Node>::remove_edge(v, u);
        }

         /**
         * @brief Retorna o número de arestas não-direcionadas no grafo.
         *
         * Como a classe base representa um grafo direcionado, o número de arestas
         * é calculado dividindo o número de arestas direcionadas por dois.
         *
         * @return O número total de arestas não-direcionadas.
         */
        size_t get_size() const override {
           size_t directed_size = DirectedAdjacencyListGraph<Node>::get_size();
           size_t loop_count = 0;

            /* Procura por laços no grafo todo. Laços não são duplicados no grafo direcionado, 
                então precisam ser contados apenas uma vez */
           for(const auto& node : DirectedAdjacencyListGraph<Node>::get_nodes()) {
               const auto& neighbors = DirectedAdjacencyListGraph<Node>::get_neighbors(node);
               /*Caso um nó tenha ele mesmo como vizinho, conta como um laço*/
               loop_count += std::count(neighbors.begin(), neighbors.end(), node);
           }

           size_t regular_edges = (directed_size - loop_count) / 2;
           return regular_edges + loop_count;
        }


        size_t get_in_degree(const Node& node) const override {
            /*Em grafos não-direcionados, o grau de entrada é igual ao grau de saída*/
            return this->get_out_degree(node);
        }

        std::vector<EdgeIndex> get_all_edges() const override {
            std::vector<EdgeIndex> edges;

            // Percorre a lista de adjacência para coletar todas as arestas
            for (size_t from_index = 0; from_index < this->adjac.size(); from_index++) {
                for (int to_index : this->adjac[from_index]) {
                    // Para evitar duplicatas, adiciona a aresta apenas se from_index for menor ou igual a to_index
                    if (from_index <= static_cast<size_t>(to_index)) {
                        edges.push_back(EdgeIndex{static_cast<int>(from_index), to_index});
                    }
                }
            }
            return edges;
        }
};
#endif // UNDIRECTEDADJACENCYLISTGRAPH_H