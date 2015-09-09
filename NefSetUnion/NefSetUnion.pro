DEFINES += CGAL_EIGEN3_ENABLED
QMAKE_CXXFLAGS += -frounding-math

INCLUDEPATH += /usr/include/eigen3   \
                             /home/sway/myLibs/CGAL-4.2/include


LIBS +=    -lboost_thread   -lboost_program_options  -lboost_filesystem  -lboost_system\
                 -L/usr/lib/x86_64-linux-gnu/ -lgmp -lmpfr  \
                -L/home/sway/myLibs/CGAL-4.2/build/lib/ -lCGAL  -lCGAL_Core

SOURCES += \
    main.cpp

HEADERS +=

