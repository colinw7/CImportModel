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
    QByteArrayData data[15];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLControl_t qt_meta_stringdata_CQNewGLControl = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CQNewGLControl"
QT_MOC_LITERAL(1, 15, 11), // "bgColorSlot"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 1), // "c"
QT_MOC_LITERAL(4, 30, 13), // "depthTestSlot"
QT_MOC_LITERAL(5, 44, 8), // "cullSlot"
QT_MOC_LITERAL(6, 53, 13), // "frontFaceSlot"
QT_MOC_LITERAL(7, 67, 15), // "polygonLineSlot"
QT_MOC_LITERAL(8, 83, 11), // "ambientSlot"
QT_MOC_LITERAL(9, 95, 1), // "v"
QT_MOC_LITERAL(10, 97, 11), // "diffuseSlot"
QT_MOC_LITERAL(11, 109, 12), // "specularSlot"
QT_MOC_LITERAL(12, 122, 13), // "shininessSlot"
QT_MOC_LITERAL(13, 136, 8), // "typeSlot"
QT_MOC_LITERAL(14, 145, 4) // "type"

    },
    "CQNewGLControl\0bgColorSlot\0\0c\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "polygonLineSlot\0ambientSlot\0v\0diffuseSlot\0"
    "specularSlot\0shininessSlot\0typeSlot\0"
    "type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLControl[] = {

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
       1,    1,   64,    2, 0x08 /* Private */,
       4,    1,   67,    2, 0x08 /* Private */,
       5,    1,   70,    2, 0x08 /* Private */,
       6,    1,   73,    2, 0x08 /* Private */,
       7,    1,   76,    2, 0x08 /* Private */,
       8,    1,   79,    2, 0x08 /* Private */,
      10,    1,   82,    2, 0x08 /* Private */,
      11,    1,   85,    2, 0x08 /* Private */,
      12,    1,   88,    2, 0x08 /* Private */,
      13,    1,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Int,   14,

       0        // eod
};

void CQNewGLControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bgColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->polygonLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->ambientSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->diffuseSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->specularSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->shininessSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->typeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
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
struct qt_meta_stringdata_CQNewGLCameraControl_t {
    QByteArrayData data[12];
    char stringdata0[98];
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
QT_MOC_LITERAL(11, 90, 7) // "posSlot"

    },
    "CQNewGLCameraControl\0rotateSlot\0\0b\0"
    "zoomSlot\0r\0nearSlot\0farSlot\0yawSlot\0"
    "pitchSlot\0rollSlot\0posSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLCameraControl[] = {

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
       1,    1,   54,    2, 0x08 /* Private */,
       4,    1,   57,    2, 0x08 /* Private */,
       6,    1,   60,    2, 0x08 /* Private */,
       7,    1,   63,    2, 0x08 /* Private */,
       8,    1,   66,    2, 0x08 /* Private */,
       9,    1,   69,    2, 0x08 /* Private */,
      10,    1,   72,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,
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
struct qt_meta_stringdata_CQNewGLLightControl_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLLightControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLLightControl_t qt_meta_stringdata_CQNewGLLightControl = {
    {
QT_MOC_LITERAL(0, 0, 19) // "CQNewGLLightControl"

    },
    "CQNewGLLightControl"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLLightControl[] = {

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

void CQNewGLLightControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
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
    return _id;
}
struct qt_meta_stringdata_CQNewGLObjectsControl_t {
    QByteArrayData data[4];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQNewGLObjectsControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQNewGLObjectsControl_t qt_meta_stringdata_CQNewGLObjectsControl = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQNewGLObjectsControl"
QT_MOC_LITERAL(1, 22, 18), // "objectSelectedSlot"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 16) // "QListWidgetItem*"

    },
    "CQNewGLObjectsControl\0objectSelectedSlot\0"
    "\0QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQNewGLObjectsControl[] = {

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
       1,    2,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,

       0        // eod
};

void CQNewGLObjectsControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQNewGLObjectsControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->objectSelectedSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
