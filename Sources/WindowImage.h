#ifndef WINDOWIMAGE_H
#define WINDOWIMAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QGraphicsView>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QDebug>

/* =================================================================== *\
|    WindowImage Class                                                  |
\* =================================================================== */

class WindowImage : public QWidget {

    Q_OBJECT

public:

    WindowImage();

    // Image manipulation
    void setImage(QImage*);

    // Settings
    double DisplayRate;
    qint64 timeRef;

signals:

public slots:

    void updateDisplay();

private:

    // Widgets
    QGridLayout *Layout;
    QGraphicsView *IMAGE_VIEW;
    QGraphicsScene *IMAGE_SCENE;
    QLabel *IMAGE_LABEL;

    // Image buffer
    QPixmap pixmap;

};

#endif
