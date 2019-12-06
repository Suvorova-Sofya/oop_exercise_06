#ifndef D_ALLOCATOR_H_
#define D_ALLOCATOR_H_

#include <iostream>
#include "queue.h"
#include "list.h"

template<class T, size_t ALLOC_SIZE>
struct q_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::false_type;

    template<class U>
    struct rebind {
        using other = q_allocator<U, ALLOC_SIZE>;
    };

    q_allocator() :
            memory_pool_begin_(new char[ALLOC_SIZE]),
            memory_pool_end_(memory_pool_begin_ + ALLOC_SIZE),
            memory_pool_tail_(memory_pool_begin_) {}

    q_allocator(const q_allocator &) = delete;

    q_allocator(q_allocator &&) = delete;

    ~q_allocator() {
        delete[] memory_pool_begin_;
    }

    T *allocate(std::size_t n);

    void deallocate(T *ptr, std::size_t n);

private:
    char *memory_pool_begin_;
    char *memory_pool_end_;
    char *memory_pool_tail_;
    containersl::list<char *> free_blocks_;

};

template<class T, size_t ALLOC_SIZE>
T* q_allocator<T, ALLOC_SIZE>::allocate(std::size_t n) {
    if(size_t(memory_pool_end_ - memory_pool_tail_) < sizeof(T)){
        if(!free_blocks_.is_empty()){
            char *ptr;
            for (size_t i = 0; i < n; ++i) {
                auto it = free_blocks_.begin();
                ptr = *it;
                free_blocks_.erase(it);
            }
            return reinterpret_cast<T *>(ptr);
        }
        throw std::bad_alloc();
    }
    T* result = reinterpret_cast<T*>(memory_pool_tail_);
    memory_pool_tail_ += sizeof(T);
    return result;
}

template<class T, size_t ALLOC_SIZE>
void q_allocator<T, ALLOC_SIZE>::deallocate(T* ptr, std::size_t n) {
    if(ptr == nullptr){
        return;
    }
    free_blocks_.insert(free_blocks_.end(),reinterpret_cast<char*>(ptr));
}



#endif