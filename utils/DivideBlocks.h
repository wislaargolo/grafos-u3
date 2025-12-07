#ifndef DIVIDE_BLOCKS_H
#define DIVIDE_BLOCKS_H

#include <vector>
#include <set>
#include <iostream>
#include <stdexcept>
#include <list>

#include "../graph/IGraph.h"

// Struct do estado da dfs
struct DivideBlocksState {
    // Vetor de blocos de vértices encontrados
    std::vector<std::list<int>> blocks;
    // Vetor de articulações encontradas
    std::vector<int> articulations;
    // Vetor de descoberta da dfs, 0 para vértices não-descobertos e 1 para descobertos
    std::vector<int> discovery;
    // Vetor de profundidade de cada vértice
    std::vector<int> depth;
    // Vetor que armazena lowpt de cada vértice
    std::vector<int> lowpt;
};

// Struct de retorno do método de checagem
template<typename Node>
struct DivideBlocksResult {
    // Vetor de blocos de vértices encontrados
    std::vector<std::vector<Node>> blocks;
    // Vetor de articulações encontradas
    std::vector<Node> articulations;
};

/**
 * @brief Dfs para divisão do grafo em componentes biconexos e determinação das articulações
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser percorrido.
 * @param state O estado atual da divisão.
 * @param node O vértice que está sendo visitado no momento.
 * @param parent O pai do vértice que está sendo visitado.
 * @return uma lista representado o bloco de vértices do grafo que foi visitado.
 * Esse bloco é o bloco que os acentrais do vértice irão utilizar para popular novos blocos, ou seja, o bloco que
 * ainda não foi fechado.
 */
template<typename Node>
std::list<int> divide_blocks_visit(const IGraph<Node>& graph, DivideBlocksState& state, int node, int parent = -1) {
    // Marca o vértice atual como descoberto
    state.discovery[node] = 1;

    // O vértice é a raíz se não possui pai
    bool is_root = parent == -1;
    // Inicialmente não é articulação
    bool is_articulation = false;
    // Inicialmente, seu lowpt é ele mesmo
    int self_lowpt = node;
    // Inicialmente, o número de filhos é 0
    int number_of_children = 0;
    // O bloco formado pelo vértice atual
    std::list<int> block;

    // Para todos os vizinhos do vértice atual
    for (int neighbor_index : graph.get_neighbors_indices(node)) {
        // Se o vizinho ainda não foi descoberto
        if (state.discovery[neighbor_index] == 0) {
            // Ele é filho do vértice atual, logo incrementa o número de filhos
            number_of_children++;

            // Marca a profundidade do filho como a profundidade atual + 1
            state.depth[neighbor_index] = state.depth[node] + 1;
            // Visita o vizinho e recebe o bloco formato por ele
            std::list<int> child_block = divide_blocks_visit(graph, state, neighbor_index, node);

            // Pega o lowpt do vizinho
            int child_lowpt = state.lowpt[neighbor_index];

            // Se o lowpt do vizinho é mais alto que o lowpt atual, logo atualizamos o lowpt atual como sendo o do vizinho
            if (state.depth[child_lowpt] < state.depth[self_lowpt]) {
                self_lowpt = child_lowpt;
            }

            // Se o vértice atual for raíz ou o filho for demarcador
            if (is_root || child_lowpt == neighbor_index || child_lowpt == node) {
                // O vértice é marcado como articulação caso não seja raíz ou caso seja raíz com mais de um filho
                is_articulation = !is_root || number_of_children > 1;

                // Se adiciona no bloco do vértice filho e adiciona esse bloco no vetor de blocos
                child_block.push_front(node);

                state.blocks.push_back(std::move(child_block));
            }
            // Caso contrário, o vértice atual não é articulação
            else {
                // Unifica o bloco do filho com o bloco atual, que irá ser retornado para o pai desse vértice
                block.splice(block.end(), child_block);
            }
        }
        // Já se o vizinho já foi visitado, então temos uma aresta de retorno
        else if (parent != neighbor_index) {
            // Se ao utilizar essa aresta de retorno eu consigo um lowpt melhor, eu atualizo o lowpt atual
            if (state.depth[neighbor_index] < state.depth[self_lowpt]) {
                self_lowpt = neighbor_index;
            }
        }
    }

    // Atualizo o vetor de lowpt
    state.lowpt[node] = self_lowpt;

    // Se o vértice for articulação, adiciono no vetor de articulações
    if (is_articulation) {
        state.articulations.push_back(node);
    }

    // Adiciono o vértice em seu próŕio bloco, e o retorno para o pai
    block.push_front(node);

    return block;
}

/**
 * @brief Divide o grafo em componentes biconexos e determina articulações.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser dividido.
 * @return resultado da divisão, com vetor de componentes biconexos e vetor de articulações.
 */
template<typename Node>
DivideBlocksResult<Node> divide_blocks(const IGraph<Node>& graph) {
    size_t size = graph.get_nodes().size();

    if (size == 0) {
        throw std::invalid_argument("Graph is empty");
    }

    DivideBlocksState state;

    // Inicia o vetor de visitados com 0
    state.discovery.resize(size, 0);
    // Inicia a profundidade de todos com 0
    state.depth.resize(size, 0);
    // Aloca o vetor de lowpt (valores não importam inicialmente)
    state.lowpt.resize(size);

    // Percorre todos os vértices e visita aqueles que não foram visitados
    for (size_t i = 0; i < size; i++) {
        if (!state.discovery[i]) {
            divide_blocks_visit(graph, state, i);
        }
    }

    // Constrói o resultado do método de acordo com o estado da dfs
    DivideBlocksResult<Node> result;

    for (std::list<int>& index_block : state.blocks) {
        std::vector<Node> block;

        for (int index : index_block) {
            block.push_back(graph.get_node(index));
        }

        result.blocks.push_back(std::move(block));
    }

    for (int index : state.articulations) {
        result.articulations.push_back(graph.get_node(index));
    }

    return result;
}

#endif
