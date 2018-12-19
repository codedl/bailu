/****************************************************************************
** Meta object code from reading C++ file 'sysscpi.h'
**
** Created: Wed Dec 19 14:36:45 2018
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
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   10,    9,    9, 0x05,
      62,   54,    9,    9, 0x05,
      88,    9,    9,    9, 0x05,
     126,  109,    9,    9, 0x05,
     156,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     188,  181,    9,    9, 0x0a,
     220,    9,    9,    9, 0x08,
     237,    9,    9,    9, 0x08,
     256,    9,    9,    9, 0x08,
     271,    9,    9,    9, 0x08,
     299,    9,    9,    9, 0x08,
     324,    9,    9,    9, 0x08,
     348,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_tSysScpi[] = {
    "tSysScpi\0\0title,msg\0"
    "showAlarmMessage(QString,QString)\0"
    "helpStr\0refreshScpiState(QString)\0"
    "refreshWindowState()\0mainCode,subCode\0"
    "refreshMenuFrame(ulong,ulong)\0"
    "resetUsbSlaveInterface()\0fd,len\0"
    "clientSocketDataArrive(int,int)\0"
    "readSerialData()\0readUsbSlaveData()\0"
    "readGpibData()\0acceptClientSocketConnect()\0"
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
        case 3: _t->refreshMenuFrame((*reinterpret_cast< ulong(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2]))); break;
        case 4: _t->resetUsbSlaveInterface(); break;
        case 5: _t->clientSocketDataArrive((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->readSerialData(); break;
        case 7: _t->readUsbSlaveData(); break;
        case 8: _t->readGpibData(); break;
        case 9: _t->acceptClientSocketConnect(); break;
        case 10: _t->clientSocketDisConnect(); break;
        case 11: _t->clientSocketReadyRead(); break;
        case 12: _t->clientSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
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
void tSysScpi::refreshMenuFrame(unsigned long _t1, unsigned long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tSysScpi::resetUsbSlaveInterface()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
static const uint qt_meta_data_tSocketThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   15,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_tSocketThread[] = {
    "tSocketThread\0\0fd,len\0"
    "clientSocketDataArrive(int,int)\0"
};

void tSocketThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        tSocketThread *_t = static_cast<tSocketThread *>(_o);
        switch (_id) {
        case 0: _t->clientSocketDataArrive((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData tSocketThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject tSocketThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_tSocketThread,
      qt_meta_data_tSocketThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &tSocketThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *tSocketThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *tSocketThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_tSocketThread))
        return static_cast<void*>(const_cast< tSocketThread*>(this));
    return QThread::qt_metacast(_clname);
}

int tSocketThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void tSocketThread::clientSocketDataArrive(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
