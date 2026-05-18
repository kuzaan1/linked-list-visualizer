#include "ListController.h"
#include "ui/InputHandler.h"
#include <iostream>
#include <string>
#include <utility>

static void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

static void print_header(const std::string& title) {
    std::cout << "\n╔══════════════════════════════════╗\n"
              << "║  " << title;
    int pad = 32 - static_cast<int>(title.size());
    for (int i = 0; i < pad; ++i) std::cout << ' ';
    std::cout << "║\n"
              << "╚══════════════════════════════════╝\n";
}

template <typename T>
T ListController<T>::prompt_value(const std::string& prompt) {
    std::cout << "  " << prompt;
    return InputHandler::getValue<T>();
}

template <typename T>
void ListController<T>::print_stats_sll() {
    std::cout << "  ┌─ List info ─────────────────────────\n"
              << "  │  Size : " << sll.get_size() << "\n";
    if (sll.get_size() > 0)
        std::cout << "  │  Max  : " << sll.get_max() << "\n"
                  << "  │  Min  : " << sll.get_min() << "\n";
    std::cout << "  └─────────────────────────────────────\n";
}

template <typename T>
void ListController<T>::print_stats_dll() {
    std::cout << "  ┌─ List info ─────────────────────────\n"
              << "  │  Size : " << dll.get_size() << "\n";
    if (dll.get_size() > 0)
        std::cout << "  │  Max  : " << dll.get_max() << "\n"
                  << "  │  Min  : " << dll.get_min() << "\n";
    std::cout << "  └─────────────────────────────────────\n";
}

template <typename T>
void ListController<T>::run_sll() { sll_page1(); }

template <typename T>
void ListController<T>::run_dll() { dll_page1(); }

template <typename T>
void ListController<T>::sll_page1() {
    bool back = false;
    while (!back) {
        print_header("SLL  —  Insert / Delete");
        print_stats_sll();
        std::cout
            << "\n  INSERT\n"
            << "  1  Insert at first\n"
            << "  2  Insert at end\n"
            << "  3  Insert at position\n"
            << "\n  DELETE\n"
            << "  4  Delete at first\n"
            << "  5  Delete at end\n"
            << "  6  Delete at position\n"
            << "\n  7  Next page\n"
            << "  0  Back\n"
            << "\n  Choice: ";

        int choice = InputHandler::getInt();
        clear_screen();

        switch (choice) {
        case 1: { T v = prompt_value("Value to insert: "); sll.insert_at_first(v);  break; }
        case 2: { T v = prompt_value("Value to insert: "); sll.insert_at_end(v);    break; }
        case 3: {
            T v = prompt_value("Value to insert: ");
            std::cout << "  Position: ";
            sll.insert_at_position(InputHandler::getInt(), v);
            break;
        }
        case 4: sll.delete_at_first(); break;
        case 5: sll.delete_at_end();   break;
        case 6: {
            std::cout << "  Position to delete: ";
            sll.delete_at_position(InputHandler::getInt());
            break;
        }
        case 7: sll_page2(); break;
        case 0: back = true; break;
        default: std::cout << "  Invalid choice.\n";
        }
    }
}

