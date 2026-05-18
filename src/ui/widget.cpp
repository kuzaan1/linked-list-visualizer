#include "widget.h"
#include <utility>

#include <QApplication>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QScrollBar>
#include <QDateTime>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QLinearGradient>

static const char* C_BG              = "#0b0f19";
static const char* C_PANEL           = "#0f1524";
static const char* C_PANEL2          = "#131c2e";
static const char* C_PANEL3          = "#0d1220";
static const char* C_BORDER          = "#1c2a42";
static const char* C_BORDER2         = "#243350";
static const char* C_ACCENT          = "#60a5fa";
static const char* C_ACCENT_DIM      = "#0d1f3c";
static const char* C_ACCENT2         = "#a78bfa";
static const char* C_ACCENT2_DIM     = "#1a1040";
static const char* C_OK              = "#34d399";
static const char* C_OK_DIM          = "#022c22";
static const char* C_ERR             = "#f87171";
static const char* C_ERR_DIM         = "#3b0a0a";
static const char* C_WARN            = "#fbbf24";
static const char* C_TEXT            = "#e2e8f0";
static const char* C_TEXT2           = "#8b9ab5";
static const char* C_MUTED           = "#4a5878";
static const char* C_NODE_SLL        = "#091930";
static const char* C_NODE_DLL        = "#110b2d";
static const char* C_NODE_BORDER_SLL = "#60a5fa";
static const char* C_NODE_BORDER_DLL = "#a78bfa";
static const char* C_ARROW           = "#3d5270";
static const char* C_HEAD_GLOW       = "#60a5fa";

static const int SIDEBAR_W     = 288;
static const int DIAGRAM_MIN_H = 280;
static const int LOG_MIN_H     = 160;

class ZoomableGraphicsView : public QGraphicsView {
public:
    explicit ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent = nullptr)
        : QGraphicsView(scene, parent), _zoom(1.0) {
        setTransformationAnchor(AnchorUnderMouse);
        setResizeAnchor(AnchorUnderMouse);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                       | QPainter::TextAntialiasing);
        viewport()->setMouseTracking(true);
    }
protected:
    void wheelEvent(QWheelEvent* e) override {
        double factor = (e->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
        _zoom = qBound(0.15, _zoom * factor, 8.0);
        scale(factor, factor);
        e->accept();
    }
    void keyPressEvent(QKeyEvent* e) override {
        if (e->key() == Qt::Key_0 && (e->modifiers() & Qt::ControlModifier)) {
            resetTransform(); _zoom = 1.0;
        } else {
            QGraphicsView::keyPressEvent(e);
        }
    }
private:
    double _zoom;
};

Widget::Widget(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Linky  ·  Linked List Visualiser");
    setMinimumSize(1080, 700);


    setStyleSheet(QString(
                      "QWidget {"
                      "  background:%1; color:%2;"
                      "  font-family:'JetBrains Mono','Fira Code','Cascadia Code','Consolas',monospace;"
                      "  font-size:11px;"
                      "}"
                      "QToolTip {"
                      "  background:%3; color:%2; border:1px solid %4;"
                      "  padding:5px 9px; border-radius:5px; font-size:10px;"
                      "}"
                      ).arg(C_BG).arg(C_TEXT).arg(C_PANEL2).arg(C_BORDER2));

    buildUI();
    refreshDiagram();
    refreshStats();
}

Widget::~Widget() {}

QPushButton* Widget::makeBtn(const QString& label, const QString& col)
{
    QPushButton* b = new QPushButton(label);

    QString bg, border, hover, pressed, txt;

    if (col == "ok") {
        bg = "#0b1f14";
        border = "#00ff7f";
        hover = "#00ff99";
        pressed = "#007f4f";
        txt = "#bfffe0";

    } else if (col == "err") {
        bg = "#1f0b0b";
        border = "#ff2e2e";
        hover = "#ff4d4d";
        pressed = "#8a0000";
        txt = "#ffd6d6";

    } else if (col == "hi") {
        bg = "#0b1220";
        border = "#00a6ff";
        hover = "#33b8ff";
        pressed = "#005c99";
        txt = "#cfeeff";

    } else if (col == "ex") {
        bg = "#120b1f";
        border = "#b026ff";
        hover = "#c44dff";
        pressed = "#5a0a8a";
        txt = "#f0d6ff";

    }
    b->setFixedHeight(22);

    b->setStyleSheet(QString(
                         "QPushButton {"
                         "  background:%1;"
                         "  color:%5;"
                         "  border:1px solid %2;"
                         "  border-radius:11px;"
                         "  padding:2px 10px;"
                         "  font-size:10px;"
                         "  font-family:'JetBrains Mono','Consolas',monospace;"
                         "  font-weight:600;"
                         "}"

                         "QPushButton:hover {"
                         "  background:%3;"
                         "  border-color:%3;"
                         "  color:white;"
                         "}"

                         "QPushButton:pressed {"
                         "  background:%4;"
                         "  border-color:%4;"
                         "}"
                         ).arg(bg).arg(border).arg(hover).arg(pressed).arg(txt));

    return b;
}

static QString groupBoxSS(const char* tc) {
    return QString(
               "QGroupBox {"
               "  border:1px solid #1c2a42; border-radius:8px;"
               "  margin-top:12px; padding-top:4px; padding-bottom:4px;"
               "  font-size:9px; font-weight:700; letter-spacing:2px; color:%1;"
               "  background:#0c1220;"
               "}"
               "QGroupBox::title {"
               "  subcontrol-origin:margin; left:10px;"
               "  padding:0 5px; color:%1; background:#0c1220;"
               "}"
               ).arg(tc);
}

static QString inputSS() {
    return QString(
        "QLineEdit, QSpinBox {"
        "  background:#07090f; color:#e2e8f0; border:1px solid #1c2a42;"
        "  border-radius:6px; padding:4px 10px; font-size:11px;"
        "}"
        "QLineEdit:focus { border-color:#60a5fa; }"
        "QSpinBox:focus  { border-color:#60a5fa; }"
        "QSpinBox::up-button, QSpinBox::down-button { width:14px; border:none; }"
        "QSpinBox::up-arrow, QSpinBox::down-arrow   { border:none; }"
        );
}

static QString comboSS() {
    return QString(
        "QComboBox {"
        "  background:#07090f; color:#e2e8f0; border:1px solid #1c2a42;"
        "  border-radius:6px; padding:5px 10px; font-size:11px;"
        "}"
        "QComboBox:focus { border-color:#60a5fa; }"
        "QComboBox::drop-down { border:none; width:18px; }"
        "QComboBox::down-arrow { image:none; }"
        "QComboBox QAbstractItemView {"
        "  background:#0f1524; color:#e2e8f0; border:1px solid #1c2a42;"
        "  selection-background-color:#0d1f3c; outline:none;"
        "}"
        );
}

void Widget::buildUI()
{
    QHBoxLayout* root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);
    splitter->setStyleSheet(QString("QSplitter::handle { background:%1; }").arg(C_BORDER));
    splitter->addWidget(buildSidebar());
    splitter->addWidget(buildMainArea());
    splitter->setSizes({SIDEBAR_W, 800});
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    root->addWidget(splitter);
}

