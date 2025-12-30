/****************************************************************************
** Meta object code from reading C++ file 'CQNewGLCanvas.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQNewGLCanvas.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQNewGLCanvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQNewGLCanvas_t {
    QByteArrayData data[14];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLCanvas_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLCanvas_t qt_meta_stringdata_CQNewGLCanvas = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQNewGLCanvas"
QT_MOC_LITERAL(1, 14, 20), // "currentObjectChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 20), // "currentCameraChanged"
QT_MOC_LITERAL(4, 57, 19), // "currentLightChanged"
QT_MOC_LITERAL(5, 77, 19), // "currentShapeChanged"
QT_MOC_LITERAL(6, 97, 11), // "cameraAdded"
QT_MOC_LITERAL(7, 109, 10), // "lightAdded"
QT_MOC_LITERAL(8, 120, 10), // "shapeAdded"
QT_MOC_LITERAL(9, 131, 12), // "shapeDeleted"
QT_MOC_LITERAL(10, 144, 9), // "depthTest"
QT_MOC_LITERAL(11, 154, 8), // "cullFace"
QT_MOC_LITERAL(12, 163, 9), // "frontFace"
QT_MOC_LITERAL(13, 173, 11) // "orientColor"

    },
    "CQNewGLCanvas\0currentObjectChanged\0\0"
    "currentCameraChanged\0currentLightChanged\0"
    "currentShapeChanged\0cameraAdded\0"
    "lightAdded\0shapeAdded\0shapeDeleted\0"
    "depthTest\0cullFace\0frontFace\0orientColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCanvas[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       4,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    0,   58,    2, 0x06 /* Public */,
       7,    0,   59,    2, 0x06 /* Public */,
       8,    0,   60,    2, 0x06 /* Public */,
       9,    0,   61,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::Bool, 0x00095103,
      11, QMetaType::Bool, 0x00095103,
      12, QMetaType::Bool, 0x00095103,
      13, QMetaType::Bool, 0x00095103,

       0        // eod
};

void CQNewGLCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLCanvas *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentObjectChanged(); break;
        case 1: _t->currentCameraChanged(); break;
        case 2: _t->currentLightChanged(); break;
        case 3: _t->currentShapeChanged(); break;
        case 4: _t->cameraAdded(); break;
        case 5: _t->lightAdded(); break;
        case 6: _t->shapeAdded(); break;
        case 7: _t->shapeDeleted(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::currentObjectChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::currentCameraChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::currentLightChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::currentShapeChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::cameraAdded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::lightAdded)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::shapeAdded)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CQNewGLCanvas::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCanvas::shapeDeleted)) {
                *result = 7;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQNewGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isDepthTest(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isCullFace(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isFrontFace(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->isOrientColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQNewGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDepthTest(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setCullFace(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setFrontFace(*reinterpret_cast< bool*>(_v)); break;
        case 3: _t->setOrientColor(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLWidget::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLCanvas.data,
    qt_meta_data_CQNewGLCanvas,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLCanvas.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLWidget::qt_metacast(_clname);
}

int CQNewGLCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CQNewGLCanvas::currentObjectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CQNewGLCanvas::currentCameraChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CQNewGLCanvas::currentLightChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CQNewGLCanvas::currentShapeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CQNewGLCanvas::cameraAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CQNewGLCanvas::lightAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CQNewGLCanvas::shapeAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CQNewGLCanvas::shapeDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
