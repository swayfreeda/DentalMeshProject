SOURCES += \
    MeshFix/meshfix.cpp \
    MeshFix/OpenNL3.2.1/src/NL/nl_context.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_superlu.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_preconditioners.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_os.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_matrix.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_iterative_solvers.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_blas.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_api.c \
    MeshFix/OpenNL3.2.1/src/NL/nl_cnc_gpu_cuda.c

QMAKE_CXXFLAGS += -frounding-math

INCLUDEPATH += /home/sway/MeshFixProj/MeshFix/JMeshExt-1.0alpha_src/include  \
               /home/sway/MeshFixProj/MeshFix/JMeshLib-1.2/include          \
               /home/sway/MeshFixProj/MeshFix/OpenNL3.2.1/src

LIBS += -L/home/sway/MeshFixProj/MeshFix/JMeshExt-1.0alpha_src/lib/ -ljmeshext  \
        -L/home/sway/MeshFixProj/MeshFix/JMeshLib-1.2/lib/ -ljmesh

DEFINES += IS64BITPLATFORM
