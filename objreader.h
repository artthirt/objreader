#ifndef OBJREADER_H
#define OBJREADER_H

#include <vector>
#include <string>
#include <functional>

#include "geomutils.h"

class ObjReader
{
public:
    typedef std::function<void(double)> call_progress;

    ObjReader();

    double progress() const {  return mProgress;  }

    bool loadObject(const std::string& fileName, Objects* objs);

    void setCallProgress(call_progress fun);

private:
    double mProgress = 0;
    call_progress mCallProgress;

    void set_progress(double v);
};

#endif // OBJREADER_H
