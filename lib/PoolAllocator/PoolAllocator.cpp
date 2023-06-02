#include "PoolAllocator.h"

MemoryPool::MemoryPool(size_type pool_size, size_type chunk_size)
        : size_(pool_size), chunk_size_(chunk_size), max_free_size_(pool_size / chunk_size) {
    start_ = static_cast<char*>(malloc(size_));
    blocks_.reserve(pool_size / chunk_size);
    blocks_.emplace_back(start_, max_free_size_, true);
}

MemoryPool::Block::Block(char* ptr, size_type size, bool free) : ptr_(ptr), size_(size), free_(free) {}

bool MemoryPool::Block::operator==(const Block& other) const {
    return (ptr_ == other.ptr_ && size_ == other.size_ && free_ == other.free_);
}

MemoryPool::~MemoryPool() {
    free(start_);
}

void* MemoryPool::allocate(size_type bytes_to_alloc) {
    size_type chunks_to_alloc = bytes_to_alloc / chunk_size_;
    void* ret = nullptr;
    if (bytes_to_alloc % chunk_size_ != 0)
        chunks_to_alloc++;
    if (max_free_size_ < chunks_to_alloc || max_free_size_ == 0) {
        return nullptr;
    } else {
        for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
            if (it->free_) {
                if (it->size_ == max_free_size_)
                    max_free_size_ -= chunks_to_alloc;
                if (it->size_ == chunks_to_alloc) {
                    it->free_ = false;
                    ret = it->ptr_;
                    break;
                } else if (it->size_ > chunks_to_alloc) {
                    it->free_ = false;
                    ret = it->ptr_;
                    blocks_.emplace(it + 1,it->ptr_ + chunks_to_alloc * chunk_size_, it->size_ - chunks_to_alloc, true);
                    it->size_ = chunks_to_alloc;
                    break;
                }
            }
        }

        return ret;
    }
}

MemoryPool::size_type MemoryPool::getOccupied() const {
    MemoryPool::size_type ret = 0;
    for (auto b : blocks_)
        if (!b.free_)
            ret += b.size_;

    return ret;
}

void MemoryPool::deallocate(char* block_start_ptr) {
    std::vector<Block>::iterator temp;
    for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
        if (it->ptr_ == block_start_ptr) {
            temp = it;
            it->free_ = true;
            break;
        }
    }
    if (temp == blocks_.begin()) {
        if (temp + 1 != blocks_.end() && (temp + 1)->free_) {
            temp->size_ += (temp + 1)->size_;
            blocks_.erase(temp + 1);
        }
        if (temp->size_ > max_free_size_)
            max_free_size_ = temp->size_;
    } else if (temp == blocks_.end() - 1) {
        if (temp != blocks_.begin() && (temp - 1)->free_) {
            (temp - 1)->size_ += temp->size_;
            blocks_.erase(temp);
        }
        if ((temp - 1)->size_ > max_free_size_)
            max_free_size_ = (temp - 1)->size_;
    } else {
        if ((temp - 1)->free_) {
            (temp - 1)->size_ += temp->size_;
            if ((temp + 1)->free_) {
                (temp - 1)->size_ += (temp + 1)->size_;
                blocks_.erase(temp, temp + 2);
            } else {
                blocks_.erase(temp);
            }

        } else if ((temp + 1)->free_) {
            temp->size_ += (temp + 1)->size_;
            blocks_.erase(temp + 1);
        }
        max_free_size_ = std::max(temp->size_, (temp - 1)->size_);
    }

}

MemoryPool::size_type MemoryPool::losses(size_type bytes_to_alloc) const {
    size_type chunks_to_alloc = bytes_to_alloc / chunk_size_;
    if (bytes_to_alloc % chunk_size_ != 0)
        chunks_to_alloc++;
    return chunks_to_alloc * chunk_size_ - bytes_to_alloc;
}

bool MemoryPool::can_alloc(size_type bytes_to_alloc) const { return bytes_to_alloc <= max_free_size_ * chunk_size_; }

bool MemoryPool::from_this_pool(char *ptr) const { return ptr >= start_ && ptr <= start_ + size_; }
