#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#include "../graph/DirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/NearestInsertion.h"
#include "../algorithm/LocalSearch.h"
#include "../algorithm/TSPResult.h"
#include "../utils/TSPUtils.h"

int main() {

    std::vector<std::string> files = {
        "data/problem_1.csv",
        "data/problem_2.csv",
        "data/problem_3.csv",
        "data/problem_4.csv",
        "data/problem_5.csv",
        "data/problem_6.csv",
        "data/problem_7.csv",
        "data/problem_8.csv",
        "data/problem_9.csv",
        "data/problem_10.csv",
        "data/problem_11.csv",
        "data/problem_12.csv",
        "data/small_example.csv"
    };

    std::ofstream output("result/nearesti_results.txt");
    if(!output.is_open()) {
        std::cerr << "Could not open output file for writing results.\n";
        return 1;
    }

    int start_node = 1;

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

        //Nearest Insertion without Local Search
        auto nn_path = nearest_insertion(graph, weights, start_node);
        double nn_cost = calculate_path_cost(weights, nn_path);

        output << "[Nearest Insertion without Local Search]\n";
        output << "Cost: " << nn_cost << "\n";
        output << "Path: ";
        for (const auto& node : nn_path) {
            output << graph.get_node(node) << " ";
        }
        output << "\n";

        //Nearest Neighbor with Local Search
        output << "[Nearest Insertion with Local Search]\n";
        for(const auto& method : methods) {
            for(const auto& improvement : improvements) {
                auto result = nearest_insertion_local_search(graph, weights, start_node, method, improvement);

                output << "Method: " << method_to_string(method)
                       << ", Improvement: " << improvement_to_string(improvement) << "\n";
                output << "Cost: " << result.cost << "\n";
                output << "Path: ";
                for (const auto& node : result.path) {
                    output << graph.get_node(node) << " ";
                }
                output << "\n";
            }
        }

    }

    output.close();
    std::cout << "Nearest Insertion tests completed. Results written to 'result/nearesti_results.txt'.\n";

    return 0;
}