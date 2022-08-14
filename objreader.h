#ifndef OBJREADER_H
#define OBJREADER_H

#include <vector>
#include <string>
#include <functional>

#include "geomutils.h"

#include <QIODevice>

class ObjReader
{
public:

    ObjReader();

    double progress() const {  return mProgress;  }

    bool loadObject(const std::string& fileName, Objects* objs);
    bool loadObject(QIODevice* device, Objects* objs);

    bool loadFromBuffer(const std::vector<char> &data, Objects *objs);

#ifdef __GNUC__
#  include <features.h>
#  if __GNUC_PREREQ(4,0)
//      If  gcc_version >= 4.0
    typedef std::function<void(double)> call_progress;
    void setCallProgress(call_progress fun);
#  endif
#else
#if defined(_MSC_VER) && _MSC_VER > 1600
    typedef std::function<void(double)> call_progress;
    void setCallProgress(call_progress fun);
#endif
#endif

private:
    double mProgress;
#ifdef __GNUC__
#  include <features.h>
#  if __GNUC_PREREQ(4,0)
//      If  gcc_version >= 4.0
    call_progress mCallProgress;
#endif
#else
#if _MSC_VER > 1600
    call_progress mCallProgress;
#endif
#endif
    void set_progress(double v);
};

#endif // OBJREADER_H
