#include <cstring>
#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <functional>

#include <tbb/concurrent_vector.h>
#include "pstl_partition.h"

typedef std::chrono::system_clock MyClock;

template<typename Container, typename Data, typename Func>
void test(Func func, std::size_t range_size, std::size_t rep, std::uint64_t seed) {
    Container range (range_size);
    auto random = std::bind(std::uniform_real_distribution<Data>(0, 100), std::mt19937_64(seed));

    typename Container::iterator pivot;
    auto start = MyClock::now();
    MyClock::duration t = start - start;

    for(std::size_t i = 0; i < rep; ++i) {
        
        std::generate(range.begin(), range.end(), random);

        start = MyClock::now();

        pivot = func(range.begin(), range.end(), [](Data x) -> bool { return x < 50; });

        t += (MyClock::now() - start);
    }

    if(!std::is_partitioned(range.begin(), range.end(), [](Data x) -> bool { return x < 50; })) {
        std::cout << func.name() << " failed" << std::endl;
    } else {
        std::cout << std::left << std::setw(33) << func.name() << std::setw(12) << (pivot - range.begin()) 
            << std::setw(12) << (t.count() / rep) << std::setw(12) << range_size
            << std::setw(12) << func.grain_size << std::setw(12) << rep 
            << std::setw(12) << seed << std::endl;
    }
}

void print_head() {
    std::cout << std::left << std::setw(33) << "name" << std::setw(12) << "result"
            << std::setw(12) << "time" << std::setw(12) << "range size"
            << std::setw(12) << "grain size" << std::setw(12) << "repetition" 
            << std::setw(12) << "seed" << std::endl;
}

int print_help(int val, const char *argv0) {
    std::cout << "Usage: " << argv0 << " <algorithm> <container> <data> [range size] [grain size] [seed]" << std::endl;
    return val;
}

#define DEFINE_TEST_FUNCTION(function) \
struct function { \
    std::size_t grain_size; \
    template<typename Iterator, typename Predicate> \
    Iterator operator()(Iterator begin, Iterator end, Predicate pred) const { \
        return mypstl::function(begin, end, pred, grain_size); \
    } \
    const char *name() const { return #function; } \
};

DEFINE_TEST_FUNCTION(partition)
DEFINE_TEST_FUNCTION(partition_deterministic)
DEFINE_TEST_FUNCTION(stable_partition)
DEFINE_TEST_FUNCTION(stable_partition_deterministic)

int main(int argc, char **argv) {
    std::size_t range_size, grain_size, rep;
    std::uint64_t seed;
    if(argc < 4) {
        return print_help(1, argv[0]);
    } else {
        range_size = (argc > 4) ? std::atoi(argv[4]) : 10000000;
        grain_size = (argc > 5) ? std::atoi(argv[5]) : 1000;
        rep = (argc > 6) ? std::atoi(argv[6]) : 100;
        seed = (argc > 7) ? std::atoi(argv[7]) : 123456789;
        if(argc > 8) {
            print_head();
        }

        partition_deterministic func1 {grain_size};
        partition func2 {grain_size};
        stable_partition_deterministic func3 {grain_size};
        stable_partition func4 {grain_size};

        if(strcmp(argv[1], "det") == 0) {  // deterministic
            if(strcmp(argv[2], "vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::vector<double>, double>(func1, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::vector<float>, float>(func1, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "deque") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::deque<double>, double>(func1, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::deque<float>, float>(func1, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "concurrent_vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<tbb::concurrent_vector<double>, double>(func1, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<tbb::concurrent_vector<float>, float>(func1, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else return print_help(1, argv[0]);

        } else if(strcmp(argv[1], "ori") == 0) {  // original
            if(strcmp(argv[2], "vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::vector<double>, double>(func2, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::vector<float>, float>(func2, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "deque") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::deque<double>, double>(func2, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::deque<float>, float>(func2, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "concurrent_vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<tbb::concurrent_vector<double>, double>(func2, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<tbb::concurrent_vector<float>, float>(func2, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else return print_help(1, argv[0]);

        } else if(strcmp(argv[1], "stab_det") == 0) {  // stable deterministic
            if(strcmp(argv[2], "vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::vector<double>, double>(func3, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::vector<float>, float>(func3, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "deque") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::deque<double>, double>(func3, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::deque<float>, float>(func3, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "concurrent_vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<tbb::concurrent_vector<double>, double>(func3, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<tbb::concurrent_vector<float>, float>(func3, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else return print_help(1, argv[0]);

        } else if(strcmp(argv[1], "stab_ori") == 0) {  // stable original
            if(strcmp(argv[2], "vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::vector<double>, double>(func4, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::vector<float>, float>(func4, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "deque") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<std::deque<double>, double>(func4, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<std::deque<float>, float>(func4, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else if(strcmp(argv[2], "concurrent_vector") == 0) {
                if(strcmp(argv[3], "double") == 0) {
                    test<tbb::concurrent_vector<double>, double>(func4, range_size, rep, seed);
                } else if(strcmp(argv[3], "float") == 0) {
                    test<tbb::concurrent_vector<float>, float>(func4, range_size, rep, seed);
                } else return print_help(1, argv[0]);
            } else return print_help(1, argv[0]);
        }
    }

    return 0;
}
