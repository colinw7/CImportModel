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
    QByteArrayData data[8];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLControl_t qt_meta_stringdata_CQNewGLControl = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CQNewGLControl"
QT_MOC_LITERAL(1, 15, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "modelTabSlot"
QT_MOC_LITERAL(4, 43, 17), // "invalidateCameras"
QT_MOC_LITERAL(5, 61, 16), // "invalidateLights"
QT_MOC_LITERAL(6, 78, 18), // "invalidateTextures"
QT_MOC_LITERAL(7, 97, 14) // "updateTextures"

    },
    "CQNewGLControl\0updateWidgets\0\0"
    "modelTabSlot\0invalidateCameras\0"
    "invalidateLights\0invalidateTextures\0"
    "updateTextures"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    1,   45,    2, 0x08 /* Private */,
       4,    0,   48,    2, 0x08 /* Private */,
       5,    0,   49,    2, 0x08 /* Private */,
       6,    0,   50,    2, 0x08 /* Private */,
       7,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->modelTabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->invalidateCameras(); break;
        case 3: _t->invalidateLights(); break;
        case 4: _t->invalidateTextures(); break;
        case 5: _t->updateTextures(); break;
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
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLFractalControl_t {
    QByteArrayData data[9];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLFractalControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLFractalControl_t qt_meta_stringdata_CQNewGLFractalControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLFractalControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 8), // "typeSlot"
QT_MOC_LITERAL(4, 46, 13), // "wireframeSlot"
QT_MOC_LITERAL(5, 60, 11), // "textureSlot"
QT_MOC_LITERAL(6, 72, 8), // "axesSlot"
QT_MOC_LITERAL(7, 81, 13), // "pointSizeSlot"
QT_MOC_LITERAL(8, 95, 12) // "generateSlot"

    },
    "CQNewGLFractalControl\0updateWidgets\0"
    "\0typeSlot\0wireframeSlot\0textureSlot\0"
    "axesSlot\0pointSizeSlot\0generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLFractalControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    1,   50,    2, 0x08 /* Private */,
       4,    1,   53,    2, 0x08 /* Private */,
       5,    1,   56,    2, 0x08 /* Private */,
       6,    1,   59,    2, 0x08 /* Private */,
       7,    1,   62,    2, 0x08 /* Private */,
       8,    0,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLFractalControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLFractalControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->textureSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->axesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->pointSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLFractalControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLFractalControl.data,
    qt_meta_data_CQNewGLFractalControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLFractalControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLFractalControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLFractalControl.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLFractalControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
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
    QByteArrayData data[11];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLGeneralControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLGeneralControl_t qt_meta_stringdata_CQNewGLGeneralControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLGeneralControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11), // "bgColorSlot"
