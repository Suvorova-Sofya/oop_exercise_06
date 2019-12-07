#ifndef D_LIST_H_
#define D_LIST_H_

#include <iostream>
#include "five_angles.h"
#include <memory>
#include <functional>
#include <cassert>
#include <iterator>


namespace containersl {

    template<class T>
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

        using unique_ptr = std::unique_ptr<node>;

        node *end_node = nullptr;

        node *end_help(node *ptr);

        struct node {
            T value;
            unique_ptr next{nullptr};
            node *parent = nullptr;
            forward_iterator nextf();
        };

        unique_ptr root{nullptr};

    };


//

    template<class T>
    typename list<T>::node *list<T>::end_help(containersl::list<T>::node *ptr) {
        if ((ptr == nullptr) || (ptr->next == nullptr)) {
            return ptr;
        }
        return list<T>::end_help(ptr->next.get());
    }


    template<class T>
    typename list<T>::forward_iterator list<T>::begin() {
        if (root == nullptr) {
            return nullptr;
        }
        forward_iterator it(root.get());
        return it;
    }

    template<class T>
    typename list<T>::forward_iterator list<T>::end() {
        return nullptr;
    }

    template<class T>
    void list<T>::insert(const list<T>::forward_iterator &it, const T &value) {
        std::unique_ptr<node> new_node{new node{value}};
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
                list<T>::root = std::move(new_node);
            }else{
                new_node->parent=end_node;
                new_node->next= nullptr;
                end_node->next=std::move(new_node);
            }
        }
        end_node = end_help(root.get());
        ++size;
    }

    template<class T>
    void list<T>::erase(const list<T>::forward_iterator &it) {
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
    template<class T>
    typename list<T>::forward_iterator list<T>::node::nextf() {
        forward_iterator result(this->next.get());
        return result;
    }

    template<class T>
    list<T>::forward_iterator::forward_iterator(node *ptr): ptr_{ptr} {}

    template<class T>
    T &list<T>::forward_iterator::operator*() {
        return ptr_->value;
    }

    template<class T>
    typename list<T>::forward_iterator &list<T>::forward_iterator::operator++() {
        if (*this != nullptr) {
            *this = ptr_->nextf();
            return *this;
        } else {
            throw std::logic_error("invalid iterator");
        }
    }

    template<class T>
    typename list<T>::forward_iterator list<T>::forward_iterator::operator+(int r) {
        for (int i = 0; i < r; ++i) {
            ++*this;
        }
        return *this;
    }

    template<class T>
    bool list<T>::forward_iterator::operator==(const forward_iterator &o) const {
        return ptr_ == o.ptr_;
    }

    template<class T>
    bool list<T>::forward_iterator::operator!=(const forward_iterator &o) const {
        return ptr_ != o.ptr_;
    }

}
#endif