QWidget* Widget::buildSidebar()
{
    QWidget* outer = new QWidget;
    outer->setFixedWidth(SIDEBAR_W);
    outer->setStyleSheet(QString("background:%1; border-right:1px solid %2;")
                             .arg(C_PANEL).arg(C_BORDER));


    QScrollArea* scroll = new QScrollArea(outer);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(
        "QScrollArea { border:none; background:transparent; }"
        "QScrollBar:vertical { background:#07090f; width:4px; border-radius:2px; }"
        "QScrollBar::handle:vertical { background:#1c2a42; border-radius:2px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
        );

    QWidget* inner = new QWidget;
    inner->setStyleSheet("background:transparent;");
    QVBoxLayout* lay = new QVBoxLayout(inner);
    lay->setContentsMargins(10, 10, 10, 12);
    lay->setSpacing(5);


    {
        QWidget* brand = new QWidget;
        brand->setFixedHeight(52);
        brand->setStyleSheet(QString(
                                 "background:qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                                 "stop:0 %1,stop:1 %2);"
                                 "border-radius:8px; border:1px solid %3;"
                                 ).arg(C_ACCENT_DIM).arg(C_ACCENT2_DIM).arg(C_BORDER2));

        QVBoxLayout* bl = new QVBoxLayout(brand);
        bl->setContentsMargins(12,6,12,6);
        bl->setSpacing(1);

        QLabel* t1 = new QLabel("⬡  LINKY");
        t1->setStyleSheet(QString(
                              "color:%1; font-size:15px; font-weight:800;"
                              "letter-spacing:5px; background:transparent; border:none;"
                              ).arg(C_ACCENT));
        bl->addWidget(t1);

        QLabel* t2 = new QLabel("Linked List Visualiser");
        t2->setStyleSheet(QString(
                              "color:%1; font-size:9px; letter-spacing:1px;"
                              "background:transparent; border:none;"
                              ).arg(C_TEXT2));
        bl->addWidget(t2);
        lay->addWidget(brand);
    }


    auto mkLabel = [&](const QString& t) -> QLabel* {
        QLabel* l = new QLabel(t);
        l->setFixedHeight(14);
        l->setStyleSheet(QString(
                             "color:%1; font-size:8px; font-weight:700;"
                             "letter-spacing:2px; background:transparent; border:none; margin-top:4px;"
                             ).arg(C_MUTED));
        return l;
    };


    lay->addWidget(mkLabel("LIST TYPE"));
    cbListKind = new QComboBox;
    cbListKind->addItems({"Singly Linked List  (SLL)", "Doubly Linked List  (DLL)"});
    cbListKind->setStyleSheet(comboSS());
    cbListKind->setFixedHeight(30);
    lay->addWidget(cbListKind);

    lay->addWidget(mkLabel("DATA TYPE"));
    cbDataType = new QComboBox;
    cbDataType->addItems({"int", "string", "char"});
    cbDataType->setStyleSheet(comboSS());
    cbDataType->setFixedHeight(30);
    lay->addWidget(cbDataType);


    lay->addWidget(buildInputsGroup());
    lay->addWidget(buildInsertGroup());
    lay->addWidget(buildDeleteGroup());
    lay->addWidget(buildQueryGroup());
    lay->addWidget(buildExtrasGroup());
    lay->addStretch();


    connect(cbListKind, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int idx){
                currentKind = (idx==0) ? ListKind::SLL : ListKind::DLL;
                sllExtrasPanel->setVisible(currentKind==ListKind::SLL);
                dllExtrasPanel->setVisible(currentKind==ListKind::DLL);
                refreshDiagram(); refreshStats();
            });

    connect(cbDataType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int){
                currentType = static_cast<DataType>(cbDataType->currentIndex());
                resetLists(); refreshDiagram(); refreshStats();
                log("Lists cleared — data type changed.", true);
            });

    scroll->setWidget(inner);

    QVBoxLayout* outerLay = new QVBoxLayout(outer);
    outerLay->setContentsMargins(0,0,0,0);
    outerLay->addWidget(scroll);
    return outer;
}

QGroupBox* Widget::buildInputsGroup()
{
    QGroupBox* grp = new QGroupBox("PARAMETERS");
    grp->setStyleSheet(groupBoxSS(C_TEXT2));
    QVBoxLayout* lay = new QVBoxLayout(grp);
    lay->setSpacing(3); lay->setContentsMargins(8,4,8,6);

    auto hint = [&](const QString& t) -> QLabel* {
        QLabel* l = new QLabel(t);
        l->setFixedHeight(12);
        l->setStyleSheet(QString(
                             "color:%1; font-size:8px; background:transparent; border:none;"
                             ).arg(C_MUTED));
        return l;
    };

    lay->addWidget(hint("Value  (insert / search / merge)"));
    leValue = new QLineEdit;
    leValue->setPlaceholderText("e.g.  42 / hello / a / 1,3,5");
    leValue->setStyleSheet(inputSS());
    leValue->setFixedHeight(27);
    lay->addWidget(leValue);

    lay->addWidget(hint("Position  (1-based)"));
    sbPos = new QSpinBox;
    sbPos->setRange(1, 9999);
    sbPos->setStyleSheet(inputSS());
    sbPos->setFixedHeight(27);
    lay->addWidget(sbPos);

    return grp;
}

QGroupBox* Widget::buildInsertGroup()
{
    QGroupBox* grp = new QGroupBox("INSERT");
    grp->setStyleSheet(groupBoxSS(C_OK));
    QGridLayout* grid = new QGridLayout(grp);
    grid->setSpacing(4); grid->setContentsMargins(8,4,8,6);

    auto mk = [&](const QString& l, const QString& tip) -> QPushButton* {
        auto* b = makeBtn(l, "ok"); b->setFixedHeight(28); b->setToolTip(tip); return b;
    };

    grid->addWidget(mk("⊕  First",  "Insert value at the head of the list."),  0, 0);
    grid->addWidget(mk("⊕  End",    "Insert value at the tail of the list."),  0, 1);
    grid->addWidget(mk("⊕  At Pos", "Insert value at the given position."),    1, 0, 1, 2);

    auto btns = grp->findChildren<QPushButton*>();
    connect(btns[0], &QPushButton::clicked, this, &Widget::op_insertFirst);
    connect(btns[1], &QPushButton::clicked, this, &Widget::op_insertEnd);
    connect(btns[2], &QPushButton::clicked, this, &Widget::op_insertAt);

    return grp;
}

