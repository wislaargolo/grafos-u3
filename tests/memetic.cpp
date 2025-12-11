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

    int start_node = 1;

    std::vector<LocalSearchMethod> methods = {
        LocalSearchMethod::SWAP,
        LocalSearchMethod::SHIFT,
        LocalSearchMethod::INVERT};

    std::vector<ImprovementType> improvements = {
        ImprovementType::FIRST_IMPROVEMENT,
        ImprovementType::BEST_IMPROVEMENT};

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

        auto population = generate_population(graph, weights);
        calculate_initial_fitness(population, weights);

        output << "[Initial Population]\n";
        for (size_t i = 0; i < population.size(); i++)
        {
            double cost = calculate_path_cost(weights, population[i].path);
            output << "Individual " << i + 1 << ": Cost = " << cost << ", Path = ";
            for (const auto &node : population[i].path)
            {
                output << graph.get_node(node) << " ";
            }
            output << "\n";
        }

        improve_population(weights, population, LocalSearchMethod::INVERT, ImprovementType::BEST_IMPROVEMENT);

        output << "[Improved Population after Local Search]\n";
        for (size_t i = 0; i < population.size(); i++)
        {
            double cost = calculate_path_cost(weights, population[i].path);
            output << "Individual " << i + 1 << ": Cost = " << cost << ", Path = ";
            for (const auto &node : population[i].path)
            {
                output << graph.get_node(node) << " ";
            }
            output << "\n";
        }

        print_population(graph, weights, population);
    }

    output.close();
    std::cout << "Memetic Search tests completed. Results written to 'result/memetic_results.txt'.\n";

    return 0;
}