#ifndef QFRDRFCSFITFUNCTIONSIMULATOR_H
#define QFRDRFCSFITFUNCTIONSIMULATOR_H

#include <QDialog>
#include <QTabBar>
#include <QVBoxLayout>
#include "jkverticalscrollarea.h"
#include "qffitfunctionmanager.h"
#include "qffitfunction.h"
#include <QMap>
#include "qffitparameterwidget.h"
#include "MersenneTwister.h"
#define sqr(x) ((x)*(x))

class QFRDRFCSData;

namespace Ui {
    class QFRDRFCSFitFunctionSimulator;
}

class QFRDRFCSFitFunctionSimulator : public QDialog, public QFFitParameterBasicInterface {
        Q_OBJECT
        Q_INTERFACES(QFFitParameterBasicInterface)
        
    public:
        explicit QFRDRFCSFitFunctionSimulator(QFPluginServices *services, QWidget *parent = 0);
        ~QFRDRFCSFitFunctionSimulator();

        QString getCSV() const {
            return csv;
        }

        QMap<QString, QVariant> getParams();


        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(const QString& id, double value);
        /*! \brief return the value of a given parameter        */
        virtual double getFitValue(const QString& id) const;
        /*! \brief return the fit error of a given parameter
            \return the error associated with the given parameter.
                    The error may only be stored in the QFRawDataRecord, if it is not available or accessible, then \c 0 is returned
        */
        virtual double getFitError(const QString& id) const;
        /*! \brief set the error of a given parameter

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(const QString& id, double error);

        /*! \brief set the fix property of a fit parameter of the current fit function
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix value to be stored
         */
        virtual void setFitFix(const QString& id, bool fix) {};
        /*! \brief return the fix property of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual bool getFitFix(const QString& id) const;
        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
            \param max upper bound for the value

         */
        virtual void setFitRange(const QString& id, double min, double max);

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
         */
        virtual void setFitMin(const QString& id, double min);

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param max upper bound for the value
         */
        virtual void setFitMax(const QString& id, double max);
        /*! \brief return the lower value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMin(const QString& id) const ;
        /*! \brief return the upper value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMax(const QString& id) const;


        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value in the initial parameters
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param error error assigned to the given \a value

         */
        virtual void setInitFitValue(const QString& id, double value, double error=0.0);
        /*! \brief set the fix property of a fit parameter in the initial parameters
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix value to be stored
         */
        virtual void setInitFitFix(const QString& id, bool fix) {};

        /*! \brief set the given parameter \a id to the given value (and error) in all files */
        virtual void setAllFitValues(const QString& id, double value, double error=0.0, bool currentFileOnly=false) {};
        /*! \brief set the given parameter \a id to the given fix value */
        virtual void setAllFitFixes(const QString& id, bool fix, bool currentFileOnly=false) {};
        /*! \brief reset the given parameter \a id to the initial/global/default value */
        virtual void resetDefaultFitValue(const QString& id) {};
        /*! \brief reset the given parameter \a id to the initial/global/default fix */
        virtual void resetDefaultFitFix(const QString& id);
        /*! \brief return the default/initial/global value of a given parameter        */
        virtual double getDefaultFitValue(const QString& id) const;
        /*! \brief return the default/initial/global fix of a given parameter        */
        virtual bool getDefaultFitFix(const QString& id)  const;
        /*! \brief reset the all parameters to the initial/global/default value in all files */
        virtual void resetAllFitValue() {};
        /*! \brief reset the all parameters to the initial/global/default fix in all files */
        virtual void resetAllFitFix() {};
        /*! \brief reset the all parameters to the initial/global/default value in current files */
        virtual void resetAllFitValueCurrent() {};
        /*! \brief reset the all parameters to the initial/global/default fix in current files */
        virtual void resetAllFitFixCurrent() {};

    signals:
        void insertRecord();
    private:
        Ui::QFRDRFCSFitFunctionSimulator *ui;
        QTabBar* tbEditRanges;
        JKVerticalScrollArea* scrollParameters;
        QGridLayout* layParameters;
        QFPluginServices *services;
        QMap<QString, QFFitFunction*> m_fitFunctions;
        QList<QPointer<QFFitParameterWidgetWrapper> > m_fitParameters;
        QFFitFunction* getFitFunction() const;
        struct paramvalue {
            double value;
            bool valueset;
            double min;
            bool minset;
            double max;
            bool maxset;
            double error;

            paramvalue() {
                value=error=min=max=0;
                valueset=minset=maxset=false;
            }
        };

        QMap<QString, paramvalue> params;
        QMap<QString, QVariant> used_params;

        double* tau;
        int tauN;
        int runs;
        double* corr;
        void updateTau();
        QString csv;
    protected slots:
        void updateFitFunction();
        void updateParameterValues();
        void modelChanged(int index);
        void displayModel(bool newWidget);
        void replotFitFunction();
        void on_cmbNoiseModel_currentIndexChanged(int index);
        void updateNoiseEnabled();
        void showHelp();
        void on_btnModelHelp_clicked();
        void on_btnAddRecord_clicked();
};

#endif // QFRDRFCSFITFUNCTIONSIMULATOR_H
