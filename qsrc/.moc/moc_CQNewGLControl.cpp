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
    QByteArrayData data[9];
    char stringdata0[129];
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
QT_MOC_LITERAL(7, 97, 16), // "invalidateShapes"
QT_MOC_LITERAL(8, 114, 14) // "updateTextures"

    },
    "CQNewGLControl\0updateWidgets\0\0"
    "modelTabSlot\0invalidateCameras\0"
    "invalidateLights\0invalidateTextures\0"
    "invalidateShapes\0updateTextures"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLControl[] = {

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
       4,    0,   53,    2, 0x08 /* Private */,
       5,    0,   54,    2, 0x08 /* Private */,
       6,    0,   55,    2, 0x08 /* Private */,
       7,    0,   56,    2, 0x08 /* Private */,
       8,    0,   57,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
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
        case 5: _t->invalidateShapes(); break;
        case 6: _t->updateTextures(); break;
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
    QByteArrayData data[14];
    char stringdata0[162];
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
QT_MOC_LITERAL(8, 88, 16), // "polygonSolidSlot"
QT_MOC_LITERAL(9, 105, 15), // "polygonLineSlot"
QT_MOC_LITERAL(10, 121, 10), // "flipYZSlot"
QT_MOC_LITERAL(11, 132, 15), // "invertDepthSlot"
QT_MOC_LITERAL(12, 148, 8), // "typeSlot"
QT_MOC_LITERAL(13, 157, 4) // "type"

    },
    "CQNewGLGeneralControl\0updateWidgets\0"
    "\0bgColorSlot\0c\0depthTestSlot\0cullSlot\0"
    "frontFaceSlot\0polygonSolidSlot\0"
    "polygonLineSlot\0flipYZSlot\0invertDepthSlot\0"
    "typeSlot\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLGeneralControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    1,   65,    2, 0x08 /* Private */,
       5,    1,   68,    2, 0x08 /* Private */,
       6,    1,   71,    2, 0x08 /* Private */,
       7,    1,   74,    2, 0x08 /* Private */,
       8,    1,   77,    2, 0x08 /* Private */,
       9,    1,   80,    2, 0x08 /* Private */,
      10,    1,   83,    2, 0x08 /* Private */,
      11,    1,   86,    2, 0x08 /* Private */,
      12,    1,   89,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   13,

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
        case 5: _t->polygonSolidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->polygonLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->flipYZSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->invertDepthSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
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
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLCameraControl_t {
    QByteArrayData data[32];
    char stringdata0[316];
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
QT_MOC_LITERAL(8, 80, 18), // "cameraSelectedSlot"
QT_MOC_LITERAL(9, 99, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(10, 116, 17), // "updateCurrentSlot"
QT_MOC_LITERAL(11, 134, 10), // "rotateSlot"
QT_MOC_LITERAL(12, 145, 12), // "rotateAtSlot"
QT_MOC_LITERAL(13, 158, 1), // "i"
QT_MOC_LITERAL(14, 160, 10), // "strafeSlot"
QT_MOC_LITERAL(15, 171, 8), // "zoomSlot"
QT_MOC_LITERAL(16, 180, 1), // "r"
QT_MOC_LITERAL(17, 182, 8), // "nearSlot"
QT_MOC_LITERAL(18, 191, 7), // "farSlot"
QT_MOC_LITERAL(19, 199, 7), // "yawSlot"
QT_MOC_LITERAL(20, 207, 9), // "pitchSlot"
QT_MOC_LITERAL(21, 217, 8), // "rollSlot"
QT_MOC_LITERAL(22, 226, 9), // "speedSlot"
QT_MOC_LITERAL(23, 236, 12), // "positionSlot"
QT_MOC_LITERAL(24, 249, 10), // "originSlot"
QT_MOC_LITERAL(25, 260, 6), // "upSlot"
QT_MOC_LITERAL(26, 267, 9), // "rightSlot"
QT_MOC_LITERAL(27, 277, 9), // "colorSlot"
QT_MOC_LITERAL(28, 287, 1), // "c"
QT_MOC_LITERAL(29, 289, 7), // "addSlot"
QT_MOC_LITERAL(30, 297, 9), // "resetSlot"
QT_MOC_LITERAL(31, 307, 8) // "initSlot"

    },
    "CQNewGLCameraControl\0updateWidgets\0\0"
    "showSlot\0b\0followSlot\0eyelineSlot\0"
    "orthoSlot\0cameraSelectedSlot\0"
    "QListWidgetItem*\0updateCurrentSlot\0"
    "rotateSlot\0rotateAtSlot\0i\0strafeSlot\0"
    "zoomSlot\0r\0nearSlot\0farSlot\0yawSlot\0"
    "pitchSlot\0rollSlot\0speedSlot\0positionSlot\0"
    "originSlot\0upSlot\0rightSlot\0colorSlot\0"
    "c\0addSlot\0resetSlot\0initSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCameraControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  139,    2, 0x0a /* Public */,
       3,    1,  140,    2, 0x08 /* Private */,
       5,    1,  143,    2, 0x08 /* Private */,
       6,    1,  146,    2, 0x08 /* Private */,
       7,    1,  149,    2, 0x08 /* Private */,
       8,    2,  152,    2, 0x08 /* Private */,
      10,    1,  157,    2, 0x08 /* Private */,
      11,    1,  160,    2, 0x08 /* Private */,
      12,    1,  163,    2, 0x08 /* Private */,
      14,    1,  166,    2, 0x08 /* Private */,
      15,    1,  169,    2, 0x08 /* Private */,
      17,    1,  172,    2, 0x08 /* Private */,
      18,    1,  175,    2, 0x08 /* Private */,
      19,    1,  178,    2, 0x08 /* Private */,
      20,    1,  181,    2, 0x08 /* Private */,
      21,    1,  184,    2, 0x08 /* Private */,
      22,    1,  187,    2, 0x08 /* Private */,
      23,    0,  190,    2, 0x08 /* Private */,
      24,    0,  191,    2, 0x08 /* Private */,
      25,    0,  192,    2, 0x08 /* Private */,
      26,    0,  193,    2, 0x08 /* Private */,
      27,    1,  194,    2, 0x08 /* Private */,
      29,    0,  197,    2, 0x08 /* Private */,
      30,    0,  198,    2, 0x08 /* Private */,
      31,    0,  199,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9,    2,    2,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,   28,
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
        case 5: _t->cameraSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 6: _t->updateCurrentSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->rotateSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->rotateAtSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->strafeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->zoomSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->speedSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->positionSlot(); break;
        case 18: _t->originSlot(); break;
        case 19: _t->upSlot(); break;
        case 20: _t->rightSlot(); break;
        case 21: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 22: _t->addSlot(); break;
        case 23: _t->resetSlot(); break;
        case 24: _t->initSlot(); break;
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
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 25;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLNormalsControl_t {
    QByteArrayData data[10];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLNormalsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLNormalsControl_t qt_meta_stringdata_CQNewGLNormalsControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLNormalsControl"
QT_MOC_LITERAL(1, 22, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 9), // "show1Slot"
QT_MOC_LITERAL(4, 47, 9), // "show2Slot"
QT_MOC_LITERAL(5, 57, 8), // "sizeSlot"
QT_MOC_LITERAL(6, 66, 1), // "r"
QT_MOC_LITERAL(7, 68, 9), // "colorSlot"
QT_MOC_LITERAL(8, 78, 1), // "c"
QT_MOC_LITERAL(9, 80, 16) // "tangentSpaceSlot"

    },
    "CQNewGLNormalsControl\0updateWidgets\0"
    "\0show1Slot\0show2Slot\0sizeSlot\0r\0"
    "colorSlot\0c\0tangentSpaceSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLNormalsControl[] = {

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
       4,    1,   48,    2, 0x08 /* Private */,
       5,    1,   51,    2, 0x08 /* Private */,
       7,    1,   54,    2, 0x08 /* Private */,
       9,    1,   57,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::QColor,    8,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CQNewGLNormalsControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLNormalsControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->show1Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->show2Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->sizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 5: _t->tangentSpaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLNormalsControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLNormalsControl.data,
    qt_meta_data_CQNewGLNormalsControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLNormalsControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLNormalsControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLNormalsControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLNormalsControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLLightsControl_t {
    QByteArrayData data[24];
    char stringdata0[303];
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
QT_MOC_LITERAL(13, 171, 17), // "lightSelectedSlot"
QT_MOC_LITERAL(14, 189, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(15, 206, 8), // "showSlot"
QT_MOC_LITERAL(16, 215, 8), // "typeSlot"
QT_MOC_LITERAL(17, 224, 11), // "enabledSlot"
QT_MOC_LITERAL(18, 236, 12), // "positionSlot"
QT_MOC_LITERAL(19, 249, 13), // "directionSlot"
QT_MOC_LITERAL(20, 263, 9), // "colorSlot"
QT_MOC_LITERAL(21, 273, 10), // "radiusSlot"
QT_MOC_LITERAL(22, 284, 10), // "cutoffSlot"
QT_MOC_LITERAL(23, 295, 7) // "addSlot"

    },
    "CQNewGLLightsControl\0updateWidgets\0\0"
    "ambientSlot\0c\0diffuseSlot\0emissionSlot\0"
    "ambientFactorSlot\0v\0diffuseFactorSlot\0"
    "specularFactorSlot\0shininessFactorSlot\0"
    "emissiveFactorSlot\0lightSelectedSlot\0"
    "QListWidgetItem*\0showSlot\0typeSlot\0"
    "enabledSlot\0positionSlot\0directionSlot\0"
    "colorSlot\0radiusSlot\0cutoffSlot\0addSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLLightsControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x0a /* Public */,
       3,    1,  110,    2, 0x08 /* Private */,
       5,    1,  113,    2, 0x08 /* Private */,
       6,    1,  116,    2, 0x08 /* Private */,
       7,    1,  119,    2, 0x08 /* Private */,
       9,    1,  122,    2, 0x08 /* Private */,
      10,    1,  125,    2, 0x08 /* Private */,
      11,    1,  128,    2, 0x08 /* Private */,
      12,    1,  131,    2, 0x08 /* Private */,
      13,    2,  134,    2, 0x08 /* Private */,
      15,    1,  139,    2, 0x08 /* Private */,
      16,    1,  142,    2, 0x08 /* Private */,
      17,    1,  145,    2, 0x08 /* Private */,
      18,    0,  148,    2, 0x08 /* Private */,
      19,    0,  149,    2, 0x08 /* Private */,
      20,    1,  150,    2, 0x08 /* Private */,
      21,    1,  153,    2, 0x08 /* Private */,
      22,    1,  156,    2, 0x08 /* Private */,
      23,    0,  159,    2, 0x08 /* Private */,

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
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,    2,    2,
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
        case 9: _t->lightSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 10: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->positionSlot(); break;
        case 14: _t->directionSlot(); break;
        case 15: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 16: _t->radiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->cutoffSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->addSlot(); break;
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
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
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
struct qt_meta_stringdata_CQNewGLBBoxControl_t {
    QByteArrayData data[5];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLBBoxControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLBBoxControl_t qt_meta_stringdata_CQNewGLBBoxControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLBBoxControl"
QT_MOC_LITERAL(1, 19, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "showSlot"
QT_MOC_LITERAL(4, 43, 1) // "b"

    },
    "CQNewGLBBoxControl\0updateWidgets\0\0"
    "showSlot\0b"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLBBoxControl[] = {

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

void CQNewGLBBoxControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLBBoxControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLBBoxControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLBBoxControl.data,
    qt_meta_data_CQNewGLBBoxControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLBBoxControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLBBoxControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLBBoxControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLBBoxControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLHullControl_t {
    QByteArrayData data[5];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLHullControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLHullControl_t qt_meta_stringdata_CQNewGLHullControl = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQNewGLHullControl"
QT_MOC_LITERAL(1, 19, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "showSlot"
QT_MOC_LITERAL(4, 43, 1) // "b"

    },
    "CQNewGLHullControl\0updateWidgets\0\0"
    "showSlot\0b"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLHullControl[] = {

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

void CQNewGLHullControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLHullControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLHullControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLHullControl.data,
    qt_meta_data_CQNewGLHullControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLHullControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLHullControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLHullControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLHullControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLBasisControl_t {
    QByteArrayData data[11];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLBasisControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLBasisControl_t qt_meta_stringdata_CQNewGLBasisControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CQNewGLBasisControl"
QT_MOC_LITERAL(1, 20, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "showSlot"
QT_MOC_LITERAL(4, 44, 1), // "b"
QT_MOC_LITERAL(5, 46, 8), // "sizeSlot"
QT_MOC_LITERAL(6, 55, 1), // "r"
QT_MOC_LITERAL(7, 57, 9), // "widthSlot"
QT_MOC_LITERAL(8, 67, 10), // "basisUSlot"
QT_MOC_LITERAL(9, 78, 10), // "basisVSlot"
QT_MOC_LITERAL(10, 89, 10) // "basisWSlot"

    },
    "CQNewGLBasisControl\0updateWidgets\0\0"
    "showSlot\0b\0sizeSlot\0r\0widthSlot\0"
    "basisUSlot\0basisVSlot\0basisWSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLBasisControl[] = {

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
       7,    1,   56,    2, 0x08 /* Private */,
       8,    0,   59,    2, 0x08 /* Private */,
       9,    0,   60,    2, 0x08 /* Private */,
      10,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQNewGLBasisControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLBasisControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->basisUSlot(); break;
        case 5: _t->basisVSlot(); break;
        case 6: _t->basisWSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLBasisControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
    qt_meta_stringdata_CQNewGLBasisControl.data,
    qt_meta_data_CQNewGLBasisControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQNewGLBasisControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQNewGLBasisControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQNewGLBasisControl.stringdata0))
        return static_cast<void*>(this);
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLBasisControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLObjectsControl_t {
    QByteArrayData data[20];
    char stringdata0[252];
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
QT_MOC_LITERAL(10, 128, 10), // "centerSlot"
QT_MOC_LITERAL(11, 139, 8), // "sizeSlot"
QT_MOC_LITERAL(12, 148, 13), // "translateSlot"
QT_MOC_LITERAL(13, 162, 10), // "rotateSlot"
QT_MOC_LITERAL(14, 173, 14), // "diffuseMapSlot"
QT_MOC_LITERAL(15, 188, 13), // "normalMapSlot"
QT_MOC_LITERAL(16, 202, 15), // "specularMapSlot"
QT_MOC_LITERAL(17, 218, 15), // "emissiveMapSlot"
QT_MOC_LITERAL(18, 234, 7), // "addSlot"
QT_MOC_LITERAL(19, 242, 9) // "resetSlot"

    },
    "CQNewGLObjectsControl\0updateWidgets\0"
    "\0updateModelMatrix\0objectSelectedSlot\0"
    "selectSlot\0deselectSlot\0showSlot\0"
    "hideSlot\0currentSlot\0centerSlot\0"
    "sizeSlot\0translateSlot\0rotateSlot\0"
    "diffuseMapSlot\0normalMapSlot\0"
    "specularMapSlot\0emissiveMapSlot\0addSlot\0"
    "resetSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsControl[] = {

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
       3,    0,  105,    2, 0x08 /* Private */,
       4,    0,  106,    2, 0x08 /* Private */,
       5,    0,  107,    2, 0x08 /* Private */,
       6,    0,  108,    2, 0x08 /* Private */,
       7,    0,  109,    2, 0x08 /* Private */,
       8,    0,  110,    2, 0x08 /* Private */,
       9,    0,  111,    2, 0x08 /* Private */,
      10,    0,  112,    2, 0x08 /* Private */,
      11,    0,  113,    2, 0x08 /* Private */,
      12,    0,  114,    2, 0x08 /* Private */,
      13,    0,  115,    2, 0x08 /* Private */,
      14,    0,  116,    2, 0x08 /* Private */,
      15,    0,  117,    2, 0x08 /* Private */,
      16,    0,  118,    2, 0x08 /* Private */,
      17,    0,  119,    2, 0x08 /* Private */,
      18,    0,  120,    2, 0x08 /* Private */,
      19,    0,  121,    2, 0x08 /* Private */,

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
        case 8: _t->centerSlot(); break;
        case 9: _t->sizeSlot(); break;
        case 10: _t->translateSlot(); break;
        case 11: _t->rotateSlot(); break;
        case 12: _t->diffuseMapSlot(); break;
        case 13: _t->normalMapSlot(); break;
        case 14: _t->specularMapSlot(); break;
        case 15: _t->emissiveMapSlot(); break;
        case 16: _t->addSlot(); break;
        case 17: _t->resetSlot(); break;
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
    QByteArrayData data[13];
    char stringdata0[153];
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
QT_MOC_LITERAL(5, 56, 18), // "showBonePointsSlot"
QT_MOC_LITERAL(6, 75, 18), // "bonesTransformSlot"
QT_MOC_LITERAL(7, 94, 11), // "nameChanged"
QT_MOC_LITERAL(8, 106, 8), // "timeSlot"
QT_MOC_LITERAL(9, 115, 8), // "playSlot"
QT_MOC_LITERAL(10, 124, 9), // "pauseSlot"
QT_MOC_LITERAL(11, 134, 8), // "stepSlot"
QT_MOC_LITERAL(12, 143, 9) // "timerSlot"

    },
    "CQNewGLAnimControl\0updateWidgets\0\0"
    "enabledSlot\0bonesSlot\0showBonePointsSlot\0"
    "bonesTransformSlot\0nameChanged\0timeSlot\0"
    "playSlot\0pauseSlot\0stepSlot\0timerSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLAnimControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    1,   70,    2, 0x08 /* Private */,
       4,    1,   73,    2, 0x08 /* Private */,
       5,    1,   76,    2, 0x08 /* Private */,
       6,    1,   79,    2, 0x08 /* Private */,
       7,    1,   82,    2, 0x08 /* Private */,
       8,    1,   85,    2, 0x08 /* Private */,
       9,    0,   88,    2, 0x08 /* Private */,
      10,    0,   89,    2, 0x08 /* Private */,
      11,    0,   90,    2, 0x08 /* Private */,
      12,    0,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
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
        case 0: _t->updateWidgets(); break;
        case 1: _t->enabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->bonesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showBonePointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->bonesTransformSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->nameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->timeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->playSlot(); break;
        case 8: _t->pauseSlot(); break;
        case 9: _t->stepSlot(); break;
        case 10: _t->timerSlot(); break;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLBonesControl_t {
    QByteArrayData data[11];
    char stringdata0[134];
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
QT_MOC_LITERAL(3, 35, 16), // "boneSelectedSlot"
QT_MOC_LITERAL(4, 52, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 69, 17), // "matrixTypeChanged"
QT_MOC_LITERAL(6, 87, 11), // "inverseSlot"
QT_MOC_LITERAL(7, 99, 15), // "animNameChanged"
QT_MOC_LITERAL(8, 115, 3), // "ind"
QT_MOC_LITERAL(9, 119, 12), // "animTimeSlot"
QT_MOC_LITERAL(10, 132, 1) // "t"

    },
    "CQNewGLBonesControl\0updateWidgets\0\0"
    "boneSelectedSlot\0QListWidgetItem*\0"
    "matrixTypeChanged\0inverseSlot\0"
    "animNameChanged\0ind\0animTimeSlot\0t"
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
       3,    2,   45,    2, 0x08 /* Private */,
       5,    1,   50,    2, 0x08 /* Private */,
       6,    1,   53,    2, 0x08 /* Private */,
       7,    1,   56,    2, 0x08 /* Private */,
       9,    1,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Double,   10,

       0        // eod
};

void CQNewGLBonesControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLBonesControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->boneSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
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
    QByteArrayData data[10];
    char stringdata0[114];
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
QT_MOC_LITERAL(7, 80, 11), // "textureSlot"
QT_MOC_LITERAL(8, 92, 8), // "gridSlot"
QT_MOC_LITERAL(9, 101, 12) // "generateSlot"

    },
    "CQNewGLTerrainControl\0updateWidgets\0"
    "\0typeSlot\0widthSlot\0heightSlot\0"
    "gridSizeSlot\0textureSlot\0gridSlot\0"
    "generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTerrainControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    1,   55,    2, 0x08 /* Private */,
       4,    1,   58,    2, 0x08 /* Private */,
       5,    1,   61,    2, 0x08 /* Private */,
       6,    1,   64,    2, 0x08 /* Private */,
       7,    1,   67,    2, 0x08 /* Private */,
       8,    1,   70,    2, 0x08 /* Private */,
       9,    0,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
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
        case 5: _t->textureSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->gridSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->generateSlot(); break;
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
    QByteArrayData data[20];
    char stringdata0[247];
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
QT_MOC_LITERAL(15, 203, 9), // "colorSlot"
QT_MOC_LITERAL(16, 213, 1), // "c"
QT_MOC_LITERAL(17, 215, 13), // "wireframeSlot"
QT_MOC_LITERAL(18, 229, 9), // "imageSlot"
QT_MOC_LITERAL(19, 239, 7) // "addSlot"

    },
    "CQNewGLEmitterControl\0updateWidgets\0"
    "\0emitterSelectedSlot\0QListWidgetItem*\0"
    "typeSlot\0enabledSlot\0runningSlot\0"
    "positionSlot\0minVelocitySlot\0"
    "maxVelocitySlot\0intervalSlot\0gravitySlot\0"
    "sizeSlot\0maxParticlesSlot\0colorSlot\0"
    "c\0wireframeSlot\0imageSlot\0addSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLEmitterControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x0a /* Public */,
       3,    2,   95,    2, 0x08 /* Private */,
       5,    1,  100,    2, 0x08 /* Private */,
       6,    1,  103,    2, 0x08 /* Private */,
       7,    1,  106,    2, 0x08 /* Private */,
       8,    0,  109,    2, 0x08 /* Private */,
       9,    0,  110,    2, 0x08 /* Private */,
      10,    0,  111,    2, 0x08 /* Private */,
      11,    1,  112,    2, 0x08 /* Private */,
      12,    1,  115,    2, 0x08 /* Private */,
      13,    1,  118,    2, 0x08 /* Private */,
      14,    1,  121,    2, 0x08 /* Private */,
      15,    1,  124,    2, 0x08 /* Private */,
      17,    1,  127,    2, 0x08 /* Private */,
      18,    0,  130,    2, 0x08 /* Private */,
      19,    0,  131,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::QColor,   16,
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
        case 12: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 13: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->imageSlot(); break;
        case 15: _t->addSlot(); break;
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
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
struct qt_meta_stringdata_CQNewGLFractalControl_t {
    QByteArrayData data[6];
    char stringdata0[73];
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
QT_MOC_LITERAL(5, 60, 12) // "generateSlot"

    },
    "CQNewGLFractalControl\0updateWidgets\0"
    "\0typeSlot\0wireframeSlot\0generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLFractalControl[] = {

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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
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
        case 3: _t->generateSlot(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQNewGLFractalControl::staticMetaObject = { {
    QMetaObject::SuperData::link<CQNewGLControlFrame::staticMetaObject>(),
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
    return CQNewGLControlFrame::qt_metacast(_clname);
}

int CQNewGLFractalControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CQNewGLDrawTreeControl_t {
    QByteArrayData data[4];
    char stringdata0[51];
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
QT_MOC_LITERAL(3, 38, 12) // "generateSlot"

    },
    "CQNewGLDrawTreeControl\0updateWidgets\0"
    "\0generateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLDrawTreeControl[] = {

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
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
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
        case 1: _t->generateSlot(); break;
        default: ;
        }
    }
    (void)_a;
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
struct qt_meta_stringdata_CQNewGLShapeControl_t {
    QByteArrayData data[17];
    char stringdata0[183];
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
QT_MOC_LITERAL(5, 70, 8), // "showSlot"
QT_MOC_LITERAL(6, 79, 8), // "typeSlot"
QT_MOC_LITERAL(7, 88, 9), // "startSlot"
QT_MOC_LITERAL(8, 98, 7), // "endSlot"
QT_MOC_LITERAL(9, 106, 9), // "widthSlot"
QT_MOC_LITERAL(10, 116, 9), // "colorSlot"
QT_MOC_LITERAL(11, 126, 1), // "c"
QT_MOC_LITERAL(12, 128, 13), // "wireframeSlot"
QT_MOC_LITERAL(13, 142, 9), // "solidSlot"
QT_MOC_LITERAL(14, 152, 11), // "textureSlot"
QT_MOC_LITERAL(15, 164, 7), // "addSlot"
QT_MOC_LITERAL(16, 172, 10) // "updateSlot"

    },
    "CQNewGLShapeControl\0updateWidgets\0\0"
    "shapeSelectedSlot\0QListWidgetItem*\0"
    "showSlot\0typeSlot\0startSlot\0endSlot\0"
    "widthSlot\0colorSlot\0c\0wireframeSlot\0"
    "solidSlot\0textureSlot\0addSlot\0updateSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLShapeControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    2,   80,    2, 0x08 /* Private */,
       5,    1,   85,    2, 0x08 /* Private */,
       6,    1,   88,    2, 0x08 /* Private */,
       7,    0,   91,    2, 0x08 /* Private */,
       8,    0,   92,    2, 0x08 /* Private */,
       9,    1,   93,    2, 0x08 /* Private */,
      10,    1,   96,    2, 0x08 /* Private */,
      12,    1,   99,    2, 0x08 /* Private */,
      13,    1,  102,    2, 0x08 /* Private */,
      14,    0,  105,    2, 0x08 /* Private */,
      15,    0,  106,    2, 0x08 /* Private */,
      16,    0,  107,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::QColor,   11,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
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
        case 2: _t->showSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->startSlot(); break;
        case 5: _t->endSlot(); break;
        case 6: _t->widthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->colorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 8: _t->wireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->solidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->textureSlot(); break;
        case 11: _t->addSlot(); break;
        case 12: _t->updateSlot(); break;
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
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
struct qt_meta_stringdata_CQNewGLTextureChooser_t {
    QByteArrayData data[6];
    char stringdata0[88];
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
QT_MOC_LITERAL(3, 38, 13), // "updateWidgets"
QT_MOC_LITERAL(4, 52, 15), // "needsUpdateSlot"
QT_MOC_LITERAL(5, 68, 19) // "currentIndexChanged"

    },
    "CQNewGLTextureChooser\0textureChanged\0"
    "\0updateWidgets\0needsUpdateSlot\0"
    "currentIndexChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLTextureChooser[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    1,   37,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
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
        case 1: _t->updateWidgets(); break;
        case 2: _t->needsUpdateSlot(); break;
        case 3: _t->currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
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

// SIGNAL 0
void CQNewGLTextureChooser::textureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
