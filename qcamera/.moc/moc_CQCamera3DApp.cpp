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
    QByteArrayData data[17];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DApp_t qt_meta_stringdata_CQCamera3DApp = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQCamera3DApp"
QT_MOC_LITERAL(1, 14, 15), // "viewTypeChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "timerStep"
QT_MOC_LITERAL(4, 41, 10), // "modelAdded"
QT_MOC_LITERAL(5, 52, 21), // "currentTextureChanged"
QT_MOC_LITERAL(6, 74, 12), // "textureAdded"
QT_MOC_LITERAL(7, 87, 14), // "textureChanged"
QT_MOC_LITERAL(8, 102, 22), // "currentMaterialChanged"
QT_MOC_LITERAL(9, 125, 13), // "materialAdded"
QT_MOC_LITERAL(10, 139, 15), // "materialChanged"
QT_MOC_LITERAL(11, 155, 15), // "boneNodeChanged"
QT_MOC_LITERAL(12, 171, 15), // "animNameChanged"
QT_MOC_LITERAL(13, 187, 15), // "animTimeChanged"
QT_MOC_LITERAL(14, 203, 7), // "tabSlot"
QT_MOC_LITERAL(15, 211, 1), // "i"
QT_MOC_LITERAL(16, 213, 9) // "timerSlot"

    },
    "CQCamera3DApp\0viewTypeChanged\0\0timerStep\0"
    "modelAdded\0currentTextureChanged\0"
    "textureAdded\0textureChanged\0"
    "currentMaterialChanged\0materialAdded\0"
    "materialChanged\0boneNodeChanged\0"
    "animNameChanged\0animTimeChanged\0tabSlot\0"
    "i\0timerSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DApp[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    0,   85,    2, 0x06 /* Public */,
       4,    0,   86,    2, 0x06 /* Public */,
       5,    0,   87,    2, 0x06 /* Public */,
       6,    0,   88,    2, 0x06 /* Public */,
       7,    0,   89,    2, 0x06 /* Public */,
       8,    0,   90,    2, 0x06 /* Public */,
       9,    0,   91,    2, 0x06 /* Public */,
      10,    0,   92,    2, 0x06 /* Public */,
      11,    0,   93,    2, 0x06 /* Public */,
      12,    0,   94,    2, 0x06 /* Public */,
      13,    0,   95,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    1,   96,    2, 0x0a /* Public */,
      16,    0,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,

       0        // eod
};

void CQCamera3DApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DApp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->viewTypeChanged(); break;
        case 1: _t->timerStep(); break;
        case 2: _t->modelAdded(); break;
        case 3: _t->currentTextureChanged(); break;
        case 4: _t->textureAdded(); break;
        case 5: _t->textureChanged(); break;
        case 6: _t->currentMaterialChanged(); break;
        case 7: _t->materialAdded(); break;
        case 8: _t->materialChanged(); break;
        case 9: _t->boneNodeChanged(); break;
        case 10: _t->animNameChanged(); break;
        case 11: _t->animTimeChanged(); break;
        case 12: _t->tabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->timerSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::viewTypeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::timerStep)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::modelAdded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::currentTextureChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::textureAdded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::textureChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::currentMaterialChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::materialAdded)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::materialChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::boneNodeChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animNameChanged)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animTimeChanged)) {
                *result = 11;
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
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void CQCamera3DApp::viewTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CQCamera3DApp::timerStep()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CQCamera3DApp::modelAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CQCamera3DApp::currentTextureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQCamera3DApp::textureAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQCamera3DApp::textureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CQCamera3DApp::currentMaterialChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CQCamera3DApp::materialAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void CQCamera3DApp::materialChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void CQCamera3DApp::boneNodeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void CQCamera3DApp::animNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void CQCamera3DApp::animTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
