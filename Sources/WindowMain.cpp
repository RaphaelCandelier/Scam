#include "WindowMain.h"
#include "ui_mainwindow.h"

WindowMain::WindowMain(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    // --- DEFINITIONS -----------------------------------------------------

    // --- Pathes

    filesep = QString(QDir::separator());
    path = QDir::currentPath().mid(0, QDir::currentPath().toStdString().find_last_of(filesep.toStdString())) + filesep;
    QString path_icons = path + "Style" + filesep + "Icons" + filesep;

    // User interface
    ui->setupUi(this);

    // --- INITIALIZATION --------------------------------------------------

    qInfo() << TITLE_1 << "Initialization";

    // Thread
    qInfo() << THREAD << "GUI Thread: " << QThread::currentThreadId();

    // Times
    DisplayRate = 25;       // Display rate (Hz)
    CamTimeRef = 0;         // Camera time reference (ns)

    // Screens
    QDesktopWidget Desktop;
    switch (Desktop.screenCount()) {
    case 0:     qInfo() << TITLE_2 << "No screen detected"; break;
    case 1:     qInfo() << TITLE_2 << "1 screen detected"; break;
    default:    qInfo().nospace() << TITLE_2 << Desktop.screenCount() << " screens detected"; break;
    }

    for (int i=0; i<Desktop.screenCount(); i++) {
        Screen.push_back(Desktop.availableGeometry(i));
        qInfo().nospace() << " [" << i << "] " << Desktop.availableGeometry(i);
    }

    // --- LAYOUT ----------------------------------------------------------

    // === The Grid ==============================

    int iSz = Screen[0].width()*0.02;

    // --- Widgets

    g_Output = new QPushButton();
    g_Output->setIcon(QIcon(QPixmap((path_icons + "Output.png").toStdString().c_str())));
    g_Output->setIconSize(QSize(iSz-10, iSz-10));
    g_Output->setFixedSize(iSz, iSz);
    g_Output->setToolTip(QString("Status output (graph and text)"));

    g_Cameras = new QPushButton();
    g_Cameras->setIcon(QIcon(QPixmap((path_icons + "Camera.png").toStdString().c_str())));
    g_Cameras->setIconSize(QSize(iSz-10, iSz-10));
    g_Cameras->setFixedSize(iSz, iSz);
    g_Cameras->setToolTip(QString("Cameras"));

    QPushButton *g_Grab = new QPushButton();
    g_Grab->setIcon(QIcon(QPixmap((path_icons + "Record.png").toStdString().c_str())));
    g_Grab->setIconSize(QSize(iSz-10, iSz-10));
    g_Grab->setFixedSize(iSz, iSz);
    g_Grab->setToolTip(QString("Grab"));

    QPushButton *g_Stop = new QPushButton();
    g_Stop->setIcon(QIcon(QPixmap((path_icons + "Stop.png").toStdString().c_str())));
    g_Stop->setIconSize(QSize(iSz-10, iSz-10));
    g_Stop->setFixedSize(iSz, iSz);
    g_Stop->setToolTip(QString("Stop"));

    // --- Layout

    l_Grid = new QGridLayout;

    l_Grid->addWidget(g_Output, 0, 0, Qt::AlignLeft);
    l_Grid->addWidget(g_Cameras, 0, 1, Qt::AlignLeft);

    l_Grid->addWidget(g_Grab, 1, 0, Qt::AlignLeft);
    l_Grid->addWidget(g_Stop, 1, 1, Qt::AlignLeft);

    // === Output ===============================

    // --- Plot
    Plot = new QCustomPlot();
    Plot->setMinimumHeight(250);
    xDuration = 5000;               // Chart length duration (ms)
    yScaleIncrement = 20;           // Scale increment (ms)

    Plot->addGraph();

    int gi = 0;
    QColor color(20+200/4.0*gi,70*(1.6-gi/4.0), 150, 150);
    Plot->graph()->setLineStyle(QCPGraph::lsLine);
    Plot->graph()->setPen(QPen(color.lighter(200)));
    Plot->graph()->setBrush(QBrush(color));

    Plot->xAxis->setRange(-xDuration/1000, 0);
    Plot->xAxis->setLabel("Time (s)");
    Plot->yAxis->setLabel("Times (ms)");
    Plot->replot();

    // --- Text output

    // Definition
    OutText = new QTextBrowser();
    QTextDocument *OutDoc = new QTextDocument;

    // Style
    OutText->setFrameStyle(QFrame::NoFrame);
    QFile File("../Style/output.css");
    File.open(QFile::ReadOnly);
    OutDoc->setDefaultStyleSheet(File.readAll());
    OutDoc->setDefaultFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    OutText->setDocument(OutDoc);

    // --- Layout

    QWidget *p_Output = new QWidget;
    QVBoxLayout *l_Output = new QVBoxLayout(p_Output);
    l_Output->addWidget(Plot, 0);
    l_Output->addWidget(OutText, 1);

    // === Camera ===============================

    QComboBox *c_Select = new QComboBox;
    c_Select->addItem("Camera 0");
    c_Select->addItem("Camera 1");

    // --- Layout

    QWidget *p_Camera = new QWidget;
    QGridLayout *l_Camera = new QGridLayout(p_Camera);
    l_Camera->addWidget(c_Select, 0, 0, Qt::AlignTop);

    // === Main Layout ==========================

    l_Main = new QVBoxLayout;
    l_Main->addLayout(l_Grid, 0);

    l_Stack = new QStackedLayout;
    l_Stack->addWidget(p_Output);
    l_Stack->addWidget(p_Camera);
    l_Main->addLayout(l_Stack, 1);

    QWidget *w_Main = new QWidget(this);
    w_Main->setLayout(l_Main);
    this->setCentralWidget(w_Main);

    // --- MAIN WINDOW SETTINGS --------------------------------------------

    int margin = 20;

    this->setWindowTitle("Scam");
    this->setGeometry(Screen[0].x(), Screen[0].y(), Screen[0].width()*0.2, Screen[0].height() - 400);

    // --- CAMERAS ---------------------------------------------------------

    Cameras = new Cameras_FLIR;

    // Create thread
    t_Cam = new QThread;
    Cameras->moveToThread(t_Cam);
    connect(Cameras, SIGNAL(closed()), t_Cam, SLOT(quit()));
    connect(Cameras, SIGNAL(closed()), t_Cam, SLOT(deleteLater()));
    t_Cam->start();

    Cameras->display_info();

    // --- IMAGE WINDOWS ---------------------------------------------------

    w_Image = new WindowImage;
    // w_Image->setGeometry(Screen[0].x()+Screen[0].width()*0.2+margin, Screen[0].y(), 1320, 1060);
    w_Image->show();

    // --- CONNECTIONS -----------------------------------------------------

    // Custom types registation
    qRegisterMetaType<SImage>();

    connect(Cameras, SIGNAL(newImage(SImage)), this, SLOT(newImage(SImage)));

    connect(g_Output,   SIGNAL(released()), this, SLOT(UpdateStack()));
    connect(g_Cameras,  SIGNAL(released()), this, SLOT(UpdateStack()));

    connect(g_Grab, SIGNAL(released()), Cameras, SLOT(grab()));
    connect(g_Stop, SIGNAL(released()), this, SLOT(stopGrab()));

    // --- TIMERS ----------------------------------------------------------

    QTimer *t_Msg = new QTimer();
    connect(t_Msg, SIGNAL(timeout()), this, SLOT(UpdateMessage()));
    t_Msg->start(50);

    QTimer *t_Plot = new QTimer();
    connect(t_Plot, SIGNAL(timeout()), this, SLOT(UpdatePlot()));
    t_Plot->start(1000/DisplayRate);

    QTimer::singleShot(0, Cameras, SLOT(grab()));

}

