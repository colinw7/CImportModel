APPNAME = CQNewGLModel

BUILD_DIR = $$PWD

include($$(MAKE_DIR)/qt_app.mk)

QMAKE_CXXFLAGS += \
-DBUILD_DIR=\"$$BUILD_DIR\"

QT += opengl

SOURCES += \
CQNewGLModel.cpp \
CQNewGLCanvas.cpp \
CQNewGLControl.cpp \

HEADERS += \
CQNewGLModel.h \
CQNewGLCanvas.h \
CQNewGLControl.h \

INCLUDEPATH += \
$(INC_DIR)/CImportModel \
$(INC_DIR)/CGeometry3D \
$(INC_DIR)/CMaterial \
$(INC_DIR)/CGLTexture \
$(INC_DIR)/CQGLUtil \
$(INC_DIR)/CQApp \
$(INC_DIR)/CQUtil \
$(INC_DIR)/CQSlider \
$(INC_DIR)/CQColorEdit \
$(INC_DIR)/CImageLib \
$(INC_DIR)/CFont \
$(INC_DIR)/CFile \
$(INC_DIR)/CFileType \
$(INC_DIR)/CMath \
$(INC_DIR)/CStrUtil \
$(INC_DIR)/CUtil \
$(INC_DIR)/COS \

PRE_TARGETDEPS = \
$(LIB_DIR)/libCImportModel.a \
$(LIB_DIR)/libCGeometry3D.a \
$(LIB_DIR)/libCQSlider.a \

unix:LIBS += \
-L$(LIB_DIR) \
$$QT_APP_LIBS \
-lCGLUtil -lCImportModel -lCGeometry3D -lCGLTexture -lCQGLUtil \
-lCQSlider -lCQColorEdit -lCQRotatedText -lCQWidgetMenu \
-lCImageLib -lCJson -lCFile -lCFileParse -lCFileUtil -lCTempFile \
-lCMath -lCStrParse -lCStrUtil -lCPrintF -lCRegExp -lCRGBUtil -lCUtil -lCOS \
-lglut -lGLU \
-ljpeg -lpng -ltre
