/****************************************************************************
** Meta object code from reading C++ file 'CQCamera3DCanvas.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQCamera3DCanvas.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQCamera3DCanvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQCamera3DCanvas_t {
    QByteArrayData data[17];
    char stringdata0[233];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DCanvas_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DCanvas_t qt_meta_stringdata_CQCamera3DCanvas = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CQCamera3DCanvas"
QT_MOC_LITERAL(1, 17, 12), // "stateChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 18), // "cameraStateChanged"
QT_MOC_LITERAL(4, 50, 13), // "materialAdded"
QT_MOC_LITERAL(5, 64, 11), // "cameraAdded"
QT_MOC_LITERAL(6, 76, 10), // "lightAdded"
QT_MOC_LITERAL(7, 87, 11), // "objectAdded"
QT_MOC_LITERAL(8, 99, 14), // "objectsChanged"
QT_MOC_LITERAL(9, 114, 14), // "eyeLineChanged"
QT_MOC_LITERAL(10, 129, 15), // "editModeChanged"
QT_MOC_LITERAL(11, 145, 17), // "selectTypeChanged"
QT_MOC_LITERAL(12, 163, 15), // "editTypeChanged"
QT_MOC_LITERAL(13, 179, 17), // "updateObjectsData"
QT_MOC_LITERAL(14, 197, 8), // "addScene"
QT_MOC_LITERAL(15, 206, 12), // "updateStatus"
QT_MOC_LITERAL(16, 219, 13) // "cameraChanged"

    },
    "CQCamera3DCanvas\0stateChanged\0\0"
    "cameraStateChanged\0materialAdded\0"
    "cameraAdded\0lightAdded\0objectAdded\0"
    "objectsChanged\0eyeLineChanged\0"
    "editModeChanged\0selectTypeChanged\0"
    "editTypeChanged\0updateObjectsData\0"
    "addScene\0updateStatus\0cameraChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DCanvas[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    0,   90,    2, 0x06 /* Public */,
       4,    0,   91,    2, 0x06 /* Public */,
       5,    0,   92,    2, 0x06 /* Public */,
       6,    0,   93,    2, 0x06 /* Public */,
       7,    0,   94,    2, 0x06 /* Public */,
       8,    0,   95,    2, 0x06 /* Public */,
       9,    0,   96,    2, 0x06 /* Public */,
      10,    0,   97,    2, 0x06 /* Public */,
      11,    0,   98,    2, 0x06 /* Public */,
      12,    0,   99,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,  100,    2, 0x0a /* Public */,
      14,    0,  101,    2, 0x0a /* Public */,
      15,    0,  102,    2, 0x08 /* Private */,
      16,    0,  103,    2, 0x08 /* Private */,

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

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQCamera3DCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DCanvas *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stateChanged(); break;
        case 1: _t->cameraStateChanged(); break;
        case 2: _t->materialAdded(); break;
        case 3: _t->cameraAdded(); break;
        case 4: _t->lightAdded(); break;
        case 5: _t->objectAdded(); break;
        case 6: _t->objectsChanged(); break;
        case 7: _t->eyeLineChanged(); break;
        case 8: _t->editModeChanged(); break;
        case 9: _t->selectTypeChanged(); break;
        case 10: _t->editTypeChanged(); break;
        case 11: _t->updateObjectsData(); break;
        case 12: _t->addScene(); break;
        case 13: _t->updateStatus(); break;
        case 14: _t->cameraChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::cameraStateChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::materialAdded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::cameraAdded)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::lightAdded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::objectAdded)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::objectsChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::eyeLineChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::editModeChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::selectTypeChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::editTypeChanged)) {
                *result = 10;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQCamera3DCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<CQCamera3DWidget::staticMetaObject>(),
    qt_meta_stringdata_CQCamera3DCanvas.data,
    qt_meta_data_CQCamera3DCanvas,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCamera3DCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCamera3DCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCamera3DCanvas.stringdata0))
        return static_cast<void*>(this);
    return CQCamera3DWidget::qt_metacast(_clname);
}

int CQCamera3DCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQCamera3DWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void CQCamera3DCanvas::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CQCamera3DCanvas::cameraStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CQCamera3DCanvas::materialAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CQCamera3DCanvas::cameraAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQCamera3DCanvas::lightAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQCamera3DCanvas::objectAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CQCamera3DCanvas::objectsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CQCamera3DCanvas::eyeLineChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void CQCamera3DCanvas::editModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void CQCamera3DCanvas::selectTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void CQCamera3DCanvas::editTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
