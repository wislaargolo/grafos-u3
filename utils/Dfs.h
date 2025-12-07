#ifndef DFS_H
#define DFS_H

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <stack>
#include <optional>
#include <iostream>
#include <numeric>
#include <unordered_map>

#include "../graph/IGraph.h"

/**
 * @enum EdgeType
 * @brief Classifica os tipos de arestas encontradas durante uma travessia DFS 
 * com base nos tempos de descoberta e saída dos nós.
 */

enum class EdgeType {
    TREE,
    BACK,
    FORWARD,
    CROSS
};

/**
 * @struct Edge
 * @brief Representa uma aresta com um nó de origem e um de destino.
 */
template<typename Node>
struct Edge {
    Node from, to;
};

/**
 * @struct DFSState
 * @brief Estrutura interna para armazenar o estado da busca DFS enquanto ela está em execução. 
 * A travessia trabalha com índices inteiros representando os nós ao invés dos próprios nós.
 */
template<typename Node>
struct DFSState {
    std::vector<int> discovery, exit;
    std::vector<int> parent;
    std::unordered_map<EdgeType, std::vector<Edge<Node>>> edges;

    DFSState(size_t num_nodes = 0)
        : discovery(num_nodes, 0),
          exit(num_nodes, 0),
          parent(num_nodes, -1) /*Todos os nós inicialmente não têm pai*/
    {}
};

/**
 * @struct DFSResult
 * @brief Armazena o resultado final da busca DFS.
 * Mapeia os tempos e arestas de volta para os objetos Node originais, em vez de índices.
 */

template<typename Node>
struct DFSResult {
    std::unordered_map<Node, int> discovery, exit;
    std::unordered_map<EdgeType, std::vector<Edge<Node>>> edges;
};


/**
 * @brief Função recursiva principal que executa a visita DFS a partir de um nó.
 *
 * @param node O índice do nó atual a ser visitado.
 * @param time O contador de tempo global passado por referência.
 * @param discovery, exit, parent Vetores para manter o tempo de descoberta, tempo de saída e pais dos nós.
 * @param find_tree, find_back, find_forward, find_cross Funções a serem chamadas ao encontrar cada tipo de aresta.
 */
template<typename Node, class FindTree, class FindBack, class FindForward, class FindCross>
void dfs_visit(const IGraph<Node>& graph,
            int node, int& time,
            std::vector<int>& discovery, std::vector<int>& exit,
            std::vector<int>& parent,
            FindTree find_tree, FindBack find_back,
            FindForward find_forward, FindCross find_cross) {

    /* Marca o tempo de descoberta do nó atual */
    discovery[node] = ++time;

    /*Explora todos os vizinhos do nó atual*/
    for(int neighbor_index : graph.get_neighbors_indices(node)) {
        /*Se o vizinho ainda não foi descoberto, é uma aresta de árvore*/
        if (discovery[neighbor_index] == 0) {
            parent[neighbor_index] = node;
            find_tree(node, neighbor_index);
            dfs_visit(graph, neighbor_index, time, discovery, exit, parent,
                      find_tree, find_back, find_forward, find_cross);
        } else if (exit[neighbor_index] == 0) {
            /*Se o vizinho foi descoberto mas não saiu, é uma aresta de retorno*/
            find_back(node, neighbor_index);
        } else if (discovery[node] < discovery[neighbor_index]) {
            /*Se o vizinho já saiu e foi descoberto depois do nó atual, é uma aresta de avanço*/
            find_forward(node, neighbor_index);
        } else {
            /*Se o vizinho já saiu e foi descoberto antes do nó atual, é uma aresta de cruzamento*/
            find_cross(node, neighbor_index);
        }
    }

    /*Todos os vizinhos do nó atual foram visitados, então marca o tempo de saída*/
    exit[node] = ++time;
}

/**
 * @brief Função que inicia a busca DFS em todo o grafo, método usado para grafos direcionados,
 * pois nesse caso existem as arestas de cruzamento e avanço.
 * @return Um DFSResult com os tempos e as arestas classificadas de toda a busca.
 */
template<typename Node>
DFSResult<Node> dfs(const IGraph<Node>& graph) {
    DFSState<Node> state(graph.get_order());
    int time = 0;

    /* Funções para classificar as arestas encontradas durante a busca */
    auto find_tree =[&](int from, int to) {
        state.edges[EdgeType::TREE].push_back({graph.get_node(from), graph.get_node(to)});
    };
    auto find_back = [&](int from, int to) {
        state.edges[EdgeType::BACK].push_back({graph.get_node(from), graph.get_node(to)});
    };
    auto find_forward = [&](int from, int to) {
        state.edges[EdgeType::FORWARD].push_back({graph.get_node(from), graph.get_node(to)});
    };
    auto find_cross = [&](int from, int to) {
        state.edges[EdgeType::CROSS].push_back({graph.get_node(from), graph.get_node(to)});
    };


    /*Inicia a DFS para cada nó não visitado, garantindo que todos os componentes sejam explorados*/
    for(size_t i = 0; i < graph.get_order(); i++) {
        if (state.discovery[i] == 0) {
            dfs_visit(graph, i, time, state.discovery, state.exit, state.parent,
                      find_tree, find_back, find_forward, find_cross);
        }
    }

    return get_result_dfs(graph, state);
}

/**
 * @brief Uma versão da DFS para grafos não-direcionados quando apenas
 * arestas de árvore e de retorno são consideradas.
 * @param start O nó inicial da busca.
 * @return Um objeto DFSResult contendo apenas arestas de árvore e de retorno.
 */
template<typename Node>
DFSResult<Node> dfs_unidirectional(const IGraph<Node>& graph, const Node& start) {
    /*Verifica se o nó inicial existe no grafo*/
    if (!graph.has_node(start)) {
        throw std::invalid_argument("Start node does not exist in the graph.");
    }

    DFSState<Node> state(graph.get_order());
    int time = 0;

    /* Funções para classificar as arestas encontradas durante a busca, considerando apenas arestas de árvore e de retorno */
    auto find_tree = [&](int from, int to) {
        state.edges[EdgeType::TREE].push_back({graph.get_node(from), graph.get_node(to)});
    };

    auto find_back = [&](int from, int to) {
        if(state.parent[from] != -1 && state.parent[from] != to) {
            state.edges[EdgeType::BACK].push_back({graph.get_node(from), graph.get_node(to)});
        }
    };

    auto empty = [](int from, int to) {};

    dfs_visit(graph, graph.get_index(start), time, state.discovery, state.exit, state.parent,
          find_tree, find_back, empty, empty);

    return get_result_dfs(graph, state);
}

/**
 * @brief Função auxiliar para converter o estado interno da DFS para o resultado final.
 * @param state O estado interno da busca, baseado em índices.
 * @return O DFSResult final, com nós e arestas.
 */
template<typename Node>
DFSResult<Node> get_result_dfs(const IGraph<Node>& graph, const DFSState<Node>& state) {
    DFSResult<Node> result;

    /*Mapeia os tempos de descoberta e saída de volta para os nós originais*/
    for(size_t i = 0; i < graph.get_order(); i++) {
        Node node = graph.get_node(i);
        result.discovery[node] = state.discovery[i];
        result.exit[node] = state.exit[i];
    }
    /*Copia as arestas classificadas*/
    result.edges = state.edges;

    return result;
}
#endif // DFS_H
