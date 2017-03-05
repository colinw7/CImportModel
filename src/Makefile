CPP_CC = g++
C_CC   = gcc

AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(LIB_DIR)/libCImportModel.a

CPP_SRC = \
CGeomImportASC.cpp \
CGeomImportCOB.cpp \
CGeomImportPLG.cpp \
CImport3DRW.cpp \
CImport3DS.cpp \
CImportASC.cpp \
CImportCOB.cpp \
CImportDXF.cpp \
CImportObj.cpp \
CImportPLG.cpp \
CImportScene.cpp \
CImportV3D.cpp \
CImportX3D.cpp \
\
CGeomAxes3D.cpp \
CGeomBox3D.cpp \
CGeomCamera3D.cpp \
CGeomCone3D.cpp \
CGeomCube3D.cpp \
CGeomCylinder3D.cpp \
CGeometry3D.cpp \
CGeomFace3D.cpp \
CGeomFill3D.cpp \
CGeomLight3D.cpp \
CGeomLine3D.cpp \
CGeomObject3D.cpp \
CGeomParticle3D.cpp \
CGeomPoint3D.cpp \
CGeomPyramid3D.cpp \
CGeomScene3D.cpp \
CGeomSphere3D.cpp \
CGeomTorus3D.cpp \
CGeomTriangle3D.cpp \
CGeomUtil3D.cpp \
CGeomVertex3D.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CPPFLAGS = \
-std=c++11 \
$(CDEBUG) \
-I$(INC_DIR) \
-I../../CImportModel/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CRGBName/include \
-I../../CStrUtil/include \
-I../../CRegExp/include \
-I../../COS/include \
-I../../CUtil/include \
-I. \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCImportModel.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(LIB_DIR)/libCImportModel.a: $(CPP_OBJS) $(C_OBJS)
	$(AR) crv $(LIB_DIR)/libCImportModel.a $(CPP_OBJS) $(C_OBJS)
