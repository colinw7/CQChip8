/****************************************************************************
** Meta object code from reading C++ file 'CQChip8Test.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CQChip8Test.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQChip8Test.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQChip8Test_t {
    QByteArrayData data[7];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQChip8Test_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQChip8Test_t qt_meta_stringdata_CQChip8Test = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CQChip8Test"
QT_MOC_LITERAL(1, 12, 8), // "stepSlot"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 7), // "runSlot"
QT_MOC_LITERAL(4, 30, 8), // "stopSlot"
QT_MOC_LITERAL(5, 39, 8), // "contSlot"
QT_MOC_LITERAL(6, 48, 10) // "updateSlot"

    },
    "CQChip8Test\0stepSlot\0\0runSlot\0stopSlot\0"
    "contSlot\0updateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQChip8Test[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQChip8Test::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CQChip8Test *_t = static_cast<CQChip8Test *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stepSlot(); break;
        case 1: _t->runSlot(); break;
        case 2: _t->stopSlot(); break;
        case 3: _t->contSlot(); break;
        case 4: _t->updateSlot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CQChip8Test::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_CQChip8Test.data,
      qt_meta_data_CQChip8Test,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CQChip8Test::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQChip8Test::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQChip8Test.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQChip8Test::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
