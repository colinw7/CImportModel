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
    QByteArrayData data[158];
    char stringdata0[2475];
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
QT_MOC_LITERAL(6, 77, 16), // "showTexturedSlot"
QT_MOC_LITERAL(7, 94, 14), // "showPointsSlot"
QT_MOC_LITERAL(8, 109, 1), // "i"
QT_MOC_LITERAL(9, 111, 12), // "quadViewSlot"
QT_MOC_LITERAL(10, 124, 13), // "pointSizeSlot"
QT_MOC_LITERAL(11, 138, 13), // "lineWidthSlot"
QT_MOC_LITERAL(12, 152, 10), // "cursorSlot"
QT_MOC_LITERAL(13, 163, 17), // "normalsPointsSlot"
QT_MOC_LITERAL(14, 181, 16), // "normalsFacesSlot"
QT_MOC_LITERAL(15, 198, 15), // "normalsSizeSlot"
QT_MOC_LITERAL(16, 214, 1), // "r"
QT_MOC_LITERAL(17, 216, 16), // "normalsColorSlot"
QT_MOC_LITERAL(18, 233, 1), // "c"
QT_MOC_LITERAL(19, 235, 13), // "showBasisSlot"
QT_MOC_LITERAL(20, 249, 12), // "showBBoxSlot"
QT_MOC_LITERAL(21, 262, 14), // "bboxOrientSlot"
QT_MOC_LITERAL(22, 277, 13), // "depthTestSlot"
QT_MOC_LITERAL(23, 291, 8), // "cullSlot"
QT_MOC_LITERAL(24, 300, 13), // "frontFaceSlot"
QT_MOC_LITERAL(25, 314, 14), // "showCameraSlot"
QT_MOC_LITERAL(26, 329, 15), // "showEyeLineSlot"
QT_MOC_LITERAL(27, 345, 14), // "showPlanesSlot"
QT_MOC_LITERAL(28, 360, 17), // "currentCameraSlot"
QT_MOC_LITERAL(29, 378, 9), // "pitchSlot"
QT_MOC_LITERAL(30, 388, 7), // "yawSlot"
QT_MOC_LITERAL(31, 396, 8), // "rollSlot"
QT_MOC_LITERAL(32, 405, 8), // "nearSlot"
QT_MOC_LITERAL(33, 414, 7), // "farSlot"
QT_MOC_LITERAL(34, 422, 11), // "xOriginSlot"
QT_MOC_LITERAL(35, 434, 11), // "yOriginSlot"
QT_MOC_LITERAL(36, 446, 11), // "zOriginSlot"
QT_MOC_LITERAL(37, 458, 5), // "xSlot"
QT_MOC_LITERAL(38, 464, 5), // "ySlot"
QT_MOC_LITERAL(39, 470, 5), // "zSlot"
QT_MOC_LITERAL(40, 476, 7), // "fovSlot"
QT_MOC_LITERAL(41, 484, 9), // "eyeZ1Slot"
QT_MOC_LITERAL(42, 494, 9), // "eyeZ2Slot"
QT_MOC_LITERAL(43, 504, 12), // "aroundXSlot1"
QT_MOC_LITERAL(44, 517, 12), // "aroundXSlot2"
QT_MOC_LITERAL(45, 530, 12), // "aroundYSlot1"
QT_MOC_LITERAL(46, 543, 12), // "aroundYSlot2"
QT_MOC_LITERAL(47, 556, 12), // "aroundZSlot1"
QT_MOC_LITERAL(48, 569, 12), // "aroundZSlot2"
QT_MOC_LITERAL(49, 582, 15), // "resetCameraSlot"
QT_MOC_LITERAL(50, 598, 13), // "topCameraSlot"
QT_MOC_LITERAL(51, 612, 14), // "sideCameraSlot"
QT_MOC_LITERAL(52, 627, 15), // "frontCameraSlot"
QT_MOC_LITERAL(53, 643, 16), // "currentLightSlot"
QT_MOC_LITERAL(54, 660, 16), // "ambientColorSlot"
QT_MOC_LITERAL(55, 677, 19), // "ambientStrengthSlot"
QT_MOC_LITERAL(56, 697, 19), // "diffuseStrengthSlot"
QT_MOC_LITERAL(57, 717, 20), // "emissiveStrengthSlot"
QT_MOC_LITERAL(58, 738, 20), // "specularStrengthSlot"
QT_MOC_LITERAL(59, 759, 16), // "fixedDiffuseSlot"
QT_MOC_LITERAL(60, 776, 13), // "lightTypeSlot"
QT_MOC_LITERAL(61, 790, 16), // "lightEnabledSlot"
QT_MOC_LITERAL(62, 807, 17), // "lightPositionSlot"
QT_MOC_LITERAL(63, 825, 14), // "lightColorSlot"
QT_MOC_LITERAL(64, 840, 18), // "lightDirectionSlot"
QT_MOC_LITERAL(65, 859, 20), // "lightPointRadiusSlot"
QT_MOC_LITERAL(66, 880, 28), // "lightConstantAttenuationSlot"
QT_MOC_LITERAL(67, 909, 26), // "lightLinearAttenuationSlot"
QT_MOC_LITERAL(68, 936, 29), // "lightQuadraticAttenuationSlot"
QT_MOC_LITERAL(69, 966, 22), // "lightSpotDirectionSlot"
QT_MOC_LITERAL(70, 989, 21), // "lightSpotExponentSlot"
QT_MOC_LITERAL(71, 1011, 20), // "lightCutOffAngleSlot"
QT_MOC_LITERAL(72, 1032, 12), // "addLightSlot"
QT_MOC_LITERAL(73, 1045, 14), // "resetLightSlot"
QT_MOC_LITERAL(74, 1060, 12), // "showAxesSlot"
QT_MOC_LITERAL(75, 1073, 12), // "axesXPosSlot"
QT_MOC_LITERAL(76, 1086, 12), // "axesYPosSlot"
QT_MOC_LITERAL(77, 1099, 12), // "axesZPosSlot"
QT_MOC_LITERAL(78, 1112, 13), // "mouseTypeSlot"
QT_MOC_LITERAL(79, 1126, 12), // "editTypeSlot"
QT_MOC_LITERAL(80, 1139, 14), // "mouseScaleSlot"
QT_MOC_LITERAL(81, 1154, 14), // "mouseBasisSlot"
QT_MOC_LITERAL(82, 1169, 14), // "selectTypeSlot"
QT_MOC_LITERAL(83, 1184, 13), // "objectVisSlot"
QT_MOC_LITERAL(84, 1198, 15), // "translationSlot"
QT_MOC_LITERAL(85, 1214, 12), // "rotationSlot"
QT_MOC_LITERAL(86, 1227, 9), // "scaleSlot"
QT_MOC_LITERAL(87, 1237, 15), // "objectColorSlot"
QT_MOC_LITERAL(88, 1253, 18), // "diffuseTextureSlot"
QT_MOC_LITERAL(89, 1272, 17), // "normalTextureSlot"
QT_MOC_LITERAL(90, 1290, 19), // "specularTextureSlot"
QT_MOC_LITERAL(91, 1310, 19), // "emissiveTextureSlot"
QT_MOC_LITERAL(92, 1330, 19), // "connectMaterialEdit"
QT_MOC_LITERAL(93, 1350, 8), // "swapSlot"
QT_MOC_LITERAL(94, 1359, 10), // "invertSlot"
QT_MOC_LITERAL(95, 1370, 14), // "addTextureSlot"
QT_MOC_LITERAL(96, 1385, 18), // "loadTextureMapSlot"
QT_MOC_LITERAL(97, 1404, 18), // "saveTextureMapSlot"
QT_MOC_LITERAL(98, 1423, 19), // "loadMaterialMapSlot"
QT_MOC_LITERAL(99, 1443, 19), // "saveMaterialMapSlot"
QT_MOC_LITERAL(100, 1463, 15), // "addMaterialSlot"
QT_MOC_LITERAL(101, 1479, 16), // "selectParentSlot"
QT_MOC_LITERAL(102, 1496, 15), // "selectFacesSlot"
QT_MOC_LITERAL(103, 1512, 16), // "selectPointsSlot"
QT_MOC_LITERAL(104, 1529, 17), // "selectRelatedSlot"
QT_MOC_LITERAL(105, 1547, 12), // "deselectSlot"
QT_MOC_LITERAL(106, 1560, 14), // "selectDumpSlot"
QT_MOC_LITERAL(107, 1575, 16), // "objectSelectSlot"
QT_MOC_LITERAL(108, 1592, 14), // "objectZoomSlot"
QT_MOC_LITERAL(109, 1607, 11), // "addCubeSlot"
QT_MOC_LITERAL(110, 1619, 15), // "addCylinderSlot"
QT_MOC_LITERAL(111, 1635, 14), // "addPyramidSlot"
QT_MOC_LITERAL(112, 1650, 13), // "addSphereSlot"
QT_MOC_LITERAL(113, 1664, 12), // "addTorusSlot"
QT_MOC_LITERAL(114, 1677, 13), // "loadModelSlot"
QT_MOC_LITERAL(115, 1691, 13), // "saveModelSlot"
QT_MOC_LITERAL(116, 1705, 19), // "currentMaterialSlot"
QT_MOC_LITERAL(117, 1725, 19), // "materialAmbientSlot"
QT_MOC_LITERAL(118, 1745, 19), // "materialDiffuseSlot"
QT_MOC_LITERAL(119, 1765, 20), // "materialSpecularSlot"
QT_MOC_LITERAL(120, 1786, 20), // "materialEmissionSlot"
QT_MOC_LITERAL(121, 1807, 21), // "materialShininessSlot"
QT_MOC_LITERAL(122, 1829, 24), // "materialTransparencySlot"
QT_MOC_LITERAL(123, 1854, 26), // "materialAmbientTextureSlot"
QT_MOC_LITERAL(124, 1881, 26), // "materialDiffuseTextureSlot"
QT_MOC_LITERAL(125, 1908, 25), // "materialNormalTextureSlot"
QT_MOC_LITERAL(126, 1934, 27), // "materialSpecularTextureSlot"
QT_MOC_LITERAL(127, 1962, 27), // "materialEmissiveTextureSlot"
QT_MOC_LITERAL(128, 1990, 24), // "currentObjectChangedSlot"
QT_MOC_LITERAL(129, 2015, 20), // "objectAutoSelectSlot"
QT_MOC_LITERAL(130, 2036, 22), // "overviewEqualScaleSlot"
QT_MOC_LITERAL(131, 2059, 22), // "overviewSelectTypeSlot"
QT_MOC_LITERAL(132, 2082, 21), // "overviewMouseTypeSlot"
QT_MOC_LITERAL(133, 2104, 21), // "overviewModelTypeSlot"
QT_MOC_LITERAL(134, 2126, 18), // "overviewCameraSlot"
QT_MOC_LITERAL(135, 2145, 18), // "overviewLightsSlot"
QT_MOC_LITERAL(136, 2164, 17), // "uvTextureTypeSlot"
QT_MOC_LITERAL(137, 2182, 15), // "textureFlipSlot"
QT_MOC_LITERAL(138, 2198, 14), // "bonesModelSlot"
QT_MOC_LITERAL(139, 2213, 17), // "bonesBoneNodeSlot"
QT_MOC_LITERAL(140, 2231, 20), // "bonesPointJointsSlot"
QT_MOC_LITERAL(141, 2252, 19), // "bonesOnlyJointsSlot"
QT_MOC_LITERAL(142, 2272, 20), // "bonesTranslationSlot"
QT_MOC_LITERAL(143, 2293, 17), // "bonesRotationSlot"
QT_MOC_LITERAL(144, 2311, 14), // "bonesScaleSlot"
QT_MOC_LITERAL(145, 2326, 15), // "currentBoneSlot"
QT_MOC_LITERAL(146, 2342, 11), // "updateBones"
QT_MOC_LITERAL(147, 2354, 17), // "updateCurrentBone"
QT_MOC_LITERAL(148, 2372, 10), // "updateAnim"
QT_MOC_LITERAL(149, 2383, 9), // "timerSlot"
QT_MOC_LITERAL(150, 2393, 12), // "animNameSlot"
QT_MOC_LITERAL(151, 2406, 12), // "animTimeSlot"
QT_MOC_LITERAL(152, 2419, 1), // "t"
QT_MOC_LITERAL(153, 2421, 16), // "animTimeStepSlot"
QT_MOC_LITERAL(154, 2438, 8), // "playSlot"
QT_MOC_LITERAL(155, 2447, 9), // "pauseSlot"
QT_MOC_LITERAL(156, 2457, 8), // "stepSlot"
QT_MOC_LITERAL(157, 2466, 8) // "animStep"

    },
    "CQCamera3DControl\0updateWidgets\0\0"
    "mainTabSlot\0showWireframeSlot\0"
    "showSolidSlot\0showTexturedSlot\0"
    "showPointsSlot\0i\0quadViewSlot\0"
    "pointSizeSlot\0lineWidthSlot\0cursorSlot\0"
    "normalsPointsSlot\0normalsFacesSlot\0"
    "normalsSizeSlot\0r\0normalsColorSlot\0c\0"
    "showBasisSlot\0showBBoxSlot\0bboxOrientSlot\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "showCameraSlot\0showEyeLineSlot\0"
    "showPlanesSlot\0currentCameraSlot\0"
    "pitchSlot\0yawSlot\0rollSlot\0nearSlot\0"
    "farSlot\0xOriginSlot\0yOriginSlot\0"
    "zOriginSlot\0xSlot\0ySlot\0zSlot\0fovSlot\0"
    "eyeZ1Slot\0eyeZ2Slot\0aroundXSlot1\0"
    "aroundXSlot2\0aroundYSlot1\0aroundYSlot2\0"
    "aroundZSlot1\0aroundZSlot2\0resetCameraSlot\0"
    "topCameraSlot\0sideCameraSlot\0"
    "frontCameraSlot\0currentLightSlot\0"
    "ambientColorSlot\0ambientStrengthSlot\0"
    "diffuseStrengthSlot\0emissiveStrengthSlot\0"
    "specularStrengthSlot\0fixedDiffuseSlot\0"
    "lightTypeSlot\0lightEnabledSlot\0"
    "lightPositionSlot\0lightColorSlot\0"
    "lightDirectionSlot\0lightPointRadiusSlot\0"
    "lightConstantAttenuationSlot\0"
    "lightLinearAttenuationSlot\0"
    "lightQuadraticAttenuationSlot\0"
    "lightSpotDirectionSlot\0lightSpotExponentSlot\0"
    "lightCutOffAngleSlot\0addLightSlot\0"
    "resetLightSlot\0showAxesSlot\0axesXPosSlot\0"
    "axesYPosSlot\0axesZPosSlot\0mouseTypeSlot\0"
    "editTypeSlot\0mouseScaleSlot\0mouseBasisSlot\0"
    "selectTypeSlot\0objectVisSlot\0"
    "translationSlot\0rotationSlot\0scaleSlot\0"
    "objectColorSlot\0diffuseTextureSlot\0"
    "normalTextureSlot\0specularTextureSlot\0"
    "emissiveTextureSlot\0connectMaterialEdit\0"
    "swapSlot\0invertSlot\0addTextureSlot\0"
    "loadTextureMapSlot\0saveTextureMapSlot\0"
    "loadMaterialMapSlot\0saveMaterialMapSlot\0"
    "addMaterialSlot\0selectParentSlot\0"
    "selectFacesSlot\0selectPointsSlot\0"
    "selectRelatedSlot\0deselectSlot\0"
    "selectDumpSlot\0objectSelectSlot\0"
    "objectZoomSlot\0addCubeSlot\0addCylinderSlot\0"
    "addPyramidSlot\0addSphereSlot\0addTorusSlot\0"
    "loadModelSlot\0saveModelSlot\0"
    "currentMaterialSlot\0materialAmbientSlot\0"
    "materialDiffuseSlot\0materialSpecularSlot\0"
    "materialEmissionSlot\0materialShininessSlot\0"
    "materialTransparencySlot\0"
    "materialAmbientTextureSlot\0"
    "materialDiffuseTextureSlot\0"
    "materialNormalTextureSlot\0"
    "materialSpecularTextureSlot\0"
    "materialEmissiveTextureSlot\0"
    "currentObjectChangedSlot\0objectAutoSelectSlot\0"
    "overviewEqualScaleSlot\0overviewSelectTypeSlot\0"
    "overviewMouseTypeSlot\0overviewModelTypeSlot\0"
    "overviewCameraSlot\0overviewLightsSlot\0"
    "uvTextureTypeSlot\0textureFlipSlot\0"
    "bonesModelSlot\0bonesBoneNodeSlot\0"
    "bonesPointJointsSlot\0bonesOnlyJointsSlot\0"
    "bonesTranslationSlot\0bonesRotationSlot\0"
    "bonesScaleSlot\0currentBoneSlot\0"
    "updateBones\0updateCurrentBone\0updateAnim\0"
    "timerSlot\0animNameSlot\0animTimeSlot\0"
    "t\0animTimeStepSlot\0playSlot\0pauseSlot\0"
    "stepSlot\0animStep"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
     152,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  774,    2, 0x08 /* Private */,
       3,    1,  775,    2, 0x08 /* Private */,
       4,    1,  778,    2, 0x08 /* Private */,
       5,    1,  781,    2, 0x08 /* Private */,
       6,    1,  784,    2, 0x08 /* Private */,
       7,    1,  787,    2, 0x08 /* Private */,
       9,    1,  790,    2, 0x08 /* Private */,
      10,    1,  793,    2, 0x08 /* Private */,
      11,    1,  796,    2, 0x08 /* Private */,
      12,    0,  799,    2, 0x08 /* Private */,
      13,    1,  800,    2, 0x08 /* Private */,
      14,    1,  803,    2, 0x08 /* Private */,
      15,    1,  806,    2, 0x08 /* Private */,
      17,    1,  809,    2, 0x08 /* Private */,
      19,    1,  812,    2, 0x08 /* Private */,
      20,    1,  815,    2, 0x08 /* Private */,
      21,    1,  818,    2, 0x08 /* Private */,
      22,    1,  821,    2, 0x08 /* Private */,
      23,    1,  824,    2, 0x08 /* Private */,
      24,    1,  827,    2, 0x08 /* Private */,
      25,    1,  830,    2, 0x08 /* Private */,
      26,    1,  833,    2, 0x08 /* Private */,
      27,    1,  836,    2, 0x08 /* Private */,
      28,    0,  839,    2, 0x08 /* Private */,
      29,    1,  840,    2, 0x08 /* Private */,
      30,    1,  843,    2, 0x08 /* Private */,
      31,    1,  846,    2, 0x08 /* Private */,
      32,    1,  849,    2, 0x08 /* Private */,
      33,    1,  852,    2, 0x08 /* Private */,
      34,    1,  855,    2, 0x08 /* Private */,
      35,    1,  858,    2, 0x08 /* Private */,
      36,    1,  861,    2, 0x08 /* Private */,
      37,    1,  864,    2, 0x08 /* Private */,
      38,    1,  867,    2, 0x08 /* Private */,
      39,    1,  870,    2, 0x08 /* Private */,
      40,    1,  873,    2, 0x08 /* Private */,
      41,    1,  876,    2, 0x08 /* Private */,
      42,    1,  879,    2, 0x08 /* Private */,
      43,    0,  882,    2, 0x08 /* Private */,
      44,    0,  883,    2, 0x08 /* Private */,
      45,    0,  884,    2, 0x08 /* Private */,
      46,    0,  885,    2, 0x08 /* Private */,
      47,    0,  886,    2, 0x08 /* Private */,
      48,    0,  887,    2, 0x08 /* Private */,
      49,    0,  888,    2, 0x08 /* Private */,
      50,    0,  889,    2, 0x08 /* Private */,
      51,    0,  890,    2, 0x08 /* Private */,
      52,    0,  891,    2, 0x08 /* Private */,
      53,    0,  892,    2, 0x08 /* Private */,
      54,    1,  893,    2, 0x08 /* Private */,
      55,    1,  896,    2, 0x08 /* Private */,
      56,    1,  899,    2, 0x08 /* Private */,
      57,    1,  902,    2, 0x08 /* Private */,
      58,    1,  905,    2, 0x08 /* Private */,
      59,    1,  908,    2, 0x08 /* Private */,
      60,    1,  911,    2, 0x08 /* Private */,
      61,    1,  914,    2, 0x08 /* Private */,
      62,    0,  917,    2, 0x08 /* Private */,
      63,    1,  918,    2, 0x08 /* Private */,
      64,    0,  921,    2, 0x08 /* Private */,
      65,    1,  922,    2, 0x08 /* Private */,
      66,    1,  925,    2, 0x08 /* Private */,
      67,    1,  928,    2, 0x08 /* Private */,
      68,    1,  931,    2, 0x08 /* Private */,
      69,    0,  934,    2, 0x08 /* Private */,
      70,    1,  935,    2, 0x08 /* Private */,
      71,    1,  938,    2, 0x08 /* Private */,
      72,    0,  941,    2, 0x08 /* Private */,
      73,    0,  942,    2, 0x08 /* Private */,
      74,    1,  943,    2, 0x08 /* Private */,
      75,    1,  946,    2, 0x08 /* Private */,
      76,    1,  949,    2, 0x08 /* Private */,
      77,    1,  952,    2, 0x08 /* Private */,
      78,    1,  955,    2, 0x08 /* Private */,
      79,    1,  958,    2, 0x08 /* Private */,
      80,    1,  961,    2, 0x08 /* Private */,
      81,    1,  964,    2, 0x08 /* Private */,
      82,    1,  967,    2, 0x08 /* Private */,
      83,    1,  970,    2, 0x08 /* Private */,
      84,    0,  973,    2, 0x08 /* Private */,
      85,    0,  974,    2, 0x08 /* Private */,
      86,    0,  975,    2, 0x08 /* Private */,
      87,    1,  976,    2, 0x08 /* Private */,
      88,    0,  979,    2, 0x08 /* Private */,
      89,    0,  980,    2, 0x08 /* Private */,
      90,    0,  981,    2, 0x08 /* Private */,
      91,    0,  982,    2, 0x08 /* Private */,
      92,    0,  983,    2, 0x08 /* Private */,
      93,    0,  984,    2, 0x08 /* Private */,
      94,    0,  985,    2, 0x08 /* Private */,
      95,    0,  986,    2, 0x08 /* Private */,
      96,    0,  987,    2, 0x08 /* Private */,
      97,    0,  988,    2, 0x08 /* Private */,
      98,    0,  989,    2, 0x08 /* Private */,
      99,    0,  990,    2, 0x08 /* Private */,
     100,    0,  991,    2, 0x08 /* Private */,
     101,    0,  992,    2, 0x08 /* Private */,
     102,    0,  993,    2, 0x08 /* Private */,
     103,    0,  994,    2, 0x08 /* Private */,
     104,    0,  995,    2, 0x08 /* Private */,
     105,    0,  996,    2, 0x08 /* Private */,
     106,    0,  997,    2, 0x08 /* Private */,
     107,    0,  998,    2, 0x08 /* Private */,
     108,    0,  999,    2, 0x08 /* Private */,
     109,    0, 1000,    2, 0x08 /* Private */,
     110,    0, 1001,    2, 0x08 /* Private */,
     111,    0, 1002,    2, 0x08 /* Private */,
     112,    0, 1003,    2, 0x08 /* Private */,
     113,    0, 1004,    2, 0x08 /* Private */,
     114,    0, 1005,    2, 0x08 /* Private */,
     115,    0, 1006,    2, 0x08 /* Private */,
     116,    0, 1007,    2, 0x08 /* Private */,
     117,    1, 1008,    2, 0x08 /* Private */,
     118,    1, 1011,    2, 0x08 /* Private */,
     119,    1, 1014,    2, 0x08 /* Private */,
     120,    1, 1017,    2, 0x08 /* Private */,
     121,    1, 1020,    2, 0x08 /* Private */,
     122,    1, 1023,    2, 0x08 /* Private */,
     123,    0, 1026,    2, 0x08 /* Private */,
     124,    0, 1027,    2, 0x08 /* Private */,
     125,    0, 1028,    2, 0x08 /* Private */,
     126,    0, 1029,    2, 0x08 /* Private */,
     127,    0, 1030,    2, 0x08 /* Private */,
     128,    0, 1031,    2, 0x08 /* Private */,
     129,    1, 1032,    2, 0x08 /* Private */,
     130,    1, 1035,    2, 0x08 /* Private */,
     131,    1, 1038,    2, 0x08 /* Private */,
     132,    1, 1041,    2, 0x08 /* Private */,
     133,    1, 1044,    2, 0x08 /* Private */,
     134,    1, 1047,    2, 0x08 /* Private */,
     135,    1, 1050,    2, 0x08 /* Private */,
     136,    1, 1053,    2, 0x08 /* Private */,
     137,    1, 1056,    2, 0x08 /* Private */,
     138,    1, 1059,    2, 0x08 /* Private */,
     139,    1, 1062,    2, 0x08 /* Private */,
     140,    1, 1065,    2, 0x08 /* Private */,
     141,    1, 1068,    2, 0x08 /* Private */,
     142,    0, 1071,    2, 0x08 /* Private */,
     143,    0, 1072,    2, 0x08 /* Private */,
     144,    0, 1073,    2, 0x08 /* Private */,
     145,    0, 1074,    2, 0x08 /* Private */,
     146,    0, 1075,    2, 0x08 /* Private */,
     147,    0, 1076,    2, 0x08 /* Private */,
     148,    0, 1077,    2, 0x08 /* Private */,
     149,    0, 1078,    2, 0x08 /* Private */,
     150,    0, 1079,    2, 0x08 /* Private */,
     151,    1, 1080,    2, 0x08 /* Private */,
     153,    1, 1083,    2, 0x08 /* Private */,
     154,    0, 1086,    2, 0x08 /* Private */,
     155,    0, 1087,    2, 0x08 /* Private */,
     156,    0, 1088,    2, 0x08 /* Private */,
     157,    0, 1089,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::QColor,   18,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Int,    8,
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
    QMetaType::Void, QMetaType::Int,    2,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,   18,
    QMetaType::Void, QMetaType::QColor,   18,
    QMetaType::Void, QMetaType::QColor,   18,
    QMetaType::Void, QMetaType::QColor,   18,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  152,
    QMetaType::Void, QMetaType::Double,  152,
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
        case 4: _t->showTexturedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->showPointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->quadViewSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->pointSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->lineWidthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->cursorSlot(); break;
        case 10: _t->normalsPointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->normalsFacesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->normalsSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->normalsColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 14: _t->showBasisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->showBBoxSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->bboxOrientSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->showCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->showEyeLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->showPlanesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->currentCameraSlot(); break;
        case 24: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 27: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 28: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 29: _t->xOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 30: _t->yOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 31: _t->zOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 32: _t->xSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 33: _t->ySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 34: _t->zSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 35: _t->fovSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 36: _t->eyeZ1Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 37: _t->eyeZ2Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 38: _t->aroundXSlot1(); break;
        case 39: _t->aroundXSlot2(); break;
        case 40: _t->aroundYSlot1(); break;
        case 41: _t->aroundYSlot2(); break;
        case 42: _t->aroundZSlot1(); break;
        case 43: _t->aroundZSlot2(); break;
        case 44: _t->resetCameraSlot(); break;
        case 45: _t->topCameraSlot(); break;
        case 46: _t->sideCameraSlot(); break;
        case 47: _t->frontCameraSlot(); break;
        case 48: _t->currentLightSlot(); break;
        case 49: _t->ambientColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 50: _t->ambientStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 51: _t->diffuseStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 52: _t->emissiveStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 53: _t->specularStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 54: _t->fixedDiffuseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 55: _t->lightTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 56: _t->lightEnabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 57: _t->lightPositionSlot(); break;
        case 58: _t->lightColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 59: _t->lightDirectionSlot(); break;
        case 60: _t->lightPointRadiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 61: _t->lightConstantAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 62: _t->lightLinearAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 63: _t->lightQuadraticAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 64: _t->lightSpotDirectionSlot(); break;
        case 65: _t->lightSpotExponentSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 66: _t->lightCutOffAngleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 67: _t->addLightSlot(); break;
        case 68: _t->resetLightSlot(); break;
        case 69: _t->showAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 70: _t->axesXPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 71: _t->axesYPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 72: _t->axesZPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 73: _t->mouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 74: _t->editTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 75: _t->mouseScaleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 76: _t->mouseBasisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 77: _t->selectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 78: _t->objectVisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 79: _t->translationSlot(); break;
        case 80: _t->rotationSlot(); break;
        case 81: _t->scaleSlot(); break;
        case 82: _t->objectColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 83: _t->diffuseTextureSlot(); break;
        case 84: _t->normalTextureSlot(); break;
        case 85: _t->specularTextureSlot(); break;
        case 86: _t->emissiveTextureSlot(); break;
        case 87: _t->connectMaterialEdit(); break;
        case 88: _t->swapSlot(); break;
        case 89: _t->invertSlot(); break;
        case 90: _t->addTextureSlot(); break;
        case 91: _t->loadTextureMapSlot(); break;
        case 92: _t->saveTextureMapSlot(); break;
        case 93: _t->loadMaterialMapSlot(); break;
        case 94: _t->saveMaterialMapSlot(); break;
        case 95: _t->addMaterialSlot(); break;
        case 96: _t->selectParentSlot(); break;
        case 97: _t->selectFacesSlot(); break;
        case 98: _t->selectPointsSlot(); break;
        case 99: _t->selectRelatedSlot(); break;
        case 100: _t->deselectSlot(); break;
        case 101: _t->selectDumpSlot(); break;
        case 102: _t->objectSelectSlot(); break;
        case 103: _t->objectZoomSlot(); break;
        case 104: _t->addCubeSlot(); break;
        case 105: _t->addCylinderSlot(); break;
        case 106: _t->addPyramidSlot(); break;
        case 107: _t->addSphereSlot(); break;
        case 108: _t->addTorusSlot(); break;
        case 109: _t->loadModelSlot(); break;
        case 110: _t->saveModelSlot(); break;
        case 111: _t->currentMaterialSlot(); break;
        case 112: _t->materialAmbientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 113: _t->materialDiffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 114: _t->materialSpecularSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 115: _t->materialEmissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 116: _t->materialShininessSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 117: _t->materialTransparencySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 118: _t->materialAmbientTextureSlot(); break;
        case 119: _t->materialDiffuseTextureSlot(); break;
        case 120: _t->materialNormalTextureSlot(); break;
        case 121: _t->materialSpecularTextureSlot(); break;
        case 122: _t->materialEmissiveTextureSlot(); break;
        case 123: _t->currentObjectChangedSlot(); break;
        case 124: _t->objectAutoSelectSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 125: _t->overviewEqualScaleSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 126: _t->overviewSelectTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 127: _t->overviewMouseTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 128: _t->overviewModelTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 129: _t->overviewCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 130: _t->overviewLightsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 131: _t->uvTextureTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 132: _t->textureFlipSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 133: _t->bonesModelSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 134: _t->bonesBoneNodeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 135: _t->bonesPointJointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 136: _t->bonesOnlyJointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 137: _t->bonesTranslationSlot(); break;
        case 138: _t->bonesRotationSlot(); break;
        case 139: _t->bonesScaleSlot(); break;
        case 140: _t->currentBoneSlot(); break;
        case 141: _t->updateBones(); break;
        case 142: _t->updateCurrentBone(); break;
        case 143: _t->updateAnim(); break;
        case 144: _t->timerSlot(); break;
        case 145: _t->animNameSlot(); break;
        case 146: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 147: _t->animTimeStepSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 148: _t->playSlot(); break;
        case 149: _t->pauseSlot(); break;
        case 150: _t->stepSlot(); break;
        case 151: _t->animStep(); break;
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
        if (_id < 152)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 152;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 152)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 152;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
