TEMPLATE = app

TARGET = CQNewGLModel

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \

MOC_DIR = .moc

CONFIG += c++17
CONFIG += silent

SOURCES += \
CQNewGLModelMain.cpp \
CQNewGLModel.cpp \
CQNewGLCanvas.cpp \
CQNewGLControl.cpp \
CQNewGLToolbar.cpp \
\
CQGLUtil.cpp \
CQPoint3DEdit.cpp \
CQRotatedText.cpp \
\
CGLCamera.cpp \
CGLTexture.cpp \

HEADERS += \
CQNewGLModel.h \
CQNewGLCanvas.h \
CQNewGLControl.h \
CQNewGLToolbar.h \
\
CQGLUtil.h \
CQPoint3DEdit.h \
CQRotatedText.h \
\
CGLCamera.h \
CGLTexture.h \

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
-L../../CGeometry3D/lib \
-L../../CQUtil/lib \
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
-lCImportModel -lCGeometry3D -lCQUtil -lCConfig \
-lCImageLib -lCFont -lCJson -lCFileUtil -lCFile -lCUtil \
-lCMath -lCRGBName -lCStrUtil -lCRegExp -lCOS \
-lglut -lGLU \
-ljpeg -lpng -ltre -lz
