/****************************************************************************
** Meta object code from reading C++ file 'CQCamera3DApp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQCamera3DApp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQCamera3DApp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQCamera3DApp_t {
    QByteArrayData data[11];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DApp_t qt_meta_stringdata_CQCamera3DApp = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQCamera3DApp"
QT_MOC_LITERAL(1, 14, 9), // "timerStep"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "modelAdded"
QT_MOC_LITERAL(4, 36, 12), // "textureAdded"
QT_MOC_LITERAL(5, 49, 15), // "boneNodeChanged"
QT_MOC_LITERAL(6, 65, 15), // "animNameChanged"
QT_MOC_LITERAL(7, 81, 15), // "animTimeChanged"
QT_MOC_LITERAL(8, 97, 7), // "tabSlot"
QT_MOC_LITERAL(9, 105, 1), // "i"
QT_MOC_LITERAL(10, 107, 9) // "timerSlot"

    },
    "CQCamera3DApp\0timerStep\0\0modelAdded\0"
    "textureAdded\0boneNodeChanged\0"
    "animNameChanged\0animTimeChanged\0tabSlot\0"
    "i\0timerSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DApp[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    0,   58,    2, 0x06 /* Public */,
       7,    0,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   60,    2, 0x0a /* Public */,
      10,    0,   63,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,

       0        // eod
};

void CQCamera3DApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DApp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->timerStep(); break;
        case 1: _t->modelAdded(); break;
        case 2: _t->textureAdded(); break;
        case 3: _t->boneNodeChanged(); break;
        case 4: _t->animNameChanged(); break;
        case 5: _t->animTimeChanged(); break;
        case 6: _t->tabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->timerSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::timerStep)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::modelAdded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::textureAdded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::boneNodeChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animNameChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animTimeChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQCamera3DApp::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQCamera3DApp.data,
    qt_meta_data_CQCamera3DApp,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCamera3DApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCamera3DApp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCamera3DApp.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQCamera3DApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void CQCamera3DApp::timerStep()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CQCamera3DApp::modelAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CQCamera3DApp::textureAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CQCamera3DApp::boneNodeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQCamera3DApp::animNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQCamera3DApp::animTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
