#ifndef DIRECTEDADJACENCYLISTGRAPH_H
#define DIRECTEDADJACENCYLISTGRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "IGraph.h"

/* Implementação de um grafo dirigido usando listas de adjacência */
template<typename Node>
class DirectedAdjacencyListGraph : public IGraph<Node> {
    protected:
        /*Armazena vizinhos de cada nó*/
        std::vector<std::vector<int>> adjac;  
        /*Armazena nós por índice*/
        std::vector<Node> index_to_node;   
        /*Mapeia nós para seus índices*/        
        std::unordered_map<Node, size_t> node_to_index; 
    public:

        /*Utiliza o mapeamento de índices para obter o número de vértices*/
        size_t get_order() const override {
            return index_to_node.size();
        }

        /*Calcula o número de arestas somando o tamanho das listas de adjacência*/
        size_t get_size() const override {
            size_t edge_count = 0;
            for(const auto& neighbors : adjac) {
                edge_count += neighbors.size();
            }
            return edge_count;
        }

        void add_node(const Node& node) override {
            auto it = node_to_index.find(node);
            /*Adiciona nó apenas se ele não existir*/
            if (it == node_to_index.end()) {
                /*Mapeia o novo nó para seu índice*/
                node_to_index[node] = index_to_node.size();
                /*Armazena o novo nó por índice, colocando no final pois os outros índices estão ocupados*/
                index_to_node.push_back(node);
                /*Adiciona uma nova lista de vizinhos vazia para o novo nó*/
                adjac.emplace_back();
            }
        }