QGroupBox* Widget::buildDeleteGroup()
{
    QGroupBox* grp = new QGroupBox("DELETE");
    grp->setStyleSheet(groupBoxSS(C_ERR));

    QVBoxLayout* vlay = new QVBoxLayout(grp);
    vlay->setSpacing(4);
    vlay->setContentsMargins(8,4,8,6);

    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(4);

    auto mk = [&](const QString& l, const QString& tip) -> QPushButton* {
        auto* b = makeBtn(l, "err");
        b->setFixedHeight(28);
        b->setToolTip(tip);
        return b;
    };

    auto b1 = mk("⊖  First",  "Remove the head element.");
    auto b2 = mk("⊖  End",    "Remove the tail element.");
    auto b3 = mk("⊖  At Pos", "Remove element at the given position.");

    grid->addWidget(b1, 0, 0);
    grid->addWidget(b2, 0, 1);
    grid->addWidget(b3, 1, 0, 1, 2);
    vlay->addLayout(grid);


    auto bDest = makeBtn("⊗  Destroy Entire List",
                         "err");

    bDest->setFixedHeight(22);
    bDest->setToolTip("Free all nodes and reset the list to empty.");

    vlay->addWidget(bDest);


    connect(b1, &QPushButton::clicked, this, &Widget::op_deleteFirst);
    connect(b2, &QPushButton::clicked, this, &Widget::op_deleteEnd);
    connect(b3, &QPushButton::clicked, this, &Widget::op_deleteAt);

    connect(bDest, &QPushButton::clicked, this, [this]{

        int nodeCount = 0;

        if (currentKind==ListKind::SLL) {
            if      (currentType==DataType::Int) nodeCount=sll_int.get_size();
            else if (currentType==DataType::Str) nodeCount=sll_str.get_size();
            else                                 nodeCount=sll_chr.get_size();
        } else {
            if      (currentType==DataType::Int) nodeCount=dll_int.get_size();
            else if (currentType==DataType::Str) nodeCount=dll_str.get_size();
            else                                 nodeCount=dll_chr.get_size();
        }

        QDialog dlg(this);
        dlg.setWindowTitle("Destroy List");
        dlg.setFixedWidth(380);

        dlg.setStyleSheet(QString(
                              "QDialog{background:%1;border:1px solid %2;border-radius:10px;}"
                              "QLabel{background:transparent;border:none;}"
                              "QPushButton{border-radius:6px;padding:7px 18px;font-size:10px;font-weight:600;}"
                              ).arg(C_PANEL2).arg(C_BORDER2));

        QVBoxLayout* vl = new QVBoxLayout(&dlg);
        vl->setContentsMargins(24,22,24,20);
        vl->setSpacing(14);

        QHBoxLayout* tr = new QHBoxLayout;
        tr->setSpacing(10);

        QLabel* ico = new QLabel("⚠");
        ico->setStyleSheet(QString("color:%1;font-size:22px;").arg(C_ERR));

        QLabel* ttl = new QLabel("Destroy Entire List?");
        ttl->setStyleSheet(QString("color:%1;font-size:13px;font-weight:700;").arg(C_TEXT));

        tr->addWidget(ico);
        tr->addWidget(ttl);
        tr->addStretch();
        vl->addLayout(tr);

        QLabel* det = new QLabel(QString(
                                     "This will permanently free all <b style='color:%1'>%2 node(s)</b> "
                                     "and reset the list to <b style='color:%3'>empty</b>.<br>"
                                     "This action <b>cannot be undone</b>."
                                     ).arg(C_ERR).arg(nodeCount).arg(C_WARN));

        det->setWordWrap(true);
        det->setStyleSheet(QString("color:%1;font-size:10px;line-height:160%;").arg(C_TEXT2));
        vl->addWidget(det);

        QFrame* sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet(QString("color:%1;").arg(C_BORDER));
        vl->addWidget(sep);

        QHBoxLayout* btns = new QHBoxLayout;
        btns->setSpacing(8);
        btns->addStretch();

        QPushButton* btnCancel = new QPushButton("Cancel");
        btnCancel->setStyleSheet(QString(
                                     "QPushButton{background:%1;color:%2;border:1px solid %3;border-radius:6px;}"
                                     "QPushButton:hover{background:#182236;border-color:%2;}"
                                     ).arg(C_PANEL2).arg(C_TEXT2).arg(C_BORDER2));

        QPushButton* btnOk = new QPushButton("⊗  Yes, Destroy");
        btnOk->setStyleSheet(QString(
                                 "QPushButton{background:%1;color:%2;border:1px solid %3;border-radius:6px;}"
                                 "QPushButton:hover{background:#5a0e0e;}"
                                 ).arg(C_ERR_DIM).arg(C_ERR).arg(C_ERR));

        connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::reject);
        connect(btnOk,&QPushButton::clicked,&dlg,&QDialog::accept);

        btns->addWidget(btnCancel);
        btns->addWidget(btnOk);
        vl->addLayout(btns);

        if (dlg.exec() != QDialog::Accepted) {
            log("Destroy cancelled.", true);
            return;
        }

        if (currentKind==ListKind::SLL) {
            if      (currentType==DataType::Int) sll_int.destroy();
            else if (currentType==DataType::Str) sll_str.destroy();
            else                                 sll_chr.destroy();
        } else {
            if      (currentType==DataType::Int) dll_int.destroy();
            else if (currentType==DataType::Str) dll_str.destroy();
            else                                 dll_chr.destroy();
        }

        log(QString("✔ List destroyed — %1 node(s) freed.").arg(nodeCount), true);
        refreshDiagram();
        refreshStats();
    });

    return grp;
}

QGroupBox* Widget::buildQueryGroup()
{
    QGroupBox* grp = new QGroupBox("QUERY");
    grp->setStyleSheet(groupBoxSS(C_ACCENT));
    QGridLayout* grid = new QGridLayout(grp);
    grid->setSpacing(4); grid->setContentsMargins(8,4,8,6);

    auto mk = [&](const QString& l, const QString& tip) -> QPushButton* {
        auto* b = makeBtn(l, "hi"); b->setFixedHeight(28); b->setToolTip(tip); return b;
    };

    grid->addWidget(mk("⌕  Search",   "Search for the value in the list."),    0,0);
    grid->addWidget(mk("⌕  At Pos",   "Get element at the given position."),   0,1);
    grid->addWidget(mk("⌕  Size",     "Get the current element count."),       0,2);
    grid->addWidget(mk("⌕  First",    "Get the head element."),                1,0);
    grid->addWidget(mk("⌕  Last",     "Get the tail element."),                1,1);
    grid->addWidget(mk("⌕  Middle",   "Middle via slow/fast pointer."),        1,2);
    grid->addWidget(mk("⌕  Max",      "Maximum value (int lists only)."),      2,0);
    grid->addWidget(mk("⌕  Min",      "Minimum value (int lists only)."),      2,1);
    grid->addWidget(mk("⌕  Print All","Dump all elements to the log."),        2,2);

    auto btns = grp->findChildren<QPushButton*>();
    connect(btns[0],&QPushButton::clicked,this,&Widget::op_search);
    connect(btns[1],&QPushButton::clicked,this,&Widget::op_getItem);
    connect(btns[2],&QPushButton::clicked,this,&Widget::op_getSize);
    connect(btns[3],&QPushButton::clicked,this,&Widget::op_getFirst);
    connect(btns[4],&QPushButton::clicked,this,&Widget::op_getLast);
    connect(btns[5],&QPushButton::clicked,this,&Widget::op_getMiddle);
    connect(btns[6],&QPushButton::clicked,this,&Widget::op_getMax);
    connect(btns[7],&QPushButton::clicked,this,&Widget::op_getMin);
    connect(btns[8],&QPushButton::clicked,this,&Widget::op_print);
    return grp;
}

QGroupBox* Widget::buildExtrasGroup()
{
    QGroupBox* grp = new QGroupBox("EXTRAS");
    grp->setStyleSheet(groupBoxSS(C_ACCENT2));
    QVBoxLayout* lay = new QVBoxLayout(grp);
    lay->setSpacing(4); lay->setContentsMargins(8,4,8,6);

    auto mkE = [&](const QString& l, const QString& tip) -> QPushButton* {
        auto* b = makeBtn(l, "ex"); b->setFixedHeight(28); b->setToolTip(tip); return b;
    };


    sllExtrasPanel = new QWidget;
    sllExtrasPanel->setStyleSheet("background:transparent;");
    QGridLayout* sg = new QGridLayout(sllExtrasPanel);
    sg->setSpacing(4); sg->setContentsMargins(0,0,0,0);
    sg->addWidget(mkE("↺  Reverse",     "Reverse SLL in-place."),               0,0);
    sg->addWidget(mkE("⇔  Palindrome?", "Check if SLL is a palindrome."),       0,1);
    sg->addWidget(mkE("⊞  Merge",       "Merge sorted SLLs. Value: 2,4,6"),     1,0,1,2);


    dllExtrasPanel = new QWidget;
    dllExtrasPanel->setStyleSheet("background:transparent;");
    QGridLayout* dg = new QGridLayout(dllExtrasPanel);
    dg->setSpacing(4); dg->setContentsMargins(0,0,0,0);
    dg->addWidget(mkE("↺  Reverse",     "Reverse DLL in-place."),               0,0);
    dg->addWidget(mkE("⇔  Palindrome?", "Check if DLL is a palindrome."),       0,1);
    dg->addWidget(mkE("⊞  Merge",       "Merge sorted DLLs. Value: 1,3,5"),     1,0,1,2);

    dllExtrasPanel->hide();
    lay->addWidget(sllExtrasPanel);
    lay->addWidget(dllExtrasPanel);

    auto sB = sllExtrasPanel->findChildren<QPushButton*>();
    connect(sB[0],&QPushButton::clicked,this,&Widget::op_sll_reverse);
    connect(sB[1],&QPushButton::clicked,this,&Widget::op_sll_palindrome);
    connect(sB[2],&QPushButton::clicked,this,&Widget::op_sll_merge);

    auto dB = dllExtrasPanel->findChildren<QPushButton*>();
    connect(dB[0],&QPushButton::clicked,this,&Widget::op_dll_reverse);
    connect(dB[1],&QPushButton::clicked,this,&Widget::op_dll_palindrome);
    connect(dB[2],&QPushButton::clicked,this,&Widget::op_dll_merge);

    return grp;
}