template <typename T>
void ListController<T>::sll_page2() {
    bool back = false;
    while (!back) {
        print_header("SLL  —  Query / Algorithms");
        print_stats_sll();
        std::cout
            << "\n  QUERY\n"
            << "  1  Search for value\n"
            << "  2  Get element at position\n"
            << "  3  Get first element\n"
            << "  4  Get middle element\n"
            << "  5  Get last element\n"
            << "  6  Size / Max / Min\n"
            << "  7  Print list\n"
            << "\n  ALGORITHMS\n"
            << "  8  Reverse list\n"
            << "  9  Check palindrome\n"
            << "  M  Merge with another list\n"
            << "\n  0  Back\n"
            << "\n  Choice: ";

        std::string raw;
        std::getline(std::cin, raw);
        if (raw.empty()) continue;
        clear_screen();

        char key = static_cast<char>(std::toupper(static_cast<unsigned char>(raw[0])));

        if (key == 'M') {
            SingleLinkedList<T> list2;
            std::cout << "  Number of elements in second list: ";
            int n = InputHandler::getInt();
            for (int i = 0; i < n; ++i) {
                T v = prompt_value("  Value " + std::to_string(i + 1) + ": ");
                list2.insert_at_end(v, false);
            }
            SingleLinkedList<T> merged = sll.merge(list2);
            std::cout << "\n  Merged result:";
            merged.print();
            std::cout << "\n  Save as current list? (1 = Yes / 2 = No): ";
            if (InputHandler::getInt() == 1) {
                sll = std::move(merged);
                std::cout << "  Saved.\n";
            }
            continue;
        }

        int choice = 0;
        if (key >= '0' && key <= '9') choice = key - '0';
        else { std::cout << "  Invalid choice.\n"; continue; }

        switch (choice) {
        case 1: {
            T v = prompt_value("Value to search: ");
            if (!sll.get_head()) { std::cout << "  List is empty.\n"; break; }
            std::cout << (sll.search(v) ? "  Found.\n" : "  Not found.\n");
            break;
        }
        case 2: {
            std::cout << "  Position (0-based): ";
            int pos = InputHandler::getInt();
            if (!sll.get_head())                   { std::cout << "  List is empty.\n";         break; }
            if (pos < 0 || pos >= sll.get_size())  { std::cout << "  Position out of range.\n"; break; }
            std::cout << "  Element at [" << pos << "] = " << sll.get_item(pos) << "\n";
            break;
        }
        case 3:
            if (!sll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  First = " << sll.get_first() << "\n";
            break;
        case 4:
            if (!sll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  Middle = " << sll.get_middle() << "\n";
            break;
        case 5:
            if (!sll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  Last = " << sll.get_last() << "\n";
            break;
        case 6:
            std::cout << "  Size = " << sll.get_size() << "\n";
            if (sll.get_size() > 0)
                std::cout << "  Max  = " << sll.get_max() << "\n"
                          << "  Min  = " << sll.get_min() << "\n";
            break;
        case 7: sll.print(); break;
        case 8: {
            SingleNode<T>*& h = sll.get_head();
            if (!h) std::cout << "  List is empty.\n";
            else    sll.reverse(h);
            break;
        }
        case 9:
            std::cout << (sll.check_palindrome()
                              ? "  The list IS a palindrome.\n"
                              : "  The list is NOT a palindrome.\n");
            break;
        case 0: back = true; break;
        default: std::cout << "  Invalid choice.\n";
        }
    }
}

template <typename T>
void ListController<T>::dll_page1() {
    bool back = false;
    while (!back) {
        print_header("DLL  —  Insert / Delete");
        print_stats_dll();
        std::cout
            << "\n  INSERT\n"
            << "  1  Insert at first\n"
            << "  2  Insert at end\n"
            << "  3  Insert at position\n"
            << "\n  DELETE\n"
            << "  4  Delete at first\n"
            << "  5  Delete at end\n"
            << "  6  Delete at position\n"
            << "\n  7  Next page\n"
            << "  0  Back\n"
            << "\n  Choice: ";

        int choice = InputHandler::getInt();
        clear_screen();

        switch (choice) {
        case 1: { T v = prompt_value("Value to insert: "); dll.insert_at_first(v);  break; }
        case 2: { T v = prompt_value("Value to insert: "); dll.insert_at_end(v);    break; }
        case 3: {
            T v = prompt_value("Value to insert: ");
            std::cout << "  Position: ";
            dll.insert_at_position(InputHandler::getInt(), v);
            break;
        }
        case 4: dll.delete_at_first(); break;
        case 5: dll.delete_at_end();   break;
        case 6: {
            std::cout << "  Position to delete: ";
            dll.delete_at_position(InputHandler::getInt());
            break;
        }
        case 7: dll_page2(); break;
        case 0: back = true; break;
        default: std::cout << "  Invalid choice.\n";
        }
    }
}

template <typename T>
void ListController<T>::dll_page2() {
    bool back = false;
    while (!back) {
        print_header("DLL  —  Query / Algorithms");
        print_stats_dll();
        std::cout
            << "\n  QUERY\n"
            << "  1  Search for value\n"
            << "  2  Get element at position\n"
            << "  3  Get first element\n"
            << "  4  Get middle element\n"
            << "  5  Get last element\n"
            << "  6  Size / Max / Min\n"
            << "  7  Print list\n"
            << "\n  ALGORITHMS\n"
            << "  8  Reverse list\n"
            << "  9  Check palindrome\n"
            << "  M  Merge with another list\n"
            << "\n  0  Back\n"
            << "\n  Choice: ";

        std::string raw;
        std::getline(std::cin, raw);
        if (raw.empty()) continue;
        clear_screen();

        char key = static_cast<char>(std::toupper(static_cast<unsigned char>(raw[0])));

        if (key == 'M') {
            DoubleLinkedList<T> list2;
            std::cout << "  Number of elements in second list: ";
            int n = InputHandler::getInt();
            for (int i = 0; i < n; ++i) {
                T v = prompt_value("  Value " + std::to_string(i + 1) + ": ");
                list2.insert_at_end(v, false);
            }
            DoubleLinkedList<T> merged = dll.merge(list2);
            std::cout << "\n  Merged result:";
            merged.print();
            std::cout << "\n  Save as current list? (1 = Yes / 2 = No): ";
            if (InputHandler::getInt() == 1) {
                dll = std::move(merged);
                std::cout << "  Saved.\n";
            }
            continue;
        }

        int choice = 0;
        if (key >= '0' && key <= '9') choice = key - '0';
        else { std::cout << "  Invalid choice.\n"; continue; }

        switch (choice) {
        case 1: {
            T v = prompt_value("Value to search: ");
            if (!dll.get_head()) { std::cout << "  List is empty.\n"; break; }
            std::cout << (dll.search(v) ? "  Found.\n" : "  Not found.\n");
            break;
        }
        case 2: {
            std::cout << "  Position (0-based): ";
            int pos = InputHandler::getInt();
            if (!dll.get_head())                   { std::cout << "  List is empty.\n";         break; }
            if (pos < 0 || pos >= dll.get_size())  { std::cout << "  Position out of range.\n"; break; }
            std::cout << "  Element at [" << pos << "] = " << dll.get_item(pos) << "\n";
            break;
        }
        case 3:
            if (!dll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  First = " << dll.get_first() << "\n";
            break;
        case 4:
            if (!dll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  Middle = " << dll.get_middle() << "\n";
            break;
        case 5:
            if (!dll.get_head()) std::cout << "  List is empty.\n";
            else                 std::cout << "  Last = " << dll.get_last() << "\n";
            break;
        case 6:
            std::cout << "  Size = " << dll.get_size() << "\n";
            if (dll.get_size() > 0)
                std::cout << "  Max  = " << dll.get_max() << "\n"
                          << "  Min  = " << dll.get_min() << "\n";
            break;
        case 7: dll.print(); break;
        case 8: {
            DoubleNode<T>*& h = dll.get_head();
            if (!h) std::cout << "  List is empty.\n";
            else    dll.reverse(h);
            break;
        }
        case 9:
            std::cout << (dll.check_palindrome()
                              ? "  The list IS a palindrome.\n"
                              : "  The list is NOT a palindrome.\n");
            break;
        case 0: back = true; break;
        default: std::cout << "  Invalid choice.\n";
        }
    }
}