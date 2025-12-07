#ifndef BFS_H
#define BFS_H

#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <iostream> 

#include "../graph/UndirectedAdjacencyListGraph.h"
#include "../graph/IGraph.h"

/**
 * @brief Realiza a travessia BFS a partir de um índice inicial, obtendo então um componente conectado.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo no qual a busca será realizada.
 * @param start_index O índice do nó de partida para esta visita específica.
 * @param visited Um vetor de controle passado por referência que controla os nós já visitados.
 * @return Um `std::vector<Node>` contendo os nós do componente visitado, na ordem em que foram descobertos.
 */
template<typename Node>
std::vector<Node> bfs_visit(const IGraph<Node>& graph, int start_index, std::vector<int>& visited) {

    std::queue<int> queue;
    /*Armazena o resultado em nós, já que durante a travessia os índices são utilizados*/
    std::vector<Node> result;

    visited[start_index] = 1;
    queue.push(start_index);
    result.push_back(graph.get_node(start_index));

    while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();

        /*Para o nó atual, marca seus vizinhos como visitados caso ainda não tenham sido visitados*/
        for(size_t neighbor_index : graph.get_neighbors_indices(current)) {
            if (visited[neighbor_index] == 0) {
                visited[neighbor_index] = 1;
                queue.push(neighbor_index);
                result.push_back(graph.get_node(neighbor_index));
            }
        }
     }
     return result;
}

/**
 * @brief Inicia uma busca BFS a partir de um nó específico.
 *
 * Esta função é chamada pelo usuário, permitindo
 * iniciar a busca passando o objeto `Node` diretamente, em vez de seu índice.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo no qual a busca será realizada.
 * @param start O nó de onde a busca deve começar.
 * @return Um `std::vector<Node>` com o resultado da travessia, ou um vetor vazio se o nó inicial não existir.
 */
template<typename Node>
std::vector<Node> bfs(const IGraph<Node>& graph, Node start) {

    if (!graph.has_node(start)) {
        std::cerr << "Start node '" << start << "' does not exist in the graph.\n";
        return {};
    }

    int start_index = graph.get_index(start);
    std::vector<int> visited(graph.get_order(), 0);
    /*Utiliza a função bfs_visit para realizar a busca a partir do índice inicial*/
    return bfs_visit(graph, start_index, visited);

}

/**
 * @brief Executa a busca BFS em todo o grafo, obtendo então todos os componentes conectados.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param graph O grafo a ser completamente percorrido.
 * @return Um vetor de vetores de nós. Cada vetor interno representa um componente
 * conectado do grafo, na ordem em que foi descoberto.
 */
template<typename Node>
std::vector<std::vector<Node>> bfs_digraph(const IGraph<Node>& graph) {

    std::vector<std::vector<Node>> result;
    std::vector<int> visited(graph.get_order(), 0);

    /*Percorre todos os nós do grafo, iniciando uma nova busca sempre que encontra um nó não visitado*/
    for (size_t i = 0; i < graph.get_order(); i++) {
        if (visited[i] == 0) {
            /*Cada chamada a bfs_visit retorna um componente conectado completo*/
            auto component = bfs_visit(graph, i, visited);
            result.push_back(component);
        }
    }
    return result;
}

#endif // BFS_H