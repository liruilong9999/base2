#ifndef translate_h
#define translate_h

#include <string>
#include <QString>
#include <cstring> // for strdup
char * QString2Char(QString str)
{
    return strdup(str.toUtf8().data());
}

#endif