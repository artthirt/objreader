#include "glutils.h"

#define SHOWGLERROR()  qDebug("line %d: glGetError=%d", __LINE__, glGetError())

#pragma comment(lib, "opengl32.lib")

GLObj::~GLObj()
{
}

void GLObj::init(OpenGLFunctions *self)
{
    mSelf = self;
}

////////////////////////////////////////

GLObjects::~GLObjects()
{
    clearBufferData();
}

Obj *GLObjects::getInstance()
{
    return new GLObj(this);
}

void GLObjects::init(OpenGLFunctions *self)
{
    mSelf = self;
}

GLObj *GLObjects::getGL(size_t index) const
{
    GLObj *o = dynamic_cast<GLObj*>(mObjs[index].get());
    return o;
}

void GLObjects::recalc()
{
    vectorvecf3 newpos;
    vectorvecf2 newtex;
    vectorvecf3 newnorm;
    newpos.reserve(pos.size());
    newtex.reserve(pos.size());
    newnorm.reserve(pos.size());
    size_t id = 0;
    for(auto obj: mObjs){
        vectorvecu newposidx;
        newposidx.reserve(obj->posidx.size());
        for(size_t i = 0; i < obj->posidx.size(); i += 3){
            unsigned *A = &obj->posidx[i];

            newpos.push_back(pos[A[0]]);
            newpos.push_back(pos[A[1]]);
            newpos.push_back(pos[A[2]]);

            if(obj->texidx.size()){
                unsigned *B = &obj->texidx[i];
                newtex.push_back(tex[B[0]]);
                newtex.push_back(tex[B[1]]);
                newtex.push_back(tex[B[2]]);
            }

            if(obj->normidx.size()){
                unsigned *C = &obj->normidx[i];
                newnorm.push_back(norm[C[0]]);
                newnorm.push_back(norm[C[1]]);
                newnorm.push_back(norm[C[2]]);
            }
            newposidx.push_back(id++);
            newposidx.push_back(id++);
            newposidx.push_back(id++);
        }
        obj->posidx = newposidx;

    }
    pos = newpos;
    tex = newtex;
    norm = newnorm;
}

void GLObjects::initBufferData()
{
    if(pos.size()){
        mSelf->glGenBuffers(1, &mVecBuf);
        mSelf->glBindBuffer(GL_ARRAY_BUFFER, mVecBuf);
        mSelf->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(Vec3f), pos.data(), GL_STATIC_DRAW);
    }

    if(tex.size()){
        mSelf->glGenBuffers(1, &mVtBuf);
        mSelf->glBindBuffer(GL_ARRAY_BUFFER, mVtBuf);
        mSelf->glBufferData(GL_ARRAY_BUFFER, tex.size() * sizeof(Vec2f), tex.data(), GL_STATIC_DRAW);
        //SHOWGLERROR();
    }

    if(norm.size()){
        mSelf->glGenBuffers(1, &mVnBuf);
        mSelf->glBindBuffer(GL_ARRAY_BUFFER, mVnBuf);
        mSelf->glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(Vec3f), norm.data(), GL_STATIC_DRAW);
        //SHOWGLERROR();
    }
}

void GLObjects::clearBufferData()
{
    if(mVecBuf){
        mSelf->glDeleteBuffers(1, &mVecBuf);
        mVecBuf = 0;
    }
    if(mVnBuf){
        mSelf->glDeleteBuffers(1, &mVnBuf);
        mVnBuf = 0;
    }
    if(mVtBuf){
        mSelf->glDeleteBuffers(1, &mVtBuf);
        mVtBuf = 0;
    }
}

void GLObjects::drawBufferData(Obj *obj, int v, int vn)
{
    mSelf->glBindBuffer(GL_ARRAY_BUFFER, mVecBuf);
    mSelf->glEnableVertexAttribArray(v);
    mSelf->glVertexAttribPointer(v, 3, GL_FLOAT, false, sizeof(Vec3f), nullptr);
    //SHOWGLERROR();

    if(obj->normidx.size()){
        mSelf->glBindBuffer(GL_ARRAY_BUFFER, mVnBuf);
        mSelf->glEnableVertexAttribArray(vn);
        mSelf->glVertexAttribPointer(vn, 3, GL_FLOAT, false, sizeof(Vec3f), nullptr);
        //SHOWGLERROR();
    }

    mSelf->glDrawElements(obj->type(), obj->posidx.size(), GL_UNSIGNED_INT, obj->posidx.data());
    //SHOWGLERROR();

    mSelf->glDisableVertexAttribArray(v);
    mSelf->glDisableVertexAttribArray(vn);
}
