#ifndef CAMERA_FLIR_H
#define CAMERA_FLIR_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QElapsedTimer>
#include <QDebug>
#include <QTime>

#include "MsgHandler.h"
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

struct SImage {
    qint64 frameId;
    qint64 timestamp;
    qint64 gain;
    QImage Img;
};

Q_DECLARE_METATYPE(SImage);

/* =================================================================== *\
|    Frame grabber Class                                                |
\* =================================================================== */

class Cameras_FLIR : public QObject {

    Q_OBJECT

public:

    // Constructor and destructor
    Cameras_FLIR();
    ~Cameras_FLIR();

    bool grabState;

public slots:

    void display_info();

    void grab();

signals:

    void newImage(SImage);
    void closed();

private:

    // --- FLIR internal properties

    SystemPtr FLIR_system;
    CameraList FLIR_camList;

};

#endif
