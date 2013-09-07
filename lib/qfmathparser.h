/*
  Name: qfmathparser.h
  Copyright: (c) 2007-2013
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/*! \defgroup qf3lib_mathtools_parser parser for mathematical expressions
    \ingroup qf3lib_mathtools

 In this group there are classes that form a parser and evaluator for mathematical expressions.
 In the context of the sequencer program this is a tool class that can be used by the classes
 in the project. E.g. by jkiniparser which allows for mathematical expressions in the configuration
 files.

 */

/** \file QFMathParser.h
 *  \ingroup qf3lib_mathtools_parser
 */
#include "lib_imexport.h"
#include "qftools.h"
#include <cmath>
#include <QTextIStream>
#include <QTextStream>

#include <QList>
#include <QMap>
#include <QList>
#include <QString>
#include <QPair>
#include <QStack>
#include "../extlibs/MersenneTwister.h"
#include <stdint.h>
#include <QDebug>

#ifndef QFMATHPARSER_H
#define QFMATHPARSER_H

class QFMathParser; // forward

/**
 * \defgroup qfmpmain main function parser class
 *  \ingroup qf3lib_mathtools_parser
 */
/*@{*/

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric argument, e.g. sin(x)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return ;\
    }\
    if (params[0].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].num));\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.setDoubleVec(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, CFUNC, CFUNC)



