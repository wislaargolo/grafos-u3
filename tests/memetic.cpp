#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#include "../graph/DirectedAdjacencyListGraph.h"
#include "../utils/GraphIO.h"
#include "../algorithm/TSPResult.h"
#include "../utils/TSPUtils.h"
#include "../algorithm/MemeticSearch.h"
#include "../algorithm/GeneticSearch.h"

int main()
{
    std::cout << "Hellow" << "\n";
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
        "data/small_example.csv"};

    std::ofstream output("result/memetic_results.txt");
    if (!output.is_open())
    {
        std::cerr << "Could not open output file for writing results.\n";
        return 1;
    }

    for (const auto &filename : files)
    {
        DirectedAdjacencyListGraph<int> graph;
        std::vector<std::vector<double>> weights;

        try
        {
            populate_graph_from_csv<int>(filename, graph, weights);
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }

        output << "\nResults for file: " << filename << "\n";

        auto memeticResult = memetic_search(graph, weights);

        output << "[Memetic Algorithm]\n";
        output << "Cost: " << memeticResult.cost << "\n";
        output << "Path: ";
        for (const auto& node : memeticResult.path) {
            output << graph.get_node(node) << " ";
        }
        output << "\n";
    }

    output.close();
    std::cout << "Memetic Search tests completed. Results written to 'result/memetic_results.txt'.\n";

    return 0;
}