QWidget* Widget::buildMainArea()
{
    QWidget* area = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(area);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);


    {
        QWidget* hdr = new QWidget;
        hdr->setFixedHeight(48);
        hdr->setStyleSheet(QString(
                               "background:%1; border-bottom:1px solid %2;"
                               ).arg(C_PANEL).arg(C_BORDER));
        QHBoxLayout* hl = new QHBoxLayout(hdr);
        hl->setContentsMargins(20,0,16,0);

        QLabel* dot = new QLabel("◈");
        dot->setStyleSheet(QString("color:%1;font-size:15px;").arg(C_ACCENT));
        hl->addWidget(dot);

        QLabel* t = new QLabel("Node Diagram");
        t->setStyleSheet(QString(
                             "color:%1;font-size:13px;font-weight:700;margin-left:7px;"
                             ).arg(C_TEXT));
        hl->addWidget(t);

        QLabel* hint = new QLabel("  drag=pan   scroll=zoom   Ctrl+0=reset");
        hint->setStyleSheet(QString("color:%1;font-size:9px;").arg(C_MUTED));
        hl->addWidget(hint);
        hl->addStretch();

        lblStats = new QLabel("size=0   max=—   min=—");
        lblStats->setStyleSheet(QString(
                                    "color:%1;font-size:10px;background:%2;border:1px solid %3;"
                                    "border-radius:5px;padding:3px 11px;"
                                    ).arg(C_ACCENT).arg(C_ACCENT_DIM).arg(C_BORDER2));
        hl->addWidget(lblStats);
        lay->addWidget(hdr);
    }


    diagramScene = new QGraphicsScene(this);
    diagramScene->setBackgroundBrush(QBrush(QColor(C_BG)));

    auto* dv = new ZoomableGraphicsView(diagramScene);
    dv->setMinimumHeight(DIAGRAM_MIN_H);
    dv->setStyleSheet(QString(
                          "background:%1; border:none; border-bottom:1px solid %2;"
                          ).arg(C_BG).arg(C_BORDER));

    QString sbSS = QString(
                       "QScrollBar:horizontal{background:%1;height:5px;border-radius:2px;}"
                       "QScrollBar::handle:horizontal{background:%2;border-radius:2px;}"
                       "QScrollBar::handle:horizontal:hover{background:%3;}"
                       "QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal{width:0;}"
                       ).arg(C_BG).arg(C_BORDER2).arg(C_ACCENT);
    dv->horizontalScrollBar()->setStyleSheet(sbSS);
    dv->verticalScrollBar()->setStyleSheet(sbSS.replace("horizontal","vertical").replace("width:0","height:0").replace("height:5px","width:5px"));

    diagramView = dv;
    lay->addWidget(diagramView, 3);


    {
        QWidget* hdr = new QWidget;
        hdr->setFixedHeight(42);
        hdr->setStyleSheet(QString(
                               "background:%1; border-bottom:1px solid %2;"
                               ).arg(C_PANEL).arg(C_BORDER));
        QHBoxLayout* hl = new QHBoxLayout(hdr);
        hl->setContentsMargins(20,0,12,0);

        QLabel* dot = new QLabel("◈");
        dot->setStyleSheet(QString("color:%1;font-size:13px;").arg(C_ACCENT2));
        hl->addWidget(dot);

        QLabel* t = new QLabel("Operation Log");
        t->setStyleSheet(QString(
                             "color:%1;font-size:13px;font-weight:700;margin-left:7px;"
                             ).arg(C_TEXT));
        hl->addWidget(t);

        QLabel* sub = new QLabel("  results appear here");
        sub->setStyleSheet(QString("color:%1;font-size:9px;").arg(C_MUTED));
        hl->addWidget(sub);
        hl->addStretch();

        QPushButton* btnClear = makeBtn("⌫  Clear");
        btnClear->setFixedWidth(76); btnClear->setFixedHeight(26);
        btnClear->setToolTip("Clear the operation log.");
        connect(btnClear,&QPushButton::clicked,this,[this]{ logBox->clear(); });
        hl->addWidget(btnClear);
        lay->addWidget(hdr);
    }


    logBox = new QTextEdit;
    logBox->setReadOnly(true);
    logBox->setMinimumHeight(LOG_MIN_H);
    logBox->setStyleSheet(QString(
                              "QTextEdit{"
                              "  background:%1; color:%2; border:none;"
                              "  padding:12px 18px; font-size:11px;"
                              "  font-family:'JetBrains Mono','Fira Code','Consolas',monospace;"
                              "}"
                              "QScrollBar:vertical{background:%3;width:4px;border-radius:2px;}"
                              "QScrollBar::handle:vertical{background:%4;border-radius:2px;}"
                              "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}"
                              ).arg(C_BG).arg(C_TEXT).arg(C_BG).arg(C_BORDER2));

    lay->addWidget(logBox, 1);
    return area;
}

