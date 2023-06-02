#include "lib/PoolAllocator/PoolAllocator.h"

#include <iostream>

int main() {
    std::allocator<int> al;
    PoolAllocator<int> alloc;
    // вектор имеет фикс размер и может быть неэффективен для выделения больших блоков памяти или неудобен для работы в сложных приложениях
    // также вектор не всегда может гарантировать выделение непрерывного куска памяти
}
