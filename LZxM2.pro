TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        LZxM00Pack.cpp \
        bit_buff.cpp \
        filters.cpp \
        main.cpp \
        rnd.cpp

HEADERS += \
    LZxM00Pack.h \
    bit_buff.h \
    defs.h \
    filters.h \
    rnd.h
