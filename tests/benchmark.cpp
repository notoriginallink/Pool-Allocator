#include "benchmark.h"

#include <fstream>

void writef(Plot& plot, std::string name) {
    std::ofstream file("..\\tests\\" + name);
    for (int i = 0; i < plot.time.size(); ++i) {
        file << plot.size[i] << ':' << plot.time[i] << '\n';
    }
    file.close();
}

const size_t kNumberOfElements = 100000;
const size_t kRate = 1000;
const size_t kMegabyte = 1 << 20;
const size_t kFour = 4;
const size_t kHKilobyte = 1 << 9;
const size_t kKilobyte = 1 << 10;

MemoryPool pool1(16 * kMegabyte, kFour);
MemoryPool pool2(16 * kMegabyte, 256);
MemoryPool pool3(16 * kMegabyte, kKilobyte);

void test_standard_vector() {
    std::allocator<int32_t> alloc;

    Test<std::allocator<int32_t>> test;

    Plot plot = test.test_vector(alloc, kNumberOfElements, kRate);
    writef(plot, "standard_vector.txt");
}

void test_pool_vector() {
    PoolAllocator<int32_t> alloc({&pool1, &pool2, &pool3});
    alloc.info();

    Test<PoolAllocator<int32_t>> test;

    Plot plot = test.test_vector(alloc, kNumberOfElements, kRate);
    alloc.info();
    writef(plot, "pool_vector.txt");
}

void test_standard_list() {
    std::allocator<int32_t> alloc;

    Test<std::allocator<int32_t>> test;

    Plot plot = test.test_list(alloc, kNumberOfElements, kRate);
    writef(plot, "standard_list.txt");
}

void test_pool_list() {
    PoolAllocator<int32_t> alloc({&pool3, &pool2, &pool1});

    Test<PoolAllocator<int32_t>> test;

    Plot plot = test.test_list(alloc, kNumberOfElements, kRate);
    alloc.info();

    writef(plot, "pool_list.txt");
}

void test_standard_map() {
    std::allocator<int32_t> alloc;

    Test<std::allocator<int32_t>> test;

    Plot plot = test.test_map(alloc, kNumberOfElements / 10, kRate / 10);
    writef(plot, "standard_map.txt");
}

void test_pool_map() {
    PoolAllocator<int32_t> alloc({&pool1, &pool2, &pool3});

    Test<PoolAllocator<int32_t>> test;

    Plot plot = test.test_map(alloc, kNumberOfElements / 10, kRate / 10);
    writef(plot, "pool_map.txt");
}

void testVector() {
    test_pool_vector();
    test_standard_vector();
}

void startTest() {
    test_standard_vector();
    test_pool_vector();
    test_standard_list();
    test_pool_list();
    test_standard_map();
    test_pool_map();
}

int main() {
    startTest();

    return 0;
}
