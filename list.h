#ifndef D_LIST_H_
#define D_LIST_H_

#include <iostream>
#include "five_angles.h"
#include <memory>
#include <functional>
#include <cassert>
#include <iterator>


namespace containersl {

    template<class T, class Allocator = std::allocator<T>>
    struct list {
    private:
        struct node;

    public:
        list() = default;

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

            friend list;

        };

        forward_iterator begin();

        forward_iterator end();

        void insert(const forward_iterator &it, const T &value);

        void erase(const forward_iterator &it);

        int is_empty(){
            return root==nullptr;
        };

        size_t size=0;

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

    template<class T,class Allocator>
    typename list<T,Allocator>::node *list<T,Allocator>::end_help(containersl::list<T,Allocator>::node *ptr) {
        if ((ptr == nullptr) || (ptr->next == nullptr)) {
            return ptr;
        }
        return list<T,Allocator>::end_help(ptr->next.get());
    }


    template<class T,class Allocator>
    typename list<T,Allocator>::forward_iterator list<T,Allocator>::begin() {
        if (root == nullptr) {
            return nullptr;
        }
        forward_iterator it(root.get());
        return it;
    }

    template<class T,class Allocator>
    typename list<T,Allocator>::forward_iterator list<T,Allocator>::end() {
        return nullptr;
    }

    template<class T,class Allocator>
    void list<T,Allocator>::insert(const list<T,Allocator>::forward_iterator &it, const T &value) {
        std::unique_ptr<node,list<T, Allocator>::deleter> new_node{new node{value},deleter{&allocator_}};
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
                list<T,Allocator>::root = std::move(new_node);
            }else{
                new_node->parent=end_node;
                new_node->next= nullptr;
                end_node->next=std::move(new_node);
            }
        }
        end_node = end_help(root.get());
        ++size;
    }

    template<class T,class Allocator>
    void list<T,Allocator>::erase(const list<T,Allocator>::forward_iterator &it) {
        if (it.ptr_ == nullptr) {
            throw std::logic_error("erasing invalid iterator");
        }
        unique_ptr &pointer_from_parent = [&]() -> unique_ptr & {
            if (it.ptr_ == root.get()) {
                return root;
            }
            return it.ptr_->parent->next;
        }();
        pointer_from_parent = std::move(it.ptr_->next);

        end_node = end_help(root.get());
        --size;
    }

//
    template<class T,class Allocator>
    typename list<T,Allocator>::forward_iterator list<T,Allocator>::node::nextf() {
        forward_iterator result(this->next.get());
        return result;
    }

    template<class T,class Allocator>
    list<T,Allocator>::forward_iterator::forward_iterator(node *ptr): ptr_{ptr} {}

    template<class T,class Allocator>
    T &list<T,Allocator>::forward_iterator::operator*() {
        return ptr_->value;
    }

    template<class T,class Allocator>
    typename list<T,Allocator>::forward_iterator &list<T,Allocator>::forward_iterator::operator++() {
        if (*this != nullptr) {
            *this = ptr_->nextf();
            return *this;
        } else {
            throw std::logic_error("invalid iterator");
        }
    }

    template<class T,class Allocator>
    typename list<T,Allocator>::forward_iterator list<T,Allocator>::forward_iterator::operator+(int r) {
        for (int i = 0; i < r; ++i) {
            ++*this;
        }
        return *this;
    }

    template<class T,class Allocator>
    bool list<T,Allocator>::forward_iterator::operator==(const forward_iterator &o) const {
        return ptr_ == o.ptr_;
    }

    template<class T,class Allocator>
    bool list<T,Allocator>::forward_iterator::operator!=(const forward_iterator &o) const {
        return ptr_ != o.ptr_;
    }

}
#endif