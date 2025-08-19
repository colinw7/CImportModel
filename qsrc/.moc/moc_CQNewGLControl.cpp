/****************************************************************************
** Meta object code from reading C++ file 'CQNewGLControl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQNewGLControl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQNewGLControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQNewGLControl_t {
    QByteArrayData data[3];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLControl_t qt_meta_stringdata_CQNewGLControl = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CQNewGLControl"
QT_MOC_LITERAL(1, 15, 7), // "tabSlot"
QT_MOC_LITERAL(2, 23, 0) // ""

    },
    "CQNewGLControl\0tabSlot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLControl[] = {

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
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CQNewGLControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLControl.data,
    qt_meta_data_CQNewGLControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLControl.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CQNewGLControlFrame_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLControlFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLControlFrame_t qt_meta_stringdata_CQNewGLControlFrame = {
    {
QT_MOC_LITERAL(0, 0, 19) // "CQNewGLControlFrame"

    },
    "CQNewGLControlFrame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLControlFrame[] = {

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

void CQNewGLControlFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLControlFrame::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLControlFrame.data,
    qt_meta_data_CQNewGLControlFrame,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLControlFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLControlFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLControlFrame.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLControlFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CQNewGLGeneralControl_t {
    QByteArrayData data[25];
    char stringdata0[309];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLGeneralControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLGeneralControl_t qt_meta_stringdata_CQNewGLGeneralControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLGeneralControl"
QT_MOC_LITERAL(1, 22, 11), // "bgColorSlot"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 1), // "c"
QT_MOC_LITERAL(4, 37, 13), // "depthTestSlot"
QT_MOC_LITERAL(5, 51, 8), // "cullSlot"
QT_MOC_LITERAL(6, 60, 13), // "frontFaceSlot"
QT_MOC_LITERAL(7, 74, 15), // "polygonLineSlot"
QT_MOC_LITERAL(8, 90, 15), // "showNormalsSlot"
QT_MOC_LITERAL(9, 106, 10), // "flipYZSlot"
QT_MOC_LITERAL(10, 117, 8), // "axisSlot"
QT_MOC_LITERAL(11, 126, 15), // "invertDepthSlot"
QT_MOC_LITERAL(12, 142, 9), // "orthoSlot"
QT_MOC_LITERAL(13, 152, 9), // "uvMapSlot"
QT_MOC_LITERAL(14, 162, 11), // "ambientSlot"
QT_MOC_LITERAL(15, 174, 11), // "diffuseSlot"
QT_MOC_LITERAL(16, 186, 12), // "emissionSlot"
QT_MOC_LITERAL(17, 199, 17), // "ambientFactorSlot"
QT_MOC_LITERAL(18, 217, 1), // "v"
QT_MOC_LITERAL(19, 219, 17), // "diffuseFactorSlot"
QT_MOC_LITERAL(20, 237, 18), // "specularFactorSlot"
QT_MOC_LITERAL(21, 256, 19), // "shininessFactorSlot"
QT_MOC_LITERAL(22, 276, 18), // "emissiveFactorSlot"
QT_MOC_LITERAL(23, 295, 8), // "typeSlot"
QT_MOC_LITERAL(24, 304, 4) // "type"

    },
    "CQNewGLGeneralControl\0bgColorSlot\0\0c\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "polygonLineSlot\0showNormalsSlot\0"
    "flipYZSlot\0axisSlot\0invertDepthSlot\0"
    "orthoSlot\0uvMapSlot\0ambientSlot\0"
    "diffuseSlot\0emissionSlot\0ambientFactorSlot\0"
    "v\0diffuseFactorSlot\0specularFactorSlot\0"
    "shininessFactorSlot\0emissiveFactorSlot\0"
    "typeSlot\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLGeneralControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  114,    2, 0x08 /* Private */,
       4,    1,  117,    2, 0x08 /* Private */,
       5,    1,  120,    2, 0x08 /* Private */,
       6,    1,  123,    2, 0x08 /* Private */,
       7,    1,  126,    2, 0x08 /* Private */,
       8,    1,  129,    2, 0x08 /* Private */,
       9,    1,  132,    2, 0x08 /* Private */,
      10,    1,  135,    2, 0x08 /* Private */,
      11,    1,  138,    2, 0x08 /* Private */,
      12,    1,  141,    2, 0x08 /* Private */,
      13,    1,  144,    2, 0x08 /* Private */,
      14,    1,  147,    2, 0x08 /* Private */,
      15,    1,  150,    2, 0x08 /* Private */,
      16,    1,  153,    2, 0x08 /* Private */,
      17,    1,  156,    2, 0x08 /* Private */,
      19,    1,  159,    2, 0x08 /* Private */,
      20,    1,  162,    2, 0x08 /* Private */,
      21,    1,  165,    2, 0x08 /* Private */,
      22,    1,  168,    2, 0x08 /* Private */,
      23,    1,  171,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void, QMetaType::Int,   24,

       0        // eod
};

void CQNewGLGeneralControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLGeneralControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bgColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->polygonLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->showNormalsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->flipYZSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->axisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->invertDepthSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->orthoSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->uvMapSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->ambientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 12: _t->diffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 13: _t->emissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 14: _t->ambientFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->diffuseFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->specularFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->shininessFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->emissiveFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLGeneralControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLGeneralControl.data,
    qt_meta_data_CQNewGLGeneralControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLGeneralControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLGeneralControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLGeneralControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLGeneralControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLCameraControl_t {
    QByteArrayData data[13];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLCameraControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLCameraControl_t qt_meta_stringdata_CQNewGLCameraControl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "CQNewGLCameraControl"
QT_MOC_LITERAL(1, 21, 10), // "rotateSlot"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 1), // "b"
QT_MOC_LITERAL(4, 35, 8), // "zoomSlot"
QT_MOC_LITERAL(5, 44, 1), // "r"
QT_MOC_LITERAL(6, 46, 8), // "nearSlot"
QT_MOC_LITERAL(7, 55, 7), // "farSlot"
QT_MOC_LITERAL(8, 63, 7), // "yawSlot"
QT_MOC_LITERAL(9, 71, 9), // "pitchSlot"
QT_MOC_LITERAL(10, 81, 8), // "rollSlot"
QT_MOC_LITERAL(11, 90, 7), // "posSlot"
QT_MOC_LITERAL(12, 98, 9) // "resetSlot"

    },
    "CQNewGLCameraControl\0rotateSlot\0\0b\0"
    "zoomSlot\0r\0nearSlot\0farSlot\0yawSlot\0"
    "pitchSlot\0rollSlot\0posSlot\0resetSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCameraControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x08 /* Private */,
       4,    1,   62,    2, 0x08 /* Private */,
       6,    1,   65,    2, 0x08 /* Private */,
       7,    1,   68,    2, 0x08 /* Private */,
       8,    1,   71,    2, 0x08 /* Private */,
       9,    1,   74,    2, 0x08 /* Private */,
      10,    1,   77,    2, 0x08 /* Private */,
      11,    0,   80,    2, 0x08 /* Private */,
      12,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLCameraControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLCameraControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rotateSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->zoomSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->posSlot(); break;
        case 8: _t->resetSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLCameraControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLCameraControl.data,
    qt_meta_data_CQNewGLCameraControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLCameraControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLCameraControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLCameraControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLCameraControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLLightControl_t {
    QByteArrayData data[8];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLLightControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLLightControl_t qt_meta_stringdata_CQNewGLLightControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CQNewGLLightControl"
QT_MOC_LITERAL(1, 20, 17), // "lightSelectedSlot"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 56, 7), // "posSlot"
QT_MOC_LITERAL(5, 64, 9), // "colorSlot"
QT_MOC_LITERAL(6, 74, 1), // "c"
QT_MOC_LITERAL(7, 76, 7) // "addSlot"

    },
    "CQNewGLLightControl\0lightSelectedSlot\0"
    "\0QListWidgetItem*\0posSlot\0colorSlot\0"
    "c\0addSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLLightControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x08 /* Private */,
       4,    0,   39,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       7,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    6,
    QMetaType::Void,

       0        // eod
};

void CQNewGLLightControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLLightControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->lightSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->posSlot(); break;
        case 2: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 3: _t->addSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLLightControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLLightControl.data,
    qt_meta_data_CQNewGLLightControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLLightControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLLightControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLLightControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLLightControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLObjectsControl_t {
    QByteArrayData data[16];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLObjectsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLObjectsControl_t qt_meta_stringdata_CQNewGLObjectsControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLObjectsControl"
QT_MOC_LITERAL(1, 22, 17), // "updateModelMatrix"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(4, 60, 10), // "selectSlot"
QT_MOC_LITERAL(5, 71, 12), // "deselectSlot"
QT_MOC_LITERAL(6, 84, 8), // "showSlot"
QT_MOC_LITERAL(7, 93, 8), // "hideSlot"
QT_MOC_LITERAL(8, 102, 10), // "centerSlot"
QT_MOC_LITERAL(9, 113, 8), // "sizeSlot"
QT_MOC_LITERAL(10, 122, 13), // "translateSlot"
QT_MOC_LITERAL(11, 136, 10), // "rotateSlot"
QT_MOC_LITERAL(12, 147, 14), // "diffuseMapSlot"
QT_MOC_LITERAL(13, 162, 13), // "normalMapSlot"
QT_MOC_LITERAL(14, 176, 15), // "specularMapSlot"
QT_MOC_LITERAL(15, 192, 15) // "emissiveMapSlot"

    },
    "CQNewGLObjectsControl\0updateModelMatrix\0"
    "\0objectSelectedSlot\0selectSlot\0"
    "deselectSlot\0showSlot\0hideSlot\0"
    "centerSlot\0sizeSlot\0translateSlot\0"
    "rotateSlot\0diffuseMapSlot\0normalMapSlot\0"
    "specularMapSlot\0emissiveMapSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    0,   85,    2, 0x08 /* Private */,
       4,    0,   86,    2, 0x08 /* Private */,
       5,    0,   87,    2, 0x08 /* Private */,
       6,    0,   88,    2, 0x08 /* Private */,
       7,    0,   89,    2, 0x08 /* Private */,
       8,    0,   90,    2, 0x08 /* Private */,
       9,    0,   91,    2, 0x08 /* Private */,
      10,    0,   92,    2, 0x08 /* Private */,
      11,    0,   93,    2, 0x08 /* Private */,
      12,    0,   94,    2, 0x08 /* Private */,
      13,    0,   95,    2, 0x08 /* Private */,
      14,    0,   96,    2, 0x08 /* Private */,
      15,    0,   97,    2, 0x08 /* Private */,

 // slots: parameters
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
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLObjectsControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLObjectsControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateModelMatrix(); break;
        case 1: _t->objectSelectedSlot(); break;
        case 2: _t->selectSlot(); break;
        case 3: _t->deselectSlot(); break;
        case 4: _t->showSlot(); break;
        case 5: _t->hideSlot(); break;
        case 6: _t->centerSlot(); break;
        case 7: _t->sizeSlot(); break;
        case 8: _t->translateSlot(); break;
        case 9: _t->rotateSlot(); break;
        case 10: _t->diffuseMapSlot(); break;
        case 11: _t->normalMapSlot(); break;
        case 12: _t->specularMapSlot(); break;
        case 13: _t->emissiveMapSlot(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLObjectsControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLObjectsControl.data,
    qt_meta_data_CQNewGLObjectsControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLObjectsControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLObjectsControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLObjectsControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLObjectsControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CQNewGLTexturesControl_t {
    QByteArrayData data[5];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLTexturesControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLTexturesControl_t qt_meta_stringdata_CQNewGLTexturesControl = {
    {
QT_MOC_LITERAL(0, 0, 22), // "CQNewGLTexturesControl"
QT_MOC_LITERAL(1, 23, 19), // "textureSelectedSlot"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 61, 8) // "loadSlot"

    },
    "CQNewGLTexturesControl\0textureSelectedSlot\0"
    "\0QListWidgetItem*\0loadSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTexturesControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x08 /* Private */,
       4,    0,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLTexturesControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLTexturesControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->textureSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->loadSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLTexturesControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLTexturesControl.data,
    qt_meta_data_CQNewGLTexturesControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLTexturesControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLTexturesControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLTexturesControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLTexturesControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLTextureImage_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLTextureImage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLTextureImage_t qt_meta_stringdata_CQNewGLTextureImage = {
    {
QT_MOC_LITERAL(0, 0, 19) // "CQNewGLTextureImage"

    },
    "CQNewGLTextureImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTextureImage[] = {

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

void CQNewGLTextureImage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLTextureImage::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLTextureImage.data,
    qt_meta_data_CQNewGLTextureImage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLTextureImage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLTextureImage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLTextureImage.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLTextureImage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CQNewGLTextureChooser_t {
    QByteArrayData data[4];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLTextureChooser_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLTextureChooser_t qt_meta_stringdata_CQNewGLTextureChooser = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLTextureChooser"
QT_MOC_LITERAL(1, 22, 14), // "textureChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19) // "currentIndexChanged"

    },
    "CQNewGLTextureChooser\0textureChanged\0"
    "\0currentIndexChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTextureChooser[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   25,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CQNewGLTextureChooser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLTextureChooser *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->textureChanged(); break;
        case 1: _t->currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLTextureChooser::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLTextureChooser::textureChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLTextureChooser::staticMetaObject = { {
    QMetaObject::SuperData::link<QComboBox::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLTextureChooser.data,
    qt_meta_data_CQNewGLTextureChooser,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLTextureChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLTextureChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLTextureChooser.stringdata0))
        return static_cast<void*>(this);
    return QComboBox::qt_metacast(_clname);
}

int CQNewGLTextureChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CQNewGLTextureChooser::textureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CQNewGLUVControl_t {
    QByteArrayData data[4];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLUVControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLUVControl_t qt_meta_stringdata_CQNewGLUVControl = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CQNewGLUVControl"
QT_MOC_LITERAL(1, 17, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11) // "typeChanged"

    },
    "CQNewGLUVControl\0objectSelectedSlot\0"
    "\0typeChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLUVControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    1,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CQNewGLUVControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLUVControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->objectSelectedSlot(); break;
        case 1: _t->typeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLUVControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLUVControl.data,
    qt_meta_data_CQNewGLUVControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLUVControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLUVControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLUVControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLUVControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLAnimControl_t {
    QByteArrayData data[11];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLAnimControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLAnimControl_t qt_meta_stringdata_CQNewGLAnimControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLAnimControl"
QT_MOC_LITERAL(1, 19, 11), // "enabledSlot"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 9), // "bonesSlot"
QT_MOC_LITERAL(4, 42, 18), // "bonesTransformSlot"
QT_MOC_LITERAL(5, 61, 11), // "nameChanged"
QT_MOC_LITERAL(6, 73, 8), // "timeSlot"
QT_MOC_LITERAL(7, 82, 8), // "playSlot"
QT_MOC_LITERAL(8, 91, 9), // "pauseSlot"
QT_MOC_LITERAL(9, 101, 8), // "stepSlot"
QT_MOC_LITERAL(10, 110, 9) // "timerSlot"

    },
    "CQNewGLAnimControl\0enabledSlot\0\0"
    "bonesSlot\0bonesTransformSlot\0nameChanged\0"
    "timeSlot\0playSlot\0pauseSlot\0stepSlot\0"
    "timerSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLAnimControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x08 /* Private */,
       3,    1,   62,    2, 0x08 /* Private */,
       4,    1,   65,    2, 0x08 /* Private */,
       5,    1,   68,    2, 0x08 /* Private */,
       6,    1,   71,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    0,   75,    2, 0x08 /* Private */,
       9,    0,   76,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLAnimControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLAnimControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->bonesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->bonesTransformSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->nameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->timeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->playSlot(); break;
        case 6: _t->pauseSlot(); break;
        case 7: _t->stepSlot(); break;
        case 8: _t->timerSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLAnimControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLAnimControl.data,
    qt_meta_data_CQNewGLAnimControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLAnimControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLAnimControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLAnimControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLAnimControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLBonesControl_t {
    QByteArrayData data[10];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLBonesControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLBonesControl_t qt_meta_stringdata_CQNewGLBonesControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CQNewGLBonesControl"
QT_MOC_LITERAL(1, 20, 16), // "boneSelectedSlot"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 55, 17), // "matrixTypeChanged"
QT_MOC_LITERAL(5, 73, 11), // "inverseSlot"
QT_MOC_LITERAL(6, 85, 15), // "animNameChanged"
QT_MOC_LITERAL(7, 101, 3), // "ind"
QT_MOC_LITERAL(8, 105, 12), // "animTimeSlot"
QT_MOC_LITERAL(9, 118, 1) // "t"

    },
    "CQNewGLBonesControl\0boneSelectedSlot\0"
    "\0QListWidgetItem*\0matrixTypeChanged\0"
    "inverseSlot\0animNameChanged\0ind\0"
    "animTimeSlot\0t"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLBonesControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x08 /* Private */,
       4,    1,   44,    2, 0x08 /* Private */,
       5,    1,   47,    2, 0x08 /* Private */,
       6,    1,   50,    2, 0x08 /* Private */,
       8,    1,   53,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Double,    9,

       0        // eod
};

void CQNewGLBonesControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLBonesControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->boneSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->matrixTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->inverseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->animNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLBonesControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLBonesControl.data,
    qt_meta_data_CQNewGLBonesControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLBonesControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLBonesControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLBonesControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLBonesControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CQNewGLObjectsList_t {
    QByteArrayData data[5];
    char stringdata0[75];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLObjectsList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLObjectsList_t qt_meta_stringdata_CQNewGLObjectsList = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLObjectsList"
QT_MOC_LITERAL(1, 19, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(4, 58, 16) // "QListWidgetItem*"

    },
    "CQNewGLObjectsList\0currentItemChanged\0"
    "\0objectSelectedSlot\0QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsList[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   25,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,

       0        // eod
};

void CQNewGLObjectsList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLObjectsList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->objectSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLObjectsList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLObjectsList::currentItemChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLObjectsList::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLObjectsList.data,
    qt_meta_data_CQNewGLObjectsList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLObjectsList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLObjectsList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLObjectsList.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLObjectsList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CQNewGLObjectsList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