void Widget::refreshDiagram()
{
    diagramScene->clear();

    const int NODE_MIN_W  = 80;
    const int NODE_H      = 52;
    const int CORNER      = 9;
    const int H_PAD       = 18;
    const int ARROW_GAP   = 44;
    const int Y           = 90;
    const int AY_NEXT     = Y + NODE_H/2 - 5;
    const int AY_PREV     = Y + NODE_H/2 + 7;
    const int AY          = Y + NODE_H/2;
    const int NULL_W      = 38;
    const int NULL_GAP    = 36;

    QFont dataFont("Consolas", 13);
    bool isDLL = (currentKind == ListKind::DLL);
    const int PAD_L = isDLL ? (NULL_GAP + NULL_W + 16) : 30;

    QColor nodeBg    (isDLL ? C_NODE_DLL        : C_NODE_SLL);
    QColor nodeBorder(isDLL ? C_NODE_BORDER_DLL : C_NODE_BORDER_SLL);
    QColor glowCol   (isDLL ? C_ACCENT2         : C_HEAD_GLOW);

    QVector<QString> items;
    if (currentType==DataType::Int) {
        if (isDLL) for(auto*c=dll_int.get_head();c;c=c->next) items<<QString::number(c->data);
        else       for(auto*c=sll_int.get_head();c;c=c->next) items<<QString::number(c->data);
    } else if (currentType==DataType::Str) {
        if (isDLL) for(auto*c=dll_str.get_head();c;c=c->next) items<<QString::fromStdString(c->data);
        else       for(auto*c=sll_str.get_head();c;c=c->next) items<<QString::fromStdString(c->data);
    } else {
        if (isDLL) for(auto*c=dll_chr.get_head();c;c=c->next) items<<QString(c->data);
        else       for(auto*c=sll_chr.get_head();c;c=c->next) items<<QString(c->data);
    }

    if (items.isEmpty()) {
        QPainterPath p; p.addRoundedRect(PAD_L,Y,NODE_MIN_W,NODE_H,CORNER,CORNER);
        diagramScene->addPath(p,QPen(QColor(C_BORDER2),1.5,Qt::DashLine),QBrush(Qt::transparent));
        auto* lbl=diagramScene->addText("empty list");
        lbl->setDefaultTextColor(QColor(C_MUTED));
        lbl->setFont(QFont("Consolas",10));
        QRectF br=lbl->boundingRect();
        lbl->setPos(PAD_L+(NODE_MIN_W-br.width())/2,Y+(NODE_H-br.height())/2);
        diagramScene->setSceneRect(0,0,300,180);
        diagramView->fitInView(diagramScene->sceneRect(),Qt::KeepAspectRatio);
        return;
    }

    QFontMetrics fm(dataFont);
    QVector<int> nodeWidths(items.size()), xOffsets(items.size());
    { int cx=PAD_L; for(int i=0;i<items.size();++i){
            nodeWidths[i]=qMax(NODE_MIN_W,fm.horizontalAdvance(items[i])+2*H_PAD);
            xOffsets[i]=cx; cx+=nodeWidths[i]+ARROW_GAP;
        }}

    int sceneW=xOffsets.last()+nodeWidths.last()+NULL_GAP+NULL_W+30;
    diagramScene->setSceneRect(0,0,qMax(sceneW,500),Y+NODE_H+40);


    int headCX=xOffsets[0]+nodeWidths[0]/2;
    diagramScene->addLine(headCX,22,headCX,Y-2,QPen(QColor(glowCol),1.5));
    { QPolygonF a; a<<QPointF(headCX,Y-2)<<QPointF(headCX-5,Y-10)<<QPointF(headCX+5,Y-10);
        diagramScene->addPolygon(a,QPen(Qt::NoPen),QBrush(QColor(glowCol))); }
    auto* hl=diagramScene->addText("HEAD");
    hl->setDefaultTextColor(QColor(glowCol));
    hl->setFont(QFont("Consolas",9,QFont::Bold));
    hl->setPos(headCX-hl->boundingRect().width()/2,4);


    int tailCX=xOffsets.last()+nodeWidths.last()/2;
    auto* tl=diagramScene->addText("TAIL");
    tl->setDefaultTextColor(QColor(C_MUTED));
    tl->setFont(QFont("Consolas",8,QFont::Bold));
    tl->setPos(tailCX-tl->boundingRect().width()/2,Y+NODE_H+5);


    auto drawNullBox=[&](int bx,int cy){
        QPainterPath np; np.addRoundedRect(bx,cy-10,NULL_W,20,4,4);
        diagramScene->addPath(np,QPen(QColor(C_ERR),1.2),QBrush(QColor(C_ERR_DIM)));
        auto* n=diagramScene->addText("null");
        n->setDefaultTextColor(QColor(C_ERR));
        n->setFont(QFont("Consolas",9,QFont::Bold));
        QRectF nb=n->boundingRect();
        n->setPos(bx+(NULL_W-nb.width())/2,cy-nb.height()/2);
    };


    if (isDLL) {
        int nx=PAD_L-NULL_GAP-NULL_W;
        diagramScene->addLine(PAD_L,AY_PREV,nx+NULL_W,AY_PREV,
                              QPen(QColor(C_ACCENT2),1.2,Qt::DashLine,Qt::RoundCap));
        QPolygonF a; a<<QPointF(nx+NULL_W,AY_PREV)<<QPointF(nx+NULL_W+9,AY_PREV-4)<<QPointF(nx+NULL_W+9,AY_PREV+4);
        diagramScene->addPolygon(a,QPen(Qt::NoPen),QBrush(QColor(C_ACCENT2)));
        drawNullBox(nx,AY_PREV);
    }

    for (int i=0;i<items.size();++i) {
        const int x=xOffsets[i], nw=nodeWidths[i];
        bool isFirst=(i==0), isLast=(i==items.size()-1);

        if (isFirst) {
            QPainterPath gp; gp.addRoundedRect(x-4,Y-4,nw+8,NODE_H+8,CORNER+4,CORNER+4);
            auto* glow=diagramScene->addPath(gp,QPen(glowCol,1),QBrush(Qt::transparent));
            glow->setOpacity(0.20);
        }

        QPainterPath np; np.addRoundedRect(x,Y,nw,NODE_H,CORNER,CORNER);
        QLinearGradient grad(x,Y,x,Y+NODE_H);
        grad.setColorAt(0.0,nodeBg.lighter(135)); grad.setColorAt(1.0,nodeBg);
        diagramScene->addPath(np,QPen(nodeBorder,isFirst?2.0:1.5),QBrush(grad));

        auto* idx=diagramScene->addText(QString("[%1]").arg(i));
        idx->setDefaultTextColor(QColor(C_MUTED));
        idx->setFont(QFont("Consolas",7));
        idx->setPos(x+5,Y+3);

        dataFont.setBold(isFirst);
        auto* dl=diagramScene->addText(items[i]);
        dl->setDefaultTextColor(QColor(isFirst?glowCol:C_TEXT));
        dl->setFont(dataFont);
        QRectF dbr=dl->boundingRect();
        dl->setPos(x+(nw-dbr.width())/2.0,Y+(NODE_H-dbr.height())/2.0);
        dataFont.setBold(false);

        if (!isLast) {
            int ax1=x+nw, ax2=xOffsets[i+1];
            diagramScene->addLine(ax1,AY_NEXT,ax2,AY_NEXT,
                                  QPen(QColor(C_ARROW),1.5,Qt::SolidLine,Qt::RoundCap));
            QPolygonF f; f<<QPointF(ax2,AY_NEXT)<<QPointF(ax2-9,AY_NEXT-4)<<QPointF(ax2-9,AY_NEXT+4);
            diagramScene->addPolygon(f,QPen(Qt::NoPen),QBrush(QColor(C_ARROW)));
            if (isDLL) {
                diagramScene->addLine(ax2,AY_PREV,ax1,AY_PREV,
                                      QPen(QColor(C_ACCENT2),1.2,Qt::DashLine,Qt::RoundCap));
                QPolygonF b; b<<QPointF(ax1,AY_PREV)<<QPointF(ax1+9,AY_PREV-4)<<QPointF(ax1+9,AY_PREV+4);
                diagramScene->addPolygon(b,QPen(Qt::NoPen),QBrush(QColor(C_ACCENT2)));
            }
        }

        if (isLast) {
            int nx=x+nw, rny=isDLL?AY_NEXT:AY;
            diagramScene->addLine(nx,rny,nx+NULL_GAP,rny,QPen(QColor(C_ARROW),1.5));
            QPolygonF a; a<<QPointF(nx+NULL_GAP,rny)<<QPointF(nx+NULL_GAP-9,rny-4)<<QPointF(nx+NULL_GAP-9,rny+4);
            diagramScene->addPolygon(a,QPen(Qt::NoPen),QBrush(QColor(C_ARROW)));
            drawNullBox(nx+NULL_GAP,rny);
        }
    }

    if (diagramView->transform()==QTransform())
        diagramView->fitInView(diagramScene->sceneRect(),Qt::KeepAspectRatio);
}

void Widget::refreshStats()
{
    auto fmt=[&](int sz,int mx,int mn)->QString{
        if(!sz) return "size=0   max=—   min=—";
        return QString("size=%1   max=%2   min=%3").arg(sz).arg(mx).arg(mn);
    };
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) lblStats->setText(fmt(sll_int.get_size(),sll_int.get_max(),sll_int.get_min()));
        else if (currentType==DataType::Str) lblStats->setText(QString("size=%1").arg(sll_str.get_size()));
        else                                 lblStats->setText(QString("size=%1").arg(sll_chr.get_size()));
    } else {
        if      (currentType==DataType::Int) lblStats->setText(fmt(dll_int.get_size(),dll_int.get_max(),dll_int.get_min()));
        else if (currentType==DataType::Str) lblStats->setText(QString("size=%1").arg(dll_str.get_size()));
        else                                 lblStats->setText(QString("size=%1").arg(dll_chr.get_size()));
    }
}

