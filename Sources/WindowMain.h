#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include <QDebug>
#include <QMainWindow>
#include <QDir>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QComboBox>
#include <QTextBrowser>
#include <QScrollBar>
#include <QFontDatabase>
#include <QTimer>
#include <QVector>

#include <algorithm>
#include <math.h>

#include "qcustomplot.h"
#include "MsgHandler.h"
#include "Cameras_FLIR.h"
#include "WindowImage.h"


using namespace std;

namespace Ui { class MainWindow; }

/* =================================================================== *\
|    WindowMain Class                                                   |
\* =================================================================== */

class WindowMain : public QMainWindow {

    Q_OBJECT

public:

    WindowMain(QWidget *parent = 0);
    ~WindowMain();
    void closeEvent(QCloseEvent *event);

signals:

public slots:

    void UpdateStack();
    void UpdatePlot();
    void UpdateMessage();

    void newImage(SImage);
    void stopGrab();

private:

    QString filesep, path;

    // --- GRAPHICS --------------------------------------------------------

    // --- Screens and windows

    QVector<QRect> Screen;

    QVector<QRect> Windows;
    WindowImage *w_Image;

    // --- Main window user interface

    Ui::MainWindow *ui;

    // Layouts
    QVBoxLayout *l_Main;
    QGridLayout *l_Grid;
    QStackedLayout *l_Stack;

    // Grid
    QPushButton *g_Output;
    QPushButton *g_Cameras;

    // Output
    QCustomPlot *Plot;
    QVector<double> x, y;
    int xDuration, yScaleIncrement;
    QTextBrowser *OutText;

    // --- CAMERAS ---------------------------------------------------------

    QThread *t_Cam;
    Cameras_FLIR *Cameras;

    qint64 DisplayRate, CamTimeRef;

};

#endif
