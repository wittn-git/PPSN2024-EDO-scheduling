#include <random>
#include <vector>
#include <algorithm>

// Generation functions -----------------------------------------------------------------------------------------------------

// Returns a vector of n random integers uniformly distributed between 1 and max
std::vector<int> get_processing_times(int seed, int n, int max){
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dist(1, max);
    std::vector<int> processing_times(n);
    std::transform(processing_times.begin(), processing_times.end(), processing_times.begin(), [&dist, &generator](int&){
        return dist(generator);
    });
    return processing_times;
}

// Returns a vector of only zeros
std::vector<int> get_release_dates(int n){
    return std::vector<int>(n, 0);
}

std::vector<int> get_due_dates(int seed, std::vector<int> processing_times){
    std::vector<int> due_dates(processing_times.size());
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> dist_factor(3, 10);
    for(int i = 0; i < processing_times.size(); i++){
        double factor = dist_factor(generator);
        double muh = factor * processing_times[i];
        double sigma = (factor - 1) * processing_times[i] / 3;
        int a = processing_times[i];
        std::normal_distribution<double> dist(muh, sigma);
        int result = dist(generator);
        while (result < a) {
            result = dist(generator);
        }
        due_dates[i] = result;
    }
    return due_dates;
}