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
    QByteArrayData data[15];
    char stringdata0[192];
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
QT_MOC_LITERAL(5, 49, 21), // "currentTextureChanged"
QT_MOC_LITERAL(6, 71, 22), // "currentMaterialChanged"
QT_MOC_LITERAL(7, 94, 13), // "materialAdded"
QT_MOC_LITERAL(8, 108, 15), // "materialChanged"
QT_MOC_LITERAL(9, 124, 15), // "boneNodeChanged"
QT_MOC_LITERAL(10, 140, 15), // "animNameChanged"
QT_MOC_LITERAL(11, 156, 15), // "animTimeChanged"
QT_MOC_LITERAL(12, 172, 7), // "tabSlot"
QT_MOC_LITERAL(13, 180, 1), // "i"
QT_MOC_LITERAL(14, 182, 9) // "timerSlot"

    },
    "CQCamera3DApp\0timerStep\0\0modelAdded\0"
    "textureAdded\0currentTextureChanged\0"
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
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    0,   77,    2, 0x06 /* Public */,
       6,    0,   78,    2, 0x06 /* Public */,
       7,    0,   79,    2, 0x06 /* Public */,
       8,    0,   80,    2, 0x06 /* Public */,
       9,    0,   81,    2, 0x06 /* Public */,
      10,    0,   82,    2, 0x06 /* Public */,
      11,    0,   83,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   84,    2, 0x0a /* Public */,
      14,    0,   87,    2, 0x0a /* Public */,

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

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   13,
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
        case 3: _t->currentTextureChanged(); break;
        case 4: _t->currentMaterialChanged(); break;
        case 5: _t->materialAdded(); break;
        case 6: _t->materialChanged(); break;
        case 7: _t->boneNodeChanged(); break;
        case 8: _t->animNameChanged(); break;
        case 9: _t->animTimeChanged(); break;
        case 10: _t->tabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->timerSlot(); break;
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::currentTextureChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::currentMaterialChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::materialAdded)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::materialChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::boneNodeChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animNameChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (CQCamera3DApp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DApp::animTimeChanged)) {
                *result = 9;
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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
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
void CQCamera3DApp::currentTextureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQCamera3DApp::currentMaterialChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQCamera3DApp::materialAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CQCamera3DApp::materialChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CQCamera3DApp::boneNodeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void CQCamera3DApp::animNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void CQCamera3DApp::animTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
