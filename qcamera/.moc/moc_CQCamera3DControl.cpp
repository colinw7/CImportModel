/****************************************************************************
** Meta object code from reading C++ file 'CQCamera3DControl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQCamera3DControl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQCamera3DControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQCamera3DControl_t {
    QByteArrayData data[70];
    char stringdata0[861];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DControl_t qt_meta_stringdata_CQCamera3DControl = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CQCamera3DControl"
QT_MOC_LITERAL(1, 18, 13), // "updateWidgets"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 17), // "showWireframeSlot"
QT_MOC_LITERAL(4, 51, 13), // "showSolidSlot"
QT_MOC_LITERAL(5, 65, 13), // "depthTestSlot"
QT_MOC_LITERAL(6, 79, 8), // "cullSlot"
QT_MOC_LITERAL(7, 88, 13), // "frontFaceSlot"
QT_MOC_LITERAL(8, 102, 7), // "altSlot"
QT_MOC_LITERAL(9, 110, 14), // "showCameraSlot"
QT_MOC_LITERAL(10, 125, 15), // "showEyeLineSlot"
QT_MOC_LITERAL(11, 141, 14), // "showPlanesSlot"
QT_MOC_LITERAL(12, 156, 9), // "pitchSlot"
QT_MOC_LITERAL(13, 166, 1), // "r"
QT_MOC_LITERAL(14, 168, 7), // "yawSlot"
QT_MOC_LITERAL(15, 176, 8), // "rollSlot"
QT_MOC_LITERAL(16, 185, 8), // "nearSlot"
QT_MOC_LITERAL(17, 194, 7), // "farSlot"
QT_MOC_LITERAL(18, 202, 11), // "xOriginSlot"
QT_MOC_LITERAL(19, 214, 11), // "yOriginSlot"
QT_MOC_LITERAL(20, 226, 11), // "zOriginSlot"
QT_MOC_LITERAL(21, 238, 5), // "xSlot"
QT_MOC_LITERAL(22, 244, 5), // "ySlot"
QT_MOC_LITERAL(23, 250, 5), // "zSlot"
QT_MOC_LITERAL(24, 256, 7), // "fovSlot"
QT_MOC_LITERAL(25, 264, 9), // "eyeZ1Slot"
QT_MOC_LITERAL(26, 274, 9), // "eyeZ2Slot"
QT_MOC_LITERAL(27, 284, 12), // "aroundXSlot1"
QT_MOC_LITERAL(28, 297, 12), // "aroundXSlot2"
QT_MOC_LITERAL(29, 310, 12), // "aroundYSlot1"
QT_MOC_LITERAL(30, 323, 12), // "aroundYSlot2"
QT_MOC_LITERAL(31, 336, 12), // "aroundZSlot1"
QT_MOC_LITERAL(32, 349, 12), // "aroundZSlot2"
QT_MOC_LITERAL(33, 362, 9), // "resetSlot"
QT_MOC_LITERAL(34, 372, 7), // "topSlot"
QT_MOC_LITERAL(35, 380, 8), // "sideSlot"
QT_MOC_LITERAL(36, 389, 9), // "frontSlot"
QT_MOC_LITERAL(37, 399, 12), // "showAxesSlot"
QT_MOC_LITERAL(38, 412, 12), // "axesXPosSlot"
QT_MOC_LITERAL(39, 425, 12), // "axesYPosSlot"
QT_MOC_LITERAL(40, 438, 12), // "axesZPosSlot"
QT_MOC_LITERAL(41, 451, 13), // "mouseTypeSlot"
QT_MOC_LITERAL(42, 465, 14), // "selectTypeSlot"
QT_MOC_LITERAL(43, 480, 13), // "objectVisSlot"
QT_MOC_LITERAL(44, 494, 15), // "translationSlot"
QT_MOC_LITERAL(45, 510, 12), // "rotationSlot"
QT_MOC_LITERAL(46, 523, 9), // "scaleSlot"
QT_MOC_LITERAL(47, 533, 15), // "objectColorSlot"
QT_MOC_LITERAL(48, 549, 14), // "diffuseMapSlot"
QT_MOC_LITERAL(49, 564, 13), // "normalMapSlot"
QT_MOC_LITERAL(50, 578, 15), // "specularMapSlot"
QT_MOC_LITERAL(51, 594, 15), // "emissiveMapSlot"
QT_MOC_LITERAL(52, 610, 10), // "swapYZSlot"
QT_MOC_LITERAL(53, 621, 11), // "invertXSlot"
QT_MOC_LITERAL(54, 633, 11), // "invertYSlot"
QT_MOC_LITERAL(55, 645, 11), // "invertZSlot"
QT_MOC_LITERAL(56, 657, 18), // "loadTextureMapSlot"
QT_MOC_LITERAL(57, 676, 14), // "addTextureSlot"
QT_MOC_LITERAL(58, 691, 16), // "selectParentSlot"
QT_MOC_LITERAL(59, 708, 12), // "deselectSlot"
QT_MOC_LITERAL(60, 721, 16), // "objectSelectSlot"
QT_MOC_LITERAL(61, 738, 14), // "objectZoomSlot"
QT_MOC_LITERAL(62, 753, 11), // "addCubeSlot"
QT_MOC_LITERAL(63, 765, 15), // "addCylinderSlot"
QT_MOC_LITERAL(64, 781, 14), // "addPyramidSlot"
QT_MOC_LITERAL(65, 796, 13), // "addSphereSlot"
QT_MOC_LITERAL(66, 810, 12), // "addTorusSlot"
QT_MOC_LITERAL(67, 823, 12), // "addModelSlot"
QT_MOC_LITERAL(68, 836, 22), // "overviewEqualScaleSlot"
QT_MOC_LITERAL(69, 859, 1) // "i"

    },
    "CQCamera3DControl\0updateWidgets\0\0"
    "showWireframeSlot\0showSolidSlot\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "altSlot\0showCameraSlot\0showEyeLineSlot\0"
    "showPlanesSlot\0pitchSlot\0r\0yawSlot\0"
    "rollSlot\0nearSlot\0farSlot\0xOriginSlot\0"
    "yOriginSlot\0zOriginSlot\0xSlot\0ySlot\0"
    "zSlot\0fovSlot\0eyeZ1Slot\0eyeZ2Slot\0"
    "aroundXSlot1\0aroundXSlot2\0aroundYSlot1\0"
    "aroundYSlot2\0aroundZSlot1\0aroundZSlot2\0"
    "resetSlot\0topSlot\0sideSlot\0frontSlot\0"
    "showAxesSlot\0axesXPosSlot\0axesYPosSlot\0"
    "axesZPosSlot\0mouseTypeSlot\0selectTypeSlot\0"
    "objectVisSlot\0translationSlot\0"
    "rotationSlot\0scaleSlot\0objectColorSlot\0"
    "diffuseMapSlot\0normalMapSlot\0"
    "specularMapSlot\0emissiveMapSlot\0"
    "swapYZSlot\0invertXSlot\0invertYSlot\0"
    "invertZSlot\0loadTextureMapSlot\0"
    "addTextureSlot\0selectParentSlot\0"
    "deselectSlot\0objectSelectSlot\0"
    "objectZoomSlot\0addCubeSlot\0addCylinderSlot\0"
    "addPyramidSlot\0addSphereSlot\0addTorusSlot\0"
    "addModelSlot\0overviewEqualScaleSlot\0"
    "i"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      66,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  344,    2, 0x08 /* Private */,
       3,    1,  345,    2, 0x08 /* Private */,
       4,    1,  348,    2, 0x08 /* Private */,
       5,    1,  351,    2, 0x08 /* Private */,
       6,    1,  354,    2, 0x08 /* Private */,
       7,    1,  357,    2, 0x08 /* Private */,
       8,    1,  360,    2, 0x08 /* Private */,
       9,    1,  363,    2, 0x08 /* Private */,
      10,    1,  366,    2, 0x08 /* Private */,
      11,    1,  369,    2, 0x08 /* Private */,
      12,    1,  372,    2, 0x08 /* Private */,
      14,    1,  375,    2, 0x08 /* Private */,
      15,    1,  378,    2, 0x08 /* Private */,
      16,    1,  381,    2, 0x08 /* Private */,
      17,    1,  384,    2, 0x08 /* Private */,
      18,    1,  387,    2, 0x08 /* Private */,
      19,    1,  390,    2, 0x08 /* Private */,
      20,    1,  393,    2, 0x08 /* Private */,
      21,    1,  396,    2, 0x08 /* Private */,
      22,    1,  399,    2, 0x08 /* Private */,
      23,    1,  402,    2, 0x08 /* Private */,
      24,    1,  405,    2, 0x08 /* Private */,
      25,    1,  408,    2, 0x08 /* Private */,
      26,    1,  411,    2, 0x08 /* Private */,
      27,    0,  414,    2, 0x08 /* Private */,
      28,    0,  415,    2, 0x08 /* Private */,
      29,    0,  416,    2, 0x08 /* Private */,
      30,    0,  417,    2, 0x08 /* Private */,
      31,    0,  418,    2, 0x08 /* Private */,
      32,    0,  419,    2, 0x08 /* Private */,
      33,    0,  420,    2, 0x08 /* Private */,
      34,    0,  421,    2, 0x08 /* Private */,
      35,    0,  422,    2, 0x08 /* Private */,
      36,    0,  423,    2, 0x08 /* Private */,
      37,    1,  424,    2, 0x08 /* Private */,
      38,    1,  427,    2, 0x08 /* Private */,
      39,    1,  430,    2, 0x08 /* Private */,
      40,    1,  433,    2, 0x08 /* Private */,
      41,    1,  436,    2, 0x08 /* Private */,
      42,    1,  439,    2, 0x08 /* Private */,
      43,    1,  442,    2, 0x08 /* Private */,
      44,    0,  445,    2, 0x08 /* Private */,
      45,    0,  446,    2, 0x08 /* Private */,
      46,    0,  447,    2, 0x08 /* Private */,
      47,    1,  448,    2, 0x08 /* Private */,
      48,    0,  451,    2, 0x08 /* Private */,
      49,    0,  452,    2, 0x08 /* Private */,
      50,    0,  453,    2, 0x08 /* Private */,
      51,    0,  454,    2, 0x08 /* Private */,
      52,    0,  455,    2, 0x08 /* Private */,
      53,    0,  456,    2, 0x08 /* Private */,
      54,    0,  457,    2, 0x08 /* Private */,
      55,    0,  458,    2, 0x08 /* Private */,
      56,    0,  459,    2, 0x08 /* Private */,
      57,    0,  460,    2, 0x08 /* Private */,
      58,    0,  461,    2, 0x08 /* Private */,
      59,    0,  462,    2, 0x08 /* Private */,
      60,    0,  463,    2, 0x08 /* Private */,
      61,    0,  464,    2, 0x08 /* Private */,
      62,    0,  465,    2, 0x08 /* Private */,
      63,    0,  466,    2, 0x08 /* Private */,
      64,    0,  467,    2, 0x08 /* Private */,
      65,    0,  468,    2, 0x08 /* Private */,
      66,    0,  469,    2, 0x08 /* Private */,
      67,    0,  470,    2, 0x08 /* Private */,
      68,    1,  471,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    2,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   69,

       0        // eod
};