        void remove_node(const Node& node) override {
            auto it = node_to_index.find(node);
            /*Remove o nó apenas se ele existir*/
            if (it != node_to_index.end()) {
                /*Obtém o índice do nó a ser removido*/
                size_t index = it->second;
                /*Obtém o índice do último nó para troca*/
                size_t last_index = index_to_node.size() - 1;

                /*Remove todas as arestas que apontam para o nó a ser removido*/
                for (auto& neighbors : adjac) {
                    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), index), neighbors.end());
                }

                /*Se o nó a ser removido não for o último, troca com o último para manter a sequência*/
                if(index != last_index) {
                    Node last_node = index_to_node[last_index];
                    std::swap(index_to_node[index], index_to_node[last_index]);
                    std::swap(adjac[index], adjac[last_index]);
                    node_to_index[last_node] = index;
                }

                /*Atualiza os índices nas listas de adjacência para refletir a troca*/
                for(auto& neighbors : adjac) {
                    for(auto& neighbor_index : neighbors) {
                        if(neighbor_index == static_cast<int>(last_index)) {
                            neighbor_index = index;
                        }
                    }
                }

                /*Remove o último nó (que é o nó a ser removido) e sua lista de vizinhos*/
                index_to_node.pop_back();
                adjac.pop_back();
                node_to_index.erase(it);

            }
        }

        void add_edge(const Node& from, const Node& to) override {
            /*Adiciona os nós se eles não existirem*/
            add_node(from);
            add_node(to);

            size_t from_index = node_to_index[from];
            size_t to_index = node_to_index[to];

            /*Adiciona o índice do nó destino na lista de vizinhos do nó origem, se ainda não existir*/
            auto& neighbors_from = adjac[from_index];
            if (std::find(neighbors_from.begin(), neighbors_from.end(), to_index) == neighbors_from.end()) {
                neighbors_from.push_back(to_index);
            }
        }

        void remove_edge(const Node& from, const Node& to) override {
            auto it_from = node_to_index.find(from);
            auto it_to = node_to_index.find(to);

            /*Remove o índice do nó destino da lista de vizinhos do nó origem, se ambos existirem*/
            if (it_from != node_to_index.end() && it_to != node_to_index.end()) {
                size_t from_index = it_from->second;
                size_t to_index = it_to->second;
                
                auto& neighbors_from = adjac[from_index];
                neighbors_from.erase(std::remove(neighbors_from.begin(), neighbors_from.end(), to_index), neighbors_from.end());
            }

        }

        std::vector<Node> get_neighbors(const Node& node) const override {
            auto it = node_to_index.find(node);
            /*Retorna uma lista vazia se o nó não existir*/
            if (it == node_to_index.end()) {
                return {};
            }

            /*Caso nó exista, cria std::vector com tamanho da quantidade de vizinhos*/
            size_t index = it->second;
            std::vector<Node> neighbors;
            neighbors.reserve(adjac[index].size());
            /*Preenche o std::vector com os nós vizinhos usando o mapeamento de índices*/
            for (int neighbor_index : adjac[index]) {
                neighbors.push_back(index_to_node[neighbor_index]);
            }
            return neighbors;
        }

        std::vector<int> get_neighbors_indices(int index) const override {
            /*Caso o índice seja inválido, lança uma exceção*/
            if (index < 0 || static_cast<size_t>(index) >= index_to_node.size()) {
                throw std::out_of_range("get_neighbors_indices: Index out of range");
            }
            /*Retorna diretamente a lista de índices dos vizinhos*/
            return adjac[index];
        }

        std::vector<Node> get_nodes() const override {
            /*Retorna todos os nós do grafo*/
            return index_to_node;
        }

        bool has_node(const Node& node) const override {
            /*Verifica se o nó existe no mapeamento*/
            return node_to_index.find(node) != node_to_index.end();
        }

        /*Imprime o grafo com ordem, tamanho e seus vizinhos no formato utilizado para lista de adjacência*/
        void print() const override {
            std::cout  << "Graph (order: " << get_order() << ", size: " << get_size() << "):\n";
            for(size_t i = 0; i < index_to_node.size(); ++i) {
                std::cout << index_to_node[i] << ": ";
                for(int neighbor_index : adjac[i]) {
                    std::cout << index_to_node[neighbor_index] << " ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }

        int get_index(const Node& node) const override {
            auto it = node_to_index.find(node);
            /*Se o nó existir, retorna seu índice. Caso contrário, retorna -1*/
            if (it != node_to_index.end()) {
                return it->second;
            }
            return -1;
        }

        Node get_node(int index) const override {
            /*Se o índice for válido, retorna o nó correspondente utilizando mapeamento de índices para nós.
             Caso contrário, lança uma exceção*/
            if (index >= 0 && static_cast<size_t>(index) < index_to_node.size()) {
                return index_to_node[index];
            }
            throw std::out_of_range("get_node: Index out of range");
        }

        size_t get_in_degree(const Node& node) const override {
            /*Se o nó não existir, retorna grau de entrada 0*/
            if (!has_node(node)) {
                return 0;
            }

            /*Caso nó exista, calcula quantos nós apontam para ele*/
            size_t node_index = node_to_index.at(node);
            size_t in_degree = 0;
            for (const auto& neighbors : adjac) {
                for (int neighbor_index : neighbors) {
                    if (neighbor_index == static_cast<int>(node_index)) {
                        in_degree++;
                    }
                }
            }
            return in_degree;
        }

        size_t get_out_degree(const Node& node) const override {
            /*Se o nó não existir, retorna grau de saída 0*/
            if (!has_node(node)) {
                return 0;
            }
            /*Caso nó exista, retorna o tamanho da lista de vizinhos, já que cada vizinho representa uma aresta de saída*/
            size_t node_index = node_to_index.at(node);
            return adjac[node_index].size();
        }
    
        bool is_adjacent(const Node& u, const Node& v) const override {
            const auto& neighbors = adjac[get_index(u)];
            return std::find(neighbors.begin(), neighbors.end(), get_index(v)) != neighbors.end();
        }

        std::vector<EdgeIndex> get_all_edges() const override {
            std::vector<EdgeIndex> edges;
            // Percorre a lista de adjacência para coletar todas as arestas
            for (size_t from_index = 0; from_index < adjac.size(); from_index++) {
                // Para cada vizinho do nó atual, cria uma aresta
                for (int to_index : adjac[from_index]) {
                    edges.push_back(EdgeIndex{static_cast<int>(from_index), to_index});
                }
            }
            return edges;
        }
};

#endif // DIRECTEDADJACENCYLISTGRAPH_H
