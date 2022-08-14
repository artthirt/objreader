#include "geomutils.h"


bool Obj::empty() const
{
    return posidx.empty();
}

void Obj::clear()
{
    posidx.clear();
    texidx.clear();
    normidx.clear();
}

uint32_t Obj::type() const{
    return mType;
}

void Obj::setType(uint32_t tp){
    mType = tp;
}

/////////////////////////////////

Objects::Objects()
    : mId(0)
{

}

Objects::~Objects()
{

}

Obj *Objects::getInstance()
{
    return new Obj(this);
}

size_t Objects::count() const
{
    return mObjs.size();
}

bool Objects::empty() const
{
    return mObjs.empty();
}

void Objects::push(Obj *obj)
{
    PObj o;
    o.reset(obj);
    mObjs.push_back(o);
}

Obj *Objects::get(size_t index)
{
    return mObjs[index].get();
}

void Objects::clear()
{
    mObjs.clear();
    pos.clear();
    tex.clear();
    norm.clear();
}

