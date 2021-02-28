#ifndef GLUTILS_H
#define GLUTILS_H

#include <QOpenGLFunctions_4_0_Core>

#include "geomutils.h"

typedef QOpenGLFunctions_4_0_Core OpenGLFunctions;

class GLObj: public Obj{
public:
    GLObj(Objects* parent): Obj(parent){ setType(GL_TRIANGLES); }
    ~GLObj();

    void init(OpenGLFunctions* self);

private:
    OpenGLFunctions* mSelf = nullptr;
    uint mFBuf = 0;
};

typedef std::shared_ptr<GLObj> PGLObj;

class GLObjects: public Objects{
public:
    virtual ~GLObjects();
    virtual Obj *getInstance();

    void init(OpenGLFunctions* self);
    void initBufferData();
    void clearBufferData();

    void drawBufferData(Obj* obj, int v, int vn);

    GLObj *getGL(size_t index) const;

    void recalc();
private:
    OpenGLFunctions* mSelf = nullptr;
    uint mVecBuf = 0;
    uint mVtBuf = 0;
    uint mVnBuf = 0;
    bool mInitObj = false;
};

#endif // GLUTILS_H
