#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

#include "Dfs.h"
#include "../graph/UndirectedAdjacencyListGraph.h"
#include "../graph/UndirectedAdjacencyMatrixGraph.h"

/**
 * @brief Popula um grafo a partir de um arquivo CSV
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param filename O nome do arquivo CSV.
 * @param graph O grafo a ser populado.
 * @param weights A matriz de pesos
 */
template<typename Node>
void populate_graph_from_csv(const std::string& filename, 
    IGraph<Node>& graph, std::vector<std::vector<double>>& weights) {
    std::ifstream file(filename);

    if(!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;

    // Lê o cabeçalho 
    if(!std::getline(file, line)) {
        throw std::runtime_error("File is empty: " + filename);
    }

    std::stringstream header_ss(line);
    std::string cell;

    // Ignora a primeira célula do cabeçalho
    if(!std::getline(header_ss, cell, ',')) {
        throw std::runtime_error("Invalid header in file: " + filename);
    }

    // Vetor para armazenar os nós
    std::vector<Node> nodes;
    // Lê os nós do cabeçalho
    while(std::getline(header_ss, cell, ',')) {

        // Ignora células vazias
        if(cell.empty()) {
            continue;
        }

        // Converte a string para o tipo Node
        std::stringstream cell_ss(cell);
        
        Node node;
        cell_ss >> node;
        nodes.push_back(node);
    }

    // Adequa a matriz de pesos ao número de nós
    weights.assign(nodes.size(), std::vector<double>(nodes.size(), std::numeric_limits<double>::infinity()));

    // Lê as linhas subsequentes para popular a matriz de pesos e adicionar as arestas ao grafo
    size_t row_index = 0;
    while(std::getline(file, line)) {
        // Ignora linhas vazias
        if(line.empty()) {
            continue;
        }

        std::stringstream line_ss(line);
        //Primeiro campo da linha é o nó de origem
        if(!std::getline(line_ss, cell, ',')) {
            continue;
        }

        size_t col_index = 0;
        // Lê os pesos das arestas
        while(std::getline(line_ss, cell, ',')) {
        
            // Se a célula não estiver vazia, o peso está definido
            if(!cell.empty()) {
                double weight = std::stod(cell);
                weights[row_index][col_index] = weight;

                Node from_node = nodes[row_index];
                Node to_node = nodes[col_index];
                graph.add_edge(from_node, to_node);
            }

            col_index++;
        }
        row_index++;
    }

    file.close();
}

/**
 * @brief Exibe a matriz de pesos de forma formatada
 *
 * @tparam Node O tipo de dado dos nós no grafo
 * @param weights A matriz de pesos
 * @param graph O grafo associado à matriz de pesos
 */
template<typename Node>
void print_weights_matrix(const std::vector<std::vector<double>>& weights, const IGraph<Node>& graph) {
    size_t order = graph.get_order();

    std::cout << "Weights Matrix:\n";

    int col_width = 8;

    std::cout << "  |";
    for (size_t j = 0; j < order; ++j) {
        std::cout << std::setw(col_width) << graph.get_node(j);
    }
    std::cout << "\n";

    std::cout << std::setfill('-') << "----";
    for (size_t j = 0; j < order; ++j) {
        std::cout << std::setw(col_width) << "";
    }
    std::cout << std::setfill(' ') << "\n";

    for (size_t i = 0; i < order; ++i) {
        std::cout << graph.get_node(i) << " |"; 

        for (size_t j = 0; j < order; ++j) {
            if (j < weights[i].size() && weights[i][j] != std::numeric_limits<double>::infinity()) {
                std::cout << "\033[1;32m"
                          << std::setw(col_width) << std::setprecision(2) << std::fixed << weights[i][j]
                          << "\033[0m"; 
            } else {
                std::cout << std::setw(col_width) << "INF";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;


}



/**
 * @brief Adiciona um nó ao grafo a partir de uma representação em string.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param str A string contendo o valor do nó a ser adicionado.
 * @param graph O grafo onde o nó será inserido.
 */
template<typename Node>
void add_node_from_string(const std::string& str, IGraph<Node>& graph) {
    Node node;
    std::stringstream ss(str);
    ss >> node;
    graph.add_node(node);
}

/**
 * @brief Remove um nó do grafo a partir de uma representação em string.
 *
 * @tparam Node O tipo de dado dos nós do grafo.
 * @param str A string contendo o valor do nó a ser removido.
 * @param graph O grafo de onde o nó será removido.
 */
template<typename Node>
void remove_node_from_string(const std::string& str, IGraph<Node>& graph) {
    Node node;
    std::stringstream ss(str);
    ss >> node;
    graph.remove_node(node);
} 

#endif // GRAPH_IO_H
