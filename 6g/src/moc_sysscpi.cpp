/****************************************************************************
** Meta object code from reading C++ file 'sysscpi.h'
**
** Created: Fri Aug 16 14:58:46 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "scpi/sysscpi.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sysscpi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_tSysScpi[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      23,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   10,    9,    9, 0x05,
      62,   54,    9,    9, 0x05,
      88,    9,    9,    9, 0x05,
     109,    9,    9,    9, 0x05,
     134,    9,    9,    9, 0x05,
     148,    9,    9,    9, 0x05,
     185,  164,    9,    9, 0x05,
     232,  209,    9,    9, 0x05,
     263,    9,    9,    9, 0x05,
     284,    9,    9,    9, 0x05,
     306,    9,    9,    9, 0x05,
     326,    9,    9,    9, 0x05,
     343,    9,    9,    9, 0x05,
     363,    9,    9,    9, 0x05,
     384,    9,    9,    9, 0x05,
     405,    9,    9,    9, 0x05,
     419,    9,    9,    9, 0x05,
     437,    9,    9,    9, 0x05,
     457,  452,    9,    9, 0x05,
     472,    9,    9,    9, 0x05,
     487,    9,    9,    9, 0x05,
     499,    9,    9,    9, 0x05,
     519,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     532,    9,    9,    9, 0x08,
     549,    9,    9,    9, 0x08,
     568,    9,    9,    9, 0x08,
     596,    9,    9,    9, 0x08,
     621,    9,    9,    9, 0x08,
     645,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_tSysScpi[] = {
    "tSysScpi\0\0title,msg\0"
    "showAlarmMessage(QString,QString)\0"
    "helpStr\0refreshScpiState(QString)\0"
    "refreshWindowState()\0resetUsbSlaveInterface()\0"
    "drawProcess()\0drawsListData()\0"
    "Display,RatioProgBar\0drawProgBar(bool,float)\0"
    "Display,current,amount\0"
    "drawDownloadProg(bool,int,int)\0"
    "drawAnalogModulate()\0drawDigitalModulate()\0"
    "drawPulseModulate()\0drawSystemPara()\0"
    "drawArbitraryWave()\0drawFreqInParamter()\0"
    "drawAmplInParamter()\0drawExfFref()\0"
    "drawDrgFunction()\0drawUSBPower()\0type\0"
    "drawSweep(int)\0drawMainPara()\0drawState()\0"
    "drawInputParamter()\0drawButton()\0"
    "readSerialData()\0readUsbSlaveData()\0"
    "acceptClientSocketConnect()\0"
    "clientSocketDisConnect()\0"
    "clientSocketReadyRead()\0"
    "clientSocketError(QAbstractSocket::SocketError)\0"
};

void tSysScpi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        tSysScpi *_t = static_cast<tSysScpi *>(_o);
        switch (_id) {
        case 0: _t->showAlarmMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->refreshScpiState((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->refreshWindowState(); break;
        case 3: _t->resetUsbSlaveInterface(); break;
        case 4: _t->drawProcess(); break;
        case 5: _t->drawsListData(); break;
        case 6: _t->drawProgBar((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 7: _t->drawDownloadProg((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: _t->drawAnalogModulate(); break;
        case 9: _t->drawDigitalModulate(); break;
        case 10: _t->drawPulseModulate(); break;
        case 11: _t->drawSystemPara(); break;
        case 12: _t->drawArbitraryWave(); break;
        case 13: _t->drawFreqInParamter(); break;
        case 14: _t->drawAmplInParamter(); break;
        case 15: _t->drawExfFref(); break;
        case 16: _t->drawDrgFunction(); break;
        case 17: _t->drawUSBPower(); break;
        case 18: _t->drawSweep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->drawMainPara(); break;
        case 20: _t->drawState(); break;
        case 21: _t->drawInputParamter(); break;
        case 22: _t->drawButton(); break;
        case 23: _t->readSerialData(); break;
        case 24: _t->readUsbSlaveData(); break;
        case 25: _t->acceptClientSocketConnect(); break;
        case 26: _t->clientSocketDisConnect(); break;
        case 27: _t->clientSocketReadyRead(); break;
        case 28: _t->clientSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData tSysScpi::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject tSysScpi::staticMetaObject = {
    { &tBaseScpi::staticMetaObject, qt_meta_stringdata_tSysScpi,
      qt_meta_data_tSysScpi, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &tSysScpi::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *tSysScpi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *tSysScpi::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_tSysScpi))
        return static_cast<void*>(const_cast< tSysScpi*>(this));
    return tBaseScpi::qt_metacast(_clname);
}

int tSysScpi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = tBaseScpi::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void tSysScpi::showAlarmMessage(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tSysScpi::refreshScpiState(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tSysScpi::refreshWindowState()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void tSysScpi::resetUsbSlaveInterface()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void tSysScpi::drawProcess()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void tSysScpi::drawsListData()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void tSysScpi::drawProgBar(bool _t1, float _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void tSysScpi::drawDownloadProg(bool _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void tSysScpi::drawAnalogModulate()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void tSysScpi::drawDigitalModulate()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void tSysScpi::drawPulseModulate()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void tSysScpi::drawSystemPara()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void tSysScpi::drawArbitraryWave()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void tSysScpi::drawFreqInParamter()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void tSysScpi::drawAmplInParamter()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void tSysScpi::drawExfFref()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void tSysScpi::drawDrgFunction()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void tSysScpi::drawUSBPower()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void tSysScpi::drawSweep(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void tSysScpi::drawMainPara()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void tSysScpi::drawState()
{
    QMetaObject::activate(this, &staticMetaObject, 20, 0);
}

// SIGNAL 21
void tSysScpi::drawInputParamter()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}

// SIGNAL 22
void tSysScpi::drawButton()
{
    QMetaObject::activate(this, &staticMetaObject, 22, 0);
}
QT_END_MOC_NAMESPACE
