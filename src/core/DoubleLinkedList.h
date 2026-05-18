#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H

#include "LinkedList.h"
#include <iostream>
#include <utility>

template <typename T>
struct DoubleNode {
    T              data = T();
    DoubleNode<T>* next = nullptr;
    DoubleNode<T>* prev = nullptr;
};

template <typename T>
class DoubleLinkedList : public LinkedList<T> {
public:
    DoubleLinkedList();
    ~DoubleLinkedList() override;
    DoubleLinkedList(const DoubleLinkedList<T>& other);
    DoubleLinkedList<T>& operator=(const DoubleLinkedList<T>& other);
    DoubleLinkedList(DoubleLinkedList<T>&& other) noexcept;
    DoubleLinkedList<T>& operator=(DoubleLinkedList<T>&& other) noexcept;

    void destroy() override;

    DoubleNode<T>*& get_head();
    DoubleNode<T>*& get_tail();

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

    void                reverse(DoubleNode<T>*& start, bool show = true);
    bool                check_palindrome();
    DoubleLinkedList<T> merge(DoubleLinkedList<T>& other);

private:
    DoubleNode<T>* head      = nullptr;
    DoubleNode<T>* tail      = nullptr;
    int            size      = 0;
    bool           is_empty  = true;
    T              max_val   = T();
    T              min_val   = T();
    int            max_count = 0;
    int            min_count = 0;

    void scan_min_max();
    void on_insert(const T& value);
    void on_remove(const T& value);
    void _copy_from(const DoubleLinkedList<T>& src);
    void _zero() noexcept;
};

#include "DoubleLinkedList.cpp"
#endif