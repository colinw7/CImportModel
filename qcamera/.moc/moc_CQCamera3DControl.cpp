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
    QByteArrayData data[129];
    char stringdata0[1959];
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
QT_MOC_LITERAL(3, 33, 11), // "mainTabSlot"
QT_MOC_LITERAL(4, 45, 17), // "showWireframeSlot"
QT_MOC_LITERAL(5, 63, 13), // "showSolidSlot"
QT_MOC_LITERAL(6, 77, 15), // "showNormalsSlot"
QT_MOC_LITERAL(7, 93, 15), // "normalsSizeSlot"
QT_MOC_LITERAL(8, 109, 1), // "r"
QT_MOC_LITERAL(9, 111, 16), // "normalsColorSlot"
QT_MOC_LITERAL(10, 128, 1), // "c"
QT_MOC_LITERAL(11, 130, 13), // "depthTestSlot"
QT_MOC_LITERAL(12, 144, 8), // "cullSlot"
QT_MOC_LITERAL(13, 153, 13), // "frontFaceSlot"
QT_MOC_LITERAL(14, 167, 7), // "altSlot"
QT_MOC_LITERAL(15, 175, 14), // "showCameraSlot"
QT_MOC_LITERAL(16, 190, 15), // "showEyeLineSlot"
QT_MOC_LITERAL(17, 206, 14), // "showPlanesSlot"
QT_MOC_LITERAL(18, 221, 9), // "pitchSlot"
QT_MOC_LITERAL(19, 231, 7), // "yawSlot"
QT_MOC_LITERAL(20, 239, 8), // "rollSlot"
QT_MOC_LITERAL(21, 248, 8), // "nearSlot"
QT_MOC_LITERAL(22, 257, 7), // "farSlot"
QT_MOC_LITERAL(23, 265, 11), // "xOriginSlot"
QT_MOC_LITERAL(24, 277, 11), // "yOriginSlot"
QT_MOC_LITERAL(25, 289, 11), // "zOriginSlot"
QT_MOC_LITERAL(26, 301, 5), // "xSlot"
QT_MOC_LITERAL(27, 307, 5), // "ySlot"
QT_MOC_LITERAL(28, 313, 5), // "zSlot"
QT_MOC_LITERAL(29, 319, 7), // "fovSlot"
QT_MOC_LITERAL(30, 327, 9), // "eyeZ1Slot"
QT_MOC_LITERAL(31, 337, 9), // "eyeZ2Slot"
QT_MOC_LITERAL(32, 347, 12), // "aroundXSlot1"
QT_MOC_LITERAL(33, 360, 12), // "aroundXSlot2"
QT_MOC_LITERAL(34, 373, 12), // "aroundYSlot1"
QT_MOC_LITERAL(35, 386, 12), // "aroundYSlot2"
QT_MOC_LITERAL(36, 399, 12), // "aroundZSlot1"
QT_MOC_LITERAL(37, 412, 12), // "aroundZSlot2"
QT_MOC_LITERAL(38, 425, 9), // "resetSlot"
QT_MOC_LITERAL(39, 435, 7), // "topSlot"
QT_MOC_LITERAL(40, 443, 8), // "sideSlot"
QT_MOC_LITERAL(41, 452, 9), // "frontSlot"
QT_MOC_LITERAL(42, 462, 16), // "currentLightSlot"
QT_MOC_LITERAL(43, 479, 16), // "ambientColorSlot"
QT_MOC_LITERAL(44, 496, 19), // "ambientStrengthSlot"
QT_MOC_LITERAL(45, 516, 19), // "diffuseStrengthSlot"
QT_MOC_LITERAL(46, 536, 20), // "emissiveStrengthSlot"
QT_MOC_LITERAL(47, 557, 20), // "specularStrengthSlot"
QT_MOC_LITERAL(48, 578, 16), // "fixedDiffuseSlot"
QT_MOC_LITERAL(49, 595, 1), // "i"
QT_MOC_LITERAL(50, 597, 13), // "lightTypeSlot"
QT_MOC_LITERAL(51, 611, 16), // "lightEnabledSlot"
QT_MOC_LITERAL(52, 628, 17), // "lightPositionSlot"
QT_MOC_LITERAL(53, 646, 14), // "lightColorSlot"
QT_MOC_LITERAL(54, 661, 18), // "lightDirectionSlot"
QT_MOC_LITERAL(55, 680, 20), // "lightPointRadiusSlot"
QT_MOC_LITERAL(56, 701, 28), // "lightConstantAttenuationSlot"
QT_MOC_LITERAL(57, 730, 26), // "lightLinearAttenuationSlot"
QT_MOC_LITERAL(58, 757, 29), // "lightQuadraticAttenuationSlot"
QT_MOC_LITERAL(59, 787, 22), // "lightSpotDirectionSlot"
QT_MOC_LITERAL(60, 810, 21), // "lightSpotExponentSlot"
QT_MOC_LITERAL(61, 832, 20), // "lightCutOffAngleSlot"
QT_MOC_LITERAL(62, 853, 12), // "addLightSlot"
QT_MOC_LITERAL(63, 866, 14), // "resetLightSlot"
QT_MOC_LITERAL(64, 881, 12), // "showAxesSlot"
QT_MOC_LITERAL(65, 894, 12), // "axesXPosSlot"
QT_MOC_LITERAL(66, 907, 12), // "axesYPosSlot"
QT_MOC_LITERAL(67, 920, 12), // "axesZPosSlot"
QT_MOC_LITERAL(68, 933, 13), // "mouseTypeSlot"
QT_MOC_LITERAL(69, 947, 14), // "mouseScaleSlot"
QT_MOC_LITERAL(70, 962, 14), // "selectTypeSlot"
QT_MOC_LITERAL(71, 977, 13), // "objectVisSlot"
QT_MOC_LITERAL(72, 991, 15), // "translationSlot"
QT_MOC_LITERAL(73, 1007, 12), // "rotationSlot"
QT_MOC_LITERAL(74, 1020, 9), // "scaleSlot"
QT_MOC_LITERAL(75, 1030, 15), // "objectColorSlot"
QT_MOC_LITERAL(76, 1046, 18), // "diffuseTextureSlot"
QT_MOC_LITERAL(77, 1065, 17), // "normalTextureSlot"
QT_MOC_LITERAL(78, 1083, 19), // "specularTextureSlot"
QT_MOC_LITERAL(79, 1103, 19), // "emissiveTextureSlot"
QT_MOC_LITERAL(80, 1123, 8), // "swapSlot"
QT_MOC_LITERAL(81, 1132, 10), // "invertSlot"
QT_MOC_LITERAL(82, 1143, 14), // "addTextureSlot"
QT_MOC_LITERAL(83, 1158, 18), // "loadTextureMapSlot"
QT_MOC_LITERAL(84, 1177, 18), // "saveTextureMapSlot"
QT_MOC_LITERAL(85, 1196, 19), // "loadMaterialMapSlot"
QT_MOC_LITERAL(86, 1216, 19), // "saveMaterialMapSlot"
QT_MOC_LITERAL(87, 1236, 16), // "selectParentSlot"
QT_MOC_LITERAL(88, 1253, 17), // "selectRelatedSlot"
QT_MOC_LITERAL(89, 1271, 12), // "deselectSlot"
QT_MOC_LITERAL(90, 1284, 16), // "objectSelectSlot"
QT_MOC_LITERAL(91, 1301, 14), // "objectZoomSlot"
QT_MOC_LITERAL(92, 1316, 11), // "addCubeSlot"
QT_MOC_LITERAL(93, 1328, 15), // "addCylinderSlot"
QT_MOC_LITERAL(94, 1344, 14), // "addPyramidSlot"
QT_MOC_LITERAL(95, 1359, 13), // "addSphereSlot"
QT_MOC_LITERAL(96, 1373, 12), // "addTorusSlot"
QT_MOC_LITERAL(97, 1386, 12), // "addModelSlot"
QT_MOC_LITERAL(98, 1399, 19), // "currentMaterialSlot"
QT_MOC_LITERAL(99, 1419, 19), // "materialAmbientSlot"
QT_MOC_LITERAL(100, 1439, 19), // "materialDiffuseSlot"
QT_MOC_LITERAL(101, 1459, 20), // "materialSpecularSlot"
QT_MOC_LITERAL(102, 1480, 20), // "materialEmissionSlot"
QT_MOC_LITERAL(103, 1501, 21), // "materialShininessSlot"
QT_MOC_LITERAL(104, 1523, 24), // "materialTransparencySlot"
QT_MOC_LITERAL(105, 1548, 26), // "materialAmbientTextureSlot"
QT_MOC_LITERAL(106, 1575, 26), // "materialDiffuseTextureSlot"
QT_MOC_LITERAL(107, 1602, 25), // "materialNormalTextureSlot"
QT_MOC_LITERAL(108, 1628, 27), // "materialSpecularTextureSlot"
QT_MOC_LITERAL(109, 1656, 27), // "materialEmissiveTextureSlot"
QT_MOC_LITERAL(110, 1684, 22), // "overviewEqualScaleSlot"
QT_MOC_LITERAL(111, 1707, 22), // "overviewSelectTypeSlot"
QT_MOC_LITERAL(112, 1730, 21), // "overviewMouseTypeSlot"
QT_MOC_LITERAL(113, 1752, 21), // "overviewModelTypeSlot"
QT_MOC_LITERAL(114, 1774, 18), // "overviewCameraSlot"
QT_MOC_LITERAL(115, 1793, 18), // "overviewLightsSlot"
QT_MOC_LITERAL(116, 1812, 17), // "uvTextureTypeSlot"
QT_MOC_LITERAL(117, 1830, 14), // "bonesModelSlot"
QT_MOC_LITERAL(118, 1845, 15), // "currentBoneSlot"
QT_MOC_LITERAL(119, 1861, 11), // "updateBones"
QT_MOC_LITERAL(120, 1873, 10), // "updateAnim"
QT_MOC_LITERAL(121, 1884, 9), // "timerSlot"
QT_MOC_LITERAL(122, 1894, 12), // "animNameSlot"
QT_MOC_LITERAL(123, 1907, 12), // "animTimeSlot"
QT_MOC_LITERAL(124, 1920, 1), // "t"
QT_MOC_LITERAL(125, 1922, 8), // "playSlot"
QT_MOC_LITERAL(126, 1931, 9), // "pauseSlot"
QT_MOC_LITERAL(127, 1941, 8), // "stepSlot"
QT_MOC_LITERAL(128, 1950, 8) // "animStep"

    },
    "CQCamera3DControl\0updateWidgets\0\0"
    "mainTabSlot\0showWireframeSlot\0"
    "showSolidSlot\0showNormalsSlot\0"
    "normalsSizeSlot\0r\0normalsColorSlot\0c\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "altSlot\0showCameraSlot\0showEyeLineSlot\0"
    "showPlanesSlot\0pitchSlot\0yawSlot\0"
    "rollSlot\0nearSlot\0farSlot\0xOriginSlot\0"
    "yOriginSlot\0zOriginSlot\0xSlot\0ySlot\0"
    "zSlot\0fovSlot\0eyeZ1Slot\0eyeZ2Slot\0"
    "aroundXSlot1\0aroundXSlot2\0aroundYSlot1\0"
    "aroundYSlot2\0aroundZSlot1\0aroundZSlot2\0"
    "resetSlot\0topSlot\0sideSlot\0frontSlot\0"
    "currentLightSlot\0ambientColorSlot\0"
    "ambientStrengthSlot\0diffuseStrengthSlot\0"
    "emissiveStrengthSlot\0specularStrengthSlot\0"
    "fixedDiffuseSlot\0i\0lightTypeSlot\0"
    "lightEnabledSlot\0lightPositionSlot\0"
    "lightColorSlot\0lightDirectionSlot\0"
    "lightPointRadiusSlot\0lightConstantAttenuationSlot\0"
    "lightLinearAttenuationSlot\0"
    "lightQuadraticAttenuationSlot\0"
    "lightSpotDirectionSlot\0lightSpotExponentSlot\0"
    "lightCutOffAngleSlot\0addLightSlot\0"
    "resetLightSlot\0showAxesSlot\0axesXPosSlot\0"
    "axesYPosSlot\0axesZPosSlot\0mouseTypeSlot\0"
    "mouseScaleSlot\0selectTypeSlot\0"
    "objectVisSlot\0translationSlot\0"
    "rotationSlot\0scaleSlot\0objectColorSlot\0"
    "diffuseTextureSlot\0normalTextureSlot\0"
    "specularTextureSlot\0emissiveTextureSlot\0"
    "swapSlot\0invertSlot\0addTextureSlot\0"
    "loadTextureMapSlot\0saveTextureMapSlot\0"
    "loadMaterialMapSlot\0saveMaterialMapSlot\0"
    "selectParentSlot\0selectRelatedSlot\0"
    "deselectSlot\0objectSelectSlot\0"
    "objectZoomSlot\0addCubeSlot\0addCylinderSlot\0"
    "addPyramidSlot\0addSphereSlot\0addTorusSlot\0"
    "addModelSlot\0currentMaterialSlot\0"
    "materialAmbientSlot\0materialDiffuseSlot\0"
    "materialSpecularSlot\0materialEmissionSlot\0"
    "materialShininessSlot\0materialTransparencySlot\0"
    "materialAmbientTextureSlot\0"
    "materialDiffuseTextureSlot\0"
    "materialNormalTextureSlot\0"
    "materialSpecularTextureSlot\0"
    "materialEmissiveTextureSlot\0"
    "overviewEqualScaleSlot\0overviewSelectTypeSlot\0"
    "overviewMouseTypeSlot\0overviewModelTypeSlot\0"
    "overviewCameraSlot\0overviewLightsSlot\0"
    "uvTextureTypeSlot\0bonesModelSlot\0"
    "currentBoneSlot\0updateBones\0updateAnim\0"
    "timerSlot\0animNameSlot\0animTimeSlot\0"
    "t\0playSlot\0pauseSlot\0stepSlot\0animStep"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
     123,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  629,    2, 0x08 /* Private */,
       3,    1,  630,    2, 0x08 /* Private */,
       4,    1,  633,    2, 0x08 /* Private */,
       5,    1,  636,    2, 0x08 /* Private */,
       6,    1,  639,    2, 0x08 /* Private */,
       7,    1,  642,    2, 0x08 /* Private */,
       9,    1,  645,    2, 0x08 /* Private */,
      11,    1,  648,    2, 0x08 /* Private */,
      12,    1,  651,    2, 0x08 /* Private */,
      13,    1,  654,    2, 0x08 /* Private */,
      14,    1,  657,    2, 0x08 /* Private */,
      15,    1,  660,    2, 0x08 /* Private */,
      16,    1,  663,    2, 0x08 /* Private */,
      17,    1,  666,    2, 0x08 /* Private */,
      18,    1,  669,    2, 0x08 /* Private */,
      19,    1,  672,    2, 0x08 /* Private */,
      20,    1,  675,    2, 0x08 /* Private */,
      21,    1,  678,    2, 0x08 /* Private */,
      22,    1,  681,    2, 0x08 /* Private */,
      23,    1,  684,    2, 0x08 /* Private */,
      24,    1,  687,    2, 0x08 /* Private */,
      25,    1,  690,    2, 0x08 /* Private */,
      26,    1,  693,    2, 0x08 /* Private */,
      27,    1,  696,    2, 0x08 /* Private */,
      28,    1,  699,    2, 0x08 /* Private */,
      29,    1,  702,    2, 0x08 /* Private */,
      30,    1,  705,    2, 0x08 /* Private */,
      31,    1,  708,    2, 0x08 /* Private */,
      32,    0,  711,    2, 0x08 /* Private */,
      33,    0,  712,    2, 0x08 /* Private */,
      34,    0,  713,    2, 0x08 /* Private */,
      35,    0,  714,    2, 0x08 /* Private */,
      36,    0,  715,    2, 0x08 /* Private */,
      37,    0,  716,    2, 0x08 /* Private */,
      38,    0,  717,    2, 0x08 /* Private */,
      39,    0,  718,    2, 0x08 /* Private */,
      40,    0,  719,    2, 0x08 /* Private */,
      41,    0,  720,    2, 0x08 /* Private */,
      42,    0,  721,    2, 0x08 /* Private */,
      43,    1,  722,    2, 0x08 /* Private */,
      44,    1,  725,    2, 0x08 /* Private */,
      45,    1,  728,    2, 0x08 /* Private */,
      46,    1,  731,    2, 0x08 /* Private */,
      47,    1,  734,    2, 0x08 /* Private */,
      48,    1,  737,    2, 0x08 /* Private */,
      50,    1,  740,    2, 0x08 /* Private */,
      51,    1,  743,    2, 0x08 /* Private */,
      52,    0,  746,    2, 0x08 /* Private */,
      53,    1,  747,    2, 0x08 /* Private */,
      54,    0,  750,    2, 0x08 /* Private */,
      55,    1,  751,    2, 0x08 /* Private */,
      56,    1,  754,    2, 0x08 /* Private */,
      57,    1,  757,    2, 0x08 /* Private */,
      58,    1,  760,    2, 0x08 /* Private */,
      59,    0,  763,    2, 0x08 /* Private */,
      60,    1,  764,    2, 0x08 /* Private */,
      61,    1,  767,    2, 0x08 /* Private */,
      62,    0,  770,    2, 0x08 /* Private */,
      63,    0,  771,    2, 0x08 /* Private */,
      64,    1,  772,    2, 0x08 /* Private */,
      65,    1,  775,    2, 0x08 /* Private */,
      66,    1,  778,    2, 0x08 /* Private */,
      67,    1,  781,    2, 0x08 /* Private */,
      68,    1,  784,    2, 0x08 /* Private */,
      69,    1,  787,    2, 0x08 /* Private */,
      70,    1,  790,    2, 0x08 /* Private */,
      71,    1,  793,    2, 0x08 /* Private */,
      72,    0,  796,    2, 0x08 /* Private */,
      73,    0,  797,    2, 0x08 /* Private */,
      74,    0,  798,    2, 0x08 /* Private */,
      75,    1,  799,    2, 0x08 /* Private */,
      76,    0,  802,    2, 0x08 /* Private */,
      77,    0,  803,    2, 0x08 /* Private */,
      78,    0,  804,    2, 0x08 /* Private */,
      79,    0,  805,    2, 0x08 /* Private */,
      80,    0,  806,    2, 0x08 /* Private */,
      81,    0,  807,    2, 0x08 /* Private */,
      82,    0,  808,    2, 0x08 /* Private */,
      83,    0,  809,    2, 0x08 /* Private */,
      84,    0,  810,    2, 0x08 /* Private */,
      85,    0,  811,    2, 0x08 /* Private */,
      86,    0,  812,    2, 0x08 /* Private */,
      87,    0,  813,    2, 0x08 /* Private */,
      88,    0,  814,    2, 0x08 /* Private */,
      89,    0,  815,    2, 0x08 /* Private */,
      90,    0,  816,    2, 0x08 /* Private */,
      91,    0,  817,    2, 0x08 /* Private */,
      92,    0,  818,    2, 0x08 /* Private */,
      93,    0,  819,    2, 0x08 /* Private */,
      94,    0,  820,    2, 0x08 /* Private */,
      95,    0,  821,    2, 0x08 /* Private */,
      96,    0,  822,    2, 0x08 /* Private */,
      97,    0,  823,    2, 0x08 /* Private */,
      98,    0,  824,    2, 0x08 /* Private */,
      99,    1,  825,    2, 0x08 /* Private */,
     100,    1,  828,    2, 0x08 /* Private */,
     101,    1,  831,    2, 0x08 /* Private */,
     102,    1,  834,    2, 0x08 /* Private */,
     103,    1,  837,    2, 0x08 /* Private */,
     104,    1,  840,    2, 0x08 /* Private */,
     105,    0,  843,    2, 0x08 /* Private */,
     106,    0,  844,    2, 0x08 /* Private */,
     107,    0,  845,    2, 0x08 /* Private */,
     108,    0,  846,    2, 0x08 /* Private */,
     109,    0,  847,    2, 0x08 /* Private */,
     110,    1,  848,    2, 0x08 /* Private */,
     111,    1,  851,    2, 0x08 /* Private */,
     112,    1,  854,    2, 0x08 /* Private */,
     113,    1,  857,    2, 0x08 /* Private */,
     114,    1,  860,    2, 0x08 /* Private */,
     115,    1,  863,    2, 0x08 /* Private */,
     116,    1,  866,    2, 0x08 /* Private */,
     117,    1,  869,    2, 0x08 /* Private */,
     118,    0,  872,    2, 0x08 /* Private */,
     119,    0,  873,    2, 0x08 /* Private */,
     120,    0,  874,    2, 0x08 /* Private */,
     121,    0,  875,    2, 0x08 /* Private */,
     122,    0,  876,    2, 0x08 /* Private */,
     123,    1,  877,    2, 0x08 /* Private */,
     125,    0,  880,    2, 0x08 /* Private */,
     126,    0,  881,    2, 0x08 /* Private */,
     127,    0,  882,    2, 0x08 /* Private */,
     128,    0,  883,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::QColor,   10,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
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
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,   10,
    QMetaType::Void, QMetaType::QColor,   10,
    QMetaType::Void, QMetaType::QColor,   10,
    QMetaType::Void, QMetaType::QColor,   10,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  124,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CQCamera3DControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQCamera3DControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateWidgets(); break;
        case 1: _t->mainTabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->showWireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showSolidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->showNormalsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->normalsSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->normalsColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 7: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->altSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->showCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->showEyeLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->showPlanesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->xOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->yOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: _t->zOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 22: _t->xSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 23: _t->ySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: _t->zSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: _t->fovSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: _t->eyeZ1Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 27: _t->eyeZ2Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 28: _t->aroundXSlot1(); break;
        case 29: _t->aroundXSlot2(); break;
        case 30: _t->aroundYSlot1(); break;
        case 31: _t->aroundYSlot2(); break;
        case 32: _t->aroundZSlot1(); break;
        case 33: _t->aroundZSlot2(); break;
        case 34: _t->resetSlot(); break;
        case 35: _t->topSlot(); break;
        case 36: _t->sideSlot(); break;
        case 37: _t->frontSlot(); break;
        case 38: _t->currentLightSlot(); break;
        case 39: _t->ambientColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 40: _t->ambientStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 41: _t->diffuseStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 42: _t->emissiveStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 43: _t->specularStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 44: _t->fixedDiffuseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 45: _t->lightTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 46: _t->lightEnabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 47: _t->lightPositionSlot(); break;
        case 48: _t->lightColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 49: _t->lightDirectionSlot(); break;
        case 50: _t->lightPointRadiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 51: _t->lightConstantAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 52: _t->lightLinearAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 53: _t->lightQuadraticAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 54: _t->lightSpotDirectionSlot(); break;
        case 55: _t->lightSpotExponentSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 56: _t->lightCutOffAngleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 57: _t->addLightSlot(); break;
        case 58: _t->resetLightSlot(); break;
        case 59: _t->showAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 60: _t->axesXPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 61: _t->axesYPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 62: _t->axesZPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 63: _t->mouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 64: _t->mouseScaleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 65: _t->selectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 66: _t->objectVisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 67: _t->translationSlot(); break;
        case 68: _t->rotationSlot(); break;
        case 69: _t->scaleSlot(); break;
        case 70: _t->objectColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 71: _t->diffuseTextureSlot(); break;
        case 72: _t->normalTextureSlot(); break;
        case 73: _t->specularTextureSlot(); break;
        case 74: _t->emissiveTextureSlot(); break;
        case 75: _t->swapSlot(); break;
        case 76: _t->invertSlot(); break;
        case 77: _t->addTextureSlot(); break;
        case 78: _t->loadTextureMapSlot(); break;
        case 79: _t->saveTextureMapSlot(); break;
        case 80: _t->loadMaterialMapSlot(); break;
        case 81: _t->saveMaterialMapSlot(); break;
        case 82: _t->selectParentSlot(); break;
        case 83: _t->selectRelatedSlot(); break;
        case 84: _t->deselectSlot(); break;
        case 85: _t->objectSelectSlot(); break;
        case 86: _t->objectZoomSlot(); break;
        case 87: _t->addCubeSlot(); break;
        case 88: _t->addCylinderSlot(); break;
        case 89: _t->addPyramidSlot(); break;
        case 90: _t->addSphereSlot(); break;
        case 91: _t->addTorusSlot(); break;
        case 92: _t->addModelSlot(); break;
        case 93: _t->currentMaterialSlot(); break;
        case 94: _t->materialAmbientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 95: _t->materialDiffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 96: _t->materialSpecularSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 97: _t->materialEmissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 98: _t->materialShininessSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 99: _t->materialTransparencySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 100: _t->materialAmbientTextureSlot(); break;
        case 101: _t->materialDiffuseTextureSlot(); break;
        case 102: _t->materialNormalTextureSlot(); break;
        case 103: _t->materialSpecularTextureSlot(); break;
        case 104: _t->materialEmissiveTextureSlot(); break;
        case 105: _t->overviewEqualScaleSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 106: _t->overviewSelectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 107: _t->overviewMouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 108: _t->overviewModelTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 109: _t->overviewCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 110: _t->overviewLightsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 111: _t->uvTextureTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 112: _t->bonesModelSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 113: _t->currentBoneSlot(); break;
        case 114: _t->updateBones(); break;
        case 115: _t->updateAnim(); break;
        case 116: _t->timerSlot(); break;
        case 117: _t->animNameSlot(); break;
        case 118: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 119: _t->playSlot(); break;
        case 120: _t->pauseSlot(); break;
        case 121: _t->stepSlot(); break;
        case 122: _t->animStep(); break;
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
        if (_id < 123)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 123;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 123)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 123;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
