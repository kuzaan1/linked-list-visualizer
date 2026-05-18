#ifndef SINGLE_LINKED_LIST_H
#define SINGLE_LINKED_LIST_H

#include "LinkedList.h"
#include <iostream>
#include <utility>

template <typename T>
struct SingleNode {
    T              data = T();
    SingleNode<T>* next = nullptr;
};

template <typename T>
class SingleLinkedList : public LinkedList<T> {
public:
    SingleLinkedList();
    ~SingleLinkedList() override;
    SingleLinkedList(const SingleLinkedList<T>& other);
    SingleLinkedList<T>& operator=(const SingleLinkedList<T>& other);
    SingleLinkedList(SingleLinkedList<T>&& other) noexcept;
    SingleLinkedList<T>& operator=(SingleLinkedList<T>&& other) noexcept;

    void destroy() override;

    SingleNode<T>*& get_head();
    SingleNode<T>*& get_tail();

    int  get_size()        override;
    bool empty()           override;
    T&   get_item(int pos) override;
    T    get_first()       override;
    T    get_last()        override;
    T    get_middle()      override;
    T    get_max()         override;
    T    get_min()         override;
    bool search(T value)   override;
    void update_min_max()  override;

    void insert_at_first    (T value, bool show = true)          override;
    void insert_at_end      (T value, bool show = true)          override;
    void insert_at_position (int pos, T value, bool show = true) override;

    void delete_at_first    (bool show = true)                   override;
    void delete_at_end      (bool show = true)                   override;
    void delete_at_position (int pos, bool show = true)          override;

    void print() override;
    void reset() override;

    void                reverse(SingleNode<T>*& start, bool show = true);
    bool                check_palindrome();
    SingleLinkedList<T> merge(SingleLinkedList<T>& other);

private:
    SingleNode<T>* head      = nullptr;
    SingleNode<T>* tail      = nullptr;
    int            size      = 0;
    bool           is_empty  = true;
    T              max_val   = T();
    T              min_val   = T();
    int            max_count = 0;
    int            min_count = 0;

    void scan_min_max();
    void on_insert(const T& value);
    void on_remove(const T& value);
    void _copy_from(const SingleLinkedList<T>& src);
    void _zero() noexcept;
};

#include "SingleLinkedList.cpp"
#endif