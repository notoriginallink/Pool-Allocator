#pragma once

#include "../lib/PoolAllocator/PoolAllocator.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <chrono>

struct Plot {
    std::vector<uint32_t> time{0};
    std::vector<uint32_t> size{0};
};

template<typename Allocator>
class Test {
    template<typename Container, typename Func>
    Plot test(Allocator& allocator, size_t number_of_tests, size_t rate, Func add) {
        Container container(allocator);
        Plot plot;
        plot.size.reserve(number_of_tests);
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < number_of_tests; i += rate) {
            for (size_t j = 0; j < rate; ++j) {
                add(container);
            }
            auto time = std::chrono::high_resolution_clock::now() - start;

            plot.time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(time).count());
            plot.size.push_back(i + rate);
            std::cout << i + rate << ' ' << std::chrono::duration_cast<std::chrono::microseconds>(time).count() << '\n';
        }

        return plot;
    }
public:
    Plot test_vector(Allocator& allocator, size_t number_of_tests, size_t rate) {
        return test<std::vector<int32_t , Allocator>>(allocator, number_of_tests, rate, [](std::vector<int32_t, Allocator>& container) { container.push_back(1); });
    }

    Plot test_list(Allocator& allocator, size_t number_of_tests, size_t rate) {
        return test<std::list<int32_t , Allocator>>(allocator, number_of_tests, rate, [](std::list<int32_t , Allocator>& container) { container.push_back(1); });
    }

    Plot test_map(Allocator& allocator, size_t number_of_tests, size_t rate) {
        return test<std::map<int32_t , int32_t , std::less<int32_t>, Allocator>>(allocator, number_of_tests, rate, [](std::map<int32_t , int32_t, std::less<int32_t>, Allocator>& container) { container.emplace(container.size(), container.size()); });
    }

};
