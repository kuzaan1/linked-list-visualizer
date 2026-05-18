#include "SingleLinkedList.h"
#include <iostream>
#include <utility>

template <typename T>
void SingleLinkedList<T>::_zero() noexcept {
    head = tail = nullptr;
    size = 0;
    is_empty = true;
    max_val = min_val = T();
    max_count = min_count = 0;
}

template <typename T>
void SingleLinkedList<T>::_copy_from(const SingleLinkedList<T>& src) {
    for (const SingleNode<T>* c = src.head; c; c = c->next)
        insert_at_end(c->data, false);
}

template <typename T>
SingleLinkedList<T>::SingleLinkedList() { _zero(); }

template <typename T>
SingleLinkedList<T>::~SingleLinkedList() { destroy(); }

template <typename T>
SingleLinkedList<T>::SingleLinkedList(const SingleLinkedList<T>& other) {
    _zero();
    _copy_from(other);
}

template <typename T>
SingleLinkedList<T>& SingleLinkedList<T>::operator=(const SingleLinkedList<T>& other) {
    if (this != &other) { destroy(); _copy_from(other); }
    return *this;
}

template <typename T>
SingleLinkedList<T>::SingleLinkedList(SingleLinkedList<T>&& other) noexcept {
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
SingleLinkedList<T>& SingleLinkedList<T>::operator=(SingleLinkedList<T>&& other) noexcept {
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
void SingleLinkedList<T>::destroy() {
    SingleNode<T>* cur = head;
    while (cur) {
        SingleNode<T>* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    _zero();
}

template <typename T>
void SingleLinkedList<T>::scan_min_max() {
    if (!head) { max_count = min_count = 0; return; }
    max_val = min_val = head->data;
    max_count = min_count = 1;
    for (SingleNode<T>* c = head->next; c; c = c->next) {
        if      (c->data > max_val)  { max_val = c->data; max_count = 1; }
        else if (c->data == max_val) { ++max_count; }
        if      (c->data < min_val)  { min_val = c->data; min_count = 1; }
        else if (c->data == min_val) { ++min_count; }
    }
}

template <typename T>
void SingleLinkedList<T>::on_insert(const T& value) {
    if (size == 0) { max_val = min_val = value; max_count = min_count = 1; return; }
    if      (value > max_val)  { max_val = value; max_count = 1; }
    else if (value == max_val) { ++max_count; }
    if      (value < min_val)  { min_val = value; min_count = 1; }
    else if (value == min_val) { ++min_count; }
}

template <typename T>
void SingleLinkedList<T>::on_remove(const T& value) {
    if (value == max_val) { --max_count; if (!max_count) scan_min_max(); }
    if (value == min_val) { --min_count; if (!min_count) scan_min_max(); }
}

template <typename T>
void SingleLinkedList<T>::reset() {
    if (!head) { tail = nullptr; is_empty = true; size = 0; max_count = min_count = 0; }
    else       { is_empty = false; }
}

template <typename T> void            SingleLinkedList<T>::update_min_max() { scan_min_max(); }
template <typename T> SingleNode<T>*& SingleLinkedList<T>::get_head()       { return head; }
template <typename T> SingleNode<T>*& SingleLinkedList<T>::get_tail()       { return tail; }
template <typename T> int             SingleLinkedList<T>::get_size()        { return size; }
template <typename T> bool            SingleLinkedList<T>::empty()           { return is_empty; }
template <typename T> T               SingleLinkedList<T>::get_max()         { return max_val; }
template <typename T> T               SingleLinkedList<T>::get_min()         { return min_val; }

template <typename T>
bool SingleLinkedList<T>::search(T value) {
    for (SingleNode<T>* c = head; c; c = c->next)
        if (c->data == value) return true;
    return false;
}

template <typename T>
T& SingleLinkedList<T>::get_item(int pos) {
    SingleNode<T>* c = head;
    for (int i = 0; i < pos; ++i) c = c->next;
    return c->data;
}

template <typename T>
T SingleLinkedList<T>::get_first() {
    if (!head) { std::cout << "\nList is empty.\n"; return T(); }
    return head->data;
}

template <typename T>
T SingleLinkedList<T>::get_last() {
    if (!tail) { std::cout << "\nList is empty.\n"; return T(); }
    return tail->data;
}

template <typename T>
T SingleLinkedList<T>::get_middle() {
    if (!head) { std::cout << "\nList is empty.\n"; return T(); }
    SingleNode<T>* slow = head, *fast = head;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    return slow->data;
}

template <typename T>
void SingleLinkedList<T>::insert_at_first(T value, bool show) {
    auto* node = new SingleNode<T>;
    node->data = value;
    node->next = head;
    if (!head) { head = tail = node; is_empty = false; }
    else { head = node; }
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::insert_at_end(T value, bool show) {
    if (!head) { insert_at_first(value, show); return; }
    auto* node = new SingleNode<T>;
    node->data = value;
    node->next = nullptr;
    tail->next = node;
    tail = node;
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::insert_at_position(int location, T value, bool show) {
    if (location < 1 || location > size + 1) {
        std::cout << "\nPosition out of range.\n"; return;
    }
    if (location == 1) { insert_at_first(value, show); return; }
    if (location == size + 1) { insert_at_end(value, show);   return; }

    SingleNode<T>* cur = head;
    for (int i = 1; i < location - 1; ++i) cur = cur->next;
    auto* node = new SingleNode<T>;
    node->data = value;
    node->next = cur->next;
    cur->next  = node;
    if (!node->next) tail = node;
    on_insert(value);
    ++size;
    if (show) std::cout << "\nItem added successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::delete_at_first(bool show) {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    auto*    dying = head;
    const T  bye   = dying->data;
    head = head->next;
    if (!head) tail = nullptr;
    delete dying;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::delete_at_end(bool show) {
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
    SingleNode<T>* cur = head;
    while (cur->next != tail) cur = cur->next;
    const T bye = tail->data;
    delete tail;
    tail       = cur;
    tail->next = nullptr;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::delete_at_position(int location, bool show) {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    if (location < 1 || location > size) {
        std::cout << "\nPosition out of range.\n"; return;
    }
    if (location == 1) { delete_at_first(show); return; }
    SingleNode<T>* pre = head, *cur = head->next;
    for (int i = 2; i < location; ++i) { pre = cur; cur = cur->next; }
    const T bye  = cur->data;
    pre->next    = cur->next;
    if (!cur->next) tail = pre;
    delete cur;
    on_remove(bye);
    --size;
    reset();
    if (show) std::cout << "\nItem deleted successfully.\n";
}

template <typename T>
void SingleLinkedList<T>::print() {
    if (!head) { std::cout << "\nList is empty.\n"; return; }
    std::cout << "\n[ ";
    for (SingleNode<T>* c = head; c; c = c->next) {
        std::cout << c->data;
        if (c->next) std::cout << " -> ";
    }
    std::cout << " -> null ]\n";
}

template <typename T>
void SingleLinkedList<T>::reverse(SingleNode<T>*& start, bool show) {
    if (!start) return;
    tail = start;
    SingleNode<T>* prev = nullptr, *curr = start, *nxt = nullptr;
    while (curr) { nxt = curr->next; curr->next = prev; prev = curr; curr = nxt; }
    start = prev;
    if (show) std::cout << "\nList reversed successfully.\n";
}

template <typename T>
bool SingleLinkedList<T>::check_palindrome() {
    if (!head || !head->next) return true;

    SingleNode<T>* slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    SingleNode<T>* second_start = slow->next;
    SingleNode<T>* real_tail    = tail;

    SingleNode<T>* rev = second_start;
    reverse(rev, false);

    SingleNode<T>* L = head, *R = rev;
    bool ok = true;
    while (R) {
        if (L->data != R->data) { ok = false; break; }
        L = L->next;
        R = R->next;
    }

    reverse(rev, false);
    slow->next = rev;
    tail       = real_tail;
    return ok;
}

template <typename T>
SingleLinkedList<T> SingleLinkedList<T>::merge(SingleLinkedList<T>& other) {
    SingleLinkedList<T>    result;
    const SingleNode<T>*   a = head;
    const SingleNode<T>*   b = other.get_head();
    while (a && b) {
        if (a->data <= b->data) { result.insert_at_end(a->data, false); a = a->next; }
        else                    { result.insert_at_end(b->data, false); b = b->next; }
    }
    while (a) { result.insert_at_end(a->data, false); a = a->next; }
    while (b) { result.insert_at_end(b->data, false); b = b->next; }
    return result;
}