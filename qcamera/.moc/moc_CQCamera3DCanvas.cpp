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
    QByteArrayData data[14];
    char stringdata0[186];
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
QT_MOC_LITERAL(3, 31, 13), // "materialAdded"
QT_MOC_LITERAL(4, 45, 11), // "cameraAdded"
QT_MOC_LITERAL(5, 57, 10), // "lightAdded"
QT_MOC_LITERAL(6, 68, 11), // "objectAdded"
QT_MOC_LITERAL(7, 80, 14), // "objectsChanged"
QT_MOC_LITERAL(8, 95, 14), // "eyeLineChanged"
QT_MOC_LITERAL(9, 110, 15), // "editTypeChanged"
QT_MOC_LITERAL(10, 126, 17), // "updateObjectsData"
QT_MOC_LITERAL(11, 144, 14), // "addObjectsData"
QT_MOC_LITERAL(12, 159, 12), // "updateStatus"
QT_MOC_LITERAL(13, 172, 13) // "cameraChanged"

    },
    "CQCamera3DCanvas\0stateChanged\0\0"
    "materialAdded\0cameraAdded\0lightAdded\0"
    "objectAdded\0objectsChanged\0eyeLineChanged\0"
    "editTypeChanged\0updateObjectsData\0"
    "addObjectsData\0updateStatus\0cameraChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DCanvas[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    0,   77,    2, 0x06 /* Public */,
       6,    0,   78,    2, 0x06 /* Public */,
       7,    0,   79,    2, 0x06 /* Public */,
       8,    0,   80,    2, 0x06 /* Public */,
       9,    0,   81,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    0,   85,    2, 0x08 /* Private */,

 // signals: parameters
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
        case 1: _t->materialAdded(); break;
        case 2: _t->cameraAdded(); break;
        case 3: _t->lightAdded(); break;
        case 4: _t->objectAdded(); break;
        case 5: _t->objectsChanged(); break;
        case 6: _t->eyeLineChanged(); break;
        case 7: _t->editTypeChanged(); break;
        case 8: _t->updateObjectsData(); break;
        case 9: _t->addObjectsData(); break;
        case 10: _t->updateStatus(); break;
        case 11: _t->cameraChanged(); break;
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::materialAdded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::cameraAdded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::lightAdded)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::objectAdded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::objectsChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::eyeLineChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CQCamera3DCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQCamera3DCanvas::editTypeChanged)) {
                *result = 7;
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
void CQCamera3DCanvas::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CQCamera3DCanvas::materialAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CQCamera3DCanvas::cameraAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CQCamera3DCanvas::lightAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQCamera3DCanvas::objectAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQCamera3DCanvas::objectsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CQCamera3DCanvas::eyeLineChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CQCamera3DCanvas::editTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
