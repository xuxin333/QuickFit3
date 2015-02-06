/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrdrimagestack_data.h"
#include <QtXml>
#include "qfexporterimageseries.h"
#include "qfexportermanager.h"

QFRDRImageStackData::QFRDRImageStackData(QFProject* parent):
    QFRawDataRecord(parent)
{
    memsize=0;
}

QFRDRImageStackData::~QFRDRImageStackData() {
    clearMemory();
}



void QFRDRImageStackData::intWriteData(QXmlStreamWriter& w) const {
	// write data to the project XML file using the QXmlStreamWriter
    /*
    w.writeStartElement("imagestacks");
    w.writeAttribute("imagestack", "123456789");
    w.writeEndElement();
    */
    w.writeAttribute(QString("mask"), maskToListString(", ", "; "));
}

void QFRDRImageStackData::intReadData(QDomElement* e) {
	// read data from the project XML file 

	/*
    if (e) {
        QDomElement te=e->firstChildElement("mytag");
        QString data=te.attribute("data");
    }
	*/

    QString stacktype=getProperty("STACKTYPE", "ONEFILEPERCHANNEL").toString().toUpper();
    QString maskS="";
    if (e) maskS=e->attribute("mask", "").simplified().trimmed();
    //qDebug()<<maskS;
    clearMemory();
    stacks.clear();
    loadImageStacks(stacktype, maskS);
}

