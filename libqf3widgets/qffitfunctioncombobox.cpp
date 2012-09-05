#include "qffitfunctioncombobox.h"
#include "qffitfunctionmanager.h"

QFFitFunctionComboBox::QFFitFunctionComboBox(QWidget *parent) :
    QComboBox(parent)
{
    updateFitFunctions("");
}

QString QFFitFunctionComboBox::currentFitFunctionID() const
{
    return itemData(currentIndex()).toString();
}

QFFitFunction *QFFitFunctionComboBox::createCurrentInstance(QObject* parent) const
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    return manager->createFunction(currentFitFunctionID(), parent);
}

void QFFitFunctionComboBox::setCurrentFitFunction(const QString &id)
{
    int idx=findData(id);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
}

void QFFitFunctionComboBox::updateFitFunctions(const QString &filter)
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    clear();
    QMap<QString, QFFitFunction*> m_fitFunctions=manager->getModels(filter, this);
    QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
    while (it.hasNext())  {
        it.next();
        addItem(QIcon(":/lib/fitfunc_icon.png"), it.value()->shortName(), it.key());
        delete it.value();
    }
    setUpdatesEnabled(upd);
}