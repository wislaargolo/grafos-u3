#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "../graph/DirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/CheapestInsertion.h"
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

    std::ofstream output("result/cheapestinsertion_results.txt");
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

        // Começa a marcar o tempo de execução
        auto start_time = std::chrono::high_resolution_clock::now();

        //Cheapest Insertion without Local Search
        auto nn_path = cheapest_insertion(graph, weights, start_node);
        double nn_cost = calculate_path_cost(weights, nn_path);

        // Termina de marcar o tempo de execução
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end_time - start_time);

        output << "[Cheapest Insertion without Local Search]\n";
        output << "Cost: " << nn_cost << "\n";
        output << "Path: ";
        for (const auto& node : nn_path) {
            output << graph.get_node(node) << " ";
        }
        output << "\n";
        output << "Time: " << duration.count() << "\n";

        //Cheapest Insertion with Local Search
        output << "[Cheapest Insertion with Local Search]\n";
        for(const auto& method : methods) {
            for(const auto& improvement : improvements) {
                // Começa a marcar o tempo de execução
                auto start_time = std::chrono::high_resolution_clock::now();

                auto result = cheapest_insertion_local_search(graph, weights, start_node, method, improvement);

                // Termina de marcar o tempo de execução
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double, std::milli>(end_time - start_time);

                output << "Method: " << method_to_string(method)
                       << ", Improvement: " << improvement_to_string(improvement) << "\n";
                output << "Cost: " << result.cost << "\n";
                output << "Path: ";
                for (const auto& node : result.path) {
                    output << graph.get_node(node) << " ";
                }
                output << "\n";
                output << "Time: " << duration.count() << "\n";
            }
        }

    }

    output.close();
    std::cout << "Cheapest Insertion tests completed. Results written to 'result/cheapestinsertion_results.txt'.\n";

    return 0;
}