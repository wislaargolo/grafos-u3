#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#include "../graph/DirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/NearestNeighbor.h"
#include "../algorithm/LocalSearch.h"
#include "../algorithm/TSPResult.h"
#include "../utils/TSPUtils.h"

int main() {

    std::vector<std::string> files = {
        "data/problem_1_2.csv",
        "data/problem_3_4.csv",
        "data/problem_5_6.csv",
        "data/problem_7_8.csv",
        "data/problem_9_10.csv",
        "data/problem_11_12.csv"
    };

    std::ofstream output("result/nearestn_results.txt");
    if(!output.is_open()) {
        std::cerr << "Could not open output file for writing results.\n";
        return 1;
    }

    int start_index = 0; // Índice do nó inicial para o percurso

    std::vector<LocalSearchMethod> methods = {
        LocalSearchMethod::SWAP,
        LocalSearchMethod::SHIFT,
        LocalSearchMethod::INVERT
    };

    std::vector<ImprovementType> improvements = {
        ImprovementType::FIRST_IMPROVEMENT,
        ImprovementType::BEST_IMPROVEMENT
    };

    for(const auto& filename : files) {
        DirectedAdjacencyListGraph<int> graph;
        std::vector<std::vector<double>> weights;

        try {
            populate_graph_from_csv<int>(filename, graph, weights);
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            continue;
        }

        output << "\nResults for file: " << filename << "\n";

        //Nearest Neighbor without Local Search
        auto nn_path = nearest_neighbor(graph.get_order(), weights, start_index);
        double nn_cost = calculate_path_cost(weights, nn_path);

        output << "[Nearest Neighbor without Local Search]\n";
        output << "Cost: " << nn_cost << "\n";
        output << "Path: ";
        for (const auto& node : nn_path) {
            output << node << " ";
        }
        output << "\n";

        //Nearest Neighbor with Local Search
        output << "[Nearest Neighbor with Local Search]\n";
        for(const auto& method : methods) {
            for(const auto& improvement : improvements) {
                auto result = nearest_neighbor_local_search(graph, weights, start_index, method, improvement);

                output << "Method: " << method_to_string(method)
                       << ", Improvement: " << improvement_to_string(improvement) << "\n";
                output << "Cost: " << result.cost << "\n";
                output << "Path: ";
                for (const auto& node : result.path) {
                    output << node << " ";
                }
                output << "\n";
            }
        }

    }

    output.close();
    std::cout << "Nearest Neighbor tests completed. Results written to 'result/nearestn_results.txt'.\n";

    return 0;
}