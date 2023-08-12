#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include <random>
#include <stdexcept>

#include "../operators/operators_mutation.hpp"

template <typename list_type>
std::vector<list_type> parse_list(std::string input){
    std::vector<list_type> result;
    std::string temp = "";
    for(int i = 0; i < input.size(); i++){
        if(input[i] == ','){
            result.push_back(std::stod(temp));
            temp = "";
        }else{
            temp += input[i];
        }
    }
    result.push_back(std::stod(temp));
    return result;
}

using T = std::vector<std::vector<int>>;

std::tuple<std::string, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>, std::string, std::vector<int>, std::vector<int>, std::vector<int>, std::vector<double>, int> parse_arguments(int argc, char **argv){
    if(argc != 9){
        throw std::invalid_argument("Pass 8 arguments. You only passed "+ std::to_string(argc - 1) + ".");
    }

    std::string experiment_type(argv[1]);    
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator;
    std::string mutation_operator_name(argv[2]);
    if(mutation_operator_name == "1RAI"){
        mutation_operator = mutate_removeinsert(1);
    }else if(mutation_operator_name == "XRAI"){
        mutation_operator = mutate_xremoveinsert(1);
    }else if(mutation_operator_name == "NSWAP"){
        mutation_operator = mutate_neighborswap(1);
    }else{
        throw std::invalid_argument("Invalid mutation operator.");
    }
    std::string output_file = argv[3];
    std::vector<int> mus = parse_list<int>(argv[4]);
    std::vector<int> ns = parse_list<int>(argv[5]);
    std::vector<int> ms = parse_list<int>(argv[6]);
    std::vector<double> alphas = parse_list<double>(argv[7]);
    int runs = std::stoi(argv[8]);

    return std::make_tuple(experiment_type, mutation_operator, output_file, mus, ns, ms, alphas, runs);
}