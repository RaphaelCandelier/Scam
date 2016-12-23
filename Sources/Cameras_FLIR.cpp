#include "Cameras_FLIR.h"

using namespace std;

/* ====================================================================== *\
|    Constructor                                                           |
\* ====================================================================== */

Cameras_FLIR::Cameras_FLIR() {

    // --- Initialization
    grabState = false;

    // --- FLIR cameras
    FLIR_system = System::GetInstance();
    FLIR_camList = FLIR_system->GetCameras();

}

Cameras_FLIR::~Cameras_FLIR() {

    CameraPtr pCam = FLIR_camList.GetByIndex(0);
    pCam->DeInit();

    // Clean FLIR internal pointers
    FLIR_camList.Clear();
    FLIR_system->ReleaseInstance();

    emit closed();
}

/* ====================================================================== *\
|    Camera settings                                                       |
\* ====================================================================== */


/* ====================================================================== *\
|    Camera information output                                             |
\* ====================================================================== */

void Cameras_FLIR::display_info() {

    unsigned int FLIR_nCam = FLIR_camList.GetSize();
    switch (FLIR_nCam) {
    case 0:     qInfo() << TITLE_2 << "No camera detected"; break;
    case 1:     qInfo() << TITLE_2 << "1 camera detected"; break;
    default:    qInfo().nospace() << TITLE_2 << FLIR_nCam << " cameras detected"; break;
    }

    for (int i=0; i<FLIR_nCam; i++) {

        CameraPtr pCamera = FLIR_camList.GetByIndex(i);
        INodeMap &nodeMap = pCamera->GetTLDeviceNodeMap();

        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");

        if (IsAvailable(category) && IsReadable(category)) {

            category->GetFeatures(features);

            FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it) {

                CNodePtr pfeatureNode = *it;
                CValuePtr pValue = (CValuePtr)pfeatureNode;
                qInfo() << pfeatureNode->GetName() << ":" << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");

            }

        } else { qInfo() << "<i>Not available</i>"; }

    }

}

/* ====================================================================== *\
|    Frame grabber                                                         |
\* ====================================================================== */

void Cameras_FLIR::grab() {

    qInfo() << TITLE_1 << "Acquisition";

    qInfo() << THREAD << "FrameGrabberThread:" << QThread::currentThreadId();

    // --- Camera & nodemaps definitions -----------------------------------

    CameraPtr pCam = FLIR_camList.GetByIndex(0);
    pCam->Init();

    INodeMap &nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
    INodeMap &nodeMap = pCam->GetNodeMap();

    // --- Configure ChunkData ---------------------------------------------

    // --- Activate chunk mode

    CBooleanPtr ptrChunkModeActive = nodeMap.GetNode("ChunkModeActive");

    if (!IsAvailable(ptrChunkModeActive) || !IsWritable(ptrChunkModeActive)) {
        qWarning() << "Unable to activate chunk mode. Aborting.";
        return;
    }

    ptrChunkModeActive->SetValue(true);

    qInfo() << "Chunk mode activated";

    // --- Chunk data types

    NodeList_t entries;

    // Retrieve the selector node
    CEnumerationPtr ptrChunkSelector = nodeMap.GetNode("ChunkSelector");

    if (!IsAvailable(ptrChunkSelector) || !IsReadable(ptrChunkSelector)) {
        qWarning() << "Unable to retrieve chunk selector. Aborting.";
        return;
    }

    // Retrieve entries
    ptrChunkSelector->GetEntries(entries);

    for (int i = 0; i < entries.size(); i++) {

        // Select entry to be enabled
        CEnumEntryPtr ptrChunkSelectorEntry = entries.at(i);

        // Go to next node if problem occurs
        if (!IsAvailable(ptrChunkSelectorEntry) || !IsReadable(ptrChunkSelectorEntry)) { continue; }

        ptrChunkSelector->SetIntValue(ptrChunkSelectorEntry->GetValue());

        // Retrieve corresponding boolean
        CBooleanPtr ptrChunkEnable = nodeMap.GetNode("ChunkEnable");

        // Enable the boolean, thus enabling the corresponding chunk data
        if (IsWritable(ptrChunkEnable)) {
            ptrChunkEnable->SetValue(true);
        }

    }

    // --- Acquisition parameters ------------------------------------------

    // === Continuous mode ======================

    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
        qWarning() << "Unable to set acquisition mode to continuous (enum retrieval)";
        return;
    }

    // Retrieve entry node from enumeration node
    CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous)) {
        qWarning() << "Unable to set acquisition mode to continuous (entry retrieval)";
        return;
    }

    // Retrieve integer value from entry node
    int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

    // Set integer value from entry node as new value of enumeration node
    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

    qInfo() << "Acquisition mode set to continuous";

    // === Exposure time ========================

    CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
    exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());

    CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
    exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());

    CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
    exposureTime->SetValue(5000);

    qInfo() << "Exposure time set to 5ms";

    // --- Acquire images --------------------------------------------------

    grabState = true;
    qInfo() << "Starting image acquisition";

    pCam->BeginAcquisition();

    while (grabState) {

        ImagePtr pImg = pCam->GetNextImage();

        if (pImg->IsIncomplete()) {

            qWarning() << "Image incomplete with image status " << pImg->GetImageStatus();

        } else {

            SImage SImg;

            // --- Get image and convert to QImage
            unsigned char* Raw = (unsigned char*)pImg->GetData();
            SImg.Img = QImage(Raw, pImg->GetWidth(), pImg->GetHeight(), QImage::Format_Indexed8);
            for (unsigned int i=0 ; i<=255; i++) { SImg.Img.setColor(i, qRgb(i,i,i)); }

            // --- Get ChunkData
            ChunkData chunkData = pImg->GetChunkData();
            SImg.timestamp = (qint64) chunkData.GetTimestamp();
            SImg.frameId = (qint64) chunkData.GetFrameID();
            SImg.gain = (qint64) chunkData.GetGain();

            // --- Send new image
            emit newImage(SImg);

        }

        pImg->Release();

    }

    pCam->EndAcquisition();
    pCam->DeInit();

    return;
}