void Widget::log(const QString& msg, bool ok)
{
    QString ts=QTime::currentTime().toString("HH:mm:ss");
    QString color=ok?C_OK:C_ERR;
    QString bg=ok?C_OK_DIM:C_ERR_DIM;
    QString icon=ok?"✔":"✘";
    logBox->append(QString(
                       "<span style='background:%1;border-radius:3px;padding:1px 5px;"
                       "color:%2;font-weight:bold'> %3 </span>"
                       " <span style='color:%4;font-size:9px'>[%5]</span>"
                       "  <span style='color:%6'>%7</span>"
                       ).arg(bg).arg(color).arg(icon).arg(C_MUTED).arg(ts)
                       .arg(C_TEXT).arg(msg.toHtmlEscaped()));
    logBox->moveCursor(QTextCursor::End);
}

void Widget::resetLists()
{
    sll_int.destroy(); dll_int.destroy();
    sll_str.destroy(); dll_str.destroy();
    sll_chr.destroy(); dll_chr.destroy();
}

template<> int Widget::parseValue<int>(bool& ok){ return leValue->text().toInt(&ok); }
template<> std::string Widget::parseValue<std::string>(bool& ok){
    ok=!leValue->text().isEmpty(); return leValue->text().toStdString();
}
template<> char Widget::parseValue<char>(bool& ok){
    ok=!leValue->text().isEmpty();
    return leValue->text().isEmpty()?'\0':leValue->text().at(0).toLatin1();
}

#define REFRESH refreshDiagram(); refreshStats();

void Widget::op_insertFirst()
{
    bool ok;
    if (currentType==DataType::Int) {
        int v=parseValue<int>(ok); if(!ok){log("Invalid integer value.",false);return;}
        (currentKind==ListKind::SLL)?sll_int.insert_at_first(v,false):dll_int.insert_at_first(v,false);
    } else if (currentType==DataType::Str) {
        auto v=parseValue<std::string>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_str.insert_at_first(v,false):dll_str.insert_at_first(v,false);
    } else {
        char v=parseValue<char>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_chr.insert_at_first(v,false):dll_chr.insert_at_first(v,false);
    }
    log(QString("Inserted '%1' at first.").arg(leValue->text())); REFRESH
}

void Widget::op_insertEnd()
{
    bool ok;
    if (currentType==DataType::Int) {
        int v=parseValue<int>(ok); if(!ok){log("Invalid integer value.",false);return;}
        (currentKind==ListKind::SLL)?sll_int.insert_at_end(v,false):dll_int.insert_at_end(v,false);
    } else if (currentType==DataType::Str) {
        auto v=parseValue<std::string>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_str.insert_at_end(v,false):dll_str.insert_at_end(v,false);
    } else {
        char v=parseValue<char>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_chr.insert_at_end(v,false):dll_chr.insert_at_end(v,false);
    }
    log(QString("Inserted '%1' at end.").arg(leValue->text())); REFRESH
}

void Widget::op_insertAt()
{
    bool ok; int pos=sbPos->value();
    int sz=0;
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sz=sll_int.get_size();
        else if (currentType==DataType::Str) sz=sll_str.get_size();
        else                                 sz=sll_chr.get_size();
    } else {
        if      (currentType==DataType::Int) sz=dll_int.get_size();
        else if (currentType==DataType::Str) sz=dll_str.get_size();
        else                                 sz=dll_chr.get_size();
    }
    if (pos < 1 || pos > sz + 1) {
        log(QString("Position %1 out of range — list has %2 node(s), valid range is 1..%3.")
                .arg(pos).arg(sz).arg(sz+1), false);
        return;
    }
    if (currentType==DataType::Int) {
        int v=parseValue<int>(ok); if(!ok){log("Invalid integer value.",false);return;}
        (currentKind==ListKind::SLL)?sll_int.insert_at_position(pos,v,false):dll_int.insert_at_position(pos,v,false);
    } else if (currentType==DataType::Str) {
        auto v=parseValue<std::string>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_str.insert_at_position(pos,v,false):dll_str.insert_at_position(pos,v,false);
    } else {
        char v=parseValue<char>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        (currentKind==ListKind::SLL)?sll_chr.insert_at_position(pos,v,false):dll_chr.insert_at_position(pos,v,false);
    }
    log(QString("Inserted '%1' at position %2.").arg(leValue->text()).arg(pos)); REFRESH
}

void Widget::op_deleteFirst()
{
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sll_int.delete_at_first(false);
        else if (currentType==DataType::Str) sll_str.delete_at_first(false);
        else                                 sll_chr.delete_at_first(false);
    } else {
        if      (currentType==DataType::Int) dll_int.delete_at_first(false);
        else if (currentType==DataType::Str) dll_str.delete_at_first(false);
        else                                 dll_chr.delete_at_first(false);
    }
    log("Deleted first element."); REFRESH
}

void Widget::op_deleteEnd()
{
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sll_int.delete_at_end(false);
        else if (currentType==DataType::Str) sll_str.delete_at_end(false);
        else                                 sll_chr.delete_at_end(false);
    } else {
        if      (currentType==DataType::Int) dll_int.delete_at_end(false);
        else if (currentType==DataType::Str) dll_str.delete_at_end(false);
        else                                 dll_chr.delete_at_end(false);
    }
    log("Deleted last element."); REFRESH
}

void Widget::op_deleteAt()
{
    int pos=sbPos->value();
    int sz=0;
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sz=sll_int.get_size();
        else if (currentType==DataType::Str) sz=sll_str.get_size();
        else                                 sz=sll_chr.get_size();
    } else {
        if      (currentType==DataType::Int) sz=dll_int.get_size();
        else if (currentType==DataType::Str) sz=dll_str.get_size();
        else                                 sz=dll_chr.get_size();
    }
    if (sz == 0) {
        log("List is empty — nothing to delete.", false);
        return;
    }
    if (pos < 1 || pos > sz) {
        log(QString("Position %1 out of range — list has %2 node(s), valid range is 1..%3.")
                .arg(pos).arg(sz).arg(sz), false);
        return;
    }
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sll_int.delete_at_position(pos,false);
        else if (currentType==DataType::Str) sll_str.delete_at_position(pos,false);
        else                                 sll_chr.delete_at_position(pos,false);
    } else {
        if      (currentType==DataType::Int) dll_int.delete_at_position(pos,false);
        else if (currentType==DataType::Str) dll_str.delete_at_position(pos,false);
        else                                 dll_chr.delete_at_position(pos,false);
    }
    log(QString("Deleted element at position %1.").arg(pos)); REFRESH
}

void Widget::op_search()
{
    bool ok,found=false;
    if (currentType==DataType::Int) {
        int v=parseValue<int>(ok); if(!ok){log("Invalid integer.",false);return;}
        found=(currentKind==ListKind::SLL)?sll_int.search(v):dll_int.search(v);
    } else if (currentType==DataType::Str) {
        auto v=parseValue<std::string>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        found=(currentKind==ListKind::SLL)?sll_str.search(v):dll_str.search(v);
    } else {
        char v=parseValue<char>(ok); if(!ok){log("Value cannot be empty.",false);return;}
        found=(currentKind==ListKind::SLL)?sll_chr.search(v):dll_chr.search(v);
    }
    log(QString("Search '%1': %2").arg(leValue->text()).arg(found?"FOUND ✔":"NOT FOUND"),found);
}

void Widget::op_getItem()
{
    int pos=sbPos->value()-1;
    if (currentType==DataType::Int) {
        int sz=(currentKind==ListKind::SLL)?sll_int.get_size():dll_int.get_size();
        if(pos<0||pos>=sz){log("Position out of range.",false);return;}
        log(QString("Item at [%1] = %2").arg(pos).arg(
            (currentKind==ListKind::SLL)?sll_int.get_item(pos):dll_int.get_item(pos)));
    } else if (currentType==DataType::Str) {
        int sz=(currentKind==ListKind::SLL)?sll_str.get_size():dll_str.get_size();
        if(pos<0||pos>=sz){log("Position out of range.",false);return;}
        log(QString("Item at [%1] = %2").arg(pos).arg(QString::fromStdString(
            (currentKind==ListKind::SLL)?sll_str.get_item(pos):dll_str.get_item(pos))));
    } else {
        int sz=(currentKind==ListKind::SLL)?sll_chr.get_size():dll_chr.get_size();
        if(pos<0||pos>=sz){log("Position out of range.",false);return;}
        log(QString("Item at [%1] = %2").arg(pos).arg(
            (currentKind==ListKind::SLL)?sll_chr.get_item(pos):dll_chr.get_item(pos)));
    }
}

