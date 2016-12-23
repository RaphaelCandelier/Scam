#include "WindowImage.h"

/* =================================================================== *\
|    Constructor                                                        |
\* =================================================================== */

WindowImage::WindowImage() {

    // --- INITIALIZATION --------------------------------------------------

    DisplayRate = 40;    // Display rate (Hertz)
    timeRef = 0;         // Time reference (seconds)

    // --- WINDOW SETTINGS -------------------------------------------------

    this->setStyleSheet("QWidget { background-color: white; }");
    this->setWindowFlags(Qt::FramelessWindowHint);

    // --- LAYOUT ----------------------------------------------------------


    // --- Image
    IMAGE_LABEL = new QLabel;
    IMAGE_LABEL->setPixmap(pixmap);
    IMAGE_LABEL->setFixedSize(QSize(1280, 1024));
    IMAGE_LABEL->setAlignment(Qt::AlignCenter);

    // --- Graphics scene (zoom & pan)
    IMAGE_SCENE = new QGraphicsScene();
    IMAGE_SCENE->addWidget(IMAGE_LABEL);
    IMAGE_SCENE->installEventFilter(this);

    IMAGE_VIEW = new QGraphicsView(IMAGE_SCENE);
    IMAGE_VIEW->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    IMAGE_VIEW->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    IMAGE_VIEW->setInteractive(true);
    IMAGE_VIEW->setDragMode(QGraphicsView::ScrollHandDrag);
    IMAGE_VIEW->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //IMAGE_VIEW->setFixedSize(QSize(642, 514));
    IMAGE_VIEW->scale(1, 1);

    // --- Apply layout

    Layout = new QGridLayout();
    Layout->addWidget(IMAGE_VIEW, 0, 0);
    this->setLayout(Layout);

    // --- DISPLAY TIMER ---------------------------------------------------

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    timer->start(1000/DisplayRate);

}

/* =================================================================== *\
|    Image handling and update                                          |
\* =================================================================== */

void WindowImage::setImage(QImage *Img) {

    pixmap = QPixmap::fromImage(*Img);

}

void WindowImage::updateDisplay() {

    IMAGE_LABEL->setPixmap(pixmap);

}
