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
CONFIG += silent

SOURCES += \
CQCamera3DAnnotation.cpp \
CQCamera3DApp.cpp \
CQCamera3DAxes.cpp \
CQCamera3DCanvas.cpp \
CQCamera3DControl.cpp \
CQCamera3DFont.cpp \
CQCamera3DMain.cpp \
CQCamera3DObject.cpp \
CQCamera3DOverlay2D.cpp \
CQCamera3DOverlay.cpp \
CQCamera3DOverview.cpp \
CQCamera3DShaderProgram.cpp \
CQCamera3DShape.cpp \
CQCamera3DShapes.cpp \
CQCamera3DStatus.cpp \
CQCamera3DTextureChooser.cpp \
\
CQGLTexture.cpp \
CQGLUtil.cpp \
CQPoint3DEdit.cpp \

HEADERS += \
CDisplayRange2D.h \
CQCamera3DAnnotation.h \
CQCamera3DApp.h \
CQCamera3DAxes.h \
CQCamera3DCamera.h \
CQCamera3DCanvas.h \
CQCamera3DControl.h \
CQCamera3DFaceData.h \
CQCamera3DFont.h \
CQCamera3DGeomObject.h \
CQCamera3DMain.h \
CQCamera3DObject.h \
CQCamera3DOverlay2D.h \
CQCamera3DOverlay.h \
CQCamera3DOverview.h \
CQCamera3DShaderProgram.h \
CQCamera3DShape.h \
CQCamera3DShapes.h \
CQCamera3DStatus.h \
CQCamera3DTextureChooser.h \
CQCamera3DUtil.h \
\
CQGLUtil.cpp \
CQGLBuffer.h \
CQGLTexture.h \
CQPoint3DEdit.h \
CWindowRange2D.h \
stb_truetype.h \

INCLUDEPATH += \
../../CImportModel/include \
../../CGeometry3D/include \
../../CMaterial/include \
../../CGLTexture/includ e\
../../CQGLUtil/include \
../../CQApp/include \
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
$(LIB_DIR)/libCImportModel.a \
$(LIB_DIR)/libCGeometry3D.a \
$(LIB_DIR)/libCQUtil.a \

unix:LIBS += \
-L../lib \
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
-lCImportModel -lCVoxel -lCGeometry3D -lCQUtil -lCXML -lCConfig \
-lCImageLib -lCFont -lCJson -lCFileUtil -lCFile -lCUtil \
-lCMath -lCRGBName -lCStrUtil -lCRegExp -lCOS \
-lglut -lGLU \
-ljpeg -lpng -ltre -lz
