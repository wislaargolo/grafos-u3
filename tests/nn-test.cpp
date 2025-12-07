#include <iostream>
#include <vector>
#include <string>

#include "../graph/UndirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/NearestNeighbor.h"
#include "../algorithm/TSPResult.h"
#include "../utils/TSPUtils.h"
#include "../algorithm/LocalSearch.h"


int main() {
    UndirectedAdjacencyListGraph<int> graph;

    std::cout << "Creating graph from file 'small_example.csv'...\n";
    std::vector<std::vector<double>> weights;

    try {
        populate_graph_from_csv<int>("data/small_example.csv", graph, weights);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    print_weights_matrix(weights, graph);
    graph.print();

    std::cout << "Nearest Neighbor Algorithm:\n";
    std::vector<int> nn_path = nearest_neighbor<int>(graph, weights, 0);
    std::cout << "Nearest Neighbor Path: ";
    for (int index : nn_path) {
        std::cout << graph.get_node(index) << " ";
    }
    std::cout << std::endl;
    std::cout << "Path Cost: " << calculate_path_cost(weights, nn_path) <<  "\n\n";

    std::cout << "Applying Local Search...\n";
    std::cout << "SWAP with BEST_IMPROVEMENT\n";
    auto result = nearest_neighbor_local_search(graph, weights, 0, LocalSearchMethod::SWAP, ImprovementType::BEST_IMPROVEMENT);
    print_tsp_result(graph, result);

    std::cout << "\nSHIFT with FIRST_IMPROVEMENT\n";
    result = nearest_neighbor_local_search(graph, weights, 0, LocalSearchMethod::SHIFT, ImprovementType::FIRST_IMPROVEMENT);
    print_tsp_result(graph, result);
    std::cout << "\nINVERT with BEST_IMPROVEMENT\n";
    result = nearest_neighbor_local_search(graph, weights, 0, LocalSearchMethod::INVERT, ImprovementType::BEST_IMPROVEMENT);
    print_tsp_result(graph, result);   

    return 0;
}