QT_MOC_LITERAL(4, 49, 1), // "c"
QT_MOC_LITERAL(5, 51, 13), // "depthTestSlot"
QT_MOC_LITERAL(6, 65, 8), // "cullSlot"
QT_MOC_LITERAL(7, 74, 13), // "frontFaceSlot"
QT_MOC_LITERAL(8, 88, 14), // "showOrientSlot"
QT_MOC_LITERAL(9, 103, 8), // "typeSlot"
QT_MOC_LITERAL(10, 112, 4) // "type"

    },
    "CQNewGLGeneralControl\0updateWidgets\0"
    "\0bgColorSlot\0c\0depthTestSlot\0cullSlot\0"
    "frontFaceSlot\0showOrientSlot\0typeSlot\0"
    "type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLGeneralControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    1,   50,    2, 0x08 /* Private */,
       5,    1,   53,    2, 0x08 /* Private */,
       6,    1,   56,    2, 0x08 /* Private */,
       7,    1,   59,    2, 0x08 /* Private */,
       8,    1,   62,    2, 0x08 /* Private */,
       9,    1,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void CQNewGLGeneralControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLGeneralControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->bgColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 2: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->showOrientSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
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
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLCameraControl_t {
    QByteArrayData data[30];
    char stringdata0[280];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLCameraControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLCameraControl_t qt_meta_stringdata_CQNewGLCameraControl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "CQNewGLCameraControl"
QT_MOC_LITERAL(1, 21, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 8), // "showSlot"
QT_MOC_LITERAL(4, 45, 1), // "b"
QT_MOC_LITERAL(5, 47, 10), // "followSlot"
QT_MOC_LITERAL(6, 58, 11), // "eyelineSlot"
QT_MOC_LITERAL(7, 70, 9), // "orthoSlot"
QT_MOC_LITERAL(8, 80, 17), // "updateCurrentSlot"
QT_MOC_LITERAL(9, 98, 10), // "rotateSlot"
QT_MOC_LITERAL(10, 109, 12), // "rotateAtSlot"
QT_MOC_LITERAL(11, 122, 1), // "i"
QT_MOC_LITERAL(12, 124, 10), // "strafeSlot"
QT_MOC_LITERAL(13, 135, 8), // "zoomSlot"
QT_MOC_LITERAL(14, 144, 1), // "r"
QT_MOC_LITERAL(15, 146, 8), // "nearSlot"
QT_MOC_LITERAL(16, 155, 7), // "farSlot"
QT_MOC_LITERAL(17, 163, 7), // "yawSlot"
QT_MOC_LITERAL(18, 171, 9), // "pitchSlot"
QT_MOC_LITERAL(19, 181, 8), // "rollSlot"
QT_MOC_LITERAL(20, 190, 9), // "speedSlot"
QT_MOC_LITERAL(21, 200, 12), // "positionSlot"
QT_MOC_LITERAL(22, 213, 10), // "originSlot"
QT_MOC_LITERAL(23, 224, 6), // "upSlot"
QT_MOC_LITERAL(24, 231, 9), // "rightSlot"
QT_MOC_LITERAL(25, 241, 9), // "colorSlot"
QT_MOC_LITERAL(26, 251, 1), // "c"
QT_MOC_LITERAL(27, 253, 7), // "addSlot"
QT_MOC_LITERAL(28, 261, 9), // "resetSlot"
QT_MOC_LITERAL(29, 271, 8) // "initSlot"

    },
    "CQNewGLCameraControl\0updateWidgets\0\0"
    "showSlot\0b\0followSlot\0eyelineSlot\0"
    "orthoSlot\0updateCurrentSlot\0rotateSlot\0"
    "rotateAtSlot\0i\0strafeSlot\0zoomSlot\0r\0"
    "nearSlot\0farSlot\0yawSlot\0pitchSlot\0"
    "rollSlot\0speedSlot\0positionSlot\0"
    "originSlot\0upSlot\0rightSlot\0colorSlot\0"
    "c\0addSlot\0resetSlot\0initSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCameraControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  134,    2, 0x0a /* Public */,
       3,    1,  135,    2, 0x08 /* Private */,
       5,    1,  138,    2, 0x08 /* Private */,
       6,    1,  141,    2, 0x08 /* Private */,
       7,    1,  144,    2, 0x08 /* Private */,
       8,    1,  147,    2, 0x08 /* Private */,
       9,    1,  150,    2, 0x08 /* Private */,
      10,    1,  153,    2, 0x08 /* Private */,
      12,    1,  156,    2, 0x08 /* Private */,
      13,    1,  159,    2, 0x08 /* Private */,
      15,    1,  162,    2, 0x08 /* Private */,
      16,    1,  165,    2, 0x08 /* Private */,
      17,    1,  168,    2, 0x08 /* Private */,
      18,    1,  171,    2, 0x08 /* Private */,
      19,    1,  174,    2, 0x08 /* Private */,
      20,    1,  177,    2, 0x08 /* Private */,
      21,    0,  180,    2, 0x08 /* Private */,
      22,    0,  181,    2, 0x08 /* Private */,
      23,    0,  182,    2, 0x08 /* Private */,
      24,    0,  183,    2, 0x08 /* Private */,
      25,    1,  184,    2, 0x08 /* Private */,
      27,    0,  187,    2, 0x08 /* Private */,
      28,    0,  188,    2, 0x08 /* Private */,
      29,    0,  189,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,   26,
    QMetaType::Void,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->followSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->eyelineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->orthoSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->updateCurrentSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->rotateSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->rotateAtSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->strafeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->zoomSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->speedSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->positionSlot(); break;
        case 17: _t->originSlot(); break;
        case 18: _t->upSlot(); break;
        case 19: _t->rightSlot(); break;
        case 20: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 21: _t->addSlot(); break;
        case 22: _t->resetSlot(); break;
        case 23: _t->initSlot(); break;
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
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 24;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLAnnotationsControl_t {
    QByteArrayData data[20];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLAnnotationsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLAnnotationsControl_t qt_meta_stringdata_CQNewGLAnnotationsControl = {
    {
QT_MOC_LITERAL(0, 0, 25), // "CQNewGLAnnotationsControl"
QT_MOC_LITERAL(1, 26, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 15), // "showNormalsSlot"
QT_MOC_LITERAL(4, 57, 18), // "showNewNormalsSlot"
QT_MOC_LITERAL(5, 76, 15), // "normalsSizeSlot"
QT_MOC_LITERAL(6, 92, 1), // "r"
QT_MOC_LITERAL(7, 94, 16), // "normalsColorSlot"
QT_MOC_LITERAL(8, 111, 1), // "c"
QT_MOC_LITERAL(9, 113, 16), // "tangentSpaceSlot"
QT_MOC_LITERAL(10, 130, 12), // "showBBoxSlot"
QT_MOC_LITERAL(11, 143, 12), // "showHullSlot"
QT_MOC_LITERAL(12, 156, 13), // "showBasisSlot"
QT_MOC_LITERAL(13, 170, 1), // "b"
QT_MOC_LITERAL(14, 172, 13), // "basisSizeSlot"
QT_MOC_LITERAL(15, 186, 14), // "basisWidthSlot"
QT_MOC_LITERAL(16, 201, 10), // "basisUSlot"
QT_MOC_LITERAL(17, 212, 10), // "basisVSlot"
QT_MOC_LITERAL(18, 223, 10), // "basisWSlot"
QT_MOC_LITERAL(19, 234, 17) // "showWireframeSlot"

    },
    "CQNewGLAnnotationsControl\0updateWidgets\0"
    "\0showNormalsSlot\0showNewNormalsSlot\0"
    "normalsSizeSlot\0r\0normalsColorSlot\0c\0"
    "tangentSpaceSlot\0showBBoxSlot\0"
    "showHullSlot\0showBasisSlot\0b\0basisSizeSlot\0"
    "basisWidthSlot\0basisUSlot\0basisVSlot\0"
    "basisWSlot\0showWireframeSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLAnnotationsControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a /* Public */,
       3,    1,   90,    2, 0x08 /* Private */,
       4,    1,   93,    2, 0x08 /* Private */,
       5,    1,   96,    2, 0x08 /* Private */,
       7,    1,   99,    2, 0x08 /* Private */,
       9,    1,  102,    2, 0x08 /* Private */,
      10,    1,  105,    2, 0x08 /* Private */,
      11,    1,  108,    2, 0x08 /* Private */,
      12,    1,  111,    2, 0x08 /* Private */,
      14,    1,  114,    2, 0x08 /* Private */,
      15,    1,  117,    2, 0x08 /* Private */,
      16,    0,  120,    2, 0x08 /* Private */,
      17,    0,  121,    2, 0x08 /* Private */,
      18,    0,  122,    2, 0x08 /* Private */,
      19,    1,  123,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::QColor,    8,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CQNewGLAnnotationsControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLAnnotationsControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showNormalsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->showNewNormalsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->normalsSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->normalsColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 5: _t->tangentSpaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->showBBoxSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->showHullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->showBasisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->basisSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->basisWidthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->basisUSlot(); break;
        case 12: _t->basisVSlot(); break;
        case 13: _t->basisWSlot(); break;
        case 14: _t->showWireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLAnnotationsControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLAnnotationsControl.data,
    qt_meta_data_CQNewGLAnnotationsControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLAnnotationsControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLAnnotationsControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLAnnotationsControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLAnnotationsControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CQNewGLLightsControl_t {
    QByteArrayData data[22];
    char stringdata0[268];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLLightsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLLightsControl_t qt_meta_stringdata_CQNewGLLightsControl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "CQNewGLLightsControl"
QT_MOC_LITERAL(1, 21, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 11), // "ambientSlot"
QT_MOC_LITERAL(4, 48, 1), // "c"
QT_MOC_LITERAL(5, 50, 11), // "diffuseSlot"
QT_MOC_LITERAL(6, 62, 12), // "emissionSlot"
QT_MOC_LITERAL(7, 75, 17), // "ambientFactorSlot"
QT_MOC_LITERAL(8, 93, 1), // "v"
QT_MOC_LITERAL(9, 95, 17), // "diffuseFactorSlot"
QT_MOC_LITERAL(10, 113, 18), // "specularFactorSlot"
QT_MOC_LITERAL(11, 132, 19), // "shininessFactorSlot"
QT_MOC_LITERAL(12, 152, 18), // "emissiveFactorSlot"
QT_MOC_LITERAL(13, 171, 8), // "showSlot"
QT_MOC_LITERAL(14, 180, 8), // "typeSlot"
QT_MOC_LITERAL(15, 189, 11), // "enabledSlot"
QT_MOC_LITERAL(16, 201, 12), // "positionSlot"
QT_MOC_LITERAL(17, 214, 13), // "directionSlot"
QT_MOC_LITERAL(18, 228, 9), // "colorSlot"
QT_MOC_LITERAL(19, 238, 10), // "radiusSlot"
QT_MOC_LITERAL(20, 249, 10), // "cutoffSlot"
QT_MOC_LITERAL(21, 260, 7) // "addSlot"

    },
    "CQNewGLLightsControl\0updateWidgets\0\0"
    "ambientSlot\0c\0diffuseSlot\0emissionSlot\0"
    "ambientFactorSlot\0v\0diffuseFactorSlot\0"
    "specularFactorSlot\0shininessFactorSlot\0"
    "emissiveFactorSlot\0showSlot\0typeSlot\0"
    "enabledSlot\0positionSlot\0directionSlot\0"
    "colorSlot\0radiusSlot\0cutoffSlot\0addSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLLightsControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  104,    2, 0x0a /* Public */,
       3,    1,  105,    2, 0x08 /* Private */,
       5,    1,  108,    2, 0x08 /* Private */,
       6,    1,  111,    2, 0x08 /* Private */,
       7,    1,  114,    2, 0x08 /* Private */,
       9,    1,  117,    2, 0x08 /* Private */,
      10,    1,  120,    2, 0x08 /* Private */,
      11,    1,  123,    2, 0x08 /* Private */,
      12,    1,  126,    2, 0x08 /* Private */,
      13,    1,  129,    2, 0x08 /* Private */,
      14,    1,  132,    2, 0x08 /* Private */,
      15,    1,  135,    2, 0x08 /* Private */,
      16,    0,  138,    2, 0x08 /* Private */,
      17,    0,  139,    2, 0x08 /* Private */,
      18,    1,  140,    2, 0x08 /* Private */,
      19,    1,  143,    2, 0x08 /* Private */,
      20,    1,  146,    2, 0x08 /* Private */,
      21,    0,  149,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLLightsControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLLightsControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->ambientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 2: _t->diffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 3: _t->emissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 4: _t->ambientFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->diffuseFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->specularFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->shininessFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->emissiveFactorSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->positionSlot(); break;
        case 13: _t->directionSlot(); break;
        case 14: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 15: _t->radiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->cutoffSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->addSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLLightsControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLLightsControl.data,
    qt_meta_data_CQNewGLLightsControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLLightsControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLLightsControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLLightsControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLLightsControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLAxesControl_t {
    QByteArrayData data[5];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLAxesControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLAxesControl_t qt_meta_stringdata_CQNewGLAxesControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLAxesControl"
QT_MOC_LITERAL(1, 19, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "showSlot"
QT_MOC_LITERAL(4, 43, 1) // "b"

    },
    "CQNewGLAxesControl\0updateWidgets\0\0"
    "showSlot\0b"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLAxesControl[] = {

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
       1,    0,   24,    2, 0x0a /* Public */,
       3,    1,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,

       0        // eod
};

void CQNewGLAxesControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLAxesControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLAxesControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLAxesControl.data,
    qt_meta_data_CQNewGLAxesControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLAxesControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLAxesControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLAxesControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLAxesControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLObjectsControl_t {
    QByteArrayData data[24];
    char stringdata0[303];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLObjectsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLObjectsControl_t qt_meta_stringdata_CQNewGLObjectsControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLObjectsControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 17), // "updateModelMatrix"
QT_MOC_LITERAL(4, 55, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(5, 74, 10), // "selectSlot"
QT_MOC_LITERAL(6, 85, 12), // "deselectSlot"
QT_MOC_LITERAL(7, 98, 8), // "showSlot"
QT_MOC_LITERAL(8, 107, 8), // "hideSlot"
QT_MOC_LITERAL(9, 116, 11), // "currentSlot"
QT_MOC_LITERAL(10, 128, 9), // "solidSlot"
QT_MOC_LITERAL(11, 138, 13), // "wireframeSlot"
QT_MOC_LITERAL(12, 152, 10), // "flipYZSlot"
QT_MOC_LITERAL(13, 163, 15), // "invertDepthSlot"
QT_MOC_LITERAL(14, 179, 10), // "centerSlot"
QT_MOC_LITERAL(15, 190, 8), // "sizeSlot"
QT_MOC_LITERAL(16, 199, 13), // "translateSlot"
QT_MOC_LITERAL(17, 213, 10), // "rotateSlot"
QT_MOC_LITERAL(18, 224, 14), // "diffuseMapSlot"
QT_MOC_LITERAL(19, 239, 13), // "normalMapSlot"
QT_MOC_LITERAL(20, 253, 15), // "specularMapSlot"
QT_MOC_LITERAL(21, 269, 15), // "emissiveMapSlot"
QT_MOC_LITERAL(22, 285, 7), // "addSlot"
QT_MOC_LITERAL(23, 293, 9) // "resetSlot"

    },
    "CQNewGLObjectsControl\0updateWidgets\0"
    "\0updateModelMatrix\0objectSelectedSlot\0"
    "selectSlot\0deselectSlot\0showSlot\0"
    "hideSlot\0currentSlot\0solidSlot\0"
    "wireframeSlot\0flipYZSlot\0invertDepthSlot\0"
    "centerSlot\0sizeSlot\0translateSlot\0"
    "rotateSlot\0diffuseMapSlot\0normalMapSlot\0"
    "specularMapSlot\0emissiveMapSlot\0addSlot\0"
    "resetSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  124,    2, 0x0a /* Public */,
       3,    0,  125,    2, 0x08 /* Private */,
       4,    0,  126,    2, 0x08 /* Private */,
       5,    0,  127,    2, 0x08 /* Private */,
       6,    0,  128,    2, 0x08 /* Private */,
       7,    0,  129,    2, 0x08 /* Private */,
       8,    0,  130,    2, 0x08 /* Private */,
       9,    0,  131,    2, 0x08 /* Private */,
      10,    1,  132,    2, 0x08 /* Private */,
      11,    1,  135,    2, 0x08 /* Private */,
      12,    1,  138,    2, 0x08 /* Private */,
      13,    1,  141,    2, 0x08 /* Private */,
      14,    0,  144,    2, 0x08 /* Private */,
      15,    0,  145,    2, 0x08 /* Private */,
      16,    0,  146,    2, 0x08 /* Private */,
      17,    0,  147,    2, 0x08 /* Private */,
      18,    0,  148,    2, 0x08 /* Private */,
      19,    0,  149,    2, 0x08 /* Private */,
      20,    0,  150,    2, 0x08 /* Private */,
      21,    0,  151,    2, 0x08 /* Private */,
      22,    0,  152,    2, 0x08 /* Private */,
      23,    0,  153,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->updateModelMatrix(); break;
        case 2: _t->objectSelectedSlot(); break;
        case 3: _t->selectSlot(); break;
        case 4: _t->deselectSlot(); break;
        case 5: _t->showSlot(); break;
        case 6: _t->hideSlot(); break;
        case 7: _t->currentSlot(); break;
        case 8: _t->solidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->flipYZSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->invertDepthSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->centerSlot(); break;
        case 13: _t->sizeSlot(); break;
        case 14: _t->translateSlot(); break;
        case 15: _t->rotateSlot(); break;
        case 16: _t->diffuseMapSlot(); break;
        case 17: _t->normalMapSlot(); break;
        case 18: _t->specularMapSlot(); break;
        case 19: _t->emissiveMapSlot(); break;
        case 20: _t->addSlot(); break;
        case 21: _t->resetSlot(); break;
        default: ;
        }
    }
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
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLTexturesControl_t {
    QByteArrayData data[6];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLTexturesControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLTexturesControl_t qt_meta_stringdata_CQNewGLTexturesControl = {
    {
QT_MOC_LITERAL(0, 0, 22), // "CQNewGLTexturesControl"
QT_MOC_LITERAL(1, 23, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19), // "textureSelectedSlot"
QT_MOC_LITERAL(4, 58, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 75, 8) // "loadSlot"

    },
    "CQNewGLTexturesControl\0updateWidgets\0"
    "\0textureSelectedSlot\0QListWidgetItem*\0"
    "loadSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTexturesControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    2,   30,    2, 0x08 /* Private */,
       5,    0,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLTexturesControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLTexturesControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->textureSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->loadSlot(); break;
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
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
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
struct qt_meta_stringdata_CQNewGLUVControl_t {
    QByteArrayData data[5];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLUVControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLUVControl_t qt_meta_stringdata_CQNewGLUVControl = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CQNewGLUVControl"
QT_MOC_LITERAL(1, 17, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(4, 51, 11) // "typeChanged"

    },
    "CQNewGLUVControl\0updateWidgets\0\0"
    "objectSelectedSlot\0typeChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLUVControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    1,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->objectSelectedSlot(); break;
        case 2: _t->typeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLUVControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
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
    return QFrame::qt_metacast(_clname);
}

int CQNewGLUVControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLAnimControl_t {
    QByteArrayData data[17];
    char stringdata0[215];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLAnimControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLAnimControl_t qt_meta_stringdata_CQNewGLAnimControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLAnimControl"
QT_MOC_LITERAL(1, 19, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 11), // "enabledSlot"
QT_MOC_LITERAL(4, 46, 9), // "bonesSlot"
QT_MOC_LITERAL(5, 56, 16), // "boneVerticesSlot"
QT_MOC_LITERAL(6, 73, 12), // "boneListSlot"
QT_MOC_LITERAL(7, 86, 14), // "boneSelectSlot"
QT_MOC_LITERAL(8, 101, 16), // "boneDeselectSlot"
QT_MOC_LITERAL(9, 118, 18), // "showBonePointsSlot"
QT_MOC_LITERAL(10, 137, 18), // "bonesTransformSlot"
QT_MOC_LITERAL(11, 156, 11), // "nameChanged"
QT_MOC_LITERAL(12, 168, 8), // "timeSlot"
QT_MOC_LITERAL(13, 177, 8), // "playSlot"
QT_MOC_LITERAL(14, 186, 9), // "pauseSlot"
QT_MOC_LITERAL(15, 196, 8), // "stepSlot"
QT_MOC_LITERAL(16, 205, 9) // "timerSlot"

    },
    "CQNewGLAnimControl\0updateWidgets\0\0"
    "enabledSlot\0bonesSlot\0boneVerticesSlot\0"
    "boneListSlot\0boneSelectSlot\0"
    "boneDeselectSlot\0showBonePointsSlot\0"
    "bonesTransformSlot\0nameChanged\0timeSlot\0"
    "playSlot\0pauseSlot\0stepSlot\0timerSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLAnimControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a /* Public */,
       3,    1,   90,    2, 0x08 /* Private */,
       4,    1,   93,    2, 0x08 /* Private */,
       5,    1,   96,    2, 0x08 /* Private */,
       6,    0,   99,    2, 0x08 /* Private */,
       7,    0,  100,    2, 0x08 /* Private */,
       8,    0,  101,    2, 0x08 /* Private */,
       9,    1,  102,    2, 0x08 /* Private */,
      10,    1,  105,    2, 0x08 /* Private */,
      11,    1,  108,    2, 0x08 /* Private */,
      12,    1,  111,    2, 0x08 /* Private */,
      13,    0,  114,    2, 0x08 /* Private */,
      14,    0,  115,    2, 0x08 /* Private */,
      15,    0,  116,    2, 0x08 /* Private */,
      16,    0,  117,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->bonesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->boneVerticesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->boneListSlot(); break;
        case 5: _t->boneSelectSlot(); break;
        case 6: _t->boneDeselectSlot(); break;
        case 7: _t->showBonePointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->bonesTransformSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->nameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->timeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->playSlot(); break;
        case 12: _t->pauseSlot(); break;
        case 13: _t->stepSlot(); break;
        case 14: _t->timerSlot(); break;
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
struct qt_meta_stringdata_CQNewGLBonesControl_t {
    QByteArrayData data[10];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLBonesControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLBonesControl_t qt_meta_stringdata_CQNewGLBonesControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CQNewGLBonesControl"
QT_MOC_LITERAL(1, 20, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 13), // "bonesListSlot"
QT_MOC_LITERAL(4, 49, 17), // "matrixTypeChanged"
QT_MOC_LITERAL(5, 67, 11), // "inverseSlot"
QT_MOC_LITERAL(6, 79, 15), // "animNameChanged"
QT_MOC_LITERAL(7, 95, 3), // "ind"
QT_MOC_LITERAL(8, 99, 12), // "animTimeSlot"
QT_MOC_LITERAL(9, 112, 1) // "t"

    },
    "CQNewGLBonesControl\0updateWidgets\0\0"
    "bonesListSlot\0matrixTypeChanged\0"
    "inverseSlot\0animNameChanged\0ind\0"
    "animTimeSlot\0t"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLBonesControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x08 /* Private */,
       5,    1,   49,    2, 0x08 /* Private */,
       6,    1,   52,    2, 0x08 /* Private */,
       8,    1,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->bonesListSlot(); break;
        case 2: _t->matrixTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->inverseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->animNameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
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
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLTerrainControl_t {
    QByteArrayData data[11];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLTerrainControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLTerrainControl_t qt_meta_stringdata_CQNewGLTerrainControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLTerrainControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 8), // "typeSlot"
QT_MOC_LITERAL(4, 46, 9), // "widthSlot"
QT_MOC_LITERAL(5, 56, 10), // "heightSlot"
QT_MOC_LITERAL(6, 67, 12), // "gridSizeSlot"
QT_MOC_LITERAL(7, 80, 11), // "octavesSlot"
QT_MOC_LITERAL(8, 92, 11), // "textureSlot"
QT_MOC_LITERAL(9, 104, 8), // "gridSlot"
QT_MOC_LITERAL(10, 113, 12) // "generateSlot"

    },
    "CQNewGLTerrainControl\0updateWidgets\0"
    "\0typeSlot\0widthSlot\0heightSlot\0"
    "gridSizeSlot\0octavesSlot\0textureSlot\0"
    "gridSlot\0generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTerrainControl[] = {

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
       1,    0,   59,    2, 0x0a /* Public */,
       3,    1,   60,    2, 0x08 /* Private */,
       4,    1,   63,    2, 0x08 /* Private */,
       5,    1,   66,    2, 0x08 /* Private */,
       6,    1,   69,    2, 0x08 /* Private */,
       7,    1,   72,    2, 0x08 /* Private */,
       8,    1,   75,    2, 0x08 /* Private */,
       9,    1,   78,    2, 0x08 /* Private */,
      10,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLTerrainControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLTerrainControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->heightSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->gridSizeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->octavesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->textureSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->gridSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLTerrainControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLTerrainControl.data,
    qt_meta_data_CQNewGLTerrainControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLTerrainControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLTerrainControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLTerrainControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLTerrainControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLMazeControl_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLMazeControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLMazeControl_t qt_meta_stringdata_CQNewGLMazeControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLMazeControl"
QT_MOC_LITERAL(1, 19, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 9), // "widthSlot"
QT_MOC_LITERAL(4, 44, 10), // "heightSlot"
QT_MOC_LITERAL(5, 55, 12) // "generateSlot"

    },
    "CQNewGLMazeControl\0updateWidgets\0\0"
    "widthSlot\0heightSlot\0generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLMazeControl[] = {

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
       1,    0,   34,    2, 0x0a /* Public */,
       3,    1,   35,    2, 0x08 /* Private */,
       4,    1,   38,    2, 0x08 /* Private */,
       5,    0,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLMazeControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLMazeControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->heightSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLMazeControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLMazeControl.data,
    qt_meta_data_CQNewGLMazeControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLMazeControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLMazeControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLMazeControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLMazeControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLSkyboxControl_t {
    QByteArrayData data[7];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLSkyboxControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLSkyboxControl_t qt_meta_stringdata_CQNewGLSkyboxControl = {
    {
QT_MOC_LITERAL(0, 0, 20), // "CQNewGLSkyboxControl"
QT_MOC_LITERAL(1, 21, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 11), // "dirNameSlot"
QT_MOC_LITERAL(4, 48, 9), // "widthSlot"
QT_MOC_LITERAL(5, 58, 13), // "wireframeSlot"
QT_MOC_LITERAL(6, 72, 12) // "generateSlot"

    },
    "CQNewGLSkyboxControl\0updateWidgets\0\0"
    "dirNameSlot\0widthSlot\0wireframeSlot\0"
    "generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLSkyboxControl[] = {

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
       1,    0,   39,    2, 0x0a /* Public */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    1,   41,    2, 0x08 /* Private */,
       5,    1,   44,    2, 0x08 /* Private */,
       6,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void CQNewGLSkyboxControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLSkyboxControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->dirNameSlot(); break;
        case 2: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLSkyboxControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLSkyboxControl.data,
    qt_meta_data_CQNewGLSkyboxControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLSkyboxControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLSkyboxControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLSkyboxControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLSkyboxControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLEmitterControl_t {
    QByteArrayData data[22];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLEmitterControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLEmitterControl_t qt_meta_stringdata_CQNewGLEmitterControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLEmitterControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 19), // "emitterSelectedSlot"
QT_MOC_LITERAL(4, 57, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 74, 8), // "typeSlot"
QT_MOC_LITERAL(6, 83, 11), // "enabledSlot"
QT_MOC_LITERAL(7, 95, 11), // "runningSlot"
QT_MOC_LITERAL(8, 107, 12), // "positionSlot"
QT_MOC_LITERAL(9, 120, 15), // "minVelocitySlot"
QT_MOC_LITERAL(10, 136, 15), // "maxVelocitySlot"
QT_MOC_LITERAL(11, 152, 12), // "intervalSlot"
QT_MOC_LITERAL(12, 165, 11), // "gravitySlot"
QT_MOC_LITERAL(13, 177, 8), // "sizeSlot"
QT_MOC_LITERAL(14, 186, 16), // "maxParticlesSlot"
QT_MOC_LITERAL(15, 203, 10), // "maxAgeSlot"
QT_MOC_LITERAL(16, 214, 14), // "startColorSlot"
QT_MOC_LITERAL(17, 229, 1), // "c"
QT_MOC_LITERAL(18, 231, 12), // "endColorSlot"
QT_MOC_LITERAL(19, 244, 13), // "wireframeSlot"
QT_MOC_LITERAL(20, 258, 9), // "imageSlot"
QT_MOC_LITERAL(21, 268, 7) // "addSlot"

    },
    "CQNewGLEmitterControl\0updateWidgets\0"
    "\0emitterSelectedSlot\0QListWidgetItem*\0"
    "typeSlot\0enabledSlot\0runningSlot\0"
    "positionSlot\0minVelocitySlot\0"
    "maxVelocitySlot\0intervalSlot\0gravitySlot\0"
    "sizeSlot\0maxParticlesSlot\0maxAgeSlot\0"
    "startColorSlot\0c\0endColorSlot\0"
    "wireframeSlot\0imageSlot\0addSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLEmitterControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  104,    2, 0x0a /* Public */,
       3,    2,  105,    2, 0x08 /* Private */,
       5,    1,  110,    2, 0x08 /* Private */,
       6,    1,  113,    2, 0x08 /* Private */,
       7,    1,  116,    2, 0x08 /* Private */,
       8,    0,  119,    2, 0x08 /* Private */,
       9,    0,  120,    2, 0x08 /* Private */,
      10,    0,  121,    2, 0x08 /* Private */,
      11,    1,  122,    2, 0x08 /* Private */,
      12,    1,  125,    2, 0x08 /* Private */,
      13,    1,  128,    2, 0x08 /* Private */,
      14,    1,  131,    2, 0x08 /* Private */,
      15,    1,  134,    2, 0x08 /* Private */,
      16,    1,  137,    2, 0x08 /* Private */,
      18,    1,  140,    2, 0x08 /* Private */,
      19,    1,  143,    2, 0x08 /* Private */,
      20,    0,  146,    2, 0x08 /* Private */,
      21,    0,  147,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QColor,   17,
    QMetaType::Void, QMetaType::QColor,   17,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLEmitterControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLEmitterControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->emitterSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->runningSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->positionSlot(); break;
        case 6: _t->minVelocitySlot(); break;
        case 7: _t->maxVelocitySlot(); break;
        case 8: _t->intervalSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->gravitySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->sizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->maxParticlesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->maxAgeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->startColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 14: _t->endColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 15: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->imageSlot(); break;
        case 17: _t->addSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLEmitterControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLEmitterControl.data,
    qt_meta_data_CQNewGLEmitterControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLEmitterControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLEmitterControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLEmitterControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLEmitterControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLDrawTreeControl_t {
    QByteArrayData data[14];
    char stringdata0[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLDrawTreeControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLDrawTreeControl_t qt_meta_stringdata_CQNewGLDrawTreeControl = {
    {
QT_MOC_LITERAL(0, 0, 22), // "CQNewGLDrawTreeControl"
QT_MOC_LITERAL(1, 23, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 9), // "widthSlot"
QT_MOC_LITERAL(4, 48, 1), // "w"
QT_MOC_LITERAL(5, 50, 10), // "heightSlot"
QT_MOC_LITERAL(6, 61, 1), // "h"
QT_MOC_LITERAL(7, 63, 17), // "leftDirectionSlot"
QT_MOC_LITERAL(8, 81, 18), // "rightDirectionSlot"
QT_MOC_LITERAL(9, 100, 13), // "leftAlphaSlot"
QT_MOC_LITERAL(10, 114, 14), // "rightAlphaSlot"
QT_MOC_LITERAL(11, 129, 9), // "depthSlot"
QT_MOC_LITERAL(12, 139, 5), // "depth"
QT_MOC_LITERAL(13, 145, 12) // "generateSlot"

    },
    "CQNewGLDrawTreeControl\0updateWidgets\0"
    "\0widthSlot\0w\0heightSlot\0h\0leftDirectionSlot\0"
    "rightDirectionSlot\0leftAlphaSlot\0"
    "rightAlphaSlot\0depthSlot\0depth\0"
    "generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLDrawTreeControl[] = {

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
       1,    0,   59,    2, 0x0a /* Public */,
       3,    1,   60,    2, 0x0a /* Public */,
       5,    1,   63,    2, 0x0a /* Public */,
       7,    0,   66,    2, 0x0a /* Public */,
       8,    0,   67,    2, 0x0a /* Public */,
       9,    1,   68,    2, 0x0a /* Public */,
      10,    1,   71,    2, 0x0a /* Public */,
      11,    1,   74,    2, 0x0a /* Public */,
      13,    0,   77,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,

       0        // eod
};

void CQNewGLDrawTreeControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLDrawTreeControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->heightSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->leftDirectionSlot(); break;
        case 4: _t->rightDirectionSlot(); break;
        case 5: _t->leftAlphaSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->rightAlphaSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->depthSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLDrawTreeControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLDrawTreeControl.data,
    qt_meta_data_CQNewGLDrawTreeControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLDrawTreeControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLDrawTreeControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLDrawTreeControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLDrawTreeControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLShapeControl_t {
    QByteArrayData data[21];
    char stringdata0[233];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLShapeControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLShapeControl_t qt_meta_stringdata_CQNewGLShapeControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CQNewGLShapeControl"
QT_MOC_LITERAL(1, 20, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 17), // "shapeSelectedSlot"
QT_MOC_LITERAL(4, 53, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 70, 8), // "typeSlot"
QT_MOC_LITERAL(6, 79, 8), // "showSlot"
QT_MOC_LITERAL(7, 88, 13), // "wireframeSlot"
QT_MOC_LITERAL(8, 102, 9), // "solidSlot"
QT_MOC_LITERAL(9, 112, 9), // "startSlot"
QT_MOC_LITERAL(10, 122, 7), // "endSlot"
QT_MOC_LITERAL(11, 130, 9), // "widthSlot"
QT_MOC_LITERAL(12, 140, 9), // "colorSlot"
QT_MOC_LITERAL(13, 150, 1), // "c"
QT_MOC_LITERAL(14, 152, 14), // "angleStartSlot"
QT_MOC_LITERAL(15, 167, 14), // "angleDeltaSlot"
QT_MOC_LITERAL(16, 182, 8), // "sizeSlot"
QT_MOC_LITERAL(17, 191, 11), // "textureSlot"
QT_MOC_LITERAL(18, 203, 7), // "addSlot"
QT_MOC_LITERAL(19, 211, 10), // "updateSlot"
QT_MOC_LITERAL(20, 222, 10) // "deleteSlot"

    },
    "CQNewGLShapeControl\0updateWidgets\0\0"
    "shapeSelectedSlot\0QListWidgetItem*\0"
    "typeSlot\0showSlot\0wireframeSlot\0"
    "solidSlot\0startSlot\0endSlot\0widthSlot\0"
    "colorSlot\0c\0angleStartSlot\0angleDeltaSlot\0"
    "sizeSlot\0textureSlot\0addSlot\0updateSlot\0"
    "deleteSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLShapeControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x0a /* Public */,
       3,    2,  100,    2, 0x08 /* Private */,
       5,    1,  105,    2, 0x08 /* Private */,
       6,    1,  108,    2, 0x08 /* Private */,
       7,    1,  111,    2, 0x08 /* Private */,
       8,    1,  114,    2, 0x08 /* Private */,
       9,    0,  117,    2, 0x08 /* Private */,
      10,    0,  118,    2, 0x08 /* Private */,
      11,    1,  119,    2, 0x08 /* Private */,
      12,    1,  122,    2, 0x08 /* Private */,
      14,    1,  125,    2, 0x08 /* Private */,
      15,    1,  128,    2, 0x08 /* Private */,
      16,    0,  131,    2, 0x08 /* Private */,
      17,    0,  132,    2, 0x08 /* Private */,
      18,    0,  133,    2, 0x08 /* Private */,
      19,    0,  134,    2, 0x08 /* Private */,
      20,    0,  135,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::QColor,   13,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLShapeControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLShapeControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->shapeSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->solidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->startSlot(); break;
        case 7: _t->endSlot(); break;
        case 8: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 10: _t->angleStartSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->angleDeltaSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->sizeSlot(); break;
        case 13: _t->textureSlot(); break;
        case 14: _t->addSlot(); break;
        case 15: _t->updateSlot(); break;
        case 16: _t->deleteSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLShapeControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLShapeControl.data,
    qt_meta_data_CQNewGLShapeControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLShapeControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLShapeControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLShapeControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLShapeControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQNewGLControlFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLObjectsList_t {
    QByteArrayData data[6];
    char stringdata0[89];
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
QT_MOC_LITERAL(3, 39, 13), // "updateObjects"
QT_MOC_LITERAL(4, 53, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(5, 72, 16) // "QTreeWidgetItem*"

    },
    "CQNewGLObjectsList\0currentItemChanged\0"
    "\0updateObjects\0objectSelectedSlot\0"
    "QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsList[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   30,    2, 0x0a /* Public */,
       4,    2,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5,    2,    2,

       0        // eod
};

void CQNewGLObjectsList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLObjectsList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->updateObjects(); break;
        case 2: _t->objectSelectedSlot((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
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
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CQNewGLObjectsList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CQNewGLCamerasList_t {
    QByteArrayData data[5];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLCamerasList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLCamerasList_t qt_meta_stringdata_CQNewGLCamerasList = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLCamerasList"
QT_MOC_LITERAL(1, 19, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 15), // "currentItemSlot"
QT_MOC_LITERAL(4, 55, 16) // "QListWidgetItem*"

    },
    "CQNewGLCamerasList\0currentItemChanged\0"
    "\0currentItemSlot\0QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCamerasList[] = {

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

void CQNewGLCamerasList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLCamerasList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->currentItemSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLCamerasList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLCamerasList::currentItemChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLCamerasList::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLCamerasList.data,
    qt_meta_data_CQNewGLCamerasList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLCamerasList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLCamerasList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLCamerasList.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLCamerasList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void CQNewGLCamerasList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CQNewGLLightsList_t {
    QByteArrayData data[5];
    char stringdata0[71];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLLightsList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLLightsList_t qt_meta_stringdata_CQNewGLLightsList = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CQNewGLLightsList"
QT_MOC_LITERAL(1, 18, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 15), // "currentItemSlot"
QT_MOC_LITERAL(4, 54, 16) // "QListWidgetItem*"

    },
    "CQNewGLLightsList\0currentItemChanged\0"
    "\0currentItemSlot\0QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLLightsList[] = {

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

void CQNewGLLightsList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLLightsList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->currentItemSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLLightsList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLLightsList::currentItemChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLLightsList::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLLightsList.data,
    qt_meta_data_CQNewGLLightsList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLLightsList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLLightsList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLLightsList.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLLightsList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void CQNewGLLightsList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CQNewGLBonesList_t {
    QByteArrayData data[5];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLBonesList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLBonesList_t qt_meta_stringdata_CQNewGLBonesList = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CQNewGLBonesList"
QT_MOC_LITERAL(1, 17, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 15), // "currentItemSlot"
QT_MOC_LITERAL(4, 53, 16) // "QTreeWidgetItem*"

    },
    "CQNewGLBonesList\0currentItemChanged\0"
    "\0currentItemSlot\0QTreeWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLBonesList[] = {

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

void CQNewGLBonesList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLBonesList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->currentItemSlot((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLBonesList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLBonesList::currentItemChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLBonesList::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLBonesList.data,
    qt_meta_data_CQNewGLBonesList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLBonesList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLBonesList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLBonesList.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLBonesList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void CQNewGLBonesList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CQNewGLShapesList_t {
    QByteArrayData data[6];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLShapesList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLShapesList_t qt_meta_stringdata_CQNewGLShapesList = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CQNewGLShapesList"
QT_MOC_LITERAL(1, 18, 18), // "currentItemChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "invalidateShapes"
QT_MOC_LITERAL(4, 55, 15), // "currentItemSlot"
QT_MOC_LITERAL(5, 71, 16) // "QListWidgetItem*"

    },
    "CQNewGLShapesList\0currentItemChanged\0"
    "\0invalidateShapes\0currentItemSlot\0"
    "QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLShapesList[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   30,    2, 0x08 /* Private */,
       4,    2,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5,    2,    2,

       0        // eod
};

void CQNewGLShapesList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLShapesList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->currentItemChanged(); break;
        case 1: _t->invalidateShapes(); break;
        case 2: _t->currentItemSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQNewGLShapesList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQNewGLShapesList::currentItemChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLShapesList::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLShapesList.data,
    qt_meta_data_CQNewGLShapesList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLShapesList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLShapesList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLShapesList.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQNewGLShapesList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CQNewGLShapesList::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