/* ====================================================================== *\
|    SLOTS                                                                 |
\* ====================================================================== */

// === DISPLAY =============================================================

void WindowMain::UpdateStack() {

    QObject* obj = sender();
    if(obj == g_Output) { l_Stack->setCurrentIndex(0); }
    if(obj == g_Cameras) { l_Stack->setCurrentIndex(1); }

}

// === OUTPUT ==============================================================

void WindowMain::UpdateMessage() {

    while (Messages.length()) {

        Message MSG = Messages.takeFirst();
        QString S;

        switch (MSG.type) {
        case QtDebugMsg:
            cout << MSG.text.toStdString() << endl;
            break;
        case QtInfoMsg:
            S = "<" + MSG.css + ">" + MSG.text + "</p>" ;
            break;
        case QtWarningMsg:
            S = "<p class='warning'>" + MSG.text + "</p>";
            break;
        case QtCriticalMsg:
            S= "<p class='critical'>" + MSG.text + "</p>";
            break;
        case QtFatalMsg:
            S = "<p class='fatal'>" + MSG.text + "</p>";
            break;
        }

        OutText->setHtml(OutText->toHtml().append(S));
        OutText->verticalScrollBar()->setValue(OutText->verticalScrollBar()->maximum());
    }
}

void WindowMain::UpdatePlot() {

    if (x.count()) {

        // Data
        QVector<double> t;
        for (QVector<double>::iterator it = x.begin(); it!=x.end(); ++it) {
            t.push_back((*it - x.last())*1e-9);
        }

        Plot->graph(0)->setData(t, y);

        // Axis scales
        double m = *max_element(y.constBegin(), y.constEnd());
        Plot->yAxis->setRange(0, ceil(m/yScaleIncrement)*yScaleIncrement);

        // Replot
        Plot->replot();

    }
}

// === IMAGES ==============================================================

void WindowMain::newImage(SImage SImg) {

    // --- Plot
    if (x.count()) {
        y.push_back((SImg.timestamp - x.last())/1e6);   // Frame differential time (ms)
        x.push_back(SImg.timestamp);                    // Frame timestamp
    } else {
        x.push_back(SImg.timestamp);
        y.push_back(0);
    }

    while (x.last()-x.first()>xDuration*1e6) {
        x.pop_front();
        y.pop_front();
    }

    // --- Send image to display

    w_Image->setImage(&SImg.Img);

    return;
}

void WindowMain::stopGrab() {

    qInfo() << "Stoping image acquisition";
    Cameras->grabState = false;

}

/* ====================================================================== *\
|     CLOSING EVENT                                                        |
\* ====================================================================== */

void WindowMain::closeEvent (QCloseEvent *event) {

    // --- Stop grab
    Cameras->grabState = false;

    // --- Windows
    w_Image->close();

    // --- Proceed

    event->accept();
}

WindowMain::~WindowMain() {
    delete ui;
}

