/****************************************************************************
** Meta object code from reading C++ file 'mainform.h'
**
** Created: Thu Jul 18 11:27:15 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainform/mainform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mainForm[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   10,    9,    9, 0x0a,
      44,    9,    9,    9, 0x08,
      63,    9,    9,    9, 0x08,
      88,    9,    9,    9, 0x08,
     107,    9,    9,    9, 0x08,
     149,  128,    9,    9, 0x08,
     201,  178,    9,    9, 0x08,
     237,    9,    9,    9, 0x08,
     263,    9,    9,    9, 0x08,
     290,    9,    9,    9, 0x08,
     315,    9,    9,    9, 0x08,
     337,    9,    9,    9, 0x08,
     362,    9,    9,    9, 0x08,
     385,    9,    9,    9, 0x08,
     411,    9,    9,    9, 0x08,
     437,    9,    9,    9, 0x08,
     461,  456,    9,    9, 0x08,
     481,    9,    9,    9, 0x08,
     508,    9,    9,    9, 0x08,
     525,    9,    9,    9, 0x08,
     562,  560,    9,    9, 0x08,
     596,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_mainForm[] = {
    "mainForm\0\0helpStr\0refreshScpiState(QString)\0"
    "readKeyBoardData()\0resetUsbSlaveInterface()\0"
    "drawProcessSlots()\0drawsListDataSlots()\0"
    "Display,RatioProgBar\0drawProgBarSlots(bool,float)\0"
    "Display,current,amount\0"
    "drawDownloadProgSlots(bool,int,int)\0"
    "drawAnalogModulateSlots()\0"
    "drawDigitalModulateSlots()\0"
    "drawPluseModulateSlots()\0drawSystemParaSlots()\0"
    "drawArbitraryWaveSlots()\0"
    "drawDrgFunctionSlots()\0drawFreqInParamterSlots()\0"
    "drawAmplInParamterSlots()\0drawExfFrefSlots()\0"
    "type\0drawSweepSlots(int)\0"
    "drawFreqAndAmplMainSlots()\0drawStateSlots()\0"
    "drawInputParamterSlotsNoParamter()\0,\0"
    "drawInputParamterSlots(char*,int)\0"
    "drawButtonSlots()\0"
};

void mainForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        mainForm *_t = static_cast<mainForm *>(_o);
        switch (_id) {
        case 0: _t->refreshScpiState((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->readKeyBoardData(); break;
        case 2: _t->resetUsbSlaveInterface(); break;
        case 3: _t->drawProcessSlots(); break;
        case 4: _t->drawsListDataSlots(); break;
        case 5: _t->drawProgBarSlots((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 6: _t->drawDownloadProgSlots((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: _t->drawAnalogModulateSlots(); break;
        case 8: _t->drawDigitalModulateSlots(); break;
        case 9: _t->drawPluseModulateSlots(); break;
        case 10: _t->drawSystemParaSlots(); break;
        case 11: _t->drawArbitraryWaveSlots(); break;
        case 12: _t->drawDrgFunctionSlots(); break;
        case 13: _t->drawFreqInParamterSlots(); break;
        case 14: _t->drawAmplInParamterSlots(); break;
        case 15: _t->drawExfFrefSlots(); break;
        case 16: _t->drawSweepSlots((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->drawFreqAndAmplMainSlots(); break;
        case 18: _t->drawStateSlots(); break;
        case 19: _t->drawInputParamterSlotsNoParamter(); break;
        case 20: _t->drawInputParamterSlots((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->drawButtonSlots(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData mainForm::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject mainForm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_mainForm,
      qt_meta_data_mainForm, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &mainForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *mainForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *mainForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mainForm))
        return static_cast<void*>(const_cast< mainForm*>(this));
    return QWidget::qt_metacast(_clname);
}

int mainForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
