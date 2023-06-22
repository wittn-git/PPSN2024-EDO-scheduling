#include "algorithms/simple_test.hpp"
#include "algorithms/noah.hpp"
#include "algorithms/mu1.hpp"

int main() {

    /*
    std::vector<int> processing_times   = {1,  1, 2, 6, 10, 2,  7,  7,  9,  15};
    std::vector<int> release_dates      = {0,  0, 0, 1, 10, 30, 0,  2,  5,  1};
    std::vector<int> due_dates          = {10, 2, 4, 7, 21, 36, 20, 10, 17, 17};
    */

    std::vector<int> processing_times   = {1,  1, 2, 6, 10, 2,  7,  7,  9,  15, 1, 1,  5,  19, 3,  4,  23, 2};
    std::vector<int> release_dates      = {0,  0, 0, 1, 10, 30, 0,  2,  5,  1,  0, 10, 7,  23, 16, 6,  21, 6};
    std::vector<int> due_dates          = {10, 2, 4, 7, 21, 36, 20, 10, 17, 17, 8, 11, 20, 45, 60, 19, 55, 21};
    int m = 3;

    simple_test(m, processing_times, release_dates, due_dates,
        100, 100
    );
    noah_test(
        m, processing_times, release_dates, due_dates,
        50, 60, 5, 25, 3, 100, 0.1, 3
    );
    mu1_test(
        m, processing_times, release_dates, due_dates,
        50, 0.35, 1/45, 0.1, 100
    );
    
}