INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/lib -lopcuad
} else {
    LIBS += -L$$PWD/lib -lopcua
}
