#ifndef OBJREADER_H
#define OBJREADER_H

#include <vector>
#include <string>

#include "geomutils.h"

class ObjReader
{
public:
    ObjReader();

    bool loadObject(const std::string& fileName, Objects* objs);

private:

};

#endif // OBJREADER_H
