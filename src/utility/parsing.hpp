#include <vector>
#include <string>

template <typename T>
std::vector<T> parse_list(std::string input){
    std::vector<T> result;
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