#include "DoubleLinkedList.h"
#include <iostream>
#include <utility>

template <typename T>
void DoubleLinkedList<T>::_zero() noexcept {
    head = tail = nullptr;
    size = 0;
    is_empty = true;
    max_val = min_val = T();
    max_count = min_count = 0;
}

template <typename T>
void DoubleLinkedList<T>::_copy_from(const DoubleLinkedList<T>& src) {
    for (const DoubleNode<T>* c = src.head; c; c = c->next)
        insert_at_end(c->data, false);
}

template <typename T>
DoubleLinkedList<T>::DoubleLinkedList() { _zero(); }

template <typename T>
DoubleLinkedList<T>::~DoubleLinkedList() { destroy(); }

template <typename T>
DoubleLinkedList<T>::DoubleLinkedList(const DoubleLinkedList<T>& other) {
    _zero();
    _copy_from(other);
}

template <typename T>
DoubleLinkedList<T>& DoubleLinkedList<T>::operator=(const DoubleLinkedList<T>& other) {
    if (this != &other) { destroy(); _copy_from(other); }
    return *this;
}

template <typename T>
DoubleLinkedList<T>::DoubleLinkedList(DoubleLinkedList<T>&& other) noexcept {
    head      = other.head;
    tail      = other.tail;
    size      = other.size;
    is_empty  = other.is_empty;
    max_val   = other.max_val;
    min_val   = other.min_val;
    max_count = other.max_count;
    min_count = other.min_count;
    other._zero();
}

template <typename T>
DoubleLinkedList<T>& DoubleLinkedList<T>::operator=(DoubleLinkedList<T>&& other) noexcept {
    if (this != &other) {
        destroy();
        head      = other.head;
        tail      = other.tail;
        size      = other.size;
        is_empty  = other.is_empty;
        max_val   = other.max_val;
        min_val   = other.min_val;
        max_count = other.max_count;
        min_count = other.min_count;
        other._zero();
    }
    return *this;
}

