#include "MenuManager.h"
#include "controller/ListController.h"
#include "InputHandler.h"
#include <iostream>
#include <string>

static void cls() {
    std::cout << "\033[2J\033[1;1H";
}


void MenuManager::run() {
    bool running = true;
    while (running) {
        running = main_menu();
    }
    std::cout << "\nGoodbye!\n";
}

bool MenuManager::main_menu() {
    while (true) {
        std::cout
            << "\n╔══════════════════════════════╗\n"
            << "║      Welcome to  Linky       ║\n"
            << "╠══════════════════════════════╣\n"
            << "║  1  Singly Linked List (SLL) ║\n"
            << "║  2  Doubly Linked List (DLL) ║\n"
            << "║  0  Exit                     ║\n"
            << "╚══════════════════════════════╝\n"
            << "  Choice: ";

        switch (InputHandler::getInt()) {
        case 1: cls(); sll_type_menu(); break;
        case 2: cls(); dll_type_menu(); break;
        case 0: return false;
        default: std::cout << "  Invalid choice — try again.\n";
        }
    }
}

void MenuManager::sll_type_menu() {
    while (true) {
        std::cout
            << "\n  Singly Linked List — select data type\n"
            << "  ──────────────────────────────────────\n"
            << "  1  int\n"
            << "  2  string\n"
            << "  3  char\n"
            << "  0  Back\n"
            << "  Choice: ";

        int choice = InputHandler::getInt();
        cls();

        switch (choice) {
        case 1: { ListController<int>         c(ListController<int>::ListType::SLL);         c.run_sll(); break; }
        case 2: { ListController<std::string> c(ListController<std::string>::ListType::SLL); c.run_sll(); break; }
        case 3: { ListController<char>        c(ListController<char>::ListType::SLL);        c.run_sll(); break; }
        case 0: return;
        default: std::cout << "  Invalid choice — try again.\n";
        }
    }
}

void MenuManager::dll_type_menu() {
    while (true) {
        std::cout
            << "\n  Doubly Linked List — select data type\n"
            << "  ──────────────────────────────────────\n"
            << "  1  int\n"
            << "  2  string\n"
            << "  3  char\n"
            << "  0  Back\n"
            << "  Choice: ";

        int choice = InputHandler::getInt();
        cls();

        switch (choice) {
        case 1: { ListController<int>         c(ListController<int>::ListType::DLL);         c.run_dll(); break; }
        case 2: { ListController<std::string> c(ListController<std::string>::ListType::DLL); c.run_dll(); break; }
        case 3: { ListController<char>        c(ListController<char>::ListType::DLL);        c.run_dll(); break; }
        case 0: return;
        default: std::cout << "  Invalid choice — try again.\n";
        }
    }
}