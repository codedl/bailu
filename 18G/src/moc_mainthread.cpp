/****************************************************************************
** Meta object code from reading C++ file 'mainthread.h'
**
** Created: Fri Jan 18 15:28:58 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main/mainthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mainThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      22,   11,   11,   11, 0x05,
      33,   11,   11,   11, 0x05,
      45,   11,   11,   11, 0x05,
      56,   11,   11,   11, 0x05,
      67,   11,   11,   11, 0x05,
      80,   11,   11,   11, 0x05,
      91,   11,   11,   11, 0x05,
     104,   11,   11,   11, 0x05,
     116,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_mainThread[] = {
    "mainThread\0\0drawTop()\0drawLogo()\0"
    "drawTitle()\0drawTime()\0drawScpi()\0"
    "drawCanvas()\0drawHelp()\0drawButton()\0"
    "drawState()\0drawAll()\0"
};

void mainThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        mainThread *_t = static_cast<mainThread *>(_o);
        switch (_id) {
        case 0: _t->drawTop(); break;
        case 1: _t->drawLogo(); break;
        case 2: _t->drawTitle(); break;
        case 3: _t->drawTime(); break;
        case 4: _t->drawScpi(); break;
        case 5: _t->drawCanvas(); break;
        case 6: _t->drawHelp(); break;
        case 7: _t->drawButton(); break;
        case 8: _t->drawState(); break;
        case 9: _t->drawAll(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData mainThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject mainThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_mainThread,
      qt_meta_data_mainThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &mainThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *mainThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *mainThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mainThread))
        return static_cast<void*>(const_cast< mainThread*>(this));
    return QThread::qt_metacast(_clname);
}

int mainThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void mainThread::drawTop()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void mainThread::drawLogo()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void mainThread::drawTitle()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void mainThread::drawTime()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void mainThread::drawScpi()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void mainThread::drawCanvas()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void mainThread::drawHelp()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void mainThread::drawButton()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void mainThread::drawState()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void mainThread::drawAll()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}
static const uint qt_meta_data_minorThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      23,   12,   12,   12, 0x05,
      35,   12,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_minorThread[] = {
    "minorThread\0\0drawTop()\0drawState()\0"
    "drawInput()\0"
};

void minorThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        minorThread *_t = static_cast<minorThread *>(_o);
        switch (_id) {
        case 0: _t->drawTop(); break;
        case 1: _t->drawState(); break;
        case 2: _t->drawInput(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData minorThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject minorThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_minorThread,
      qt_meta_data_minorThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &minorThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *minorThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *minorThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_minorThread))
        return static_cast<void*>(const_cast< minorThread*>(this));
    return QThread::qt_metacast(_clname);
}

int minorThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void minorThread::drawTop()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void minorThread::drawState()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void minorThread::drawInput()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_dataThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_dataThread[] = {
    "dataThread\0"
};

void dataThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData dataThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject dataThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_dataThread,
      qt_meta_data_dataThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &dataThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *dataThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *dataThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dataThread))
        return static_cast<void*>(const_cast< dataThread*>(this));
    return QThread::qt_metacast(_clname);
}

int dataThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_fftThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_fftThread[] = {
    "fftThread\0"
};

void fftThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData fftThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject fftThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_fftThread,
      qt_meta_data_fftThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &fftThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *fftThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *fftThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fftThread))
        return static_cast<void*>(const_cast< fftThread*>(this));
    return QThread::qt_metacast(_clname);
}

int fftThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
