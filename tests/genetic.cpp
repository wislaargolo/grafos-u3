#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "../graph/DirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/GeneticSearch.h"
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

    std::ofstream output("result/genetic_results.txt");
    if(!output.is_open()) {
        std::cerr << "Could not open output file for writing results.\n";
        return 1;
    }

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

        // Execução do algoritmo genetico
        auto nn_path = genetic_search(graph, weights);
        double nn_cost = calculate_path_cost(weights, nn_path);

        // Termina de marcar o tempo de execução
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end_time - start_time);

        output << "[Genetic Algorithm]\n";
        output << "Cost: " << nn_cost << "\n";
        output << "Path: ";
        for (const auto& node : nn_path) {
            output << graph.get_node(node) << " ";
        }
        output << "\n";
        output << "Time: " << duration.count() << "\n";

    }

    output.close();
    std::cout << "Genetic Algorithm tests completed. Results written to 'result/nearestn_results.txt'.\n";

    return 0;
}