void QFRDRImageStackData::loadImageStacks(const QString &stacktype, const QString &maskS)
{
    if (stacktype=="ONEFILEPERSTACK") {
        if (files.size()>0) {
            log_text(tr("  - loading images in image stack '%1'\n").arg(getName()));
            // we need two runs: in the first one, we only read the image stack sizes, then we can allocate the memory for the
            // image series and finally read in the data
            for (int f=0; f<files.size(); f++) {
                QString ft=files_types.value(f, "image").toLower();
                if (ft=="image" || ft=="image_hdualview" || ft=="image_vdualview") {
                    QFRDRImageStackData::ImageStack s;
                    s.file=f;
                    stacks.append(s);
                    loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmGetSize);
                    QString dvMode="none";
                    stacks.last().dvMode=dvNone;
                    if (ft=="image_hdualview") {
                        dvMode="h";
                        stacks.last().width=stacks.last().width/2;
                        stacks.last().dvMode=dvHorizontal;
                        stacks.last().channels=2;
                    } else if (ft=="image_vdualview") {
                        stacks.last().height=stacks.last().height/2;
                        dvMode="v";
                        stacks.last().dvMode=dvVertical;
                        stacks.last().channels=2;
                    }
                    getProject()->getServices()->log_text(tr("    * image stack %1 (file: '%2', %7) size:   width=%3,   height=%4,   frames=%5,   DualViewMode=%6\n").arg(stacks.size()).arg(files[f]).arg(stacks.last().width).arg(stacks.last().height).arg(stacks.last().frames).arg(dvMode).arg(ft));
                    setStackProperties(stacks.last().props, stacks.size()-1);

                    setQFProperty(QString("WIDTH%1").arg(stacks.size()-1), stacks.last().width, false, true);
                    setQFProperty(QString("HEIGHT%1").arg(stacks.size()-1), stacks.last().height, false, true);
                    setQFProperty(QString("FRAMES%1").arg(stacks.size()-1), stacks.last().frames, false, true);
                    setQFProperty(QString("CHANNELS%1").arg(stacks.size()-1), stacks.last().channels, false, true);
                    setQFProperty(QString("FILENUM%1").arg(stacks.size()-1), stacks.last().file, false, true);
                    setQFProperty(QString("DUALVIEWMODE%1").arg(stacks.size()-1), stacks.last().dvMode, false, true);



                    QString p;
                    p=QString("XUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
                    p=QString("YUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
                    p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
                    p=QString("CUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, ""), true, true);

                    p=QString("XAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "x"), true, true);
                    p=QString("YAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "y"), true, true);
                    p=QString("TAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "z/t"), true, true);
                    p=QString("CAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "channel"), true, true);

                    p=QString("XUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
                    p=QString("YUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
                    p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
                    p=QString("CUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);

                    p=QString("STACK_DESCRIPTION"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, tr("stack %1").arg(stacks.size())), true, true);
                    for (int c=0; c<stacks.last().channels; c++) {
                        p=QString("CHANNELNAME_%1_%2").arg(stacks.size()-1).arg(c);
                        setQFProperty(p, getProperty(p, QFileInfo(files[f]).completeBaseName()), true, true);
                    }

                    if (stacks.last().props.contains("PIXEL_WIDTH")) {
                        p=QString("XUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["PIXEL_WIDTH"].toDouble(), false, true);
                        p=QString("XUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
                    }
                    if (stacks.last().props.contains("PIXEL_HEIGHT")) {
                        p=QString("YUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["PIXEL_HEIGHT"].toDouble(), false, true);
                        p=QString("YUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
                    }
                    if (stacks.last().props.contains("STEPSIZE")) {
                        p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["STEPSIZE"].toDouble(), false, true);
                        p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
                    }
                    if (stacks.last().props.contains("DELTAZ")) {
                        p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["DELTAZ"].toDouble(), false, true);
                        p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
                    }
                    if (stacks.last().props.contains("FRAMETIME")) {
                        p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["DELTAZ"].toDouble(), false, true);
                        p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
                    }

                }

            }
            if (allocateMemory()) {
                getProject()->getServices()->log_text(tr("  - allocated %1 of memory\n").arg(bytestostr(memsize).c_str()));
                int stack=0;
                for (int f=0; f<files.size(); f++) {
                    QString ft=files_types.value(f, "image").toLower();
                    //qDebug()<<"load file "<<f<<files.size()<<ft;
                    if ((ft=="image" || ft=="image_hdualview" || ft=="image_vdualview" ) && (stack<stacks.size())) {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1/%2 (file: '%3') ...\n").arg(stack+1).arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks[stack], files[f], QFRDRImageStackData::lmReadData, 0, qfihAny)) {
                            stack++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2')\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2')\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                        //qDebug()<<"load file "<<f<<files.size()<<ft<<"OK";
                    }
                }
                //qDebug()<<"maskS="<<maskS.size();
                //if (maskS.size()>0) getProject()->getServices()->log_text(tr("    * loading image mask (%1) ...\n").arg(maskS));
                if (maskS.size()>0) getProject()->getServices()->log_text(tr("    * loading image mask ...\n"));
                //qDebug()<<"maskClear;";
                maskClear();
                //qDebug()<<"maskLoadFromListString "<<maskS;
                maskLoadFromListString(maskS,',', ';');
                //qDebug()<<"OK";
            } else {
                setError(tr("Error allocating %1 of memory!").arg(bytestostr(memsize).c_str()));
                log_error(tr("Error allocating %1 of memory!").arg(bytestostr(memsize).c_str()));
            }
        } else {
            setError(tr("there are no files in the %1 record!").arg(getName()));
            log_error(tr("there are no files in the %1 record!").arg(getName()));
        }
    } else if (stacktype=="ONEFILEPERCHANNEL") {
        if (files.size()>0) {
            getProject()->getServices()->log_text(tr("  - loading images in image stack '%1'\n").arg(getName()));
            // we need two runs: in the first one, we only read the image stack sizes, then we can allocate the memory for the
            // image series and finally read in the data

            { QFRDRImageStackData::ImageStack s;
            stacks.append(s); }
            for (int f=0; f<files.size(); f++) {
                QString ft=files_types.value(f, "image").toLower();
                if (ft=="image" || ft=="image_ldualview" || ft=="image_rdualview" || ft=="image_tdualview" || ft=="image_bdualview") {
                    stacks.last().file=f;

                    int c=stacks.last().channels;
                    int w=stacks.last().width;
                    int h=stacks.last().height;
                    int fr=stacks.last().frames;
                    loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmGetSize);

                    QString dvMode="none";
                    stacks.last().dvMode=dvNone;
                    if (ft=="image_ldualview" || ft=="image_rdualview") {
                        dvMode="h";
                        stacks.last().width=stacks.last().width/2;
                        stacks.last().dvMode=dvHorizontal;
                        stacks.last().channels=2;
                    } else if (ft=="image_tdualview" || ft=="image_bdualview") {
                        stacks.last().height=stacks.last().height/2;
                        dvMode="v";
                        stacks.last().dvMode=dvVertical;
                        stacks.last().channels=2;
                    }


                    getProject()->getServices()->log_text(tr("    * image stack %1 (file: '%2', %7) size:   width=%3,   height=%4,   frames=%5,   DualViewMode=%6\n").arg(stacks.size()).arg(files[f]).arg(stacks.last().width).arg(stacks.last().height).arg(stacks.last().frames).arg(dvMode).arg(ft));
                    setQFProperty(QString("WIDTH%1").arg(stacks.size()-1), stacks.last().width, false, true);
                    setQFProperty(QString("HEIGHT%1").arg(stacks.size()-1), stacks.last().height, false, true);
                    setQFProperty(QString("FRAMES%1").arg(stacks.size()-1), stacks.last().frames, false, true);
                    setQFProperty(QString("FILENUM%1").arg(stacks.size()-1), stacks.last().file, false, true);
                    setQFProperty(QString("DUALVIEWMODE%1").arg(stacks.size()-1), stacks.last().dvMode, false, true);



                    stacks.last().channels=c+1;

                    setQFProperty(QString("CHANNELS%1").arg(stacks.size()-1), stacks.last().channels, false, true);
                    QString  p=QString("CHANNELNAME_%1_%2").arg(stacks.size()-1).arg(stacks.last().channels-1);
                    setQFProperty(p, getProperty(p, QFileInfo(files[f]).completeBaseName()), true, true);

                    if ( (c>1) && ((w!=stacks.last().width)
                         ||(h!=stacks.last().height)
                         ||(fr!=(int64_t)stacks.last().frames)) ) {
                        setError(tr("size of file '%1'' does not fit the size of the other files in this stack").arg(files[f]));
                        log_error(tr("size of file '%1'' does not fit the size of the other files in this stack").arg(files[f]));
                    }

                }

            }

            QString p;
            p=QString("XUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
            p=QString("YUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
            p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "pixels"), true, true);
            p=QString("CUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, ""), true, true);

            p=QString("XAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "x"), true, true);
            p=QString("YAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "y"), true, true);
            p=QString("TAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "z/t"), true, true);
            p=QString("CAXIS_NAME"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, "channel"), true, true);

            p=QString("XUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
            p=QString("YUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
            p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);
            p=QString("CUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, 1.0), true, true);

            p=QString("STACK_DESCRIPTION"); setQFProperty(p+QString::number(stacks.size()-1), getEnumeratedProperty(p, stacks.size()-1, tr("stack %1").arg(stacks.size())), true, true);

            if (stacks.last().props.contains("PIXEL_WIDTH")) {
                p=QString("XUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["PIXEL_WIDTH"].toDouble(), false, true);
                p=QString("XUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
            }
            if (stacks.last().props.contains("PIXEL_HEIGHT")) {
                p=QString("YUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["PIXEL_HEIGHT"].toDouble(), false, true);
                p=QString("YUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
            }
            if (stacks.last().props.contains("STEPSIZE")) {
                p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["STEPSIZE"].toDouble(), false, true);
                p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
            }
            if (stacks.last().props.contains("DELTAZ")) {
                p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["DELTAZ"].toDouble(), false, true);
                p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
            }
            if (stacks.last().props.contains("FRAMETIME")) {
                p=QString("TUNIT_FACTOR"); setQFProperty(p+QString::number(stacks.size()-1), stacks.last().props["DELTAZ"].toDouble(), false, true);
                p=QString("TUNIT_NAME"); setQFProperty(p+QString::number(stacks.size()-1), tr("nm"), false, true);
            }

            if (allocateMemory()) {
                int channel=0;
                getProject()->getServices()->log_text(tr("  - allocated %1 of memory\n").arg(bytestostr(memsize).c_str()));
                for (int f=0; f<files.size(); f++) {
                    QString ft=files_types.value(f, "image").toLower();
                    //qDebug()<<f<<"/"<<files.size()<<ft;
                    if (ft=="image") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2') ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData, channel)) {
                            setStackProperties(stacks.last().props, stacks.size()-1, channel);
                            channel++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2')\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2')\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    } else if (ft=="image_ldualview") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2', left half) ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData, channel, qfihLeft)) {
                            setStackProperties(stacks.last().props, stacks.size()-1, channel);
                            channel++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2', left half)\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2', left half)\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    } else if (ft=="image_rdualview") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2', right half) ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData, channel, qfihRight)) {
                            setStackProperties(stacks.last().props, stacks.size()-1, channel);
                            channel++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2', right half)\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2', right half)\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    } else if (ft=="image_tdualview") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2', top half) ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData, channel, qfihTop)) {
                            setStackProperties(stacks.last().props, stacks.size()-1, channel);
                            channel++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2', top half)\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2', top half)\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    } else if (ft=="image_bdualview") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2', bottom half) ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData, channel, qfihBottom)) {
                            setStackProperties(stacks.last().props, stacks.size()-1, channel);
                            channel++;
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2', bottom half)\n").arg(stacks.size()).arg(files[f]));
                            log_error(tr("error loading image stack %1 (file: '%2', bottom half)\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    }

                    //qDebug()<<"OK";
                }
                //if (maskS.size()>0) getProject()->getServices()->log_text(tr("    * loading image mask (%1) ...\n").arg(maskS));
                if (maskS.size()>0) getProject()->getServices()->log_text(tr("    * loading image mask ...\n"));
                maskClear();
                maskLoadFromListString(maskS,',', ';');
            } else {
                setError(tr("Error allocating %1 of memory!").arg(bytestostr(memsize).c_str()));
                log_error(tr("Error allocating %1 of memory!").arg(bytestostr(memsize).c_str()));
            }
        } else {
            setError(tr("there are no files in the %1 record!").arg(getName()));
            log_error(tr("there are no files in the %1 record!").arg(getName()));
        }
    } else {
        setError(tr("Error while reading image stack: unknown stack type %1").arg(stacktype));
        log_error(tr("Error while reading image stack: unknown stack type %1").arg(stacktype));
        return;
    }

}

bool QFRDRImageStackData::loadImageFile(QFRDRImageStackData::ImageStack& stack, QString filename, loadMode mode, int channel, QFImageHalf whichHalfToLoad) {
	bool ok=true;
    QString filetype=getProperty("FILETYPE", "").toString();
    QStringList reader_id=QFRDRImageStackData::getImageReaderIDList(getProject()->getServices());
    int reader_idx=reader_id.indexOf(filetype);
    if (!reader_id.contains(filetype)) {
        setError(tr("Error while reading image: unknown file type %2 (file was: '%1')").arg(filename).arg(filetype));
        log_error(tr("Error while reading image: unknown file type %2 (file was: '%1')").arg(filename).arg(filetype));
        return false;
    }

    QFImporterImageSeries* reader=getImageReader(reader_idx, getProject()->getServices());
    if (reader) {
        if (reader->open(filename)) {
            stack.props=reader->getFileInfo().properties;
            stack.description=reader->getFileInfo().comment;
            if (mode==QFRDRImageStackData::lmGetSize) {
                stack.channels=1;
                stack.width=reader->frameWidth();
                stack.height=reader->frameHeight();
                stack.frames=reader->countFrames();
            } else {
                uint32_t i=0;
                do {
                    //qDebug()<<"read frame "<<i<<stack.frames<<stack.dvMode;
                    if (stack.dvMode==dvNone) {
                        double* d=&(stack.data[channel*stack.width*stack.height*stack.frames + i*stack.width*stack.height]);
                        reader->readFrameDouble(d);
                    } else {
                        double* tmp=(double*)qfMalloc(reader->frameWidth()*reader->frameHeight()*sizeof(double));
                        reader->readFrameDouble(tmp);
                        if (whichHalfToLoad==qfihAny) {
                            double* d1=&(stack.data[channel*stack.width*stack.height*stack.frames + i*stack.width*stack.height]);
                            double* d2=&(stack.data[(channel+1)*stack.width*stack.height*stack.frames + i*stack.width*stack.height]);
                            if (stack.dvMode==dvHorizontal) {
                                qfCopyImageHalf(d1, tmp, reader->frameWidth(), reader->frameHeight(), qfihLeft);
                                qfCopyImageHalf(d2, tmp, reader->frameWidth(), reader->frameHeight(), qfihRight);
                            } else if (stack.dvMode==dvHorizontal) {
                                qfCopyImageHalf(d1, tmp, reader->frameWidth(), reader->frameHeight(), qfihTop);
                                qfCopyImageHalf(d2, tmp, reader->frameWidth(), reader->frameHeight(), qfihBottom);
                            }
                        } else if (whichHalfToLoad!=qfihNone) {
                            double* d1=&(stack.data[channel*stack.width*stack.height*stack.frames + i*stack.width*stack.height]);
                            qfCopyImageHalf(d1, tmp, reader->frameWidth(), reader->frameHeight(), whichHalfToLoad);
                        }
                        qfFree(tmp);
                    }
                    //qDebug()<<"read frame "<<i<<" OK";
                    i++;
                } while (i<stack.frames && reader->nextFrame());
            }
            reader->close();
            delete reader;
        } else {
            ok=false;
            setError(tr("Could not open image file '%1' with reader %2").arg(filename).arg(filetype));
            log_error(tr("Could not open image file '%1' with reader %2").arg(filename).arg(filetype));
        }
    } else {
        ok=false;
        setError(tr("Could not allocate reader %2 for image file '%1'").arg(filename).arg(filetype));
        log_error(tr("Could not allocate reader %2 for image file '%1'").arg(filename).arg(filetype));
    }

    if (!ok) {
        setError(tr("Error while reading file '%1'").arg(filename));
        log_error(tr("Error while reading file '%1'").arg(filename));
    }
    return ok;
}

bool QFRDRImageStackData::allocateMemory() {
    clearMemory();
    bool ok=true;
    memsize=0;
    for (int i=0; i<stacks.size(); i++) {
        stacks[i].data=(double*)qfCalloc(stacks[i].channels*stacks[i].frames*stacks[i].height*stacks[i].width, sizeof(double));
        ok=ok && (stacks[i].data!=NULL);
        if (stacks[i].data) {
            memsize+=stacks[i].channels*stacks[i].frames*stacks[i].height*stacks[i].width*sizeof(double);
        }
    }
    if (!ok) clearMemory();
    if (ok && stacks.size()>0) maskInit(stacks[0].width, stacks[0].height);
    else maskDelete();
    return ok;
}

void QFRDRImageStackData::clearMemory() {
    for (int i=0; i<stacks.size(); i++) {
        if (stacks[i].data) qfFree(stacks[i].data);
        stacks[i].data=NULL;
    }
    memsize=0;
    maskDelete();
}


QVariant QFRDRImageStackData::getEnumeratedProperty(const QString &prop, int stack, QVariant defaultValue) const {
    QVariant res=getProperty(prop, defaultValue);
    if (stack>=0) return getProperty(prop+QString::number(stack), res);
    else return res;
}

QVariant QFRDRImageStackData::getDoubleEnumeratedProperty(const QString &prop, int stack, int channel, QVariant defaultValue) const {
    QVariant res=getProperty(prop, defaultValue);
    if (stack>=0) res=getProperty(prop+QString(stack), res);
    if (stack>=0) res=getProperty(prop+QString(stack), res);
    if (channel>=0) return getProperty(prop+"_"+QString::number(channel), res);
    if (stack>=0 && channel>=0) return  getProperty(prop+QString(stack)+"_"+QString::number(channel), res);
    else return res;
}

void QFRDRImageStackData::setStackProperties(const QMap<QString, QVariant> &props, int stack, int channel)
{
    QMapIterator<QString,QVariant> it(props);
    while (it.hasNext()) {
        it.next();
        if (!propertyExists(it.key())) {
            setQFProperty(QString(it.key())+QString::number(stack), it.value(), false, true);
        }

    }
}

int QFRDRImageStackData::getImageStackCount() const {
    return stacks.size();
}

uint32_t QFRDRImageStackData::getImageStackFrames(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.frames;
}

int QFRDRImageStackData::getImageStackWidth(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.width;
}

int QFRDRImageStackData::getImageStackHeight(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.height;
}

int QFRDRImageStackData::getImageStackChannels(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.channels;
}

double *QFRDRImageStackData::getImageStack(int stackID, uint32_t frame, uint32_t channel) const {
    QFRDRImageStackData::ImageStack stack=stacks.value(stackID, QFRDRImageStackData::ImageStack());
    if (stack.data==NULL) return NULL;
    else {
        if (frame>=stack.frames) return NULL;
        if ((int64_t)channel>=stack.channels) return NULL;
        return &(stack.data[channel*stack.width*stack.height*stack.frames+frame*stack.width*stack.height]);
    }
}

double QFRDRImageStackData::getImageStackXUnitFactor(int stack) const {
    return getEnumeratedProperty("XUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackXUnitName(int stack) const {
    return getEnumeratedProperty("XUNIT_NAME", stack, tr("pixels")).toString();
}

QString QFRDRImageStackData::getImageStackXName(int stack) const {
    return getEnumeratedProperty("XAXIS_NAME", stack, "x").toString();
}

double QFRDRImageStackData::getImageStackYUnitFactor(int stack) const {
    return getEnumeratedProperty("YUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackYUnitName(int stack) const {
    return getEnumeratedProperty("YUNIT_NAME", stack, tr("pixels")).toString();
}

QString QFRDRImageStackData::getImageStackYName(int stack) const {
    return getEnumeratedProperty("YAXIS_NAME", stack, "y").toString();
}

double QFRDRImageStackData::getImageStackTUnitFactor(int stack) const {
    return getEnumeratedProperty("TUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackTUnitName(int stack) const {
    return getEnumeratedProperty("TUNIT_NAME", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackTName(int stack) const {
    return getEnumeratedProperty("TAXIS_NAME", stack, "time").toString();
}

double QFRDRImageStackData::getImageStackCUnitFactor(int stack) const {
    return getEnumeratedProperty("CUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackCUnitName(int stack) const {
    return getEnumeratedProperty("YUNIT_NAME", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackCName(int stack) const {
    return getEnumeratedProperty("CAXIS_NAME", stack, tr("channel")).toString();
}

QString QFRDRImageStackData::getImageStackDescription(int stack) const {
    return getEnumeratedProperty("STACK_DESCRIPTION", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackChannelName(int stack, int channel) const {
    return getDoubleEnumeratedProperty("CHANNELNAME_", stack, channel, tr("channel %1").arg(channel+1)).toString();
}

QString QFRDRImageStackData::getImageStackTimepointName(int stack, int t) const {
    return getDoubleEnumeratedProperty("TIMEPOINTNAME_", stack, t, QString("")).toString();
}


QStringList QFRDRImageStackData::getImageFilterList(QFPluginServices* pluginservices)   {
    QStringList l;
    int i=0;
    QFImporterImageSeries* r=NULL;
    while ((r=getImageReader(i, pluginservices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFRDRImageStackData::getImageReaderIDList(QFPluginServices* pluginservices)   {
    return pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();
}

QStringList QFRDRImageStackData::getImageWriterFilterList(QFPluginServices *pluginservices)
{
    QStringList l;
    int i=0;
    QFExporterImageSeries* r=NULL;
    while ((r=getImageWriter(i, pluginservices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFRDRImageStackData::getImageWriterFormatNameList(QFPluginServices *pluginservices)
{
    QStringList l;
    int i=0;
    QFExporterImageSeries* r=NULL;
    while ((r=getImageWriter(i, pluginservices))!=NULL) {
        l.append(r->formatName());
        delete r;
        i++;
    }
    return l;
}

QFExporterImageSeries *QFRDRImageStackData::getImageWriter(int idx, QFPluginServices *pluginservices)
{
    QFExporterImageSeries* r=NULL;

    QStringList imp=pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFExporterImageSeries*>(pluginservices->getExporterManager()->createExporter(imp[idx]));
    }

    return r;
}

int QFRDRImageStackData::getImageWriterCount(QFPluginServices *pluginservices)
{
    QStringList imp=pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();
    return imp.size();
}

QStringList QFRDRImageStackData::getImageWriterIDList(QFPluginServices *pluginservices)
{
    return pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();
}

QStringList QFRDRImageStackData::getImageFormatNameList(QFPluginServices* pluginservices)   {
     QStringList l;
     int i=0;
     QFImporterImageSeries* r=NULL;
     while ((r=getImageReader(i, pluginservices))!=NULL) {
         l.append(r->formatName());
         delete r;
         i++;
     }
     return l;
}

QFImporterImageSeries* QFRDRImageStackData::getImageReader(int idx, QFPluginServices* pluginservices)  {
    QFImporterImageSeries* r=NULL;

    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFImporterImageSeries*>(pluginservices->getImporterManager()->createImporter(imp[idx]));
    }

    return r;
}

int QFRDRImageStackData::getImageReaderCount(QFPluginServices* pluginservices)  {
    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();
    return imp.size();
}

QFRDRImageStackData::ImageStack::ImageStack() {
    data=NULL;
    width=0;
    height=0;
    frames=0;
    channels=0;
    file=-1;
}



void QFRDRImageStackData::exportData(const QString &format_in, const QString &filename) const
{
    /*QStringList sl=format_in.split(";;;;");
    bool ok=true;
    int ch=sl.value(1, "0").toInt(&ok);
    if (!ok) ch=0;
    QString format=sl.value(0, "");*/
    QString format=format_in;
    if (getExportFiletypes().contains(format)) {
        QFExporterImageSeries* exp=dynamic_cast<QFExporterImageSeries*>(QFPluginServices::getInstance()->getExporterManager()->createExporter(format));
        if (exp) {
            exp->setFileComment(getName());
            exp->setFrameSize(getImageStackWidth(0), getImageStackHeight(0), 1);//getImageStackChannels(0));
            exp->setResolution(getImageStackXUnitFactor(0), getImageStackYUnitFactor(0), getImageStackZUnitFactor(0), getImageStackXUnitName(0));
            uint32_t w=getImageStackWidth(0);
            uint32_t h=getImageStackHeight(0);
            uint32_t f=getImageStackFrames(0);
            uint32_t ch=getImageStackChannels(0);
            /*if (exp->open(filename)){
                for (uint32_t i=0; i<f; i++) {
                    QVector<double> d(w*h*ch);
                    for (uint32_t c=0; c<ch; c++) {
                        double* dat=getImageStack(0, i, c);
                        for (uint32_t j=0; j<w*h; j++) {
                            d[c*w*h+j]=dat[j];
                        }
                    }
                    exp->writeFrameDouble(d.data());
                }
            }*/
            for (uint32_t c=0; c<ch; c++) {
                if (exp->open(QFileInfo(filename).absoluteDir(QFileInfo(filename).baseName()+QString("_ch%1.").arg(c)+QFileInfo(filename).completeSuffix()))){
                    for (uint32_t i=0; i<f; i++) {
                        QVector<double> d(w*h);
                        double* dat=getImageStack(0, i, c);
                        for (uint32_t j=0; j<w*h; j++) {
                            d[j]=dat[j];
                        }
                    }
                    exp->writeFrameDouble(d.data());
                    exp->close();
                }
            }
            delete exp;
        }
    }
}

QStringList QFRDRImageStackData::getExportFiletypes() const
{
    return getImageWriterIDList(QFPluginServices::getInstance());
}

QString QFRDRImageStackData::getExportDialogFiletypes() const
{
    return getImageWriterFilterList(QFPluginServices::getInstance()).join(";;");
}

bool QFRDRImageStackData::showNextPreviousOfSameRoleButton() const
{
    return false;
}
