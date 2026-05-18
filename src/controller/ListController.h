#ifndef LIST_CONTROLLER_H
#define LIST_CONTROLLER_H

#include "core/SingleLinkedList.h"
#include "core/DoubleLinkedList.h"
#include <string>

template <typename T>
class ListController {
public:
    enum class ListType { SLL, DLL };

    explicit ListController(ListType type) : active_type(type) {}

    void run_sll();
    void run_dll();

private:
    ListType            active_type;
    SingleLinkedList<T> sll;
    DoubleLinkedList<T> dll;

    void sll_page1();
    void sll_page2();
    void dll_page1();
    void dll_page2();

    T    prompt_value(const std::string& prompt);
    void print_stats_sll();
    void print_stats_dll();
};

#include "controller/ListController.cpp"
#endif