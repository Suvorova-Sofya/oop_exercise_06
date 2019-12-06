#ifndef D_QUEUE_H_
#define D_QUEUE_H_

#include <iostream>
#include "five_angles.h"
#include <memory>
#include <functional>
#include <cassert>
#include <iterator>
#include <type_traits>

namespace containers {

    template<class T,class Allocator>
    struct queue {
    private:
        struct node;

    public:
        queue() = default;

        struct forward_iterator {
            using value_type = T;
            using reference = T &;
            using pointer = T *;
            using difference_type = ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            forward_iterator(node *ptr);

            T &operator*();

            forward_iterator &operator++();

            forward_iterator operator+(int r);

            bool operator==(const forward_iterator &o) const;

            bool operator!=(const forward_iterator &o) const;

        private:
            node *ptr_;

            friend queue;

        };

        forward_iterator begin();

        forward_iterator end();

        void insert(const forward_iterator &it, const T &value);

        void erase(const forward_iterator &it);

        void pop();

        void push(const T &value);

        T front();


    private:

        using allocator_type = typename Allocator::template rebind<node>::other;

        struct deleter {
            deleter(allocator_type* allocator): allocator_(allocator) {}

            void operator() (node* ptr) {
                if(ptr != nullptr){
                    std::allocator_traits<allocator_type>::destroy(*allocator_, ptr);
                    allocator_->deallocate(ptr, 1);
                }
            }

        private:
            allocator_type* allocator_;

        };

        using unique_ptr = std::unique_ptr<node, deleter>;

        node *end_node = nullptr;

        node *end_help(node *ptr);

        struct node {
            T value;
            unique_ptr next{nullptr, deleter{nullptr}};
            node *parent = nullptr;

            forward_iterator nextf();
        };

        allocator_type allocator_{};

        unique_ptr root{nullptr, deleter{nullptr}};
    };


//

    template<class T,class Allocater>
    typename queue<T,Allocater>::node *queue<T,Allocater>::end_help(containers::queue<T,Allocater>::node *ptr) {
        if ((ptr == nullptr) || (ptr->next == nullptr)) {
            return ptr;
        }
        return queue<T,Allocater>::end_help(ptr->next.get());
    }


    template<class T,class Allocater>
    typename queue<T,Allocater>::forward_iterator queue<T,Allocater>::begin() {
        if (root == nullptr) {
            return nullptr;
        }
        forward_iterator it(root.get());
        return it;
    }

    template<class T,class Allocater>
    typename queue<T,Allocater>::forward_iterator queue<T,Allocater>::end() {
        return nullptr;
    }

    template<class T,class Allocator>
    void queue<T,Allocator>::insert(const queue<T,Allocator>::forward_iterator &it, const T &value) {
        node* ptr_result =allocator_.allocate(1);
        ptr_result->value=value;
        //std::allocator_traits<allocator_type>::construct(allocator_,ptr_result, value);
        std::unique_ptr<node,queue<T, Allocator>::deleter> new_node(ptr_result,deleter{&allocator_});
        if (it != nullptr) {
            node *ptr = it.ptr_->parent;
            new_node->parent = it.ptr_->parent;
            it.ptr_->parent = new_node.get();
            if (ptr) {
                new_node->next = std::move(ptr->next);
                ptr->next = std::move(new_node);
            } else {
                new_node->next = std::move(root);
                root = std::move(new_node);
            }
        } else {
            new_node->next = nullptr;
            if(end_node==nullptr) {
                new_node->parent= nullptr;
                new_node->next= nullptr;
                queue<T,Allocator>::root = std::move(new_node);
            }else{
                new_node->parent=end_node;
                new_node->next= nullptr;
                end_node->next=std::move(new_node);
            }
        }
        end_node = end_help(root.get());
    }

    template<class T,class Allocater>
    void queue<T,Allocater>::erase(const queue<T,Allocater>::forward_iterator &it) {
        if (it.ptr_ == nullptr) {
            throw std::logic_error("erasing invalid iterator");
        }
        unique_ptr &pointer_from_parent = [&]() -> unique_ptr & {
            if (it.ptr_ == root.get()) {
                return root;
            }
            return it.ptr_->parent->next;
        }();
        if(it.ptr_->next) {
            it.ptr_->next->parent = it.ptr_->parent;
        }
        pointer_from_parent = std::move(it.ptr_->next);
        end_node = end_help(root.get());
    }

//
    template<class T,class Allocater>
    typename queue<T,Allocater>::forward_iterator queue<T,Allocater>::node::nextf() {
        forward_iterator result(this->next.get());
        return result;
    }

    template<class T,class Allocater>
    queue<T,Allocater>::forward_iterator::forward_iterator(node *ptr): ptr_{ptr} {}

    template<class T,class Allocater>
    T &queue<T,Allocater>::forward_iterator::operator*() {
        return ptr_->value;
    }

    template<class T,class Allocater>
    typename queue<T,Allocater>::forward_iterator &queue<T,Allocater>::forward_iterator::operator++() {
        if (*this != nullptr) {
            *this = ptr_->nextf();
            return *this;
        } else {
            throw std::logic_error("invalid iterator");
        }
    }

    template<class T,class Allocater>
    typename queue<T,Allocater>::forward_iterator queue<T,Allocater>::forward_iterator::operator+(int r) {
        for (int i = 0; i < r; ++i) {
            ++*this;
        }
        return *this;
    }

    template<class T,class Allocater>
    bool queue<T,Allocater>::forward_iterator::operator==(const forward_iterator &o) const {
        return ptr_ == o.ptr_;
    }

    template<class T,class Allocater>
    bool queue<T,Allocater>::forward_iterator::operator!=(const forward_iterator &o) const {
        return ptr_ != o.ptr_;
    }

    template<class T,class Allocater>
    T queue<T,Allocater>::front() {
        if (queue<T,Allocater>::root == nullptr) {
            throw std::logic_error("no elements");
        }
        return queue<T,Allocater>::root->value;
    }

    template<class T,class Allocater>
    void queue<T,Allocater>::pop() {
        if (queue<T,Allocater>::root == nullptr) {
            throw std::logic_error("no elements");
        }
        erase(queue<T,Allocater>::begin());
    }

    template<class T,class Allocater>
    void queue<T,Allocater>::push(const T &value) {
        forward_iterator it(end_node);
        node* ptr_result =allocator_.allocate(1);
        ptr_result->value=value;
        //std::allocator_traits<allocator_type>::construct(allocator_,ptr_result, value);
        std::unique_ptr<node,queue<T, Allocater>::deleter> new_node(ptr_result,deleter{&allocator_});
        if (it.ptr_) {
            new_node->parent = it.ptr_;
            it.ptr_->next = std::move(new_node);
        } else {
            new_node->next = nullptr;
            queue<T,Allocater>::root = std::move(new_node);
        }
        queue<T,Allocater>::end_node = end_help(root.get());
    }

}
#endif