#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <limits>

class MemoryPool {
public:
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using pointer = char*;

    struct Block {
        bool free_;
        char* ptr_;
        size_type size_;

        Block(char* ptr, size_type size, bool free);
        bool operator==(const Block& other) const;
    };

    MemoryPool() = delete;
    MemoryPool(size_type pool_size, size_type chunk_size);
    ~MemoryPool();

    void* allocate(size_type bytes_to_alloc);
    void deallocate(char* block_start_ptr);

    size_type losses(size_type bytes_to_alloc) const;
    bool can_alloc(size_type bytes_to_alloc) const;
    bool from_this_pool(char* ptr) const;

    size_type getMaxFreeSize() { return max_free_size_; }
    std::vector<Block>& getBlocksList() { return blocks_; }
    size_type getOccupied() const;

private:
    char* start_; // Start of the memory pool
    size_type max_free_size_; // Max of continuous free chunks
    size_type size_; // Size of pool
    size_type chunk_size_; // Size of chunks in pool
    std::vector<Block> blocks_; // Deque of all blocks of pool (block is a group of chunks)

};

template<typename T>
class PoolAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
//    template<typename U> using rebind = PoolAllocator<U>;
    template<class U> struct rebind { typedef PoolAllocator<U> other; };

    PoolAllocator() = default;

    explicit PoolAllocator(MemoryPool& pool) : pools_(std::vector<MemoryPool*>{&pool}) {}

    PoolAllocator(std::initializer_list<MemoryPool*> list) : pools_(list) {}

    ~PoolAllocator() = default;

    PoolAllocator& operator=(const PoolAllocator& other) {
        this->pools_ = other.pools_;

        return *this;
    }

    PoolAllocator(const PoolAllocator& other) = default;

    template<class U>
    explicit PoolAllocator(const PoolAllocator<U>& other) {
        pools_ = std::vector<MemoryPool*>{other.getPoolsList()};
    }

    void addPool(MemoryPool& pool) {
        pools_.push_back(&pool);
    }

    T* allocate(size_type n) {
        size_type bytes_to_allocate = n * sizeof(value_type);
        size_type min_loss = std::numeric_limits<size_type>::max();
        MemoryPool* p;
        for (MemoryPool* pool : pools_) {
            if (pool->can_alloc(bytes_to_allocate)) {
                size_type loss = pool->losses(bytes_to_allocate);
                if (loss < min_loss) {
                    min_loss = loss;
                    p = pool;
                }
            }
        }

        if (min_loss == std::numeric_limits<size_type>::max())
            throw std::bad_alloc{};
        else
            return static_cast<pointer>(p->allocate(bytes_to_allocate));
    }

    void deallocate(pointer pointer_to_deallocate, size_type n) {
        for (auto& pool : pools_) {
            if (pool->from_this_pool(reinterpret_cast<char*>(pointer_to_deallocate))) {
                pool->deallocate(reinterpret_cast<char*>(pointer_to_deallocate));
                break;
            }
        }
    }

    size_type max_size() const {
        size_type ret = 0;
        for (MemoryPool* pool : pools_) {
            if (pool->getMaxFreeSize() > ret)
                ret = pool->getMaxFreeSize();
        }

        return ret;
    }

    size_type total_space_left() const {
        size_type ret = 0;
        for (MemoryPool* pool : pools_) {
            for (auto b : pool->getBlocksList()) {
                if (b.free_)
                    ret += b.size_;
            }
        }

        return ret;
    }

    const std::vector<MemoryPool*>& getPoolsList() const { return pools_; }

    bool operator==(const PoolAllocator& other) const { return pools_ == other.pools_; }
    bool operator!=(const PoolAllocator& other) const { return pools_ != other.pools_; }

    void info() {
        for (MemoryPool* pool : pools_) {
            std::cout << "MaxFreeLinearSize: " << pool->getMaxFreeSize() << std::endl;
            std::cout << "Blocks:\n";
            for (auto b : pool->getBlocksList()) {
                std::cout << "size: " << b.size_ << " free " << b.free_ << std::endl;
            }
            std::cout << "------End of this pool-------\n";
        }
    }

private:
    std::vector<MemoryPool*> pools_;
};
