/****************************************************************************
** Meta object code from reading C++ file 'CQCamera3DOpWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQCamera3DOpWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQCamera3DOpWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQCamera3DOpWidget_t {
    QByteArrayData data[3];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DOpWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DOpWidget_t qt_meta_stringdata_CQCamera3DOpWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQCamera3DOpWidget"
QT_MOC_LITERAL(1, 19, 9), // "closeSlot"
QT_MOC_LITERAL(2, 29, 0) // ""

    },
    "CQCamera3DOpWidget\0closeSlot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DOpWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CQCamera3DOpWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DOpWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->closeSlot(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQCamera3DOpWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<CQWinWidget::staticMetaObject>(),
    qt_meta_stringdata_CQCamera3DOpWidget.data,
    qt_meta_data_CQCamera3DOpWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCamera3DOpWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCamera3DOpWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCamera3DOpWidget.stringdata0))
        return static_cast<void*>(this);
    return CQWinWidget::qt_metacast(_clname);
}

int CQCamera3DOpWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQWinWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CQCamera3DOpContents_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DOpContents_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DOpContents_t qt_meta_stringdata_CQCamera3DOpContents = {
    {
QT_MOC_LITERAL(0, 0, 20) // "CQCamera3DOpContents"

    },
    "CQCamera3DOpContents"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DOpContents[] = {

 // content:
       8,       // revision
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

void CQCamera3DOpContents::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQCamera3DOpContents::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQCamera3DOpContents.data,
    qt_meta_data_CQCamera3DOpContents,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCamera3DOpContents::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCamera3DOpContents::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCamera3DOpContents.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQCamera3DOpContents::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
