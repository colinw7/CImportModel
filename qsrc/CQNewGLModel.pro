BUILD_DIR = $$PWD

TEMPLATE = app

TARGET = CQNewGLModel

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DBUILD_DIR=\"$$BUILD_DIR\"

MOC_DIR = .moc

CONFIG += c++17
CONFIG += silent

SOURCES += \
CQNewGLMain.cpp \
CQNewGLModel.cpp \
CQNewGLModelObject.cpp \
CQNewGLCanvas.cpp \
CQNewGLFont.cpp \
CQNewGLShaderProgram.cpp \
CQNewGLToolbar.cpp \
CQNewGLStatusBar.cpp \
CQNewGLControl.cpp \
CQNewGLUVMap.cpp \
CQNewGLCamera.cpp \
CQNewGLLight.cpp \
CQNewGLAxes.cpp \
CQNewGLNormals.cpp \
CQNewGLWireframe.cpp \
CQNewGLBBox.cpp \
CQNewGLHull.cpp \
CQNewGLBasis.cpp \
CQNewGLTerrain.cpp \
CQNewGLMaze.cpp \
CQNewGLSkybox.cpp \
CQNewGLEmitter.cpp \
CQNewGLFractal.cpp \
CQNewGLDrawTree.cpp \
CQNewGLShape.cpp \
CQNewGLPath.cpp \
CQNewGLObject.cpp \
\
CQGLCubemap.cpp \
CQGLTexture.cpp \
CQGLUtil.cpp \
CQPoint3DEdit.cpp \
CQRotatedText.cpp \
CQMatrix3D.cpp \
\
CGLCamera.cpp \
CGLTexture.cpp \
\
CFlocking.cpp \
CFlock.cpp \
CBoid.cpp \
\
CFireworks.cpp \
\
CDungeon.cpp \
CDungeonXML.cpp \
\
CDrawTree3D.cpp \
CTurtle3D.cpp \
\
CWaterSurface.cpp \
\
CLorenzCalc.cpp \
\
CSolidNoise.cpp \

HEADERS += \
CQNewGLModel.h \
CQNewGLModelObject.h \
CQNewGLCanvas.h \
CQNewGLFont.h \
CQNewGLShaderProgram.h \
CQNewGLToolbar.h \
CQNewGLStatusBar.h \
CQNewGLControl.h \
CQNewGLUVMap.h \
CQNewGLCamera.h \
CQNewGLLight.h \
CQNewGLAxes.h \
CQNewGLNormals.h \
CQNewGLWireframe.h \
CQNewGLBBox.h \
CQNewGLHull.h \
CQNewGLBasis.h \
CQNewGLTerrain.h \
CQNewGLMaze.h \
CQNewGLSkybox.h \
CQNewGLEmitter.h \
CQNewGLFractal.h \
CQNewGLDrawTree.h \
CQNewGLShape.h \
CQNewGLPath.h \
CQNewGLObject.h \
\
CQGLCubemap.h \
CQGLTexture.h \
CQGLUtil.h \
CQPoint3DEdit.h \
CQRotatedText.h \
CQMatrix3D.h \
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
