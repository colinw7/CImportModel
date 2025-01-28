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
    QByteArrayData data[6];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLCanvas_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLCanvas_t qt_meta_stringdata_CQNewGLCanvas = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQNewGLCanvas"
QT_MOC_LITERAL(1, 14, 7), // "bgColor"
QT_MOC_LITERAL(2, 22, 9), // "depthTest"
QT_MOC_LITERAL(3, 32, 8), // "cullFace"
QT_MOC_LITERAL(4, 41, 9), // "frontFace"
QT_MOC_LITERAL(5, 51, 11) // "polygonLine"

    },
    "CQNewGLCanvas\0bgColor\0depthTest\0"
    "cullFace\0frontFace\0polygonLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCanvas[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QColor, 0x00095103,
       2, QMetaType::Bool, 0x00095103,
       3, QMetaType::Bool, 0x00095103,
       4, QMetaType::Bool, 0x00095103,
       5, QMetaType::Bool, 0x00095103,

       0        // eod
};

void CQNewGLCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQNewGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->bgColor(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isDepthTest(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isCullFace(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->isFrontFace(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->isPolygonLine(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQNewGLCanvas *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBgColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setDepthTest(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setCullFace(*reinterpret_cast< bool*>(_v)); break;
        case 3: _t->setFrontFace(*reinterpret_cast< bool*>(_v)); break;
        case 4: _t->setPolygonLine(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<QGLWidget::staticMetaObject>(),
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
    return QGLWidget::qt_metacast(_clname);
}

int CQNewGLCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