/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and returns a vector

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to any number parameter
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDoubleVec(CFUNC(params[0].numVec));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and returns a number, e.g. qfstatisticsMedian()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].numVec));\
    } else {\
        p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one numeric vector argument and one numeric argument and returns a number, e.g. qfstatisticsQuantile()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, p) needs exacptly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {\
        r.setDouble(CFUNC(params[0].numVec, params[1].num));\
    } else {\
        p->qfmpError(QObject::tr("%1(x, p) argument x has to be a vector of numbers and p a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric vector arguments and returns a number, e.g. qfstatisticsCorrelationCoefficient()

    The resulting function will:
      - check the number of arguments and their type
      - apply the C-function to the argument
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM2VEC_VECTONUM_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(x, y) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        r.setDouble(CFUNC(params[0].asVector(), params[1].asVector()));\
    } else {\
        p->qfmpError(QObject::tr("%1(x, y) arguments x and y have to be a vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has one string (QString!!!) argument, e.g. sin(x)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any string parameter
      - result is also string
    .

    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpString;\
    r.isValid=true;\
    if (n!=1) {\
        p->qfmpError(QObject::tr("%1(...) needs exacptly 1 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpString) {\
        r.str=CFUNC(params[0].str);\
    } else {\
        p->qfmpError(QObject::tr("%1(...) argument has to be a string").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(FName, CFUNC, CFUNC)


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. yn(p, x), p is a number, x may be a vector

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.type=qfmpDouble;\
    r.isValid=true;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    if (params[0].type==qfmpDouble) {\
        pa=params[0].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[1].type==qfmpDouble) {\
        r.num=CFUNC(pa, params[1].num);\
    } else if(params[1].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[1].numVec.size());\
        for (int i=0; i<params[1].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(pa, params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. yn(x, p), p is a number, x may be a vector

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    double pa=0; \
    if (params[1].type==qfmpDouble) {\
        pa=params[1].num;\
    } else {\
        p->qfmpError(QObject::tr("%1(...) second argument has to be a number").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble) {\
        r.num=CFUNC(params[0].num, pa);\
    } else if(params[0].type==qfmpDoubleVector) {\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], pa);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}


/*! \brief This macro allows to easily define functions for QFMathParser from a C-function that
           has two numeric argument, e.g. fmod(x, y),  x and y may be a vector (both, same length) or a number (both)

    The resulting function will:
      - check the number of arguments
      - apply the C-function to any number parameter
      - apply the C-function item-wise to any number vector parameter, i.e. [ fmod(x1,y1), fmod(x2,y2), ... ]
      - result is also a number or number vector
    .
    \param FName name of the function to declare
    \param NAME_IN_PARSER name the function should have in the parser (used for error messages only)
    \param CFUNC name of the C function to call
*/
#define QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC) \
void FName(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){\
    r.isValid=true;\
    r.type=qfmpDouble;\
    if (n!=2) {\
        p->qfmpError(QObject::tr("%1(...) needs exactly 2 argument").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
        return; \
    }\
    if (params[0].type==qfmpDouble && params[1].type==qfmpDouble) {\
    r.num=CFUNC(params[0].num, params[1].num);\
    } else if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {\
        if (params[0].numVec.size()!=params[1].numVec.size()) {\
            p->qfmpError(QObject::tr("%1(x,y) both arguments have to have same length").arg(#NAME_IN_PARSER));\
            r.setInvalid();\
            return; \
        }\
        r.type=qfmpDoubleVector;\
        r.numVec.resize(params[0].numVec.size());\
        for (int i=0; i<params[0].numVec.size(); i++) {\
            r.numVec[i]=CFUNC(params[0].numVec[i], params[1].numVec[i]);\
        }\
    } else {\
        p->qfmpError(QObject::tr("%1(...) first argument has to be a number or vector of numbers").arg(#NAME_IN_PARSER));\
        r.setInvalid();\
    }\
    return; \
}

/** \brief same as QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)

/** \brief same as QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, NAME_IN_PARSER, CFUNC), but NAME_IN_PARSER==CFUNC */
#define QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(FName, CFUNC) QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(FName, CFUNC, CFUNC)




/**
 * \defgroup qfmpultil utilities for QFMathParser function parser class
 * \ingroup qf3lib_mathtools_parser
 */
/*@{*/

/** possible result types
 *    - \c qfmpDouble: a floating-point number with double precision. This is
 *                     also used to deal with integers
 *    - \c qfmpString: a string of characters
 *    - \c qfmpBool:   a boolean value true|false
 *  .
 */
enum qfmpResultType {qfmpDouble=0x01,  /*!< \brief a floating-point number with double precision. This is also used to deal with integers */
                     qfmpString=0x02,  /*!< \brief a string of characters */
                     qfmpBool=0x04,   /*!< \brief a boolean value true|false */
                     qfmpDoubleVector=0x08,  /*!< \brief a vector of floating point numbers */
                     qfmpStringVector=0x10, /*!< \brief a vector of strings */
                     qfmpBoolVector=0x20, /*!< \brief a vector of booleans */
                     qfmpVoid=0x40  /*!< \brief a vector of floating point numbers */
                     };

/** \brief result of any expression  */
struct QFLIB_EXPORT qfmpResult {
    public:
        qfmpResult();
        qfmpResult(double value);
        qfmpResult(int value);
        qfmpResult(QString value);
        qfmpResult(bool value);
        qfmpResult(const QVector<double> &value);
        qfmpResult(const QVector<bool> &value);
        qfmpResult(const QStringList &value);

        qfmpResult(const qfmpResult &value);
        //qfmpResult(qfmpResult &value);
        //explicit qfmpResult(qfmpResult value);
        qfmpResult& operator=(const qfmpResult &value);

         QFLIB_EXPORT void setInvalid();
         QFLIB_EXPORT void setVoid();
        /** \brief convert the value this struct representens into a QString */
         QFLIB_EXPORT QString toString(int precision=10) const;

        /** \brief convert the value this struct representens into a QString and adds the name of the datatype in \c [...] */
         QFLIB_EXPORT QString toTypeString(int precision=10) const;

        /** \brief convert the value this struct to an integer */
         QFLIB_EXPORT int32_t toInteger() const;
        /** \brief convert the value this struct to an integer */
         QFLIB_EXPORT uint32_t toUInt() const;
        /** \brief is this result convertible to integer? */
         QFLIB_EXPORT bool isInteger() const;
        /** \brief is this result convertible to unsigned integer? */
         QFLIB_EXPORT bool isUInt() const;
        /** \brief returns the size of the result (number of characters for string, numbers of entries in vectors, 0 for void and 1 else) */
         QFLIB_EXPORT int length() const;

         QFLIB_EXPORT void setDouble(double val);
         QFLIB_EXPORT void setBoolean(bool val);
         QFLIB_EXPORT void setString(const QString& val);
         QFLIB_EXPORT void setDoubleVec(const QVector<double>& val);
         QFLIB_EXPORT void setDoubleVec(int size=0, double defaultVal=0);
         QFLIB_EXPORT void setBoolVec(const QVector<bool>& val);
         QFLIB_EXPORT void setBoolVec(int size=0, bool defaultVal=false);
         QFLIB_EXPORT void setStringVec(const QStringList& val);
         QFLIB_EXPORT void setStringVec(int size=0, const QString& defaultVal=QString(""));
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
         QFLIB_EXPORT QVector<double> asVector() const;
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
         QFLIB_EXPORT QStringList asStrVector() const;
        /** \brief converst the result to a vector of number (numbers and number vectors are converted!) */
         QFLIB_EXPORT QVector<bool> asBoolVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of number */
         QFLIB_EXPORT bool  convertsToVector() const;
        /** \brief converst the result to a vector of integers (numbers and number vectors are converted!) */
         QFLIB_EXPORT QVector<int> asIntVector() const;
        /** \brief returns \c true, if the result may be converted to a vector of integers */
         QFLIB_EXPORT bool  convertsToIntVector() const;
        /** \brief returns \c true if the result is valid and not void */
         QFLIB_EXPORT bool isUsableResult() const;
        /** \brief converst the result to a number (numbers are converted!) */
         QFLIB_EXPORT double asNumber() const;
         /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
          QFLIB_EXPORT double asNumberAlsoVector() const;
          /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
           QFLIB_EXPORT QString asStringAlsoVector() const;
           /** \brief converst the result to a number (numbers are converted and from a number vector the first element is returned!) */
            QFLIB_EXPORT bool asBooleanAlsoVector() const;
        /** \brief converst the result to a string (strings are converted!) */
         QFLIB_EXPORT QString asString() const;
        /** \brief converst the result to a boolean (numbers and booleans are converted!) */
         QFLIB_EXPORT bool asBool() const;
        /** \brief returns the type */
         QFLIB_EXPORT qfmpResultType getType() const;
        /** \brief returns a string, describing the type of this result */
        QFLIB_EXPORT QString typeName() const;

        /** \brief returns an invalid result */
        QFLIB_EXPORT static qfmpResult invalidResult();
        /** \brief returns an void result */
        QFLIB_EXPORT static qfmpResult voidResult();

        QFLIB_EXPORT static qfmpResult add(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult sub(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult mul(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult div(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult mod(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult power(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult bitwiseand(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult bitwiseor(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult logicand(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult logicor(const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult logicnot(const qfmpResult& l, QFMathParser *p);
        QFLIB_EXPORT static qfmpResult neg(const qfmpResult& l, QFMathParser* p);
        QFLIB_EXPORT static qfmpResult bitwisenot(const qfmpResult& l, QFMathParser* p);

        QFLIB_EXPORT static void add(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void sub(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void mul(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void div(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void mod(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void power(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void bitwiseand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void bitwiseor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicnot(qfmpResult& result, const qfmpResult& l, QFMathParser *p);
        QFLIB_EXPORT static void logicnand(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicnor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void logicxor(qfmpResult& result, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void neg(qfmpResult& result, const qfmpResult& l, QFMathParser* p);
        QFLIB_EXPORT static void bitwisenot(qfmpResult& result, const qfmpResult& l, QFMathParser* p);

        QFLIB_EXPORT static void la_add(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        /*QFLIB_EXPORT static void la_sub(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_mul(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_div(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_mod(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_power(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_bitwiseand(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_bitwiseor(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_logicand(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_logicor(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_logicnot(qfmpResult& l, QFMathParser *p);
        QFLIB_EXPORT static void la_logicnand(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_logicnor(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_logicxor(qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void la_neg(qfmpResult& l, QFMathParser* p);
        QFLIB_EXPORT static void la_bitwisenot(qfmpResult& l, QFMathParser* p);*/

        QFLIB_EXPORT bool operator==(const qfmpResult& other) const;
        QFLIB_EXPORT bool operator!=(const qfmpResult& other) const;

        QFLIB_EXPORT static void compareequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparenotequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparegreater(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparegreaterequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparesmaller(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);
        QFLIB_EXPORT static void comparesmallerequal(qfmpResult& res, const qfmpResult& l, const qfmpResult& r, QFMathParser* p);

        bool isValid;
        qfmpResultType type;   /*!< \brief type of the result */
        QString str;       /*!< \brief contains result if \c type==qfmpString */
        double num;            /*!< \brief contains result if \c type==qfmpDouble */
        bool boolean;          /*!< \brief contains result if \c type==qfmpBool */
        QVector<double> numVec; /*!< \brief contains result if \c type==qfmpDoubleVector */
        QStringList strVec;
        QVector<bool> boolVec;

};
/*@}*/


/*! \brief A simple function parser to parse (build memory tree representation) and
           evaluate simple mathematical expressions
    \ingroup qf3lib_mathtools_parser
 This class implements a simple function parser which can parse
 mathematical expressions like <code> z=a*3+2.34^2*sin(pi*sqrt(x))</code> .
 More than one expression can be separated by semicolon ';'. The result of
 a parse operation will be a memory structure (tree) representing the given
 expression.
 The parser can cope with constants and (user defined) variables and supports
 the data types number (double precision floating point), boolean (true/false)
 and string. It already contains a lot of fundamental mathematical functions
 (i.e. nearly all the functions from C StdLib).

 \section qfmp_constantsAndVars constants and variables:
 This class provides management utilities for constants and variables. Variables
 can also be defined as external variables, when a pointer to corresponding
 memory is provided by the calling program. The parser supports a variable
 assign operation \code a=<expression>\endcode  which allows to define new
 variables during evaluation. There are some mathematical standard constants
 registered by calling QFMathParser::addStandardVariables():
   - \c pi = \f$ \pi \f$
   - \c e = \f$ \exp(1) \f$
   - \c sqrt2 = \f$ \sqrt{2} \f$
   - \c version = the parser version
   - \c log2e = \f$ \log_2(e) \f$
   - \c log10e = \f$ \log_{10}(e) \f$
   - \c ln2 = \f$ \ln(2)=\log_{e}(2) \f$
   - \c ln10 = \f$ \ln(10)=\log_{e}(10) \f$
   - \c h = \f$ 6.6260689633\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
   - \c hbar = \f$ \hbar=\frac{h}{2\pi}= 1.05457162853\cdot 10^{-34}\;\mathrm{J\cdot s} \f$ (planck constant)
   - \c epsilon0 = \f$ \epsilon_0= 8.854187817\cdot 10^{-12}\;\mathrm{\frac{F}{m}} \f$ (electric constant)
   - \c mu0 = \f$ \mu_0=2\pi\cdot 10^{-7}= 12.566370614\cdot 10^{-7}\;\mathrm{\frac{N}{A^2}} \f$ (magnetic constant)
   - \c c = \f$ 299792458\;\mathrm{\frac{m}{s}} \f$ (speed of light in vacuum)
   - \c ce = \f$ 1.60217648740\cdot 10^{-19}\;\mathrm{C}\f$ (elementary charge)
   - \c muB = \f$ \mu_B= 927.40091523\cdot 10^{-26}\;\mathrm{\frac{J}{T}} \f$ (Bohr magneton)
   - \c muB_eV = \f$ \mu_B=5.788381755579\cdot 10^{-5}\;\mathrm{\frac{eV}{T}}  \f$ (Bohr magneton)
   - \c muN = \f$ \mu_N=5.0507832413\cdot 10^{-27}\;\mathrm{\frac{J}{T}}  \f$ (nuclear magneton)
   - \c muN_eV = \f$ \mu_N=3.152451232645\cdot 10^{-8}\;\mathrm{\frac{eV}{T}}  \f$ (nuclear magneton)
   - \c me = \f$ m_e= 9.1093821545\cdot 10^{-31}\;\mathrm{kg} \f$ (mass of electron)
   - \c mp = \f$ m_p= 1.67262163783\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of proton)
   - \c mn = \f$ m_n= 1.67492721184\cdot 10^{-27}\;\mathrm{kg} \f$ (mass of neutron)
   - \c NA = \f$ N_A= 6.0221417930\cdot 10^{23} \f$ (Avogadro constant = particles in 1 mol)
   - \c kB = \f$ k_B=1.380650424\cdot 10^{-23}\;\mathrm{\frac{J}{K}}  \f$ (Boltzman constant)
   - \c kB_eV = \f$ k_B=8.61734315\cdot 10^{-5}\;\mathrm{\frac{eV}{K}}  \f$ (Boltzman constant)
.
 You can add user-defined contants by calling QFMathParser::addVariableDouble()
 QFMathParser::addVariableBoolean() or QFMathParser::addVariableString()


 \section qfmp_functions functions:
 this class provides a wide range of (mathematical) functions:
   - sinc, gauss, slit, theta, tanc, sigmoid
   - asin, acos, atan, atan2,
   - sin, cos, tan,
   - sinh, cosh, tanh
   - log, log2, log10,
   - exp, sqr, sqrt
   - abs, sign
   - if
   - erf, erfc, lgamma, tgamma, j0, j1, jn, y0, y1, yn
   - rand, srand
   - ceil, floor, trunc, round,
   - fmod, min, max
   - floattostr, boolToQString
   - ...
 .

 these functions are registere by calling QFMathParser::addStandardFunctions().
 you can add new functions by calling QFMathParser::addFunction();

 \section qfmp_resultsofparsing result of parsing and evaluation:
 The result of calling QFMathParser::parse()
 will be a tree-like structure in memory. The parse() function will return
 a pointer to the root node of this structure. All nodes inherit from
 qfmpNode class. To evaluate such a structure simply call qfmpNode::evaluate()
 of the root node. This will then return a qfmpResult structure which contains
 the result. This scheme allows for once parsing and multiply evaluating an expression.
 So if you want to define a function by an expression you can provide an
 external variable x as the argument and then evaluate the function for
 each x.

 Evaluation can either be done with the results being returned as return-values (~5-10% slower)
 or with call-by-reference return-values.
 The third possibility is to translate the memory tree into an internal bytecode:
 \code
QFMathParser parser
parser.resetErrors();
QFMathParser::qfmpNode* n=parser.parse("sin(2*pi*(x^2+y^2))");
qfmpResult r=n->evaluate();
qDebug()<<expr<<"       =  "<<r.toTypeString()<<"\n";
QFMathParser::ByteCodeProgram bprog;
QFMathParser::ByteCodeEnvironment bcenv(&parser);
bcenv.init(&parser);
if (n->createByteCode(bprog, &bcenv)) {
    double rr=NAN;
    qDebug()<<expr<<"  =[BC]=  "<<(rr=parser.evaluateBytecode(bprog))<<"\n";
    qDebug()<<"\n-----------------------------------------------------------\n"<<QFMathParser::printBytecode(bprog)<<"\n-----------------------------------------------------------\n";
}
 \endcode
 This allows to evaluate the expression ~10-20 times fast, but not all features are supported, see \link qfmpbytecode bytecode (only number- and boolean values, limited support for
 variables, limitied support for user-defined functions (no recursion), ...).

 Here are some test-results (AMD QuadCore, 32-bit, 10000 evaluations each, gcc 4.4 no optimization, debug-build):
   - \c sqrt(a+b)+sin(b+c)+sqrt(a-c)+sin(b+a)+cos(a+c)+sqrt(b+b)+cos(a+a)+sqrt(-c)+sin(a*c)+cos(b+5.0) native: 8.7ms, return-value-evaluation: 945ms, call-by-reference: 891ms, bytecode: 40ms
   - \c cos(sqrt(a+b)+c*a) native: 1.8ms, return-value-evaluation: 186ms, call-by-reference: 177ms, bytecode: 13ms
 .
 Note that the speed-loss compared to the native execution depends on the compiler, optimization and especially the expression itself. Using a -O2 release build, the results
 of the parser  are improved ~2-fold.


 \section qfmp_ebnf EBNF definition of the parsed expressions

<pre> logical_expression ->  logical_term
                      | logical_expression <b>or</b> logical_term
                      | logical_expression <b>||</b> logical_term</pre>
<pre> logical_term    ->  comp_expression
                      | logical_term <b>and</b> comp_expression
                      | logical_term <b>&amp;&amp;</b> comp_expression</pre>
<pre> comp_expression  ->  math_expression
                      | expression <b>==</b> math_expression
                      | expression <b>!=</b> math_expression
                      | expression <b>&gt;=</b> math_expression
                      | expression <b>&lt;=</b> math_expression
                      | expression <b>&gt;</b> math_expression
                      | expression <b>&lt;</b> math_expression</pre>
<pre>  math_expression ->  term
                      | math_expression <b>+</b> math_term
                      | math_expression <b>-</b> math_term
                      | math_expression <b>|</b> math_term</pre>
<pre>  math_term       ->  vec_primary
                      | term <b>*</b> vec_primary
                      | term <b>/</b> vec_primary
                      | term <b>&amp;</b> vec_primary
                      | term ( <b>%</b> | <b>mod</b> ) vec_primary</pre>
<pre>  vec_primary     ->  primary
                      | primary <b>:</b> primary
                      | primary <b>:</b> primary <b>:</b> primary
                      | <b>[</b> vector_list <b>]</b>
<pre>  primary         ->  <b>true</b> | <b>false</b>
                      | string_constant
                      | NUMBER
                      | NAME
                      | <b>sum</b> | <b>prod</b> <b>(</b> logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression, logical_expression, logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression, logical_expression <b>)</b>
                      | vectorops_name <b>(</b> NAME, logical_expression, logical_expression <b>)</b>
                      | NAME <b>=</b> logical_expression
                      | NAME<b>[</b> logical_expression <b>] =</b> logical_expression
                      | NAME<b>[</b> logical_expression <b>]</b>
                      | NAME<b>(</b> parameter_list <b>)</b>
                      | NAME(parametername_list) <b>=</b> logical_expression
                      | <b>+</b> primary | <b>-</b> primary | <b>!</b> primary | <b>not</b> primary | <b>~</b> primary
                      | <b>(</b> logical_expression <b>)</b>
                      | primary <b>^</b> primary</pre>

<pre>  string_constant -> <b>&quot;</b> STRING <b>&quot;</b> | <b>&apos;</b> STRING <b>&apos;</b></pre>
<pre>  parameter_list  ->  \f$ \lambda \f$ | logical_expression | logical_expression <b>,</b> parameter_list</pre>
<pre>  parametername_list  ->  \f$ \lambda \f$ | NAME | NAME <b>,</b> parametername_list</pre>
<pre>  vector_list  ->  logical_expression | logical_expression <b>,</b> vector_list</pre>
<pre>  vectorops_name  ->  <b>sum</b> | <b>prod</b> | <b>for</b> </pre>




  \section qfmp_example Simple Example of Usage
     \code
        QFMathParser mp; // instanciate
        qfmpNode* n;
        qfmpResult r;
        // parse some numeric expression
        n=mp.parse("pi^2+4*sin(65*pi/exp(3.45))");
        r=n->evaluate();
        cout<<r.num<<endl;
        //delete n;

        // parse some boolean expression
        n=mp.parse("true==false");
        if (!mp.hasErrorOccured()) {
            r=n->evaluate();
            if (r.type==qfmpBool) {
                if (r.boolean) cout<<"true";
                else cout<<"false";
            }
            if (r.type==qfmpDouble) cout<<r.num<<endl;
            if (r.type==qfmpString) cout<<r.str<<endl;
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }
        if (n) delete n;

        // parse some string expression
        n=mp.parse("var1='false'; var1+'true'");
        if (!mp.hasErrorOccured()) {
            r=n->evaluate();
            if (r.type==qfmpString) cout<<r.str<<endl;
        }
        if (mp.hasErrorOccured()) {
            qDebug()<<"ERROR: "<<mp.getLastError();
        }
        if (n) delete n;
     \endcode



 */
class QFLIB_EXPORT QFMathParser
{
    protected:

        /** \brief the possible tokens that can be recognized by the tokenizer in QFMathParser::getToken() */
        enum qfmpTokenType {
            END,                /*!< \brief end token */
            PRINT,              /*!< \brief a semicolon ';' */
            COMMA,              /*!< \brief a comma ',' between two function parameters */
            STRING_DELIM,       /*!< \brief a string delimiter ' or " */
            NAME,               /*!< \brief a name (consisting of characters) of a variable or function */
            NUMBER,             /*!< \brief a number in scientific notation */
            PLUS,               /*!< \brief a plus operator '+' */
            MINUS,              /*!< \brief a minus operator '-' */
            MUL,                /*!< \brief a multiplication operator '*' */
            DIV,                /*!< \brief a division operator '/' */
            MODULO,             /*!< \brief a modulo operator '%' */
            ASSIGN,             /*!< \brief a variable assignment = */
            LPARENTHESE,        /*!< \brief left parentheses '(' */
            RPARENTHESE,        /*!< \brief right parentheses ')' */
            LBRACKET,           /*!< \brief left bracket '[' */
            RBRACKET,           /*!< \brief right bracket ']' */
            LBRACE,             /*!< \brief left brace '{' */
            RBRACE,             /*!< \brief right brace '}' */
            POWER,              /*!< \brief a power operator '^' */
            COLON,              /*!< \brief a colon ':' */
            TILDE,              /*!< \brief a tilde symbol '~' */
            FACTORIAL_LOGIC_NOT, /*!< \brief a factorial operator or a logical NOT '!' */
            LOGIC_NOT,          /*!< \brief a logical NOT '!' / 'not' */
            LOGIC_AND,          /*!< \brief a logical AND operator '&&' / 'and' */
            LOGIC_OR,           /*!< \brief a logical OR operator '||' / 'or' */
            LOGIC_XOR,          /*!< \brief a logical XOR operator 'xor' */
            LOGIC_NOR,          /*!< \brief a logical NOR operator 'nor' */
            LOGIC_NAND,         /*!< \brief a logical NAND operator 'nand' */
            LOGIC_TRUE,         /*!< \brief 'true' */
            LOGIC_FALSE,        /*!< \brief  'false' */
            BINARY_AND,          /*!< \brief a binary AND operator '&' */
            BINARY_OR,           /*!< \brief a binary OR operator '|' */
            COMP_EQUALT,        /*!< \brief equals operation '==' */
            COMP_UNEQUAL,       /*!< \brief unequal operation '!=' */
            COMP_GREATER,       /*!< \brief greater than operation '>' */
            COMP_SMALLER,       /*!< \brief smaller than operation '<' */
            COMP_GEQUAL,        /*!< \brief greater than or equal operation '>=' */
            COMP_SEQUAL        /*!< \brief smaller than or equal operation '<=' */
        };



        /** \brief internal names for logic/binary operations */
        enum {
            qfmpLOPand='a',
            qfmpLOPor='o',
            qfmpLOPxor='x',
            qfmpLOPnor='r',
            qfmpLOPnand='d',
            qfmpLOPnot='n'

        };


        /** \brief qfmpCOMPdefs internal names for compare operations */
        enum {
            qfmpCOMPequal='=',
            qfmpCOMPnequal='!',
            qfmpCOMPlesser='<',
            qfmpCOMPgreater='>',
            qfmpCOMPlesserequal='a',
            qfmpCOMPgreaterequal='b'
        };





        /*! \defgroup qfmpbytecode utilities for QFMathParser function parser class, that allow to build a bytecode program from a parse tree
            \ingroup qf3lib_mathtools_parser

            Sometimes the evaluation of the expression can be done faster, if the tree structure is translated into a bytecode run on a simple
            stack machine. To do so, the program has to be translated into that bytecode and has to meet certain conditions:
               # Only numbers (and booleans, as \c false==(number!=0)) may be used, no strings or number vectors
               # For evaluated functions, the C-function call of the form double name([double[, double[, ...]]]) should be known. If not, the parser will try to call
                 the qfmpResultFunction, but this will be very expensive, compared to calling the C-function!
               # variables may NOT hange their adress, i.e. if an external variable is defined as a pointer during compile,
                 the pointr may not change until the evaluation, as the pointer is hard-coded into the program
               # no recursion
               # functions may only be defined in the global scope
            .
            So not all expressions may be translated into ByteCode. the method qfmpNode::createByteCode() thus returns true on success
            and false else.

            \warning The bytecode utilities are NOT YET COMPLETE ... and not guaranteed to be faster than the evaluate(r) call!
         */
        /*@{*/

    public:
        /** \brief possible bytecodes */
        enum ByteCodes {
            bcNOP,
            bcPush,
            bcPop,
            bcVarRead,
            bcVarWrite,
            bcHeapRead,
            bcHeapWrite,

            bcAdd,
            bcMul,
            bcDiv,
            bcSub,
            bcMod,
            bcPow,
            bcNeg,


            bcBitAnd,
            bcBitOr,
            bcBitNot,

            bcLogicAnd,
            bcLogicOr,
            bcLogicNot,
            bcLogicXor,

            bcCmpEqual,
            bcCmpLesser,
            bcCmpLesserEqual,

            bcCallCFunction,
            bcCallCMPFunction,
            bcCallResultFunction,

            bcJumpRel,
            bcJumpCondRel,

            bcBJumpRel,
            bcBJumpCondRel

        };

        enum {
            ByteCodeInitialHeapSize=128
        };

        struct QFLIB_EXPORT ByteCodeInstruction {
            public:
                ByteCodeInstruction(ByteCodes opcode=bcNOP);
                ByteCodeInstruction(ByteCodes opcode, double numpar);
                ByteCodeInstruction(ByteCodes opcode, int intpar);
                ByteCodeInstruction(ByteCodes opcode, void* pntpar);
                ByteCodeInstruction(ByteCodes opcode, void* pntpar, int intpar);
                ByteCodeInstruction(ByteCodes opcode, QString strpar, int intpar);
                ByteCodes opcode;
                double numpar;
                int intpar;
                void* pntpar;
                QString strpar;
        };

        class qfmpNode; // forward

        struct QFLIB_EXPORT ByteCodeEnvironment {
                ByteCodeEnvironment(QFMathParser* parser=NULL);

                void init(QFMathParser* parser);

                QFMathParser* parser;
                int heapItemPointer;
                QMap<QString, QList<int> > heapVariables;
                int pushVar(const QString& name);
                void popVar(const QString& name);
                QMap<QString, QPair<QStringList, qfmpNode*> > functionDefs;
                QSet<QString> inFunctionCalls;
        };

        friend struct ByteCodeEnvironment;

        typedef QVector<ByteCodeInstruction> ByteCodeProgram;

        double evaluateBytecode(const ByteCodeProgram &program);
        static QString printBytecode(const ByteCodeInstruction& instruction);
        static QString printBytecode(const ByteCodeProgram& program);

        /*@}*/
    public:


        /**
         * \defgroup qfmpultil utilities for QFMathParser function parser class
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/

        static QString resultTypeToString(qfmpResultType type);


        static QString resultToString(const qfmpResult &r);
        static QString resultToTypeString(const qfmpResult &r);

        static qfmpResult getInvalidResult();


        /** \brief This struct is for managing variables. Unlike qfmpResult this struct
          * only contains pointers to the data
          */
        struct QFLIB_EXPORT qfmpVariable {
                friend struct ByteCodeEnvironment;
            public:
                qfmpVariable();
                qfmpVariable(double* ref);
                qfmpVariable(QString* ref);
                qfmpVariable(bool* ref);
                qfmpVariable(QVector<double>* ref);
                qfmpVariable(QVector<bool>* ref);
                qfmpVariable(QStringList* ref);
                //~qfmpVariable();
                QFLIB_EXPORT void clearMemory();
                QFLIB_EXPORT qfmpResult toResult() const;
                QFLIB_EXPORT void toResult(qfmpResult& r) const;
                QFLIB_EXPORT bool isInternal() const;
                QFLIB_EXPORT void set(const qfmpResult& result);
                inline  qfmpResultType getType() const { return type; }
                inline QString* getStr() const { return str; }
                inline double* getNum() const { return num; }
                inline bool* getBoolean() const { return boolean; }
                inline QVector<double>* getNumVec() const { return numVec; }
                inline QVector<bool>* getBoolVec() const { return boolVec; }
                inline QStringList* getStrVec() const { return strVec; }


            protected:
                qfmpResultType type;     /*!< \brief type of the variable */
                bool internal;           /*!< \brief this is an internal variable */
                QString *str;        /*!< \brief this points to the variable data if \c type==qfmpString */
                double *num;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
                bool *boolean;           /*!< \brief this points to the variable data if \c type==qfmpBool */
                QVector<double>* numVec; /*!< \brief this points to the variable data if \c type==qfmpDoubleVector */
                QVector<bool>* boolVec; /*!< \brief this points to the variable data if \c type==qfmpBoolVector */
                QStringList* strVec; /*!< \brief this points to the variable data if \c type==qfmpStringVector */
        };





        /*@}*/


        /**
         * \defgroup qfmpNodes memory representation of expressions
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/

        /**
         * \brief This class is the abstract base class for nodes.
         * All allowed node types must inherit from qfmpNode
         */
        class QFLIB_EXPORT qfmpNode {
          protected:
            QFMathParser* parser;  /*!< \brief points to the parser object that is used to evaluate this node */
            qfmpNode* parent;      /*!< \brief points to the parent node */
          public:
            explicit qfmpNode(QFMathParser* parser, qfmpNode* parent=NULL) { this->parser=parser;  this->parent=parent; }
            /** \brief virtual class destructor */
            virtual ~qfmpNode() {}

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();

            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result)=0;

            /** \brief return a pointer to the QFMathParser  */
            inline QFMathParser* getParser(){ return parser; }

            /** \brief set the QFMathParser  */
            inline void setParser(QFMathParser* mp){ parser=mp; }

            /** \brief returns a pointer to the parent node */
            inline qfmpNode* getParent(){ return parent; }

            /** \brief sets the parent node  */
            inline void setParent(qfmpNode* par) { parent=par; }

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) =0;

            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) { return false; }
        };


        /**
         * \brief This class represents a binary arithmetic operation:
         *  add (+), subtract (-), multiply (*), divide (/), a to the power of b (a^b),
         *  binary and (&), binary or (|)
         */
        class QFLIB_EXPORT qfmpBinaryArithmeticNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpBinaryArithmeticNode
             *  \param op the operation to be performed: add (+), subtract (-), multiply (*), divide (/), a to the power of b (a^b)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpBinaryArithmeticNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryArithmeticNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);
        };

        /**
         * \brief This class represents a binary boolean operation: and, or, xor, nor, nand
         */
        class QFLIB_EXPORT qfmpBinaryBoolNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpBinaryBoolNode
             *  \param op the operation to be performed: (a)nd, (o)r, (x)or, (n)or, nand (A)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpBinaryBoolNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpBinaryBoolNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);

        };

        /**
         * \brief This class represents a binary compare operation: !=, ==, >=, <=, >, <
         */
        class QFLIB_EXPORT qfmpCompareNode: public qfmpNode {
          private:
            qfmpNode* left, *right;
            char operation;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param op the operation to be performed: != (!), == (=), >= (b), <= (a), (>), (<)
             *  \param l left child node/operand
             *  \param r right child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpCompareNode(char op, qfmpNode* l, qfmpNode* r, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpCompareNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);

        };

        /**
         * \brief This class represents a unary operations: ! (bool negation), - (arithmetic negation)
         */
        class QFLIB_EXPORT qfmpUnaryNode: public qfmpNode {
          private:
            qfmpNode* child;
            char operation;
          public:
            /** \brief constructor for a qfmpUnaryNode
             *  \param op the operation to be performed: (!), (-)
             *  \param c child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpUnaryNode(char op, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpUnaryNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL);
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);

        };

        /**
         * \brief This class represents a vector element access
         */
        class QFLIB_EXPORT qfmpVectorAccessNode: public qfmpNode {
          protected:
            qfmpNode* index;
            QString variable;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorAccessNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param var name of the variable to assign to
             *  \param index index child node
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorAccessNode(QString var, qfmpNode* index, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a variable assignment (a = expression)
         */
        class QFLIB_EXPORT qfmpVariableAssignNode: public qfmpNode {
          protected:
            qfmpNode* child;
            QString variable;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVariableAssignNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param var name of the variable to assign to
             *  \param c child node/right-hand-side expression
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVariableAssignNode(QString var, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);


        };

        /**
         * \brief This class represents a vector element assignment (a[index] = expression)
         */
        class QFLIB_EXPORT qfmpVectorElementAssignNode: public qfmpVariableAssignNode {
          protected:
            qfmpNode* index;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorElementAssignNode() ;

            /** \brief constructor for a qfmpVectorElementAssignNode
             *  \param var name of the variable to assign to
             *  \param index index node
             *  \param expression child node/right-hand-side expression
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorElementAssignNode(QString var, qfmpNode* index, qfmpNode* expression, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };


        /**
         * \brief This class represents a variable assignment (a = expression)
         */
        class QFLIB_EXPORT qfmpFunctionAssignNode: public qfmpNode {
          private:
            qfmpNode* child;
            QString function;
            QStringList parameterNames;
          public:
            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpFunctionAssignNode() ;

            /** \brief constructor for a qfmpVariableAssignNode
             *  \param function name of the function to assign to
             *  \param parameterNames list of parameters of the new function
             *  \param c child node/right-hand-side expression
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpFunctionAssignNode(QString function, QStringList parameterNames, qfmpNode* c, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);

        };

        /**
         * \brief This class represents a number, a string contant or a boolean contant (true/false)
         */
        class QFLIB_EXPORT qfmpConstantNode: public qfmpNode {
          private:
            qfmpResult data;
          public:
            /** \brief constructor for a qfmpConstantNode
             *  \param d the value of the constant
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpConstantNode(qfmpResult d, QFMathParser* p, qfmpNode* par):qfmpNode(p, par) { data=d; }

            /** \brief evaluate this node */
            virtual qfmpResult evaluate() ;
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment);

        };

        /**
         * \brief This class represents a variable.
         */
        class QFLIB_EXPORT qfmpVariableNode: public qfmpNode {
          private:
            QString var;
          public:
            /** \brief constructor for a qfmpVariableNode
             *  \param name name of the variable
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVariableNode(QString name, QFMathParser* p, qfmpNode* par);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            inline QString getName() const { return var; }

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment *environment);

        };


        /**
         * \brief This class represents a an invalid node that always resturns an invalid value
         */
        class QFLIB_EXPORT qfmpInvalidNode: public qfmpNode {
          public:
            /** \brief constructor for a qfmpConstantNode
             *  \param d the value of the constant
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpInvalidNode(QFMathParser* p, qfmpNode* par):qfmpNode(p, par) { };

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };




        /** \brief This is a function prototype for adding new mathematical functions
         *         to the parser
         *
         * If you want to add more math functions (like sin, cos , abs ...) to the
         * parser, you will have to implement it with this prototype and then register
         * it with QFMathParser::addFunction(). The first parameter points to an array
         * containing the input parameters while the second one specifies the number
         * of supplied parameters. The result has to be of type qfmpResult.
         *
         * All error handling has to be done inside the function definition. Here is a
         * simple example:
         * \code
         * qfmpResult Abs(qfmpResult* params, unsigned char n){
         *   qfmpResult r;
         *   r.type=qfmpDouble;
         *   if (n!=1) qfmpError("abs accepts 1 argument");
         *   if (params[0].type!=qfmpDouble) qfmpError("abs needs double argument");
         *   r.num=fabs(params[0].num);
         *   return r;
         * }
         * \endcode
         */
        typedef qfmpResult (*qfmpEvaluateFunc)(const qfmpResult*, unsigned int, QFMathParser*);
        typedef double (*qfmpEvaluateFuncSimple0Param)();
        typedef double (*qfmpEvaluateFuncSimple1Param)(double);
        typedef double (*qfmpEvaluateFuncSimple2Param)(double,double);
        typedef double (*qfmpEvaluateFuncSimple3Param)(double,double,double);
        typedef double (*qfmpEvaluateFuncSimple0ParamMP)(QFMathParser*);
        typedef double (*qfmpEvaluateFuncSimple1ParamMP)(double, QFMathParser*);
        typedef double (*qfmpEvaluateFuncSimple2ParamMP)(double,double, QFMathParser*);
        typedef double (*qfmpEvaluateFuncSimple3ParamMP)(double,double,double, QFMathParser*);

        /** \brief This is a function prototype like qfmpEvaluateFunc but returns its result with call by reference
         */
        typedef void (*qfmpEvaluateFuncRefReturn)(qfmpResult&, const qfmpResult*, unsigned int, QFMathParser*);

        /** \brief types of functions */
        enum qfmpFunctiontype {
            functionC,
            functionCRefReturn,
            functionNode
        };

        /** \brief description of a user registered function */
        struct QFLIB_EXPORT qfmpFunctionDescriptor {
            explicit qfmpFunctionDescriptor();
            //~qfmpFunctionDescriptor();
            QFLIB_EXPORT void clearMemory();
            qfmpEvaluateFunc function;    /*!< \brief a pointer to the function implementation */
            qfmpEvaluateFuncRefReturn functionRR;    /*!< \brief a pointer to the function implementation */
            QString name;             /*!< \brief name of the function */
            qfmpFunctiontype type;  /*!< \brief type of the function */
            qfmpNode* functionNode;   /*!< \brief points to the node definig the function */
            QStringList parameterNames;  /*!< \brief a list of the function parameters, if the function is defined by a node */

            QMap<int, void*> simpleFuncPointer; /*!<  \brief points to the simple implementation of the function, e.g. of type qfmpEvaluateFuncSimple0Param or qfmpEvaluateFuncSimple0ParamMP, the integer-key indexes the function as its number of parameters for a simple call to simpleFuncPointer ... values >100 indicate the use of a MP-variant, i.e. 102 means a call to qfmpEvaluateFuncSimple2ParamMP whereas 1 means a call to  qfmpEvaluateFuncSimple1Param */

            QFLIB_EXPORT void evaluate(qfmpResult& res, const QVector<qfmpResult> &parameters, QFMathParser *parent) const;
            QFLIB_EXPORT QString toDefString() const;
        };

        /**
         * \brief This class represents an arbitrary function.
         *
         * When initialized this class will get the function description that is
         * linked to the supplied function name from QFMathParser object. This
         * information is saved locally and won't be changed when evaluating!
         *
         * Functions may have 8 parameters at the most.
         */
        class QFLIB_EXPORT qfmpFunctionNode: public qfmpNode {
          private:
            QString fun;
            QVector<QFMathParser::qfmpNode*> child;
          public:
            /** \brief constructor for a qfmpFunctionNode
             *  \param name name of the function
             *  \param c a pointer to an array of qfmpNode objects that represent the parameter expressions
             *  \param num number of children in c
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpFunctionNode(QString name, QVector<QFMathParser::qfmpNode*> params, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpFunctionNode();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);


            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;


            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) ;

        };

        /**
         * \brief This class represents a list of qfmpNode.
         *
         * when evaluating the result will be the result of the last node in the list.
         */
        class QFLIB_EXPORT qfmpNodeList: public qfmpNode {
          protected:
            QList<qfmpNode*> list;
          public:
            /** \brief constructor for a qfmpNodeList
             *  \param p a pointer to a QFMathParser object
             */
            explicit qfmpNodeList(QFMathParser* p, qfmpNode* par=NULL);

            /** \brief standard destructor, also destroy the children (recursively) */
            virtual ~qfmpNodeList();

            /** \brief add a qfmpNode n to the list */
            void add(qfmpNode* n);
            /** \brief remove the first node from the list and only delet it if \a deleteObject is \c true, returns a pointer to the first node, if \a deleteObject is \c false otherwise returns \c NULL */
            qfmpNode* popFirst(bool deleteObject=false);
            /** \brief remove the first node from the list and only delet it if \a deleteObject is \c true, returns a pointer to the first node, if \a deleteObject is \c false otherwise returns \c NULL */
            qfmpNode* popLast(bool deleteObject=false);

            /** \brief evaluate the node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief get the number of nodes in the list */
            int getCount() {return list.size();};

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) ;

        };

        /**
         * \brief This class represents a fector of qfmpNode.
         *
         * when evaluating the result will be the result of the last node in the list.
         */
        class QFLIB_EXPORT qfmpVectorList: public qfmpNodeList {
          public:
            /** \brief constructor for a qfmpNodeList
             *  \param p a pointer to a QFMathParser object
             */
            explicit qfmpVectorList(QFMathParser* p, qfmpNode* par=NULL): qfmpNodeList(p, par) { setParser(p); setParent(par); };

            /** \brief standard destructor, also destroy the children (recursively) */
            ~qfmpVectorList() {};


            /** \brief evaluate the node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) ;

        };


        /**
         * \brief This class represents a vector construction in the form start:end or start:step:end
         */
        class QFLIB_EXPORT qfmpVectorConstructionNode: public qfmpNode {
          private:
            qfmpNode* start, *end, *step;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param start start child node/operand
             *  \param end end child node/operand
             *  \param step step child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpVectorConstructionNode(qfmpNode* start, qfmpNode* end, qfmpNode* step, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorConstructionNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /**
         * \brief This class represents a cases or if construction
         */
        class QFLIB_EXPORT qfmpCasesNode: public qfmpNode {
          private:
            qfmpNode* elseNode;
            QList<QPair<qfmpNode*, qfmpNode*> > casesNodes;
          public:
            /** \brief constructor for a qfmpCompareNode
             *  \param start start child node/operand
             *  \param end end child node/operand
             *  \param step step child node/operand
             *  \param p a pointer to a QFMathParser object
             *  \param par a pointer to the parent node
             */
            explicit qfmpCasesNode(QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpCasesNode ();

            void setElse(qfmpNode* elseNode);
            void addCase(qfmpNode* decision, qfmpNode* value);

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) ;


        };

        /**
         * \brief This class represents a cases or if construction
         */
        class QFLIB_EXPORT qfmpVectorOperationNode: public qfmpNode {
          private:
            QString operationName;
            QString variableName;
            qfmpNode* items;
            qfmpNode* start;
            qfmpNode* end;
            qfmpNode* delta;
            qfmpNode* expression;
            QList<QPair<qfmpNode*, qfmpNode*> > casesNodes;
          public:
            /** \brief constructor for a qfmpVectorOperationNode
             */
            explicit qfmpVectorOperationNode(const QString& operationName, const QString& variableName, qfmpNode* items, qfmpNode* expression, QFMathParser* p, qfmpNode* par);
            explicit qfmpVectorOperationNode(const QString& operationName, const QString& variableName, qfmpNode* start, qfmpNode* end, qfmpNode* delta, qfmpNode* expression, QFMathParser* p, qfmpNode* par);

            /** \brief standard destructor, also destroy the children (recursively)  */
            ~qfmpVectorOperationNode ();

            /** \brief evaluate this node */
            virtual qfmpResult evaluate();
            /** \brief evaluate this node, return result as call-by-reference (faster!) */
            virtual void evaluate(qfmpResult& result);
            /** \brief create bytecode that evaluates the current node */
            virtual bool createByteCode(ByteCodeProgram& program, ByteCodeEnvironment* environment) ;

            /** \brief returns a copy of the current node (and the subtree). The parent is set to \a par */
            virtual qfmpNode* copy(qfmpNode* par=NULL) ;

        };

        /*@}*/



        class QFLIB_EXPORT executionEnvironment {
            protected:
                /** \brief map to manage all currently defined variables */
                QMap<QString, QList<QPair<int, qfmpVariable> > > variables;

                /** \brief map to manage all currently rtegistered functions */
                QMap<QString, QList<QPair<int, qfmpFunctionDescriptor> > > functions;

                int currentLevel;

                QFMathParser* parent;
            public:
                executionEnvironment(QFMathParser* parent=NULL);
                ~executionEnvironment();

                QFLIB_EXPORT void setParent(QFMathParser* parent);

                inline void enterBlock() {
                    currentLevel++;
                }
                inline void leaveBlock(){
                    if (currentLevel>0) {
                        currentLevel--;
                        QStringList keys=variables.keys();
                        for (int i=0; i<keys.size(); i++) {
                            while (!(variables[keys[i]].isEmpty())&&variables[keys[i]].last().first>currentLevel) {
                                variables[keys[i]].last().second.clearMemory();
                                variables[keys[i]].removeLast();
                            }
                            //if (keys[i]=="x") //qDebug()<<"**LEAVE_BLOCK "<<currentLevel+1<<": var:"<<keys[i]<<"   levels="<<variables[keys[i]].size();
                            if (variables[keys[i]].isEmpty()) variables.remove(keys[i]);
                        }

                        keys=functions.keys();
                        for (int i=0; i<keys.size(); i++) {
                            while ((!functions[keys[i]].isEmpty()) && functions[keys[i]].last().first>currentLevel) {
                                functions[keys[i]].last().second.clearMemory();
                                functions[keys[i]].removeLast();
                            }
                            if (functions[keys[i]].isEmpty()) functions.remove(keys[i]);
                        }
                    } else {
                        parent->qfmpError(QObject::tr("cannot leave toplevel block!"));
                    }
                 }

                QFLIB_EXPORT int getBlocklevel() const;
                QFLIB_EXPORT void clearVariables();
                QFLIB_EXPORT void clearFunctions();
                QFLIB_EXPORT void clear();

                QFLIB_EXPORT void addVariable(const QString& name, const qfmpVariable& variable);
                QFLIB_EXPORT void setFunction(const QString& name, const qfmpFunctionDescriptor& function);
                QFLIB_EXPORT void addFunction(const QString& name, const QStringList& parameterNames, qfmpNode* function);

                /** \brief  tests whether a variable exists */
                inline bool variableExists(const QString& name){ return (variables.find(name)!=variables.end()); }

                /** \brief  tests whether a function exists */
                inline bool functionExists(const QString& name){ return !(functions.find(name)==functions.end()); }

                inline qfmpResult getVariable(const QString& name) const {
                    qfmpResult res;
                    res.isValid=false;
                    if (variables.contains(name) && variables[name].size()>0) {
                        res=variables[name].last().second.toResult();
                        res.isValid=true;
                        //qDebug()<<"getVariable("<<name<<"): "<<res.toTypeString();
                        return res;
                    }
                    if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
                    return res;
                }
                inline bool getVariableDef(const QString& name, qfmpVariable& vardef) const {
                    if (variables.contains(name) && variables[name].size()>0) {
                        vardef=variables[name].last().second;
                        return true;
                    }
                    if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
                    return false;
                }
                inline int getVariableLevel(const QString& name) const {
                    if (variables.contains(name) && variables[name].size()>0) {
                        return variables[name].last().first;
                    }
                    if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
                    return -1;
                }

                inline qfmpResult evaluateFunction(const QString& name, const QVector<qfmpResult> &parameters) const{
                    qfmpResult res;
                    if (functions.contains(name) && functions[name].size()>0) {
                        functions[name].last().second.evaluate(res, parameters, parent);
                    } else {
                        if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
                        res.setInvalid();
                    }
                    return res;
                }

                inline bool getFunctionDef(const QString& name, qfmpFunctionDescriptor& vardef) const {
                    if (functions.contains(name) && functions[name].size()>0) {
                        vardef=functions[name].last().second;
                        return true;
                    }
                    if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
                    return false;
                }

                inline int getFunctionLevel(const QString& name) const {
                    if (functions.contains(name) && functions[name].size()>0) {
                        return functions[name].last().first;
                    }
                    if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
                    return -1;
                }

                inline void getVariable(qfmpResult& res, const QString& name) const{
                    if (variables.contains(name) && variables[name].size()>0) {
                        variables[name].last().second.toResult(res);
                    } else {
                        if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
                        res.setInvalid();
                    }
                    //qDebug()<<"getVariable(res, "<<name<<", "<<res.toTypeString()<<")";
                }

                inline void evaluateFunction(qfmpResult& res, const QString& name, const QVector<qfmpResult> &parameters) const{
                    if (functions.contains(name) && functions[name].size()>0) {
                        functions[name].last().second.evaluate(res, parameters, parent);
                    } else {
                        if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
                        res.setInvalid();
                    }
                }

                inline void addVariable(const QString& name, const qfmpResult& result){
                    bool add=true;
                    if (variables.contains(name)) {
                        if (variables[name].size()>0) {
                            if (variables[name].last().first==currentLevel) {
                                variables[name].last().second.set(result);
                                add=false;
                                return;
                            }
                        }
                    }
                    if (add) {
                        QFMathParser::qfmpVariable v;
                        v.set(result);
                        variables[name].append(qMakePair(currentLevel, v));
                    }
                }

                inline void setVariable(const QString& name, const qfmpResult& result){
                    //qDebug()<<"setVariable("<<name<<", "<<result.toTypeString()<<")";
                    if (variables.contains(name) && variables[name].size()>0) {
                        variables[name].last().second.set(result);
                    } else {
                        QFMathParser::qfmpVariable v;
                        v.set(result);
                        QList<QPair<int, qfmpVariable> > l;
                        l.append(qMakePair(currentLevel, v));
                        variables[name]=l;
                    }
                }

                QFLIB_EXPORT void setVariableDouble(const QString& name, double result);
                QFLIB_EXPORT void setVariableDoubleVec(const QString& name, const QVector<double>& result);
                QFLIB_EXPORT void setVariableStringVec(const QString& name, const QStringList& result);
                QFLIB_EXPORT void setVariableBoolVec(const QString& name, const QVector<bool>& result);
                QFLIB_EXPORT void setVariableString(const QString& name, const QString& result);
                QFLIB_EXPORT void setVariableBoolean(const QString& name, bool result);
                QFLIB_EXPORT void addVariableDouble(const QString& name, double result);
                QFLIB_EXPORT void addVariableDoubleVec(const QString& name, const QVector<double>& result);
                QFLIB_EXPORT void addVariableStringVec(const QString& name, const QStringList& result);
                QFLIB_EXPORT void addVariableBoolVec(const QString& name, const QVector<bool>& result);
                QFLIB_EXPORT void addVariableString(const QString& name, const QString& result);
                QFLIB_EXPORT void addVariableBoolean(const QString& name, bool result);
                QFLIB_EXPORT void deleteVariable(const QString& name);

                QString printVariables() const;
                QString printFunctions() const;
                QList<QPair<QString, qfmpVariable> > getVariables() const;
                QList<QPair<QString, qfmpFunctionDescriptor> > getFunctions() const;


        };

    public:
        /**
         * \defgroup qfmpErrorhandling error handling
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/






        /** \brief macro that throws an exception or calls an error handler */
        void qfmpError(QString st);


        /*@}*/

    protected:
        /** \brief return the given token as human-readable string */
        QString tokentostring(qfmpTokenType token);

        /** \brief return the current token as human-readable string */
        QString currenttokentostring();

		/** \brief Tokenizer: extract the next token from the input */
        qfmpTokenType getToken();

		/** \brief return a delimited text, i.e. extract the texte between the delimiters <code>"</code> in: of <code>"Hallo!"</code>, i.e. returns <code> Hallo!</code>
         *         This is used to parse string constants.
         *
         * This functions actually reads pascal style delimited string constants. So if you want to use the delimiter as part of the string you will have to
         * write it as doubled character. So <code>'Jan''s Test'</code> stands for <code>Jan's Test</code>.
         */
        QString readDelim(QChar delimiter);

		/** \brief recognizes an compExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* compExpression(bool get);

		/** \brief recognizes a logicalExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* logicalExpression(bool get);

		/** \brief recognizes a logicalTerm while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* logicalTerm(bool get);

		/** \brief recognizes a mathExpression while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* mathExpression(bool get);

		/** \brief recognizes a term while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* mathTerm(bool get);

        /** \brief recognizes a vector_primary while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* vectorPrimary(bool get);

        /** \brief recognizes a primary while parsing. If \a get ist \c true, this function first retrieves a new token by calling getToken() */
        qfmpNode* primary(bool get);

		/** \brief this stream is used to read in the program. An object is created and assigned
  		 * (and destroyed) by the parse()-function */
        QTextStream * program;
        QString progStr;

        MTRand rng;

        double readNumber();
        double readBin();
        double readHex();
        double readOct();
        double readDec();



        executionEnvironment environment;

        /** \brief the current token while parsing a string */
        qfmpTokenType CurrentToken;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        QString StringValue;

        /** \brief the string value of the current token (when applicable) during the parsing step */
        double NumberValue;


        void* data;

        QStringList lastError;
        int errors;

        QChar peekStream(QTextStream* stream);
        void putbackStream(QTextStream* stream, QChar ch);
        bool getFromStream(QTextStream* stream, QChar& ch);

        QMap<QString, QVariant> m_generalData;

	public:
        /** \brief class constructor */
        QFMathParser();

        /** \brief class destructor */
        virtual ~QFMathParser();

        void setGeneralData(const QString& id, const QVariant& data);
        QVariant getGeneraldata(const QString& id, const QVariant& defaultData=QVariant());

        /** \brief returns whether an error has occured since the last call to resetError() */
        bool hasErrorOccured() const;
        /** \brief retuns the last error */
        QString getLastError() const;
        /** \brief retuns the last error */
        QString getFirstError() const;
        /** \brief return a list of the last errors */
        QStringList getLastErrors() const;
        /** \brief returns the number of errors since the last call to resetErrors() */
        int getLastErrorCount();
        /** \brief resets the error state to no errors */
        void resetErrors();

        void* get_data() const { return data; }
        void set_data(void* _data) { data=_data; }

        MTRand* get_rng() { return &rng; }


        /** \brief  register a new function
		 * \param name name of the new function
		 * \param function a pointer to the implementation
		 */
        void addFunction(const QString &name, qfmpEvaluateFunc function);
        void addFunction(const QString &name, qfmpEvaluateFunc function, qfmpEvaluateFuncSimple0Param f0, qfmpEvaluateFuncSimple1Param f1=NULL, qfmpEvaluateFuncSimple2Param f2=NULL, qfmpEvaluateFuncSimple3Param f3=NULL);
        void addFunction(const QString &name, qfmpEvaluateFunc function, qfmpEvaluateFuncSimple0ParamMP f0, qfmpEvaluateFuncSimple1ParamMP f1=NULL, qfmpEvaluateFuncSimple2ParamMP f2=NULL, qfmpEvaluateFuncSimple3ParamMP f3=NULL);


        /** \brief  register a new function
         * \param name name of the new function
         * \param function a pointer to the implementation
         */
        void addFunction(const QString &name, qfmpEvaluateFuncRefReturn function);
        void addFunction(const QString &name, qfmpEvaluateFuncRefReturn function, qfmpEvaluateFuncSimple0Param f0, qfmpEvaluateFuncSimple1Param f1=NULL, qfmpEvaluateFuncSimple2Param f2=NULL, qfmpEvaluateFuncSimple3Param f3=NULL);
        void addFunction(const QString &name, qfmpEvaluateFuncRefReturn function, qfmpEvaluateFuncSimple0ParamMP f0, qfmpEvaluateFuncSimple1ParamMP f1=NULL, qfmpEvaluateFuncSimple2ParamMP f2=NULL, qfmpEvaluateFuncSimple3ParamMP f3=NULL);

        inline void addFunction(const QString& name, const QStringList& parameterNames, qfmpNode* function){
            environment.addFunction(name, parameterNames, function);
        }


        /** \brief set the defining struct of the given variable */
        inline void addVariable(const QString& name, const qfmpVariable &value) {
            environment.addVariable(name, value);
        }


        /** \brief  register a new internal variable of type double
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        inline void addVariableDouble(const QString& name, double v) {
            environment.addVariableDouble(name, v);
        }

        /** \brief  register a new internal variable of type string
		 * \param name name of the new variable
		 * \param v initial value of this variable
		 */
        inline void addVariableString(const QString &name, const QString& v) {
            environment.addVariableString(name, v);
        }

        /** \brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        inline void addVariableBoolean(const QString& name, bool v) {
            environment.addVariableBoolean(name, v);
        }
        /** \brief  register a new internal variable of type boolean vector
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        inline void addVariableBoolVector(const QString& name, const QVector<bool>& v) {
            environment.addVariableBoolVec(name, v);
        }
        /** \brief  register a new internal variable of type boolean vector
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        inline void addVariableStringVector(const QString& name, const QStringList& v) {
            environment.addVariableStringVec(name, v);
        }

        /** \brief  register a new internal variable of type boolean
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        inline void addVariable(const QString &name, const qfmpResult &result) {
            environment.addVariable(name, result);
        }

        /** \brief  register a new internal variable if the given variable does not exist, otherwise set the axisting variable to the given value
         * \param name name of the new variable
         * \param v initial value of this variable
         */
        inline void setVariable(const QString &name, const qfmpResult &result) {
            environment.setVariable(name, result);
        }

        inline void deleteVariable(const QString& name) {
            environment.deleteVariable(name);
        }

        /** \brief  returns the value of the given variable */
        inline void getVariable(qfmpResult& r, QString name) {
            environment.getVariable(r, name);
        }

        /** \brief  returns the value of the given variable */
        inline void getVariableOrInvalid(qfmpResult& r, QString name) {
            if (environment.variableExists(name)) {
                environment.getVariable(r, name);
            } else {
                r.setInvalid();
            }
        }



        /** \brief  evaluates a registered function
         * \param name name of the (registered function) to be evaluated
         * \param params array of the input parameters
         */
        inline void evaluateFunction(qfmpResult& r, const QString &name, const QVector<qfmpResult>& params) {
            environment.evaluateFunction(r, name, params);
        }



        /** \brief  returns the value of the given variable */
        inline qfmpResult getVariable(QString name) {
            return environment.getVariable(name);
        }

        /** \brief  returns the value of the given variable */
        inline qfmpResult getVariableOrInvalid(QString name) {
            if (environment.variableExists(name)) return environment.getVariable(name);
            return qfmpResult::invalidResult();

        }



        /** \brief  evaluates a registered function
         * \param name name of the (registered function) to be evaluated
         * \param params array of the input parameters
         */
        inline qfmpResult evaluateFunction(const QString &name, const QVector<qfmpResult>& params) {
            return environment.evaluateFunction(name, params);
        }



        /** \brief  tests whether a variable exists */
        inline bool variableExists(const QString& name){ return environment.variableExists(name); }

        /** \brief  tests whether a function exists */
        inline bool functionExists(const QString& name){ return environment.functionExists(name); }

        inline void enterBlock() {
            environment.enterBlock();
        }
        inline void leaveBlock() {
            environment.leaveBlock();
        }

        /** \brief  deletes all defined variables. the memory of internal variables
         * will be released. the external memory will not be released.
         */
        inline void clearVariables(){
            environment.clearVariables();
        };

        /** \brief  clears the list of internal functions*/
        inline void clearFunctions(){
            environment.clearFunctions();
        };

        /** \brief  registers standard variables*/
        void addStandardVariables();

        /** \brief  registers standard functions*/
        void addStandardFunctions();

        /** \brief  parses the given expression*/
        qfmpNode* parse(QString prog);

        /** \brief evaluate the given expression */
        qfmpResult evaluate(QString prog);

        /** \brief  prints a list of all registered variables */
        inline QString printVariables(){
            return environment.printVariables();

        }

       inline  QList<QPair<QString, qfmpVariable> > getVariables(){
            return environment.getVariables();
        }

        /** \brief  prints a list of all registered variables */
        inline QString printFunctions(){
            return environment.printFunctions();
        }

        inline QList<QPair<QString, qfmpFunctionDescriptor> > getFunctions(){
            return environment.getFunctions();
        }


        /**
         * \defgroup qfmpglobal Tools to globaly extend QFMathParser
         * \ingroup qf3lib_mathtools_parser
         */
        /*@{*/
    protected:
        static QList<QPair<QString, qfmpEvaluateFunc> > externalGlobalFunctions;
        static QList<QPair<QString, qfmpEvaluateFuncRefReturn> > externalGlobalFunctionsRR;
        static QList<QPair<QString, qfmpResult> > externalGlobalVariables;
    public:
        /** \brief  add a function to the list of global functions that are defined by a call to addStandardFunctions() (or the constructor) */
        static void addGlobalFunction(const QString& name, qfmpEvaluateFunc function);
        /** \brief  add a function to the list of global functions that are defined by a call to addStandardFunctions() (or the constructor) */
        static void addGlobalFunction(const QString& name, qfmpEvaluateFuncRefReturn function);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, double value);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, const QVector<double>& value);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, const QVector<bool>& value);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, const QStringList& value);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, const QString& value);
        /** \brief  add a variablen to the list of global variables that are defined by a call to addStandardVariables() (or the constructor) */
        static void addGlobalVariable(const QString& name, bool value);
        /*@}*/

};

#endif // QFMATHPARSER_H
/*@}*/
