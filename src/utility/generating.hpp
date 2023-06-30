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

// Returns a vector of n/2 zeros and n/2 values sampled exponentially between 0 and the sum of the processing times
std::vector<int> get_release_dates(int seed, int n, std::vector<int> processing_times){
    std::mt19937 generator(seed);
    std::vector<int> release_dates(n);
    double lambda = 0.1;
    std::exponential_distribution<double> dist(lambda);
    int upperBound = std::accumulate(processing_times.begin(), processing_times.end(), 0);
    std::transform(release_dates.begin() + n/2, release_dates.end(), release_dates.begin() + n/2, [&dist, &generator, upperBound](int&){
        int result = dist(generator);
        while (result > upperBound) {
            result = dist(generator);
        }
        return result;
    });
    return release_dates;
}


// Returns a vector of n due dates sampled from a normal distribution with mug and sigma based on the processing time and release date, lower bound being the release date + processing time
std::vector<int> get_due_dates(int seed, int n, std::vector<int> processing_times, std::vector<int> release_dates){
    std::vector<int> due_dates(n);
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> dist_factor(3, 10);
    for(int i = 0; i < n; i++){
        double factor = dist_factor(generator);
        double muh = release_dates[i] + factor * processing_times[i];
        double sigma = (factor - 1) * processing_times[i] / 3;
        int a = release_dates[i] + processing_times[i];
        std::normal_distribution<double> dist(muh, sigma);
        int result = dist(generator);
        while (result < a) {
            result = dist(generator);
        }
        due_dates[i] = result;
    }
    return due_dates;
}