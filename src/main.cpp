#include <QApplication>
#include <QIcon>
#include <QString>
#include <iostream>

#include "ui/widget.h"
#include "ui/MenuManager.h"

int main(int argc, char* argv[]) {

    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == "--console") {
            std::cout << "Linky  —  console mode\n";
            MenuManager menu;
            menu.run();
            return 0;
        }
    }

    QApplication app(argc, argv);
    app.setApplicationName("Linky");
    app.setApplicationVersion("1.0");
    app.setWindowIcon(QIcon(":/images/resources/logo.png"));
    Widget window;
    window.setWindowIcon(QIcon(":/images/resources/logo.png"));
    window.show();
    return app.exec();
}