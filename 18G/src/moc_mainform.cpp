/****************************************************************************
** Meta object code from reading C++ file 'mainform.h'
**
** Created: Fri Jan 18 15:28:57 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main/mainform.h"
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
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   10,    9,    9, 0x0a,
      44,    9,    9,    9, 0x0a,
      57,    9,    9,    9, 0x0a,
      67,    9,    9,    9, 0x0a,
      80,    9,    9,    9, 0x0a,
      91,    9,    9,    9, 0x0a,
     103,    9,    9,    9, 0x0a,
     116,    9,    9,    9, 0x0a,
     126,    9,    9,    9, 0x0a,
     138,    9,    9,    9, 0x08,
     157,    9,    9,    9, 0x08,
     192,  182,    9,    9, 0x08,
     245,  228,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_mainForm[] = {
    "mainForm\0\0helpStr\0refreshScpiState(QString)\0"
    "drawPreset()\0drawTop()\0drawButton()\0"
    "drawHelp()\0drawState()\0drawCanvas()\0"
    "drawAll()\0drawInput()\0readKeyBoardData()\0"
    "resetUsbSlaveInterface()\0title,msg\0"
    "showAlaramMessages(QString,QString)\0"
    "mainCode,subCode\0refreshMenuFrame(ulong,ulong)\0"
};

void mainForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        mainForm *_t = static_cast<mainForm *>(_o);
        switch (_id) {
        case 0: _t->refreshScpiState((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->drawPreset(); break;
        case 2: _t->drawTop(); break;
        case 3: _t->drawButton(); break;
        case 4: _t->drawHelp(); break;
        case 5: _t->drawState(); break;
        case 6: _t->drawCanvas(); break;
        case 7: _t->drawAll(); break;
        case 8: _t->drawInput(); break;
        case 9: _t->readKeyBoardData(); break;
        case 10: _t->resetUsbSlaveInterface(); break;
        case 11: _t->showAlaramMessages((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->refreshMenuFrame((*reinterpret_cast< ulong(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2]))); break;
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