void Widget::op_getFirst()
{
    if (currentType==DataType::Int) {
        if(!(currentKind==ListKind::SLL?sll_int.get_size():dll_int.get_size())){log("List is empty.",false);return;}
        log(QString("First = %1").arg((currentKind==ListKind::SLL)?sll_int.get_first():dll_int.get_first()));
    } else if (currentType==DataType::Str) {
        if(!(currentKind==ListKind::SLL?sll_str.get_size():dll_str.get_size())){log("List is empty.",false);return;}
        log(QString("First = %1").arg(QString::fromStdString(
            (currentKind==ListKind::SLL)?sll_str.get_first():dll_str.get_first())));
    } else {
        if(!(currentKind==ListKind::SLL?sll_chr.get_size():dll_chr.get_size())){log("List is empty.",false);return;}
        log(QString("First = %1").arg((currentKind==ListKind::SLL)?sll_chr.get_first():dll_chr.get_first()));
    }
}

void Widget::op_getLast()
{
    if (currentType==DataType::Int) {
        if(!(currentKind==ListKind::SLL?sll_int.get_size():dll_int.get_size())){log("List is empty.",false);return;}
        log(QString("Last = %1").arg((currentKind==ListKind::SLL)?sll_int.get_last():dll_int.get_last()));
    } else if (currentType==DataType::Str) {
        if(!(currentKind==ListKind::SLL?sll_str.get_size():dll_str.get_size())){log("List is empty.",false);return;}
        log(QString("Last = %1").arg(QString::fromStdString(
            (currentKind==ListKind::SLL)?sll_str.get_last():dll_str.get_last())));
    } else {
        if(!(currentKind==ListKind::SLL?sll_chr.get_size():dll_chr.get_size())){log("List is empty.",false);return;}
        log(QString("Last = %1").arg((currentKind==ListKind::SLL)?sll_chr.get_last():dll_chr.get_last()));
    }
}

void Widget::op_getMiddle()
{
    if (currentType==DataType::Int) {
        if(!(currentKind==ListKind::SLL?sll_int.get_size():dll_int.get_size())){log("List is empty.",false);return;}
        log(QString("Middle = %1").arg((currentKind==ListKind::SLL)?sll_int.get_middle():dll_int.get_middle()));
    } else if (currentType==DataType::Str) {
        if(!(currentKind==ListKind::SLL?sll_str.get_size():dll_str.get_size())){log("List is empty.",false);return;}
        log(QString("Middle = %1").arg(QString::fromStdString(
            (currentKind==ListKind::SLL)?sll_str.get_middle():dll_str.get_middle())));
    } else {
        if(!(currentKind==ListKind::SLL?sll_chr.get_size():dll_chr.get_size())){log("List is empty.",false);return;}
        log(QString("Middle = %1").arg((currentKind==ListKind::SLL)?sll_chr.get_middle():dll_chr.get_middle()));
    }
}

void Widget::op_getMax()
{
    if(currentType!=DataType::Int){log("Max/Min for int only.",false);return;}
    if(!(currentKind==ListKind::SLL?sll_int.get_size():dll_int.get_size())){log("List is empty.",false);return;}
    log(QString("Max = %1").arg((currentKind==ListKind::SLL)?sll_int.get_max():dll_int.get_max()));
}

void Widget::op_getMin()
{
    if(currentType!=DataType::Int){log("Max/Min for int only.",false);return;}
    if(!(currentKind==ListKind::SLL?sll_int.get_size():dll_int.get_size())){log("List is empty.",false);return;}
    log(QString("Min = %1").arg((currentKind==ListKind::SLL)?sll_int.get_min():dll_int.get_min()));
}

void Widget::op_getSize()
{
    int sz=0;
    if (currentKind==ListKind::SLL) {
        if      (currentType==DataType::Int) sz=sll_int.get_size();
        else if (currentType==DataType::Str) sz=sll_str.get_size();
        else                                 sz=sll_chr.get_size();
    } else {
        if      (currentType==DataType::Int) sz=dll_int.get_size();
        else if (currentType==DataType::Str) sz=dll_str.get_size();
        else                                 sz=dll_chr.get_size();
    }
    log(QString("Size = %1").arg(sz));
}

void Widget::op_print()
{
    log("── List contents ───────────────────────────────");
    auto dump=[&](int sz,auto get){
        if(!sz){log("  (empty)",false);return;}
        for(int i=0;i<sz;i++) log(QString("  [%1]  →  %2").arg(i).arg(get(i)));
    };
    if (currentType==DataType::Int) {
        int sz=(currentKind==ListKind::SLL)?sll_int.get_size():dll_int.get_size();
        dump(sz,[&](int i)->QString{return QString::number(
                                             (currentKind==ListKind::SLL)?sll_int.get_item(i):dll_int.get_item(i));});
    } else if (currentType==DataType::Str) {
        int sz=(currentKind==ListKind::SLL)?sll_str.get_size():dll_str.get_size();
        dump(sz,[&](int i)->QString{return QString::fromStdString(
                                             (currentKind==ListKind::SLL)?sll_str.get_item(i):dll_str.get_item(i));});
    } else {
        int sz=(currentKind==ListKind::SLL)?sll_chr.get_size():dll_chr.get_size();
        dump(sz,[&](int i)->QString{return QString(
                                             (currentKind==ListKind::SLL)?sll_chr.get_item(i):dll_chr.get_item(i));});
    }
    log("────────────────────────────────────────────────");
}

void Widget::op_sll_reverse()
{
    if      (currentType==DataType::Int){auto&h=sll_int.get_head();if(!h){log("Empty.",false);return;}sll_int.reverse(h,false);}
    else if (currentType==DataType::Str){auto&h=sll_str.get_head();if(!h){log("Empty.",false);return;}sll_str.reverse(h,false);}
    else                                {auto&h=sll_chr.get_head();if(!h){log("Empty.",false);return;}sll_chr.reverse(h,false);}
    log("SLL reversed."); REFRESH
}

void Widget::op_sll_palindrome()
{
    bool r=false;
    if      (currentType==DataType::Int) r=sll_int.check_palindrome();
    else if (currentType==DataType::Str) r=sll_str.check_palindrome();
    else                                 r=sll_chr.check_palindrome();
    log(QString("Palindrome: %1").arg(r?"YES ✔":"NO ✘"),r);
}

