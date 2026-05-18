#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QVector>
#include <QString>
#include <QScrollArea>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QFont>
#include <QSplitter>
#include <QDialog>
#include <QTime>
#include "core/SingleLinkedList.h"
#include "core/DoubleLinkedList.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    enum class ListKind { SLL, DLL };
    enum class DataType { Int, Str, Char };

    ListKind currentKind = ListKind::SLL;
    DataType currentType = DataType::Int;

    SingleLinkedList<int> sll_int;
    DoubleLinkedList<int> dll_int;
    SingleLinkedList<std::string> sll_str;
    DoubleLinkedList<std::string> dll_str;
    SingleLinkedList<char> sll_chr;
    DoubleLinkedList<char> dll_chr;

    QGraphicsScene* diagramScene = nullptr;
    QGraphicsView* diagramView = nullptr;
    QTextEdit* logBox = nullptr;
    QLabel* lblStats = nullptr;

    QComboBox* cbListKind = nullptr;
    QComboBox* cbDataType = nullptr;

    QLineEdit* leValue = nullptr;
    QSpinBox* sbPos = nullptr;

    QWidget* sllExtrasPanel = nullptr;
    QWidget* dllExtrasPanel = nullptr;

    void buildUI();
    QWidget* buildSidebar();
    QWidget* buildMainArea();

    QGroupBox* buildInputsGroup();
    QGroupBox* buildInsertGroup();
    QGroupBox* buildDeleteGroup();
    QGroupBox* buildQueryGroup();
    QGroupBox* buildExtrasGroup();

    QPushButton* makeBtn(const QString& label, const QString& col = "");

    void refreshDiagram();
    void refreshStats();
    void log(const QString& msg, bool ok = true);

    void op_insertFirst();
    void op_insertEnd();
    void op_insertAt();

    void op_deleteFirst();
    void op_deleteEnd();
    void op_deleteAt();

    void op_search();
    void op_getItem();
    void op_getFirst();
    void op_getLast();
    void op_getMiddle();
    void op_getMax();
    void op_getMin();
    void op_getSize();
    void op_print();

    void op_sll_reverse();
    void op_sll_palindrome();
    void op_sll_merge();

    void op_dll_reverse();
    void op_dll_palindrome();
    void op_dll_merge();

    template<typename T> T parseValue(bool& ok);
    void resetLists();
};

#endif