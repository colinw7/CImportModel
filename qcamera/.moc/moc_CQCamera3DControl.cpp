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
    QByteArrayData data[167];
    char stringdata0[2590];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCamera3DControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCamera3DControl_t qt_meta_stringdata_CQCamera3DControl = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CQCamera3DControl"
QT_MOC_LITERAL(1, 18, 12), // "viewTypeSlot"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 13), // "updateWidgets"
QT_MOC_LITERAL(4, 46, 20), // "updateTextureWidgets"
QT_MOC_LITERAL(5, 67, 10), // "disconnect"
QT_MOC_LITERAL(6, 78, 19), // "updateCameraWidgets"
QT_MOC_LITERAL(7, 98, 11), // "mainTabSlot"
QT_MOC_LITERAL(8, 110, 17), // "showWireframeSlot"
QT_MOC_LITERAL(9, 128, 13), // "showSolidSlot"
QT_MOC_LITERAL(10, 142, 16), // "showTexturedSlot"
QT_MOC_LITERAL(11, 159, 14), // "showPointsSlot"
QT_MOC_LITERAL(12, 174, 1), // "i"
QT_MOC_LITERAL(13, 176, 18), // "wireframeColorSlot"
QT_MOC_LITERAL(14, 195, 1), // "c"
QT_MOC_LITERAL(15, 197, 12), // "quadViewSlot"
QT_MOC_LITERAL(16, 210, 15), // "debugCameraSlot"
QT_MOC_LITERAL(17, 226, 13), // "pointSizeSlot"
QT_MOC_LITERAL(18, 240, 13), // "lineWidthSlot"
QT_MOC_LITERAL(19, 254, 10), // "cursorSlot"
QT_MOC_LITERAL(20, 265, 17), // "normalsPointsSlot"
QT_MOC_LITERAL(21, 283, 16), // "normalsFacesSlot"
QT_MOC_LITERAL(22, 300, 15), // "normalsSizeSlot"
QT_MOC_LITERAL(23, 316, 1), // "r"
QT_MOC_LITERAL(24, 318, 16), // "normalsColorSlot"
QT_MOC_LITERAL(25, 335, 13), // "showBasisSlot"
QT_MOC_LITERAL(26, 349, 12), // "showBBoxSlot"
QT_MOC_LITERAL(27, 362, 14), // "bboxOrientSlot"
QT_MOC_LITERAL(28, 377, 13), // "depthTestSlot"
QT_MOC_LITERAL(29, 391, 8), // "cullSlot"
QT_MOC_LITERAL(30, 400, 13), // "frontFaceSlot"
QT_MOC_LITERAL(31, 414, 14), // "showCameraSlot"
QT_MOC_LITERAL(32, 429, 15), // "showEyeLineSlot"
QT_MOC_LITERAL(33, 445, 14), // "showPlanesSlot"
QT_MOC_LITERAL(34, 460, 17), // "currentCameraSlot"
QT_MOC_LITERAL(35, 478, 9), // "pitchSlot"
QT_MOC_LITERAL(36, 488, 7), // "yawSlot"
QT_MOC_LITERAL(37, 496, 8), // "rollSlot"
QT_MOC_LITERAL(38, 505, 8), // "nearSlot"
QT_MOC_LITERAL(39, 514, 7), // "farSlot"
QT_MOC_LITERAL(40, 522, 11), // "xOriginSlot"
QT_MOC_LITERAL(41, 534, 11), // "yOriginSlot"
QT_MOC_LITERAL(42, 546, 11), // "zOriginSlot"
QT_MOC_LITERAL(43, 558, 5), // "xSlot"
QT_MOC_LITERAL(44, 564, 5), // "ySlot"
QT_MOC_LITERAL(45, 570, 5), // "zSlot"
QT_MOC_LITERAL(46, 576, 7), // "fovSlot"
QT_MOC_LITERAL(47, 584, 9), // "eyeZ1Slot"
QT_MOC_LITERAL(48, 594, 9), // "eyeZ2Slot"
QT_MOC_LITERAL(49, 604, 12), // "aroundXSlot1"
QT_MOC_LITERAL(50, 617, 12), // "aroundXSlot2"
QT_MOC_LITERAL(51, 630, 12), // "aroundYSlot1"
QT_MOC_LITERAL(52, 643, 12), // "aroundYSlot2"
QT_MOC_LITERAL(53, 656, 12), // "aroundZSlot1"
QT_MOC_LITERAL(54, 669, 12), // "aroundZSlot2"
QT_MOC_LITERAL(55, 682, 15), // "aroundDeltaSlot"
QT_MOC_LITERAL(56, 698, 1), // "d"
QT_MOC_LITERAL(57, 700, 15), // "resetCameraSlot"
QT_MOC_LITERAL(58, 716, 13), // "topCameraSlot"
QT_MOC_LITERAL(59, 730, 14), // "sideCameraSlot"
QT_MOC_LITERAL(60, 745, 15), // "frontCameraSlot"
QT_MOC_LITERAL(61, 761, 16), // "currentLightSlot"
QT_MOC_LITERAL(62, 778, 16), // "ambientColorSlot"
QT_MOC_LITERAL(63, 795, 19), // "ambientStrengthSlot"
QT_MOC_LITERAL(64, 815, 19), // "diffuseStrengthSlot"
QT_MOC_LITERAL(65, 835, 20), // "emissiveStrengthSlot"
QT_MOC_LITERAL(66, 856, 20), // "specularStrengthSlot"
QT_MOC_LITERAL(67, 877, 16), // "fixedDiffuseSlot"
QT_MOC_LITERAL(68, 894, 13), // "lightTypeSlot"
QT_MOC_LITERAL(69, 908, 16), // "lightEnabledSlot"
QT_MOC_LITERAL(70, 925, 17), // "lightPositionSlot"
QT_MOC_LITERAL(71, 943, 14), // "lightColorSlot"
QT_MOC_LITERAL(72, 958, 18), // "lightDirectionSlot"
QT_MOC_LITERAL(73, 977, 20), // "lightPointRadiusSlot"
QT_MOC_LITERAL(74, 998, 28), // "lightConstantAttenuationSlot"
QT_MOC_LITERAL(75, 1027, 26), // "lightLinearAttenuationSlot"
QT_MOC_LITERAL(76, 1054, 29), // "lightQuadraticAttenuationSlot"
QT_MOC_LITERAL(77, 1084, 22), // "lightSpotDirectionSlot"
QT_MOC_LITERAL(78, 1107, 21), // "lightSpotExponentSlot"
QT_MOC_LITERAL(79, 1129, 20), // "lightCutOffAngleSlot"
QT_MOC_LITERAL(80, 1150, 12), // "addLightSlot"
QT_MOC_LITERAL(81, 1163, 14), // "resetLightSlot"
QT_MOC_LITERAL(82, 1178, 13), // "showXAxesSlot"
QT_MOC_LITERAL(83, 1192, 13), // "showYAxesSlot"
QT_MOC_LITERAL(84, 1206, 13), // "showZAxesSlot"
QT_MOC_LITERAL(85, 1220, 12), // "axesXPosSlot"
QT_MOC_LITERAL(86, 1233, 12), // "axesYPosSlot"
QT_MOC_LITERAL(87, 1246, 12), // "axesZPosSlot"
QT_MOC_LITERAL(88, 1259, 12), // "axesGridSlot"
QT_MOC_LITERAL(89, 1272, 11), // "infGridSlot"
QT_MOC_LITERAL(90, 1284, 14), // "mouseScaleSlot"
QT_MOC_LITERAL(91, 1299, 14), // "mouseBasisSlot"
QT_MOC_LITERAL(92, 1314, 13), // "objectVisSlot"
QT_MOC_LITERAL(93, 1328, 15), // "translationSlot"
QT_MOC_LITERAL(94, 1344, 12), // "rotationSlot"
QT_MOC_LITERAL(95, 1357, 9), // "scaleSlot"
QT_MOC_LITERAL(96, 1367, 15), // "objectColorSlot"
QT_MOC_LITERAL(97, 1383, 18), // "diffuseTextureSlot"
QT_MOC_LITERAL(98, 1402, 17), // "normalTextureSlot"
QT_MOC_LITERAL(99, 1420, 19), // "specularTextureSlot"
QT_MOC_LITERAL(100, 1440, 19), // "emissiveTextureSlot"
QT_MOC_LITERAL(101, 1460, 19), // "connectMaterialEdit"
QT_MOC_LITERAL(102, 1480, 8), // "swapSlot"
QT_MOC_LITERAL(103, 1489, 10), // "invertSlot"
QT_MOC_LITERAL(104, 1500, 14), // "addTextureSlot"
QT_MOC_LITERAL(105, 1515, 18), // "loadTextureMapSlot"
QT_MOC_LITERAL(106, 1534, 18), // "saveTextureMapSlot"
QT_MOC_LITERAL(107, 1553, 19), // "loadMaterialMapSlot"
QT_MOC_LITERAL(108, 1573, 19), // "saveMaterialMapSlot"
QT_MOC_LITERAL(109, 1593, 15), // "addMaterialSlot"
QT_MOC_LITERAL(110, 1609, 16), // "selectParentSlot"
QT_MOC_LITERAL(111, 1626, 15), // "selectFacesSlot"
QT_MOC_LITERAL(112, 1642, 16), // "selectPointsSlot"
QT_MOC_LITERAL(113, 1659, 17), // "selectRelatedSlot"
QT_MOC_LITERAL(114, 1677, 12), // "deselectSlot"
QT_MOC_LITERAL(115, 1690, 14), // "selectDumpSlot"
QT_MOC_LITERAL(116, 1705, 16), // "objectSelectSlot"
QT_MOC_LITERAL(117, 1722, 14), // "objectZoomSlot"
QT_MOC_LITERAL(118, 1737, 11), // "addConeSlot"
QT_MOC_LITERAL(119, 1749, 11), // "addCubeSlot"
QT_MOC_LITERAL(120, 1761, 15), // "addCylinderSlot"
QT_MOC_LITERAL(121, 1777, 14), // "addPyramidSlot"
QT_MOC_LITERAL(122, 1792, 13), // "addSphereSlot"
QT_MOC_LITERAL(123, 1806, 12), // "addTorusSlot"
QT_MOC_LITERAL(124, 1819, 13), // "loadModelSlot"
QT_MOC_LITERAL(125, 1833, 13), // "saveModelSlot"
QT_MOC_LITERAL(126, 1847, 19), // "currentMaterialSlot"
QT_MOC_LITERAL(127, 1867, 19), // "materialAmbientSlot"
QT_MOC_LITERAL(128, 1887, 19), // "materialDiffuseSlot"
QT_MOC_LITERAL(129, 1907, 20), // "materialSpecularSlot"
QT_MOC_LITERAL(130, 1928, 20), // "materialEmissionSlot"
QT_MOC_LITERAL(131, 1949, 21), // "materialShininessSlot"
QT_MOC_LITERAL(132, 1971, 24), // "materialTransparencySlot"
QT_MOC_LITERAL(133, 1996, 26), // "materialAmbientTextureSlot"
QT_MOC_LITERAL(134, 2023, 26), // "materialDiffuseTextureSlot"
QT_MOC_LITERAL(135, 2050, 25), // "materialNormalTextureSlot"
QT_MOC_LITERAL(136, 2076, 27), // "materialSpecularTextureSlot"
QT_MOC_LITERAL(137, 2104, 27), // "materialEmissiveTextureSlot"
QT_MOC_LITERAL(138, 2132, 24), // "currentObjectChangedSlot"
QT_MOC_LITERAL(139, 2157, 20), // "objectAutoSelectSlot"
QT_MOC_LITERAL(140, 2178, 22), // "overviewEqualScaleSlot"
QT_MOC_LITERAL(141, 2201, 21), // "overviewModelTypeSlot"
QT_MOC_LITERAL(142, 2223, 18), // "overviewCameraSlot"
QT_MOC_LITERAL(143, 2242, 18), // "overviewLightsSlot"
QT_MOC_LITERAL(144, 2261, 17), // "uvTextureTypeSlot"
QT_MOC_LITERAL(145, 2279, 17), // "uvTextureWrapSlot"
QT_MOC_LITERAL(146, 2297, 15), // "textureWrapSlot"
QT_MOC_LITERAL(147, 2313, 14), // "bonesModelSlot"
QT_MOC_LITERAL(148, 2328, 17), // "bonesBoneNodeSlot"
QT_MOC_LITERAL(149, 2346, 20), // "bonesPointJointsSlot"
QT_MOC_LITERAL(150, 2367, 19), // "bonesOnlyJointsSlot"
QT_MOC_LITERAL(151, 2387, 20), // "bonesTranslationSlot"
QT_MOC_LITERAL(152, 2408, 17), // "bonesRotationSlot"
QT_MOC_LITERAL(153, 2426, 14), // "bonesScaleSlot"
QT_MOC_LITERAL(154, 2441, 15), // "currentBoneSlot"
QT_MOC_LITERAL(155, 2457, 11), // "updateBones"
QT_MOC_LITERAL(156, 2469, 17), // "updateCurrentBone"
QT_MOC_LITERAL(157, 2487, 10), // "updateAnim"
QT_MOC_LITERAL(158, 2498, 9), // "timerSlot"
QT_MOC_LITERAL(159, 2508, 12), // "animNameSlot"
QT_MOC_LITERAL(160, 2521, 12), // "animTimeSlot"
QT_MOC_LITERAL(161, 2534, 1), // "t"
QT_MOC_LITERAL(162, 2536, 16), // "animTimeStepSlot"
QT_MOC_LITERAL(163, 2553, 8), // "playSlot"
QT_MOC_LITERAL(164, 2562, 9), // "pauseSlot"
QT_MOC_LITERAL(165, 2572, 8), // "stepSlot"
QT_MOC_LITERAL(166, 2581, 8) // "animStep"

    },
    "CQCamera3DControl\0viewTypeSlot\0\0"
    "updateWidgets\0updateTextureWidgets\0"
    "disconnect\0updateCameraWidgets\0"
    "mainTabSlot\0showWireframeSlot\0"
    "showSolidSlot\0showTexturedSlot\0"
    "showPointsSlot\0i\0wireframeColorSlot\0"
    "c\0quadViewSlot\0debugCameraSlot\0"
    "pointSizeSlot\0lineWidthSlot\0cursorSlot\0"
    "normalsPointsSlot\0normalsFacesSlot\0"
    "normalsSizeSlot\0r\0normalsColorSlot\0"
    "showBasisSlot\0showBBoxSlot\0bboxOrientSlot\0"
    "depthTestSlot\0cullSlot\0frontFaceSlot\0"
    "showCameraSlot\0showEyeLineSlot\0"
    "showPlanesSlot\0currentCameraSlot\0"
    "pitchSlot\0yawSlot\0rollSlot\0nearSlot\0"
    "farSlot\0xOriginSlot\0yOriginSlot\0"
    "zOriginSlot\0xSlot\0ySlot\0zSlot\0fovSlot\0"
    "eyeZ1Slot\0eyeZ2Slot\0aroundXSlot1\0"
    "aroundXSlot2\0aroundYSlot1\0aroundYSlot2\0"
    "aroundZSlot1\0aroundZSlot2\0aroundDeltaSlot\0"
    "d\0resetCameraSlot\0topCameraSlot\0"
    "sideCameraSlot\0frontCameraSlot\0"
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
    "resetLightSlot\0showXAxesSlot\0showYAxesSlot\0"
    "showZAxesSlot\0axesXPosSlot\0axesYPosSlot\0"
    "axesZPosSlot\0axesGridSlot\0infGridSlot\0"
    "mouseScaleSlot\0mouseBasisSlot\0"
    "objectVisSlot\0translationSlot\0"
    "rotationSlot\0scaleSlot\0objectColorSlot\0"
    "diffuseTextureSlot\0normalTextureSlot\0"
    "specularTextureSlot\0emissiveTextureSlot\0"
    "connectMaterialEdit\0swapSlot\0invertSlot\0"
    "addTextureSlot\0loadTextureMapSlot\0"
    "saveTextureMapSlot\0loadMaterialMapSlot\0"
    "saveMaterialMapSlot\0addMaterialSlot\0"
    "selectParentSlot\0selectFacesSlot\0"
    "selectPointsSlot\0selectRelatedSlot\0"
    "deselectSlot\0selectDumpSlot\0"
    "objectSelectSlot\0objectZoomSlot\0"
    "addConeSlot\0addCubeSlot\0addCylinderSlot\0"
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
    "overviewEqualScaleSlot\0overviewModelTypeSlot\0"
    "overviewCameraSlot\0overviewLightsSlot\0"
    "uvTextureTypeSlot\0uvTextureWrapSlot\0"
    "textureWrapSlot\0bonesModelSlot\0"
    "bonesBoneNodeSlot\0bonesPointJointsSlot\0"
    "bonesOnlyJointsSlot\0bonesTranslationSlot\0"
    "bonesRotationSlot\0bonesScaleSlot\0"
    "currentBoneSlot\0updateBones\0"
    "updateCurrentBone\0updateAnim\0timerSlot\0"
    "animNameSlot\0animTimeSlot\0t\0"
    "animTimeStepSlot\0playSlot\0pauseSlot\0"
    "stepSlot\0animStep"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCamera3DControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
     161,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  819,    2, 0x08 /* Private */,
       3,    0,  820,    2, 0x08 /* Private */,
       4,    1,  821,    2, 0x08 /* Private */,
       4,    0,  824,    2, 0x28 /* Private | MethodCloned */,
       6,    1,  825,    2, 0x08 /* Private */,
       6,    0,  828,    2, 0x28 /* Private | MethodCloned */,
       7,    1,  829,    2, 0x08 /* Private */,
       8,    1,  832,    2, 0x08 /* Private */,
       9,    1,  835,    2, 0x08 /* Private */,
      10,    1,  838,    2, 0x08 /* Private */,
      11,    1,  841,    2, 0x08 /* Private */,
      13,    1,  844,    2, 0x08 /* Private */,
      15,    1,  847,    2, 0x08 /* Private */,
      16,    1,  850,    2, 0x08 /* Private */,
      17,    1,  853,    2, 0x08 /* Private */,
      18,    1,  856,    2, 0x08 /* Private */,
      19,    0,  859,    2, 0x08 /* Private */,
      20,    1,  860,    2, 0x08 /* Private */,
      21,    1,  863,    2, 0x08 /* Private */,
      22,    1,  866,    2, 0x08 /* Private */,
      24,    1,  869,    2, 0x08 /* Private */,
      25,    1,  872,    2, 0x08 /* Private */,
      26,    1,  875,    2, 0x08 /* Private */,
      27,    1,  878,    2, 0x08 /* Private */,
      28,    1,  881,    2, 0x08 /* Private */,
      29,    1,  884,    2, 0x08 /* Private */,
      30,    1,  887,    2, 0x08 /* Private */,
      31,    1,  890,    2, 0x08 /* Private */,
      32,    1,  893,    2, 0x08 /* Private */,
      33,    1,  896,    2, 0x08 /* Private */,
      34,    0,  899,    2, 0x08 /* Private */,
      35,    1,  900,    2, 0x08 /* Private */,
      36,    1,  903,    2, 0x08 /* Private */,
      37,    1,  906,    2, 0x08 /* Private */,
      38,    1,  909,    2, 0x08 /* Private */,
      39,    1,  912,    2, 0x08 /* Private */,
      40,    1,  915,    2, 0x08 /* Private */,
      41,    1,  918,    2, 0x08 /* Private */,
      42,    1,  921,    2, 0x08 /* Private */,
      43,    1,  924,    2, 0x08 /* Private */,
      44,    1,  927,    2, 0x08 /* Private */,
      45,    1,  930,    2, 0x08 /* Private */,
      46,    1,  933,    2, 0x08 /* Private */,
      47,    1,  936,    2, 0x08 /* Private */,
      48,    1,  939,    2, 0x08 /* Private */,
      49,    0,  942,    2, 0x08 /* Private */,
      50,    0,  943,    2, 0x08 /* Private */,
      51,    0,  944,    2, 0x08 /* Private */,
      52,    0,  945,    2, 0x08 /* Private */,
      53,    0,  946,    2, 0x08 /* Private */,
      54,    0,  947,    2, 0x08 /* Private */,
      55,    1,  948,    2, 0x08 /* Private */,
      57,    0,  951,    2, 0x08 /* Private */,
      58,    0,  952,    2, 0x08 /* Private */,
      59,    0,  953,    2, 0x08 /* Private */,
      60,    0,  954,    2, 0x08 /* Private */,
      61,    0,  955,    2, 0x08 /* Private */,
      62,    1,  956,    2, 0x08 /* Private */,
      63,    1,  959,    2, 0x08 /* Private */,
      64,    1,  962,    2, 0x08 /* Private */,
      65,    1,  965,    2, 0x08 /* Private */,
      66,    1,  968,    2, 0x08 /* Private */,
      67,    1,  971,    2, 0x08 /* Private */,
      68,    1,  974,    2, 0x08 /* Private */,
      69,    1,  977,    2, 0x08 /* Private */,
      70,    0,  980,    2, 0x08 /* Private */,
      71,    1,  981,    2, 0x08 /* Private */,
      72,    0,  984,    2, 0x08 /* Private */,
      73,    1,  985,    2, 0x08 /* Private */,
      74,    1,  988,    2, 0x08 /* Private */,
      75,    1,  991,    2, 0x08 /* Private */,
      76,    1,  994,    2, 0x08 /* Private */,
      77,    0,  997,    2, 0x08 /* Private */,
      78,    1,  998,    2, 0x08 /* Private */,
      79,    1, 1001,    2, 0x08 /* Private */,
      80,    0, 1004,    2, 0x08 /* Private */,
      81,    0, 1005,    2, 0x08 /* Private */,
      82,    1, 1006,    2, 0x08 /* Private */,
      83,    1, 1009,    2, 0x08 /* Private */,
      84,    1, 1012,    2, 0x08 /* Private */,
      85,    1, 1015,    2, 0x08 /* Private */,
      86,    1, 1018,    2, 0x08 /* Private */,
      87,    1, 1021,    2, 0x08 /* Private */,
      88,    1, 1024,    2, 0x08 /* Private */,
      89,    1, 1027,    2, 0x08 /* Private */,
      90,    1, 1030,    2, 0x08 /* Private */,
      91,    1, 1033,    2, 0x08 /* Private */,
      92,    1, 1036,    2, 0x08 /* Private */,
      93,    0, 1039,    2, 0x08 /* Private */,
      94,    0, 1040,    2, 0x08 /* Private */,
      95,    0, 1041,    2, 0x08 /* Private */,
      96,    1, 1042,    2, 0x08 /* Private */,
      97,    0, 1045,    2, 0x08 /* Private */,
      98,    0, 1046,    2, 0x08 /* Private */,
      99,    0, 1047,    2, 0x08 /* Private */,
     100,    0, 1048,    2, 0x08 /* Private */,
     101,    0, 1049,    2, 0x08 /* Private */,
     102,    0, 1050,    2, 0x08 /* Private */,
     103,    0, 1051,    2, 0x08 /* Private */,
     104,    0, 1052,    2, 0x08 /* Private */,
     105,    0, 1053,    2, 0x08 /* Private */,
     106,    0, 1054,    2, 0x08 /* Private */,
     107,    0, 1055,    2, 0x08 /* Private */,
     108,    0, 1056,    2, 0x08 /* Private */,
     109,    0, 1057,    2, 0x08 /* Private */,
     110,    0, 1058,    2, 0x08 /* Private */,
     111,    0, 1059,    2, 0x08 /* Private */,
     112,    0, 1060,    2, 0x08 /* Private */,
     113,    0, 1061,    2, 0x08 /* Private */,
     114,    0, 1062,    2, 0x08 /* Private */,
     115,    0, 1063,    2, 0x08 /* Private */,
     116,    0, 1064,    2, 0x08 /* Private */,
     117,    0, 1065,    2, 0x08 /* Private */,
     118,    0, 1066,    2, 0x08 /* Private */,
     119,    0, 1067,    2, 0x08 /* Private */,
     120,    0, 1068,    2, 0x08 /* Private */,
     121,    0, 1069,    2, 0x08 /* Private */,
     122,    0, 1070,    2, 0x08 /* Private */,
     123,    0, 1071,    2, 0x08 /* Private */,
     124,    0, 1072,    2, 0x08 /* Private */,
     125,    0, 1073,    2, 0x08 /* Private */,
     126,    0, 1074,    2, 0x08 /* Private */,
     127,    1, 1075,    2, 0x08 /* Private */,
     128,    1, 1078,    2, 0x08 /* Private */,
     129,    1, 1081,    2, 0x08 /* Private */,
     130,    1, 1084,    2, 0x08 /* Private */,
     131,    1, 1087,    2, 0x08 /* Private */,
     132,    1, 1090,    2, 0x08 /* Private */,
     133,    0, 1093,    2, 0x08 /* Private */,
     134,    0, 1094,    2, 0x08 /* Private */,
     135,    0, 1095,    2, 0x08 /* Private */,
     136,    0, 1096,    2, 0x08 /* Private */,
     137,    0, 1097,    2, 0x08 /* Private */,
     138,    0, 1098,    2, 0x08 /* Private */,
     139,    1, 1099,    2, 0x08 /* Private */,
     140,    1, 1102,    2, 0x08 /* Private */,
     141,    1, 1105,    2, 0x08 /* Private */,
     142,    1, 1108,    2, 0x08 /* Private */,
     143,    1, 1111,    2, 0x08 /* Private */,
     144,    1, 1114,    2, 0x08 /* Private */,
     145,    1, 1117,    2, 0x08 /* Private */,
     146,    1, 1120,    2, 0x08 /* Private */,
     147,    1, 1123,    2, 0x08 /* Private */,
     148,    1, 1126,    2, 0x08 /* Private */,
     149,    1, 1129,    2, 0x08 /* Private */,
     150,    1, 1132,    2, 0x08 /* Private */,
     151,    0, 1135,    2, 0x08 /* Private */,
     152,    0, 1136,    2, 0x08 /* Private */,
     153,    0, 1137,    2, 0x08 /* Private */,
     154,    0, 1138,    2, 0x08 /* Private */,
     155,    0, 1139,    2, 0x08 /* Private */,
     156,    0, 1140,    2, 0x08 /* Private */,
     157,    0, 1141,    2, 0x08 /* Private */,
     158,    0, 1142,    2, 0x08 /* Private */,
     159,    0, 1143,    2, 0x08 /* Private */,
     160,    1, 1144,    2, 0x08 /* Private */,
     162,    1, 1147,    2, 0x08 /* Private */,
     163,    0, 1150,    2, 0x08 /* Private */,
     164,    0, 1151,    2, 0x08 /* Private */,
     165,    0, 1152,    2, 0x08 /* Private */,
     166,    0, 1153,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QColor,   14,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::QColor,   14,
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
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   56,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Int,   12,
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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int,    2,
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
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void, QMetaType::Double,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  161,
    QMetaType::Void, QMetaType::Double,  161,
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
        case 0: _t->viewTypeSlot(); break;
        case 1: _t->updateWidgets(); break;
        case 2: _t->updateTextureWidgets((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updateTextureWidgets(); break;
        case 4: _t->updateCameraWidgets((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->updateCameraWidgets(); break;
        case 6: _t->mainTabSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->showWireframeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->showSolidSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->showTexturedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->showPointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->wireframeColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 12: _t->quadViewSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->debugCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->pointSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->lineWidthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->cursorSlot(); break;
        case 17: _t->normalsPointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->normalsFacesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->normalsSizeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->normalsColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 21: _t->showBasisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->showBBoxSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->bboxOrientSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->depthTestSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->cullSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: _t->frontFaceSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->showCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->showEyeLineSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->showPlanesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->currentCameraSlot(); break;
        case 31: _t->pitchSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 32: _t->yawSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 33: _t->rollSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 34: _t->nearSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 35: _t->farSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 36: _t->xOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 37: _t->yOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 38: _t->zOriginSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 39: _t->xSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 40: _t->ySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 41: _t->zSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 42: _t->fovSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 43: _t->eyeZ1Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 44: _t->eyeZ2Slot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 45: _t->aroundXSlot1(); break;
        case 46: _t->aroundXSlot2(); break;
        case 47: _t->aroundYSlot1(); break;
        case 48: _t->aroundYSlot2(); break;
        case 49: _t->aroundZSlot1(); break;
        case 50: _t->aroundZSlot2(); break;
        case 51: _t->aroundDeltaSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 52: _t->resetCameraSlot(); break;
        case 53: _t->topCameraSlot(); break;
        case 54: _t->sideCameraSlot(); break;
        case 55: _t->frontCameraSlot(); break;
        case 56: _t->currentLightSlot(); break;
        case 57: _t->ambientColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 58: _t->ambientStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 59: _t->diffuseStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 60: _t->emissiveStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 61: _t->specularStrengthSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 62: _t->fixedDiffuseSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 63: _t->lightTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 64: _t->lightEnabledSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 65: _t->lightPositionSlot(); break;
        case 66: _t->lightColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 67: _t->lightDirectionSlot(); break;
        case 68: _t->lightPointRadiusSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 69: _t->lightConstantAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 70: _t->lightLinearAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 71: _t->lightQuadraticAttenuationSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 72: _t->lightSpotDirectionSlot(); break;
        case 73: _t->lightSpotExponentSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 74: _t->lightCutOffAngleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 75: _t->addLightSlot(); break;
        case 76: _t->resetLightSlot(); break;
        case 77: _t->showXAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 78: _t->showYAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 79: _t->showZAxesSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 80: _t->axesXPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 81: _t->axesYPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 82: _t->axesZPosSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 83: _t->axesGridSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 84: _t->infGridSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 85: _t->mouseScaleSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 86: _t->mouseBasisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 87: _t->objectVisSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 88: _t->translationSlot(); break;
        case 89: _t->rotationSlot(); break;
        case 90: _t->scaleSlot(); break;
        case 91: _t->objectColorSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 92: _t->diffuseTextureSlot(); break;
        case 93: _t->normalTextureSlot(); break;
        case 94: _t->specularTextureSlot(); break;
        case 95: _t->emissiveTextureSlot(); break;
        case 96: _t->connectMaterialEdit(); break;
        case 97: _t->swapSlot(); break;
        case 98: _t->invertSlot(); break;
        case 99: _t->addTextureSlot(); break;
        case 100: _t->loadTextureMapSlot(); break;
        case 101: _t->saveTextureMapSlot(); break;
        case 102: _t->loadMaterialMapSlot(); break;
        case 103: _t->saveMaterialMapSlot(); break;
        case 104: _t->addMaterialSlot(); break;
        case 105: _t->selectParentSlot(); break;
        case 106: _t->selectFacesSlot(); break;
        case 107: _t->selectPointsSlot(); break;
        case 108: _t->selectRelatedSlot(); break;
        case 109: _t->deselectSlot(); break;
        case 110: _t->selectDumpSlot(); break;
        case 111: _t->objectSelectSlot(); break;
        case 112: _t->objectZoomSlot(); break;
        case 113: _t->addConeSlot(); break;
        case 114: _t->addCubeSlot(); break;
        case 115: _t->addCylinderSlot(); break;
        case 116: _t->addPyramidSlot(); break;
        case 117: _t->addSphereSlot(); break;
        case 118: _t->addTorusSlot(); break;
        case 119: _t->loadModelSlot(); break;
        case 120: _t->saveModelSlot(); break;
        case 121: _t->currentMaterialSlot(); break;
        case 122: _t->materialAmbientSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 123: _t->materialDiffuseSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 124: _t->materialSpecularSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 125: _t->materialEmissionSlot((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 126: _t->materialShininessSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 127: _t->materialTransparencySlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 128: _t->materialAmbientTextureSlot(); break;
        case 129: _t->materialDiffuseTextureSlot(); break;
        case 130: _t->materialNormalTextureSlot(); break;
        case 131: _t->materialSpecularTextureSlot(); break;
        case 132: _t->materialEmissiveTextureSlot(); break;
        case 133: _t->currentObjectChangedSlot(); break;
        case 134: _t->objectAutoSelectSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 135: _t->overviewEqualScaleSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 136: _t->overviewModelTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 137: _t->overviewCameraSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 138: _t->overviewLightsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 139: _t->uvTextureTypeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 140: _t->uvTextureWrapSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 141: _t->textureWrapSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 142: _t->bonesModelSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 143: _t->bonesBoneNodeSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 144: _t->bonesPointJointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 145: _t->bonesOnlyJointsSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 146: _t->bonesTranslationSlot(); break;
        case 147: _t->bonesRotationSlot(); break;
        case 148: _t->bonesScaleSlot(); break;
        case 149: _t->currentBoneSlot(); break;
        case 150: _t->updateBones(); break;
        case 151: _t->updateCurrentBone(); break;
        case 152: _t->updateAnim(); break;
        case 153: _t->timerSlot(); break;
        case 154: _t->animNameSlot(); break;
        case 155: _t->animTimeSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 156: _t->animTimeStepSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 157: _t->playSlot(); break;
        case 158: _t->pauseSlot(); break;
        case 159: _t->stepSlot(); break;
        case 160: _t->animStep(); break;
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
        if (_id < 161)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 161;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 161)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 161;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
