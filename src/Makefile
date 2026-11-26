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
CImport3MF.cpp \
CImportASC.cpp \
CImportBlend.cpp \
CImportCOB.cpp \
CImportDAE.cpp \
CImportDXF.cpp \
CImportFBX.cpp \
CImportGLTF.cpp \
CImportObj.cpp \
CImportPLG.cpp \
CImportPly.cpp \
CImportScene.cpp \
CImportSTL.cpp \
CImportV3D.cpp \
CImportVoxel.cpp \
CImportX3D.cpp \
CImportBase.cpp \
CDeflate.cpp \
CSG.cpp \

C_SRC = \
miniz.c \
miniz_tdef.c \
miniz_tinfl.c \
miniz_zip.c \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

C_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRC))

CPPFLAGS = \
-std=c++17 \
$(CDEBUG) \
-I$(INC_DIR) \
-I../../CImportModel/include \
-I../../CGeometry3D/include \
-I../../CVoxel/include \
-I../../CXML/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CJson/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CRGBName/include \
-I../../CStrUtil/include \
-I../../CRegExp/include \
-I../../COS/include \
-I../../CUtil/include \
-I. \

CFLAGS = \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCImportModel.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(C_OBJS): $(OBJ_DIR)/%.o: %.c
	$(C_CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(LIB_DIR)/libCImportModel.a: $(CPP_OBJS) $(C_OBJS)
	$(AR) crv $(LIB_DIR)/libCImportModel.a $(CPP_OBJS) $(C_OBJS)