template <typename T>
void DoubleLinkedList<T>::destroy() {
    DoubleNode<T>* cur = head;
    while (cur) {
        DoubleNode<T>* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    _zero();
}

template <typename T>
void DoubleLinkedList<T>::scan_min_max() {
    if (!head) { max_count = min_count = 0; return; }
    max_val = min_val = head->data;
    max_count = min_count = 1;
    for (DoubleNode<T>* c = head->next; c; c = c->next) {
        if      (c->data > max_val)  { max_val = c->data; max_count = 1; }
        else if (c->data == max_val) { ++max_count; }
        if      (c->data < min_val)  { min_val = c->data; min_count = 1; }
        else if (c->data == min_val) { ++min_count; }
    }
}

template <typename T>
void DoubleLinkedList<T>::on_insert(const T& value) {
    if (size == 0) { max_val = min_val = value; max_count = min_count = 1; return; }
    if      (value > max_val)  { max_val = value; max_count = 1; }
    else if (value == max_val) { ++max_count; }
    if      (value < min_val)  { min_val = value; min_count = 1; }
    else if (value == min_val) { ++min_count; }
}

template <typename T>
void DoubleLinkedList<T>::on_remove(const T& value) {
    if (value == max_val) { --max_count; if (!max_count) scan_min_max(); }
    if (value == min_val) { --min_count; if (!min_count) scan_min_max(); }
}

template <typename T>
void DoubleLinkedList<T>::reset() {
    if (!head) { tail = nullptr; is_empty = true; size = 0; max_count = min_count = 0; }
    else       { is_empty = false; }
}

template <typename T> void            DoubleLinkedList<T>::update_min_max() { scan_min_max(); }
template <typename T> DoubleNode<T>*& DoubleLinkedList<T>::get_head()       { return head; }
template <typename T> DoubleNode<T>*& DoubleLinkedList<T>::get_tail()       { return tail; }
template <typename T> int             DoubleLinkedList<T>::get_size()        { return size; }
template <typename T> bool            DoubleLinkedList<T>::empty()           { return is_empty; }
template <typename T> T               DoubleLinkedList<T>::get_max()         { return max_val; }
template <typename T> T               DoubleLinkedList<T>::get_min()         { return min_val; }

template <typename T>
bool DoubleLinkedList<T>::search(T value) {
    for (DoubleNode<T>* c = head; c; c = c->next)
        if (c->data == value) return true;
    return false;
}

template <typename T>
T& DoubleLinkedList<T>::get_item(int pos) {
    DoubleNode<T>* c = head;
    for (int i = 0; i < pos; ++i) c = c->next;
    return c->data;
}

template <typename T>
T DoubleLinkedList<T>::get_first() {
    if (!head) { std::cout << "\nList is empty.\n"; return T(); }
    return head->data;
}

template <typename T>
T DoubleLinkedList<T>::get_last() {
    if (!tail) { std::cout << "\nList is empty.\n"; return T(); }
    return tail->data;
}

template <typename T>
T DoubleLinkedList<T>::get_middle() {
    if (!head) { std::cout << "\nList is empty.\n"; return T(); }
    DoubleNode<T>* slow = head, *fast = head;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    return slow->data;
}

template <typename T>
void DoubleLinkedList<T>::insert_at_first(T value, bool show) {
    auto* node = new DoubleNode<T>;
    node->data = value;
    node->prev = nullptr;
    node->next = head;
    if (!head) { head = tail = node; is_empty = false; }
    else       { head->prev = node; head = node; }
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::insert_at_end(T value, bool show) {
    if (!head) { insert_at_first(value, show); return; }
    auto* node = new DoubleNode<T>;
    node->data = value;
    node->next = nullptr;
    node->prev = tail;
    tail->next = node;
    tail = node;
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::insert_at_position(int location, T value, bool show) {
    if (location < 1 || location > size + 1) {
        std::cout << "\nPosition out of range.\n"; return;
    }
    if (location == 1)        { insert_at_first(value, show); return; }
    if (location == size + 1) { insert_at_end(value, show);   return; }

    DoubleNode<T>* cur = head;
    for (int i = 1; i < location - 1; ++i) cur = cur->next;
    auto* node = new DoubleNode<T>;
    node->data = value;
    node->next = cur->next;
    node->prev = cur;
    if (cur->next) cur->next->prev = node; else tail = node;
    cur->next = node;
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::delete_at_first(bool show) {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    auto*   dying = head;
    const T bye   = dying->data;
    head = head->next;
    if (head) head->prev = nullptr; else tail = nullptr;
    delete dying;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::delete_at_end(bool show) {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    if (head == tail) {
        const T bye = head->data;
        delete head;
        head = tail = nullptr;
        on_remove(bye);
        --size;
        reset();
        if (show) std::cout << "\nItem deleted successfully.\n";
        return;
    }
    auto*   dying = tail;
    const T bye   = dying->data;
    tail       = tail->prev;
    tail->next = nullptr;
    delete dying;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::delete_at_position(int location, bool show) {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    if (location < 1 || location > size) {
        std::cout << "\nPosition out of range.\n"; return;
    }
    if (location == 1) { delete_at_first(show); return; }
    DoubleNode<T>* cur = head->next;
    for (int i = 2; i < location; ++i) cur = cur->next;
    const T bye      = cur->data;
    cur->prev->next  = cur->next;
    if (cur->next) cur->next->prev = cur->prev; else tail = cur->prev;
    delete cur;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void DoubleLinkedList<T>::print() {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    std::cout << "\n[ null <-> ";
    for (DoubleNode<T>* c = head; c; c = c->next) {
        std::cout << c->data;
        if (c->next) std::cout << " <-> ";
    }
    std::cout << " <-> null ]\n";
}

template <typename T>
void DoubleLinkedList<T>::reverse(DoubleNode<T>*& start, bool show) {
    if (!start) return;
    DoubleNode<T>* cur = start, *new_head = nullptr;
    while (cur) {
        DoubleNode<T>* old_next = cur->next;
        cur->next = cur->prev;
        cur->prev = old_next;
        new_head  = cur;
        cur       = old_next;
    }
    tail  = start;
    start = new_head;
    head  = start;
    if (show) std::cout << "\nList reversed successfully.\n";
}

template <typename T>
bool DoubleLinkedList<T>::check_palindrome() {
    if (!head || head == tail) return true;
    DoubleNode<T>* L = head, *R = tail;
    while (L != R && L->next != R) {
        if (L->data != R->data) return false;
        L = L->next;
        R = R->prev;
    }
    return L->data == R->data;
}

template <typename T>
DoubleLinkedList<T> DoubleLinkedList<T>::merge(DoubleLinkedList<T>& other) {
    DoubleLinkedList<T>    result;
    const DoubleNode<T>*   a = head;
    const DoubleNode<T>*   b = other.get_head();
    while (a && b) {
        if (a->data <= b->data) { result.insert_at_end(a->data, false); a = a->next; }
        else                    { result.insert_at_end(b->data, false); b = b->next; }
    }
    while (a) { result.insert_at_end(a->data, false); a = a->next; }
    while (b) { result.insert_at_end(b->data, false); b = b->next; }
    return result;
}