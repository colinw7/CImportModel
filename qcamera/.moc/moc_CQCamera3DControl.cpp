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
    QByteArrayData data[134];
    char stringdata0[2041];
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
QT_MOC_LITERAL(6, 77, 14), // "showPointsSlot"
QT_MOC_LITERAL(7, 92, 1), // "i"
QT_MOC_LITERAL(8, 94, 13), // "pointSizeSlot"
QT_MOC_LITERAL(9, 108, 13), // "lineWidthSlot"
QT_MOC_LITERAL(10, 122, 15), // "showNormalsSlot"
QT_MOC_LITERAL(11, 138, 15), // "normalsSizeSlot"
QT_MOC_LITERAL(12, 154, 1), // "r"
QT_MOC_LITERAL(13, 156, 16), // "normalsColorSlot"
QT_MOC_LITERAL(14, 173, 1), // "c"
QT_MOC_LITERAL(15, 175, 13), // "depthTestSlot"
QT_MOC_LITERAL(16, 189, 8), // "cullSlot"
QT_MOC_LITERAL(17, 198, 13), // "frontFaceSlot"
QT_MOC_LITERAL(18, 212, 7), // "altSlot"
QT_MOC_LITERAL(19, 220, 14), // "showCameraSlot"
QT_MOC_LITERAL(20, 235, 15), // "showEyeLineSlot"
QT_MOC_LITERAL(21, 251, 14), // "showPlanesSlot"
QT_MOC_LITERAL(22, 266, 9), // "pitchSlot"
QT_MOC_LITERAL(23, 276, 7), // "yawSlot"
QT_MOC_LITERAL(24, 284, 8), // "rollSlot"
QT_MOC_LITERAL(25, 293, 8), // "nearSlot"
QT_MOC_LITERAL(26, 302, 7), // "farSlot"
QT_MOC_LITERAL(27, 310, 11), // "xOriginSlot"
QT_MOC_LITERAL(28, 322, 11), // "yOriginSlot"
QT_MOC_LITERAL(29, 334, 11), // "zOriginSlot"
QT_MOC_LITERAL(30, 346, 5), // "xSlot"
QT_MOC_LITERAL(31, 352, 5), // "ySlot"
QT_MOC_LITERAL(32, 358, 5), // "zSlot"
QT_MOC_LITERAL(33, 364, 7), // "fovSlot"
QT_MOC_LITERAL(34, 372, 9), // "eyeZ1Slot"
QT_MOC_LITERAL(35, 382, 9), // "eyeZ2Slot"
QT_MOC_LITERAL(36, 392, 12), // "aroundXSlot1"
QT_MOC_LITERAL(37, 405, 12), // "aroundXSlot2"
QT_MOC_LITERAL(38, 418, 12), // "aroundYSlot1"
QT_MOC_LITERAL(39, 431, 12), // "aroundYSlot2"
QT_MOC_LITERAL(40, 444, 12), // "aroundZSlot1"
QT_MOC_LITERAL(41, 457, 12), // "aroundZSlot2"
QT_MOC_LITERAL(42, 470, 9), // "resetSlot"
QT_MOC_LITERAL(43, 480, 7), // "topSlot"
QT_MOC_LITERAL(44, 488, 8), // "sideSlot"
QT_MOC_LITERAL(45, 497, 9), // "frontSlot"
QT_MOC_LITERAL(46, 507, 16), // "currentLightSlot"
QT_MOC_LITERAL(47, 524, 16), // "ambientColorSlot"
QT_MOC_LITERAL(48, 541, 19), // "ambientStrengthSlot"
QT_MOC_LITERAL(49, 561, 19), // "diffuseStrengthSlot"
QT_MOC_LITERAL(50, 581, 20), // "emissiveStrengthSlot"
QT_MOC_LITERAL(51, 602, 20), // "specularStrengthSlot"
QT_MOC_LITERAL(52, 623, 16), // "fixedDiffuseSlot"
QT_MOC_LITERAL(53, 640, 13), // "lightTypeSlot"
QT_MOC_LITERAL(54, 654, 16), // "lightEnabledSlot"
QT_MOC_LITERAL(55, 671, 17), // "lightPositionSlot"
QT_MOC_LITERAL(56, 689, 14), // "lightColorSlot"
QT_MOC_LITERAL(57, 704, 18), // "lightDirectionSlot"
QT_MOC_LITERAL(58, 723, 20), // "lightPointRadiusSlot"
QT_MOC_LITERAL(59, 744, 28), // "lightConstantAttenuationSlot"
QT_MOC_LITERAL(60, 773, 26), // "lightLinearAttenuationSlot"
QT_MOC_LITERAL(61, 800, 29), // "lightQuadraticAttenuationSlot"
QT_MOC_LITERAL(62, 830, 22), // "lightSpotDirectionSlot"
QT_MOC_LITERAL(63, 853, 21), // "lightSpotExponentSlot"
QT_MOC_LITERAL(64, 875, 20), // "lightCutOffAngleSlot"
QT_MOC_LITERAL(65, 896, 12), // "addLightSlot"
QT_MOC_LITERAL(66, 909, 14), // "resetLightSlot"
QT_MOC_LITERAL(67, 924, 12), // "showAxesSlot"
QT_MOC_LITERAL(68, 937, 12), // "axesXPosSlot"
QT_MOC_LITERAL(69, 950, 12), // "axesYPosSlot"
QT_MOC_LITERAL(70, 963, 12), // "axesZPosSlot"
QT_MOC_LITERAL(71, 976, 13), // "mouseTypeSlot"
QT_MOC_LITERAL(72, 990, 14), // "mouseScaleSlot"
QT_MOC_LITERAL(73, 1005, 14), // "selectTypeSlot"
QT_MOC_LITERAL(74, 1020, 13), // "objectVisSlot"
QT_MOC_LITERAL(75, 1034, 15), // "translationSlot"
QT_MOC_LITERAL(76, 1050, 12), // "rotationSlot"
QT_MOC_LITERAL(77, 1063, 9), // "scaleSlot"
QT_MOC_LITERAL(78, 1073, 15), // "objectColorSlot"
QT_MOC_LITERAL(79, 1089, 18), // "diffuseTextureSlot"
QT_MOC_LITERAL(80, 1108, 17), // "normalTextureSlot"
QT_MOC_LITERAL(81, 1126, 19), // "specularTextureSlot"
QT_MOC_LITERAL(82, 1146, 19), // "emissiveTextureSlot"
QT_MOC_LITERAL(83, 1166, 8), // "swapSlot"
QT_MOC_LITERAL(84, 1175, 10), // "invertSlot"
QT_MOC_LITERAL(85, 1186, 14), // "addTextureSlot"
QT_MOC_LITERAL(86, 1201, 18), // "loadTextureMapSlot"
QT_MOC_LITERAL(87, 1220, 18), // "saveTextureMapSlot"
QT_MOC_LITERAL(88, 1239, 19), // "loadMaterialMapSlot"
QT_MOC_LITERAL(89, 1259, 19), // "saveMaterialMapSlot"
QT_MOC_LITERAL(90, 1279, 16), // "selectParentSlot"
QT_MOC_LITERAL(91, 1296, 17), // "selectRelatedSlot"
QT_MOC_LITERAL(92, 1314, 12), // "deselectSlot"
QT_MOC_LITERAL(93, 1327, 16), // "objectSelectSlot"
QT_MOC_LITERAL(94, 1344, 14), // "objectZoomSlot"
QT_MOC_LITERAL(95, 1359, 11), // "addCubeSlot"
QT_MOC_LITERAL(96, 1371, 15), // "addCylinderSlot"
QT_MOC_LITERAL(97, 1387, 14), // "addPyramidSlot"
QT_MOC_LITERAL(98, 1402, 13), // "addSphereSlot"
QT_MOC_LITERAL(99, 1416, 12), // "addTorusSlot"
QT_MOC_LITERAL(100, 1429, 12), // "addModelSlot"
QT_MOC_LITERAL(101, 1442, 19), // "currentMaterialSlot"
QT_MOC_LITERAL(102, 1462, 19), // "materialAmbientSlot"
QT_MOC_LITERAL(103, 1482, 19), // "materialDiffuseSlot"
QT_MOC_LITERAL(104, 1502, 20), // "materialSpecularSlot"
QT_MOC_LITERAL(105, 1523, 20), // "materialEmissionSlot"
QT_MOC_LITERAL(106, 1544, 21), // "materialShininessSlot"
QT_MOC_LITERAL(107, 1566, 24), // "materialTransparencySlot"
QT_MOC_LITERAL(108, 1591, 26), // "materialAmbientTextureSlot"
QT_MOC_LITERAL(109, 1618, 26), // "materialDiffuseTextureSlot"
QT_MOC_LITERAL(110, 1645, 25), // "materialNormalTextureSlot"
QT_MOC_LITERAL(111, 1671, 27), // "materialSpecularTextureSlot"
QT_MOC_LITERAL(112, 1699, 27), // "materialEmissiveTextureSlot"
QT_MOC_LITERAL(113, 1727, 22), // "overviewEqualScaleSlot"
QT_MOC_LITERAL(114, 1750, 22), // "overviewSelectTypeSlot"
QT_MOC_LITERAL(115, 1773, 21), // "overviewMouseTypeSlot"
QT_MOC_LITERAL(116, 1795, 21), // "overviewModelTypeSlot"
QT_MOC_LITERAL(117, 1817, 18), // "overviewCameraSlot"
QT_MOC_LITERAL(118, 1836, 18), // "overviewLightsSlot"
QT_MOC_LITERAL(119, 1855, 17), // "uvTextureTypeSlot"
QT_MOC_LITERAL(120, 1873, 14), // "bonesModelSlot"
QT_MOC_LITERAL(121, 1888, 17), // "bonesBoneNodeSlot"
QT_MOC_LITERAL(122, 1906, 20), // "bonesPointJointsSlot"
QT_MOC_LITERAL(123, 1927, 15), // "currentBoneSlot"
QT_MOC_LITERAL(124, 1943, 11), // "updateBones"
QT_MOC_LITERAL(125, 1955, 10), // "updateAnim"
QT_MOC_LITERAL(126, 1966, 9), // "timerSlot"
QT_MOC_LITERAL(127, 1976, 12), // "animNameSlot"
QT_MOC_LITERAL(128, 1989, 12), // "animTimeSlot"
QT_MOC_LITERAL(129, 2002, 1), // "t"
QT_MOC_LITERAL(130, 2004, 8), // "playSlot"
QT_MOC_LITERAL(131, 2013, 9), // "pauseSlot"
QT_MOC_LITERAL(132, 2023, 8), // "stepSlot"
QT_MOC_LITERAL(133, 2032, 8) // "animStep"

    },
    "CQCamera3DControl\0updateWidgets\0\0"
    "mainTabSlot\0showWireframeSlot\0"
    "showSolidSlot\0showPointsSlot\0i\0"
    "pointSizeSlot\0lineWidthSlot\0showNormalsSlot\0"
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
    "fixedDiffuseSlot\0lightTypeSlot\0"
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
    "bonesBoneNodeSlot\0bonesPointJointsSlot\0"
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
     128,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  654,    2, 0x08 /* Private */,
       3,    1,  655,    2, 0x08 /* Private */,
       4,    1,  658,    2, 0x08 /* Private */,
       5,    1,  661,    2, 0x08 /* Private */,
       6,    1,  664,    2, 0x08 /* Private */,
       8,    1,  667,    2, 0x08 /* Private */,
       9,    1,  670,    2, 0x08 /* Private */,
      10,    1,  673,    2, 0x08 /* Private */,
      11,    1,  676,    2, 0x08 /* Private */,
      13,    1,  679,    2, 0x08 /* Private */,
      15,    1,  682,    2, 0x08 /* Private */,
      16,    1,  685,    2, 0x08 /* Private */,
      17,    1,  688,    2, 0x08 /* Private */,
      18,    1,  691,    2, 0x08 /* Private */,
      19,    1,  694,    2, 0x08 /* Private */,
      20,    1,  697,    2, 0x08 /* Private */,
      21,    1,  700,    2, 0x08 /* Private */,
      22,    1,  703,    2, 0x08 /* Private */,
      23,    1,  706,    2, 0x08 /* Private */,
      24,    1,  709,    2, 0x08 /* Private */,
      25,    1,  712,    2, 0x08 /* Private */,
      26,    1,  715,    2, 0x08 /* Private */,
      27,    1,  718,    2, 0x08 /* Private */,
      28,    1,  721,    2, 0x08 /* Private */,
      29,    1,  724,    2, 0x08 /* Private */,
      30,    1,  727,    2, 0x08 /* Private */,
      31,    1,  730,    2, 0x08 /* Private */,
      32,    1,  733,    2, 0x08 /* Private */,
      33,    1,  736,    2, 0x08 /* Private */,
      34,    1,  739,    2, 0x08 /* Private */,
      35,    1,  742,    2, 0x08 /* Private */,
      36,    0,  745,    2, 0x08 /* Private */,
      37,    0,  746,    2, 0x08 /* Private */,
      38,    0,  747,    2, 0x08 /* Private */,
      39,    0,  748,    2, 0x08 /* Private */,
      40,    0,  749,    2, 0x08 /* Private */,
      41,    0,  750,    2, 0x08 /* Private */,
      42,    0,  751,    2, 0x08 /* Private */,
      43,    0,  752,    2, 0x08 /* Private */,
      44,    0,  753,    2, 0x08 /* Private */,
      45,    0,  754,    2, 0x08 /* Private */,
      46,    0,  755,    2, 0x08 /* Private */,
      47,    1,  756,    2, 0x08 /* Private */,
      48,    1,  759,    2, 0x08 /* Private */,
      49,    1,  762,    2, 0x08 /* Private */,
      50,    1,  765,    2, 0x08 /* Private */,
      51,    1,  768,    2, 0x08 /* Private */,
      52,    1,  771,    2, 0x08 /* Private */,
      53,    1,  774,    2, 0x08 /* Private */,
      54,    1,  777,    2, 0x08 /* Private */,
      55,    0,  780,    2, 0x08 /* Private */,
      56,    1,  781,    2, 0x08 /* Private */,
      57,    0,  784,    2, 0x08 /* Private */,
      58,    1,  785,    2, 0x08 /* Private */,
      59,    1,  788,    2, 0x08 /* Private */,
      60,    1,  791,    2, 0x08 /* Private */,
      61,    1,  794,    2, 0x08 /* Private */,
      62,    0,  797,    2, 0x08 /* Private */,
      63,    1,  798,    2, 0x08 /* Private */,
      64,    1,  801,    2, 0x08 /* Private */,
      65,    0,  804,    2, 0x08 /* Private */,
      66,    0,  805,    2, 0x08 /* Private */,
      67,    1,  806,    2, 0x08 /* Private */,
      68,    1,  809,    2, 0x08 /* Private */,
      69,    1,  812,    2, 0x08 /* Private */,
      70,    1,  815,    2, 0x08 /* Private */,
      71,    1,  818,    2, 0x08 /* Private */,
      72,    1,  821,    2, 0x08 /* Private */,
      73,    1,  824,    2, 0x08 /* Private */,
      74,    1,  827,    2, 0x08 /* Private */,
      75,    0,  830,    2, 0x08 /* Private */,
      76,    0,  831,    2, 0x08 /* Private */,
      77,    0,  832,    2, 0x08 /* Private */,
      78,    1,  833,    2, 0x08 /* Private */,
      79,    0,  836,    2, 0x08 /* Private */,
      80,    0,  837,    2, 0x08 /* Private */,
      81,    0,  838,    2, 0x08 /* Private */,
      82,    0,  839,    2, 0x08 /* Private */,
      83,    0,  840,    2, 0x08 /* Private */,
      84,    0,  841,    2, 0x08 /* Private */,
      85,    0,  842,    2, 0x08 /* Private */,
      86,    0,  843,    2, 0x08 /* Private */,
      87,    0,  844,    2, 0x08 /* Private */,
      88,    0,  845,    2, 0x08 /* Private */,
      89,    0,  846,    2, 0x08 /* Private */,
      90,    0,  847,    2, 0x08 /* Private */,
      91,    0,  848,    2, 0x08 /* Private */,
      92,    0,  849,    2, 0x08 /* Private */,
      93,    0,  850,    2, 0x08 /* Private */,
      94,    0,  851,    2, 0x08 /* Private */,
      95,    0,  852,    2, 0x08 /* Private */,
      96,    0,  853,    2, 0x08 /* Private */,
      97,    0,  854,    2, 0x08 /* Private */,
      98,    0,  855,    2, 0x08 /* Private */,
      99,    0,  856,    2, 0x08 /* Private */,
     100,    0,  857,    2, 0x08 /* Private */,
     101,    0,  858,    2, 0x08 /* Private */,
     102,    1,  859,    2, 0x08 /* Private */,
     103,    1,  862,    2, 0x08 /* Private */,
     104,    1,  865,    2, 0x08 /* Private */,
     105,    1,  868,    2, 0x08 /* Private */,
     106,    1,  871,    2, 0x08 /* Private */,
     107,    1,  874,    2, 0x08 /* Private */,
     108,    0,  877,    2, 0x08 /* Private */,
     109,    0,  878,    2, 0x08 /* Private */,
     110,    0,  879,    2, 0x08 /* Private */,
     111,    0,  880,    2, 0x08 /* Private */,
     112,    0,  881,    2, 0x08 /* Private */,
     113,    1,  882,    2, 0x08 /* Private */,
     114,    1,  885,    2, 0x08 /* Private */,
     115,    1,  888,    2, 0x08 /* Private */,
     116,    1,  891,    2, 0x08 /* Private */,
     117,    1,  894,    2, 0x08 /* Private */,
     118,    1,  897,    2, 0x08 /* Private */,
     119,    1,  900,    2, 0x08 /* Private */,
     120,    1,  903,    2, 0x08 /* Private */,
     121,    1,  906,    2, 0x08 /* Private */,
     122,    1,  909,    2, 0x08 /* Private */,
     123,    0,  912,    2, 0x08 /* Private */,
     124,    0,  913,    2, 0x08 /* Private */,
     125,    0,  914,    2, 0x08 /* Private */,
     126,    0,  915,    2, 0x08 /* Private */,
     127,    0,  916,    2, 0x08 /* Private */,
     128,    1,  917,    2, 0x08 /* Private */,
     130,    0,  920,    2, 0x08 /* Private */,
     131,    0,  921,    2, 0x08 /* Private */,
     132,    0,  922,    2, 0x08 /* Private */,
     133,    0,  923,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
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
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Int,    7,
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
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  129,
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
        case 4: _t->showPointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->pointSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->lineWidthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->showNormalsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->normalsSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->normalsColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 10: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->altSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->showCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->showEyeLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->showPlanesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 22: _t->xOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 23: _t->yOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: _t->zOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: _t->xSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: _t->ySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 27: _t->zSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 28: _t->fovSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 29: _t->eyeZ1Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 30: _t->eyeZ2Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 31: _t->aroundXSlot1(); break;
        case 32: _t->aroundXSlot2(); break;
        case 33: _t->aroundYSlot1(); break;
        case 34: _t->aroundYSlot2(); break;
        case 35: _t->aroundZSlot1(); break;
        case 36: _t->aroundZSlot2(); break;
        case 37: _t->resetSlot(); break;
        case 38: _t->topSlot(); break;
        case 39: _t->sideSlot(); break;
        case 40: _t->frontSlot(); break;
        case 41: _t->currentLightSlot(); break;
        case 42: _t->ambientColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 43: _t->ambientStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 44: _t->diffuseStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 45: _t->emissiveStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 46: _t->specularStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 47: _t->fixedDiffuseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 48: _t->lightTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 49: _t->lightEnabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 50: _t->lightPositionSlot(); break;
        case 51: _t->lightColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 52: _t->lightDirectionSlot(); break;
        case 53: _t->lightPointRadiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 54: _t->lightConstantAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 55: _t->lightLinearAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 56: _t->lightQuadraticAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 57: _t->lightSpotDirectionSlot(); break;
        case 58: _t->lightSpotExponentSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 59: _t->lightCutOffAngleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 60: _t->addLightSlot(); break;
        case 61: _t->resetLightSlot(); break;
        case 62: _t->showAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 63: _t->axesXPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 64: _t->axesYPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 65: _t->axesZPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 66: _t->mouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 67: _t->mouseScaleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 68: _t->selectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 69: _t->objectVisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 70: _t->translationSlot(); break;
        case 71: _t->rotationSlot(); break;
        case 72: _t->scaleSlot(); break;
        case 73: _t->objectColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 74: _t->diffuseTextureSlot(); break;
        case 75: _t->normalTextureSlot(); break;
        case 76: _t->specularTextureSlot(); break;
        case 77: _t->emissiveTextureSlot(); break;
        case 78: _t->swapSlot(); break;
        case 79: _t->invertSlot(); break;
        case 80: _t->addTextureSlot(); break;
        case 81: _t->loadTextureMapSlot(); break;
        case 82: _t->saveTextureMapSlot(); break;
        case 83: _t->loadMaterialMapSlot(); break;
        case 84: _t->saveMaterialMapSlot(); break;
        case 85: _t->selectParentSlot(); break;
        case 86: _t->selectRelatedSlot(); break;
        case 87: _t->deselectSlot(); break;
        case 88: _t->objectSelectSlot(); break;
        case 89: _t->objectZoomSlot(); break;
        case 90: _t->addCubeSlot(); break;
        case 91: _t->addCylinderSlot(); break;
        case 92: _t->addPyramidSlot(); break;
        case 93: _t->addSphereSlot(); break;
        case 94: _t->addTorusSlot(); break;
        case 95: _t->addModelSlot(); break;
        case 96: _t->currentMaterialSlot(); break;
        case 97: _t->materialAmbientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 98: _t->materialDiffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 99: _t->materialSpecularSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 100: _t->materialEmissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 101: _t->materialShininessSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 102: _t->materialTransparencySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 103: _t->materialAmbientTextureSlot(); break;
        case 104: _t->materialDiffuseTextureSlot(); break;
        case 105: _t->materialNormalTextureSlot(); break;
        case 106: _t->materialSpecularTextureSlot(); break;
        case 107: _t->materialEmissiveTextureSlot(); break;
        case 108: _t->overviewEqualScaleSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 109: _t->overviewSelectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 110: _t->overviewMouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 111: _t->overviewModelTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 112: _t->overviewCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 113: _t->overviewLightsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 114: _t->uvTextureTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 115: _t->bonesModelSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 116: _t->bonesBoneNodeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 117: _t->bonesPointJointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 118: _t->currentBoneSlot(); break;
        case 119: _t->updateBones(); break;
        case 120: _t->updateAnim(); break;
        case 121: _t->timerSlot(); break;
        case 122: _t->animNameSlot(); break;
        case 123: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 124: _t->playSlot(); break;
        case 125: _t->pauseSlot(); break;
        case 126: _t->stepSlot(); break;
        case 127: _t->animStep(); break;
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
        if (_id < 128)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 128;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 128)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 128;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