void Widget::op_sll_merge()
{
    if(currentType!=DataType::Int){log("⚠ Merge is only available for integer lists.",false);return;}
    QStringList parts=leValue->text().split(','); bool ok;
    SingleLinkedList<int> l2;
    for(auto&p:parts){int v=p.trimmed().toInt(&ok);if(ok)l2.insert_at_end(v,false);}
    if(sll_int.empty()&&l2.empty()){log("⚠ Cannot merge — both lists are empty.",false);return;}
    auto res=sll_int.merge(l2);
    QString preview; for(auto*c=res.get_head();c;c=c->next)preview+=QString::number(c->data)+"  ";
    preview=preview.trimmed();

    QDialog dlg(this); dlg.setWindowTitle("Confirm Merge"); dlg.setFixedWidth(390);
    dlg.setStyleSheet(QString(
                          "QDialog{background:%1;border:1px solid %2;border-radius:10px;}"
                          "QLabel{background:transparent;border:none;}"
                          "QPushButton{border-radius:6px;padding:7px 18px;font-size:10px;font-weight:600;}"
                          ).arg(C_PANEL2).arg(C_BORDER2));
    QVBoxLayout* vl=new QVBoxLayout(&dlg); vl->setContentsMargins(24,22,24,20); vl->setSpacing(14);
    QHBoxLayout* tr=new QHBoxLayout; tr->setSpacing(10);
    QLabel* ico=new QLabel("⊞"); ico->setStyleSheet(QString("color:%1;font-size:22px;").arg(C_ACCENT));
    QLabel* ttl=new QLabel("Merge Sorted Lists"); ttl->setStyleSheet(QString("color:%1;font-size:13px;font-weight:700;").arg(C_TEXT));
    tr->addWidget(ico); tr->addWidget(ttl); tr->addStretch(); vl->addLayout(tr);
    QLabel* info=new QLabel(QString("Merged result contains <b style='color:%1'>%2 node(s)</b>.<br>Replacing the current SLL with this result.").arg(C_ACCENT).arg(res.get_size()));
    info->setWordWrap(true); info->setStyleSheet(QString("color:%1;font-size:10px;line-height:160%;").arg(C_TEXT2)); vl->addWidget(info);
    QLabel* prev=new QLabel(QString("[ %1 ]").arg(preview)); prev->setWordWrap(true);
    prev->setStyleSheet(QString("color:%1;background:%2;border:1px solid %3;border-radius:6px;padding:8px 12px;font-size:10px;font-family:'JetBrains Mono','Consolas',monospace;").arg(C_ACCENT).arg(C_ACCENT_DIM).arg(C_BORDER2)); vl->addWidget(prev);
    QFrame* sep=new QFrame; sep->setFrameShape(QFrame::HLine); sep->setStyleSheet(QString("color:%1;").arg(C_BORDER)); vl->addWidget(sep);
    QHBoxLayout* btns=new QHBoxLayout; btns->setSpacing(8); btns->addStretch();
    QPushButton* btnCancel=new QPushButton("Cancel");
    btnCancel->setStyleSheet(QString("QPushButton{background:%1;color:%2;border:1px solid %3;border-radius:6px;}QPushButton:hover{background:#182236;}").arg(C_PANEL2).arg(C_TEXT2).arg(C_BORDER2));
    QPushButton* btnOk=new QPushButton("⊞  Replace SLL");
    btnOk->setStyleSheet(QString("QPushButton{background:%1;color:%2;border:1px solid %2;border-radius:6px;}QPushButton:hover{background:#0e2550;}").arg(C_ACCENT_DIM).arg(C_ACCENT));
    connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::reject);
    connect(btnOk,    &QPushButton::clicked,&dlg,&QDialog::accept);
    btns->addWidget(btnCancel); btns->addWidget(btnOk); vl->addLayout(btns);
    if(dlg.exec()==QDialog::Accepted){sll_int=std::move(res);log(QString("✔ SLL merged — %1 node(s).").arg(sll_int.get_size()),true);REFRESH}
    else log("Merge cancelled.",true);
}

void Widget::op_dll_reverse()
{
    if      (currentType==DataType::Int){auto&h=dll_int.get_head();if(!h){log("Empty.",false);return;}dll_int.reverse(h,false);}
    else if (currentType==DataType::Str){auto&h=dll_str.get_head();if(!h){log("Empty.",false);return;}dll_str.reverse(h,false);}
    else                                {auto&h=dll_chr.get_head();if(!h){log("Empty.",false);return;}dll_chr.reverse(h,false);}
    log("DLL reversed."); REFRESH
}

void Widget::op_dll_palindrome()
{
    bool r=false;
    if      (currentType==DataType::Int) r=dll_int.check_palindrome();
    else if (currentType==DataType::Str) r=dll_str.check_palindrome();
    else                                 r=dll_chr.check_palindrome();
    log(QString("Palindrome: %1").arg(r?"YES ✔":"NO ✘"),r);
}

void Widget::op_dll_merge()
{
    if(currentType!=DataType::Int){log("⚠ Merge is only available for integer lists.",false);return;}
    QStringList parts=leValue->text().split(','); bool ok;
    DoubleLinkedList<int> l2;
    for(auto&p:parts){int v=p.trimmed().toInt(&ok);if(ok)l2.insert_at_end(v,false);}
    if(dll_int.empty()&&l2.empty()){log("⚠ Cannot merge — both lists are empty.",false);return;}
    auto res=dll_int.merge(l2);
    QString preview; for(auto*c=res.get_head();c;c=c->next)preview+=QString::number(c->data)+"  ";
    preview=preview.trimmed();

    QDialog dlg(this); dlg.setWindowTitle("Confirm Merge"); dlg.setFixedWidth(390);
    dlg.setStyleSheet(QString(
                          "QDialog{background:%1;border:1px solid %2;border-radius:10px;}"
                          "QLabel{background:transparent;border:none;}"
                          "QPushButton{border-radius:6px;padding:7px 18px;font-size:10px;font-weight:600;}"
                          ).arg(C_PANEL2).arg(C_BORDER2));
    QVBoxLayout* vl=new QVBoxLayout(&dlg); vl->setContentsMargins(24,22,24,20); vl->setSpacing(14);
    QHBoxLayout* tr=new QHBoxLayout; tr->setSpacing(10);
    QLabel* ico=new QLabel("⊞"); ico->setStyleSheet(QString("color:%1;font-size:22px;").arg(C_ACCENT2));
    QLabel* ttl=new QLabel("Merge Sorted Lists"); ttl->setStyleSheet(QString("color:%1;font-size:13px;font-weight:700;").arg(C_TEXT));
    tr->addWidget(ico); tr->addWidget(ttl); tr->addStretch(); vl->addLayout(tr);
    QLabel* info=new QLabel(QString("Merged result contains <b style='color:%1'>%2 node(s)</b>.<br>Replacing the current DLL with this result.").arg(C_ACCENT2).arg(res.get_size()));
    info->setWordWrap(true); info->setStyleSheet(QString("color:%1;font-size:10px;line-height:160%;").arg(C_TEXT2)); vl->addWidget(info);
    QLabel* prev=new QLabel(QString("[ %1 ]").arg(preview)); prev->setWordWrap(true);
    prev->setStyleSheet(QString("color:%1;background:%2;border:1px solid %3;border-radius:6px;padding:8px 12px;font-size:10px;font-family:'JetBrains Mono','Consolas',monospace;").arg(C_ACCENT2).arg(C_ACCENT2_DIM).arg(C_BORDER2)); vl->addWidget(prev);
    QFrame* sep=new QFrame; sep->setFrameShape(QFrame::HLine); sep->setStyleSheet(QString("color:%1;").arg(C_BORDER)); vl->addWidget(sep);
    QHBoxLayout* btns=new QHBoxLayout; btns->setSpacing(8); btns->addStretch();
    QPushButton* btnCancel=new QPushButton("Cancel");
    btnCancel->setStyleSheet(QString("QPushButton{background:%1;color:%2;border:1px solid %3;border-radius:6px;}QPushButton:hover{background:#182236;}").arg(C_PANEL2).arg(C_TEXT2).arg(C_BORDER2));
    QPushButton* btnOk=new QPushButton("⊞  Replace DLL");
    btnOk->setStyleSheet(QString("QPushButton{background:%1;color:%2;border:1px solid %2;border-radius:6px;}QPushButton:hover{background:#1e0f50;}").arg(C_ACCENT2_DIM).arg(C_ACCENT2));
    connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::reject);
    connect(btnOk,    &QPushButton::clicked,&dlg,&QDialog::accept);
    btns->addWidget(btnCancel); btns->addWidget(btnOk); vl->addLayout(btns);
    if(dlg.exec()==QDialog::Accepted){dll_int=std::move(res);log(QString("✔ DLL merged — %1 node(s).").arg(dll_int.get_size()),true);REFRESH}
    else log("Merge cancelled.",true);
}