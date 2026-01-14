BUILD_DIR = $$PWD

TEMPLATE = app

TARGET = CQCamera3D

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DBUILD_DIR=\"$$BUILD_DIR\"

MOC_DIR = .moc

CONFIG += c++17
CONFIG += debug
CONFIG += silent
CONFIG -= app_bundle

SOURCES += \
CQCamera3DAnnotation.cpp \
CQCamera3DApp.cpp \
CQCamera3DAxes.cpp \
CQCamera3DBasis.cpp \
CQCamera3DBBox.cpp \
CQCamera3DBillboard.cpp \
CQCamera3DBones.cpp \
CQCamera3DCanvas.cpp \
CQCamera3DControl.cpp \
CQCamera3DFont.cpp \
CQCamera3DLight.cpp \
CQCamera3DMain.cpp \
CQCamera3DMaterials.cpp \
CQCamera3DNormals.cpp \
CQCamera3DObject.cpp \
CQCamera3DObjectData.cpp \
CQCamera3DOverlay2D.cpp \
CQCamera3DOverlay.cpp \
CQCamera3DOverview.cpp \
CQCamera3DShaderProgram.cpp \
CQCamera3DShape.cpp \
CQCamera3DShapes.cpp \
CQCamera3DSidebar.cpp \
CQCamera3DStatus.cpp \
CQCamera3DToolbar.cpp \
CQCamera3DUVMap.cpp \
CQCamera3DTextures.cpp \
CQCamera3DWidget.cpp \
\
CQCamera3DGeomObject.cpp \
CQCamera3DGeomFace.cpp \
\
CQCamera3DAnimChooser.cpp \
CQCamera3DMaterialChooser.cpp \
CQCamera3DTextureChooser.cpp \
\
CQCamera3DBonesList.cpp \
CQCamera3DCameraList.cpp \
CQCamera3DLightList.cpp \
CQCamera3DMaterialList.cpp \
CQCamera3DObjectsList.cpp \
CQCamera3DTextureList.cpp \
\
CQGLTexture.cpp \
CQGLUtil.cpp \
CQPoint3DEdit.cpp \
CQPoint4DEdit.cpp \
CQMatrix3D.cpp \
CQTextLabel.cpp \
CQTreeWidget.cpp \
CQRubberBand.cpp \

HEADERS += \
CDisplayRange2D.h \
CQCamera3DAnnotation.h \
CQCamera3DApp.h \
CQCamera3DAxes.h \
CQCamera3DBasis.h \
CQCamera3DBBox.h \
CQCamera3DBillboard.h \
CQCamera3DBones.h \
CQCamera3DCamera.h \
CQCamera3DCanvas.h \
CQCamera3DControl.h \
CQCamera3DFaceData.h \
CQCamera3DFont.h \
CQCamera3DLight.h \
CQCamera3DMain.h \
CQCamera3DMaterials.h \
CQCamera3DNormals.h \
CQCamera3DObject.h \
CQCamera3DObjectData.h \
CQCamera3DOverlay2D.h \
CQCamera3DOverlay.h \
CQCamera3DOverview.h \
CQCamera3DShaderProgram.h \
CQCamera3DShape.h \
CQCamera3DShapes.h \
CQCamera3DSidebar.h \
CQCamera3DStatus.h \
CQCamera3DTextures.h \
CQCamera3DToolbar.h \
CQCamera3DUVMap.h \
CQCamera3DUtil.h \
CQCamera3DWidget.h \
\
CQCamera3DGeomObject.h \
CQCamera3DGeomFace.h \
\
CQCamera3DAnimChooser.h \
CQCamera3DMaterialChooser.h \
CQCamera3DTextureChooser.h \
\
CQCamera3DBonesList.h \
CQCamera3DCameraList.h \
CQCamera3DLightList.h \
CQCamera3DMaterialList.h \
CQCamera3DObjectsList.h \
CQCamera3DTextureList.h \
\
CQGLUtil.cpp \
CQGLBuffer.h \
CQGLTexture.h \
CQPoint3DEdit.h \
CQPoint4DEdit.h \
CQMatrix3D.h \
CQTextLabel.h \
CQTreeWidget.h \
CQRubberBand.h \
\
CWindowRange2D.h \
stb/stb_truetype.h \

INCLUDEPATH += \
../../CImportModel/include \
../../CGeometry3D/include \
../../CMaterial/include \
../../CGLTexture/includ e\
../../CQGLUtil/include \
../../CQApp/include \
../../CQMetaEdit/include \
../../CQUtil/include \
../../CQSlider/include\
../../CQColorEdit/include \
../../CImageLib/include \
../../CFont/include \
../../CConfig/include \
../../CXML/include \
../../CFile/include \
../../CFileType/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \

PRE_TARGETDEPS = \
../../CImportModel/lib/libCImportModel.a \
../../CGeometry3D/lib/libCGeometry3D.a \
../../CQUtil/lib/libCQUtil.a \

unix:LIBS += \
-L../lib \
-L../../CQMetaEdit/lib \
-L../../CImportModel/lib \
-L../../CVoxel/lib \
-L../../CGeometry3D/lib \
-L../../CQUtil/lib \
-L../../CXML/lib \
-L../../CConfig/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CJson/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CUtil/lib \
-L../../CMath/lib \
-L../../CRGBName/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-lCQMetaEdit \
-lCImportModel -lCVoxel -lCGeometry3D -lCQUtil -lCXML -lCConfig \
-lCImageLib -lCFont -lCJson -lCFileUtil -lCFile -lCUtil \
-lCMath -lCRGBName -lCStrUtil -lCRegExp -lCOS \
-lglut -lGLU \
-ljpeg -lpng -ltre -lz