void CQCamera3DControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->showWireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->showSolidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->altSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->showCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->showEyeLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->showPlanesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->xOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->yOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->zOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->xSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->ySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->zSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: _t->fovSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 22: _t->eyeZ1Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 23: _t->eyeZ2Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: _t->aroundXSlot1(); break;
        case 25: _t->aroundXSlot2(); break;
        case 26: _t->aroundYSlot1(); break;
        case 27: _t->aroundYSlot2(); break;
        case 28: _t->aroundZSlot1(); break;
        case 29: _t->aroundZSlot2(); break;
        case 30: _t->resetSlot(); break;
        case 31: _t->topSlot(); break;
        case 32: _t->sideSlot(); break;
        case 33: _t->frontSlot(); break;
        case 34: _t->showAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 35: _t->axesXPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 36: _t->axesYPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 37: _t->axesZPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 38: _t->mouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: _t->selectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 40: _t->objectVisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 41: _t->translationSlot(); break;
        case 42: _t->rotationSlot(); break;
        case 43: _t->scaleSlot(); break;
        case 44: _t->objectColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 45: _t->diffuseMapSlot(); break;
        case 46: _t->normalMapSlot(); break;
        case 47: _t->specularMapSlot(); break;
        case 48: _t->emissiveMapSlot(); break;
        case 49: _t->swapYZSlot(); break;
        case 50: _t->invertXSlot(); break;
        case 51: _t->invertYSlot(); break;
        case 52: _t->invertZSlot(); break;
        case 53: _t->loadTextureMapSlot(); break;
        case 54: _t->addTextureSlot(); break;
        case 55: _t->selectParentSlot(); break;
        case 56: _t->deselectSlot(); break;
        case 57: _t->objectSelectSlot(); break;
        case 58: _t->objectZoomSlot(); break;
        case 59: _t->addCubeSlot(); break;
        case 60: _t->addCylinderSlot(); break;
        case 61: _t->addPyramidSlot(); break;
        case 62: _t->addSphereSlot(); break;
        case 63: _t->addTorusSlot(); break;
        case 64: _t->addModelSlot(); break;
        case 65: _t->overviewEqualScaleSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQCamera3DControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQCamera3DControl.data,
    qt_meta_data_CQCamera3DControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCamera3DControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCamera3DControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCamera3DControl.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQCamera3DControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 66)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 66;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 66)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 66;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
