#include "qfcameracombobox.h"

QFCameraComboBox::QFCameraComboBox(QFExtensionManager* extManager, QWidget* parent):
    QComboBox(parent)
{
    m_extManager=extManager;
    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(camIndexChanged(int)));
}

QFCameraComboBox::QFCameraComboBox(QWidget* parent):
    QComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(camIndexChanged(int)));
}
QFCameraComboBox::~QFCameraComboBox()
{
    //dtor
}

void QFCameraComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFCameraComboBox::findExtensions() {
    cameras.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(extobj);
        if (cam) cameras.append(extobj);
    }

    for (int i=0; i<cameras.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(cameras[i]);
        QFExtensionCamera* cam = qobject_cast<QFExtensionCamera*>(cameras[i]);
        for (unsigned int j=0; j<cam->getCameraCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=cam->getCameraName(j);
            if (!cam->getCameraSensorName(j).isEmpty()) cname+=QString(" (%1)").arg(cam->getCameraSensorName(j));
            if (cam->getCameraCount()>0) name=name+QString(" #%1: %2").arg(j).arg(cname);
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFCameraComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    return extension;
}

QFExtensionCamera* QFCameraComboBox::currentExtensionCamera() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    //int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    }
    return cam;
}

int QFCameraComboBox::currentCameraID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    } else {
        camIdx=-1;
    }
    return camIdx;
}

int QFCameraComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFCameraComboBox::camIndexChanged(int index) {
    if (index<0) {
        emit cameraChanged(NULL, NULL, -1);
    }

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionCamera* cam=NULL;
    int camIdx=p.y();
    if ((p.x()>=0)&&(p.x()<cameras.size())) {
        extension=qobject_cast<QFExtension*>(cameras[p.x()]);
        cam=qobject_cast<QFExtensionCamera*>(cameras[p.x()]);
    } else {
        camIdx=-1;
    }
    emit cameraChanged(extension, cam, camIdx);
}
