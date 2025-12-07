#include <iostream>
#include <vector>
#include <string>
#include <functional>

#include "../algorithm/LocalSearch.h"
#include "../algorithm/TSPResult.h"
#include "../utils/TSPUtils.h"

void print_path(const std::vector<int>& v) {
    std::cout << "[ ";
    for (int x : v) std::cout << x << " ";
    std::cout << "]";
}

void run_unit_test(std::string test_name, std::vector<int> input, std::vector<int> expected, 
                   std::function<void(std::vector<int>&)> op) {
    std::cout << "Init: " << test_name << "\n";
    std::cout << "Input:  "; print_path(input); std::cout << "\n";
    
    op(input); // Aplica a operação
    
    std::cout << "Obtained:   "; print_path(input); std::cout << "\n";
    std::cout << "Expected: "; print_path(expected); std::cout << "\n";
    
}

int main() {
    std::cout << "Initiating Local Search Tests...\n";

    run_unit_test("SWAP (1 and 3)", {0, 10, 20, 30, 40}, {0, 30, 20, 10, 40}, 
        [](std::vector<int>& v){ apply_swap(v, 1, 3); });

    run_unit_test("SHIFT Forward (1 -> 3)", {0, 10, 20, 30, 40}, {0, 20, 30, 10, 40}, 
        [](std::vector<int>& v){ apply_shift(v, 1, 3); });

    run_unit_test("SHIFT Backward (3 -> 1)", {0, 10, 20, 30, 40}, {0, 30, 10, 20, 40}, 
        [](std::vector<int>& v){ apply_shift(v, 3, 1); });

    run_unit_test("INVERT (indices 1 to 3)", {0, 10, 20, 30, 40, 50}, {0, 30, 20, 10, 40, 50}, 
        [](std::vector<int>& v){ apply_invert(v, 1, 3); });


    std::cout << "\nInitiating Local Search Integration Test...\n";

    std::vector<std::vector<double>> weights = {
        {0,  10, 100, 10},
        {10, 0,  10, 100},
        {100,10, 0,  10},
        {10, 100, 10, 0}
    };

    std::vector<int> bad_path = {0, 2, 1, 3}; 

    std::cout << "Scenario: Square with expensive diagonals.\n";
    std::cout << "Initial path: "; 
    print_path(bad_path); 
    std::cout << "Cost: " << calculate_path_cost(weights, bad_path) << "\n";


    auto result = local_search(weights, bad_path, LocalSearchMethod::SWAP, ImprovementType::BEST_IMPROVEMENT);
        
    //std::vector<int> expected = {0, 1, 2, 3};
    //double expected_cost = 40.0;
    
    std::cout << "\nSWAP + BEST IMPROVEMENT\n";
    std::cout << "Optimized Path: "; 
    print_path(result.solution);
    std::cout << "\nFinal Cost: " << result.cost << "\n";

    result = local_search(weights, bad_path, LocalSearchMethod::INVERT, ImprovementType::FIRST_IMPROVEMENT);
    std::cout << "\nINVERT + FIRST IMPROVEMENT\n";
    std::cout << "Optimized Path: ";
    print_path(result.solution);
    std::cout << "\nFinal Cost: " << result.cost << "\n";
    


    return 0;
}