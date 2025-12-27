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
CQNewGLAxes.cpp \
CQNewGLBasis.cpp \
CQNewGLBBox.cpp \
CQNewGLBones.cpp \
CQNewGLCamera.cpp \
CQNewGLCanvas.cpp \
CQNewGLControl.cpp \
CQNewGLDrawTree.cpp \
CQNewGLEmitter.cpp \
CQNewGLFont.cpp \
CQNewGLFractal.cpp \
CQNewGLFractalWidget.cpp \
CQNewGLHull.cpp \
CQNewGLLight.cpp \
CQNewGLMain.cpp \
CQNewGLMaze.cpp \
CQNewGLModel.cpp \
CQNewGLModelObject.cpp \
CQNewGLNormals.cpp \
CQNewGLObject.cpp \
CQNewGLPath.cpp \
CQNewGLShaderProgram.cpp \
CQNewGLShape.cpp \
CQNewGLShapes.cpp \
CQNewGLSkybox.cpp \
CQNewGLStatusBar.cpp \
CQNewGLTerrain.cpp \
CQNewGLToolbar.cpp \
CQNewGLUVMap.cpp \
CQNewGLWidget.cpp \
CQNewGLWireframe.cpp \
\
CQNewGLBonesList.cpp \
CQNewGLCamerasList.cpp \
CQNewGLLightsList.cpp \
CQNewGLObjectsList.cpp \
CQNewGLShapesList.cpp \
\
CQNewGLAnimChooser.cpp \
CQNewGLTextureChooser.cpp \
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
CQNewGLAxes.h \
CQNewGLBasis.h \
CQNewGLBBox.h \
CQNewGLBones.h \
CQNewGLCamera.h \
CQNewGLCanvas.h \
CQNewGLControl.h \
CQNewGLDrawTree.h \
CQNewGLEmitter.h \
CQNewGLFaceData.h \
CQNewGLFont.h \
CQNewGLFractal.h \
CQNewGLFractalWidget.h \
CQNewGLHull.h \
CQNewGLLight.h \
CQNewGLMaze.h \
CQNewGLModel.h \
CQNewGLModelObject.h \
CQNewGLNormals.h \
CQNewGLObject.h \
CQNewGLPath.h \
CQNewGLShaderProgram.h \
CQNewGLShape.h \
CQNewGLShapes.h \
CQNewGLSkybox.h \
CQNewGLStatusBar.h \
CQNewGLTerrain.h \
CQNewGLToolbar.h \
CQNewGLUtil.h \
CQNewGLUVMap.h \
CQNewGLWidget.h \
CQNewGLWireframe.h \
\
CQNewGLBonesList.h \
CQNewGLCamerasList.h \
CQNewGLLightsList.h \
CQNewGLObjectsList.h \
CQNewGLShapesList.h \
\
CQNewGLAnimChooser.h \
CQNewGLTextureChooser.h \
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
