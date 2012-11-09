#include "cam_rh2v2_cfgdlg.h"
#include "ui_cam_rh2v2_cfgdlg.h"

cam_rh2v2_cfgdlg::cam_rh2v2_cfgdlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cam_rh2v2_cfgdlg),
    isEditing(false)
{
    ui->setupUi(this);
    isCalculating=false;
    updateRuntime();
}

cam_rh2v2_cfgdlg::~cam_rh2v2_cfgdlg()
{
    delete ui;
}

void cam_rh2v2_cfgdlg::checkUpdate(){
    if(ui->cbCorr->isChecked()==true){
        int binning=ui->cbBinning->itemText(ui->cbBinning->currentIndex()).toInt();
        if(binning>0){
            ui->dsbFrameTime->setMinimum((ui->sbRoiLast->value()-ui->sbRoiFirst->value()+1)*10.0/(32.0*binning));
        }
    }else{
        int roi=ui->sbRoiLast->value()-ui->sbRoiFirst->value()+1;
        ui->dsbFrameTime->setMinimum(roi*4.0/48.0);
    }
    updateRuntime();
}

void cam_rh2v2_cfgdlg::on_sbRoiFirst_valueChanged(int value)
{
    if((!isCalculating)&&(!isEditing)){
        isCalculating=true;
        ui->sbRoiLast->setMinimum(value+1);
        checkUpdate();
        isCalculating=false;
    }
    updateRuntime();
}

void cam_rh2v2_cfgdlg::on_sbRoiLast_valueChanged(int value)
{
    if((!isCalculating)&&(!isEditing)){
        isCalculating=true;
        ui->sbRoiFirst->setMaximum(value-1);
        checkUpdate();
        isCalculating=false;
    }
    updateRuntime();
}

void cam_rh2v2_cfgdlg::on_sbFrameTime_valueChanged(double value)
{
    updateRuntime();
}

void cam_rh2v2_cfgdlg::on_cbBinning_currentIndexChanged(int index)
{
    if((!isCalculating)&&(!isEditing)){
        isCalculating=true;
        checkUpdate();
        isCalculating=false;
    }
    updateRuntime();
}

void cam_rh2v2_cfgdlg::on_cbCorr_stateChanged(int state){
    if((!isCalculating)&&(!isEditing)){
        isCalculating=true;
        checkUpdate();
        isCalculating=false;
    }
    updateRuntime();
}

void cam_rh2v2_cfgdlg::updateRuntime()
{
    double runtime=double(ui->sbFrameCnt->value())*ui->dsbFrameTime->value()/1000.0;
    double lag=ui->dsbFrameTime->value();
    if (ui->cbBinning->currentIndex()==1) { runtime=runtime*3.0; lag=lag*3.0; }
    ui->labRuntime->setText(tr("runtime = %1 s = %2 ms<br>correlator min. lagtime = %3 �s").arg(runtime/1000.0).arg(runtime).arg(lag));
}

int cam_rh2v2_cfgdlg::getAccumulationCount()
{
    return ui->sbAccCnt->value();
}

void cam_rh2v2_cfgdlg::setAccumulationCount(int value)
{
    ui->sbAccCnt->setValue(value);
}

int cam_rh2v2_cfgdlg::getFrameCount()
{
    return ui->sbFrameCnt->value();
}

void cam_rh2v2_cfgdlg::setFrameCount(int value)
{
    ui->sbFrameCnt->setValue(value);
    updateRuntime();
}

int cam_rh2v2_cfgdlg::getRoiFirst()
{
    return ui->sbRoiFirst->value();
}

void cam_rh2v2_cfgdlg::setRoiFirst(int value)
{
    ui->sbRoiFirst->setValue(value);
    updateRuntime();
}

float cam_rh2v2_cfgdlg::getFrameTime()
{
    return ui->dsbFrameTime->value();
}

void cam_rh2v2_cfgdlg::setFrameTime(float value)
{
    ui->dsbFrameTime->setValue(value);
    updateRuntime();
}

int cam_rh2v2_cfgdlg::getBinning()
{
    return ui->cbBinning->currentText().toInt();
}

void cam_rh2v2_cfgdlg::setBinning(int value)
{
    ui->cbBinning->setCurrentIndex(ui->cbBinning->findText(QString::number(value)));
    updateRuntime();
}

void cam_rh2v2_cfgdlg::setEditMode()
{
    isEditing=true;
    updateRuntime();
}

void cam_rh2v2_cfgdlg::unsetEditMode()
{
    isEditing=false;
    checkUpdate();
}

int cam_rh2v2_cfgdlg::getRoiLast()
{
    return ui->sbRoiLast->value();
}

void cam_rh2v2_cfgdlg::setRoiLast(int value)
{
    ui->sbRoiLast->setValue(value);
    updateRuntime();
}

bool cam_rh2v2_cfgdlg::getCorrEnable(){
    return ui->cbCorr->isChecked();
}

void cam_rh2v2_cfgdlg::setCorrEnable(bool value){
    ui->cbCorr->setChecked(value);
    updateRuntime();
}
