#include "objreader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <functional>

typedef std::vector< std::string > ListString;
typedef std::vector< std::vector<char> > ListVector;
typedef std::vector<char*> ListPChar;
typedef std::vector< int > ListInt;

typedef std::function<void(double)> signal_progress;

void split(const std::string& text, const std::string &ref, ListString& res)
{
    res.clear();
    size_t pos = 0, prev = 0;
    while((pos = text.find(ref, prev)) != text.npos){
        res.push_back(text.substr(prev, pos - prev));
        pos += ref.length();
        prev = pos;
    }
    res.push_back(text.substr(prev));
}

void split(const char* text, const char *end, char ref, ListString& res)
{
    res.clear();
    char *buf = (char*)text;
    char *prev = buf;
    while(buf != end){
        if(*buf == ref){
            res.push_back(std::string(prev, buf - prev));
            prev = buf;
        }
        buf++;
    }
    if(buf != prev){
        res.push_back(std::string(prev, buf - prev));
    }
}

void split(const char* text, const char *end, char ref, ListPChar& res)
{
    res.clear();
    char *buf = const_cast<char*>(text);
    char *prev = buf;
    res.push_back(const_cast<char*>(text));
    while(buf != end){
        if(*buf == ref){
            res.push_back(buf);
            prev = buf;
        }
        buf++;
    }
    res.push_back(const_cast<char*>(end));
}

void split(const std::string& text, char ref, ListString& res)
{
    res.clear();
    size_t pos = 0, prev = 0;
    while((pos = text.find(ref, prev)) != text.npos){
        res.push_back(text.substr(prev, pos - prev));
        pos += 1;
        prev = pos;
    }
    res.push_back(text.substr(prev));
}

void vsplit(const std::vector<char>& text, char ref, std::vector<int> &posdel, signal_progress fun = nullptr)
{
    posdel.clear();

    posdel.push_back(-1);
    for(size_t i = 0; i < text.size(); ++i){
        if(text[i] == ref){
            posdel.push_back(i);
        }
        if((i % 50000) == 0){
            if(fun) fun(1. * i / text.size());
            std::cout << "1 progress " << 1. * i / text.size() << "          \r";
        }
    }
    if(posdel.back() != text.size() - 1){
        posdel.push_back(text.size());
    }
}

template< typename T >
void getVTN(const char *begin, const char* end, T &A, T &B, T&C)
{
    int res[64] = {0};

    char* f = const_cast<char*>(begin);
    char* prev = f;
    int id = 0;
    while(f != end){
        if(*f == '/'){
            res[id++] = std::atoi(prev);
            prev = f + 1;
        }
        f++;
    }
    res[id++] = std::atoi(prev);
    A = res[0];
    B = res[1];
    C = res[2];
}

bool getResO(const ListPChar& pc, const std::string& beg, std::string& res)
{
    if(pc.size() < 3)
        return false;
    int len = pc[1] - pc[0];
    if(len != beg.size())
        return false;
    char* b0 = pc[0];
    char* b1 = const_cast<char*>(beg.c_str());
    while(b0 != pc[1]){
        if(*b0 != *b1) return false;
        b0++; b1++;
    }

    res = std::string(pc[1], pc[2] - pc[1]);
    return true;
}

template< typename T>
bool getRes(const ListPChar& pc, const std::string& beg, T& res)
{
    if(pc.size() < 2)
        return false;
    int len = pc[1] - pc[0];
    if(len != beg.size())
        return false;
    char* b0 = pc[0];
    char* b1 = const_cast<char*>(beg.c_str());
    while(b0 != pc[1]){
        if(*b0 != *b1) return false;
        b0++; b1++;
    }

    for(size_t i = 1; i < pc.size() - 1; ++i){
        res[i - 1] = std::atof(pc[i]);
    }
    return true;
}

template< typename T>
bool getRes(const ListPChar& pc, const std::string& beg, T& res1, T& res2, T& res3)
{
    if(pc.size() < 2)
        return false;
    int len = pc[1] - pc[0];
    if(len != beg.size())
        return false;
    char* b0 = pc[0];
    char* b1 = const_cast<char*>(beg.c_str());
    while(b0 != pc[1]){
        if(*b0 != *b1) return false;
        b0++; b1++;
    }

    int res[64] = {0};

    char* f = const_cast<char*>(pc[1]);
    char* end = const_cast<char*>(pc.back());
    f++;
    char* prev = f;
    int id = 0;
    while(f != end){
        if(*f == '/' || *f == ' '){
            res[id++] = std::atoi(prev);
            prev = f + 1;
        }
        f++;
    }
    res[id++] = std::atoi(prev);
    res1[0] = res[0];
    res2[0] = res[1];
    res3[0] = res[2];

    res1[1] = res[3];
    res2[1] = res[4];
    res3[1] = res[5];

    res1[2] = res[6];
    res2[2] = res[7];
    res3[2] = res[8];
    return true;
}

std::tuple<Vec3u, Vec3u, Vec3u> getTriangle(const ListString& ls)
{
    Vec3u res1, res2, res3;
    if(ls.size() < 4)
        return {res1, res2, res3};

    getVTN(ls[1].c_str(), ls[1].c_str() + ls[1].length(), res1[0], res1[0], res1[0]);
    getVTN(ls[2].c_str(), ls[2].c_str() + ls[2].length(), res1[1], res1[1], res1[1]);
    getVTN(ls[3].c_str(), ls[3].c_str() + ls[3].length(), res1[2], res1[2], res1[2]);

    return {res1, res2, res3};
}

//////////////////////////////

ObjReader::ObjReader()
{

}

bool ObjReader::loadObject(const std::string &fileName, Objects *objs)
{
    std::fstream file(fileName, std::ios_base::in | std::ios_base::binary);
    if(!file.is_open() || !objs)
        return false;

    file.seekg (0, file.end);
    size_t size = file.tellg();
    file.seekg (0, file.beg);
    std::vector<char> data;
    data.resize(size);
    file.read(data.data(), size);
    file.close();

    std::vector<int> list;
    //ListString ls;
    auto fn = std::bind(&ObjReader::set_progress, this, std::placeholders::_1);
    vsplit(data, '\n', list, fn);

    Obj* obj;
    float x, y, z;
    size_t id = 0;

    vectorveci3 vi, vti, vni;

    unsigned vid = 0, vtid = 0, vnid = 0;

    ListPChar pc;
    Vec3f v;
    Vec2f vt;
    Vec3u idx1, idx2, idx3;
    std::string name;

    const std::string so = "o";
    const std::string sv = "v";
    const std::string svt = "vt";
    const std::string svn = "vn";
    const std::string sf = "f";

    objs->pos.reserve(1000000);
    objs->tex.reserve(1000000);
    objs->norm.reserve(1000000);

    for(size_t i = 0; i < list.size() - 1; ++i){
        split(data.data() + list[i] + 1, data.data() + list[i + 1], ' ', pc);
        if(!pc.empty()){
            if(getResO(pc, so, name)){
                obj = objs->getInstance();
                if(obj){
                    objs->push(obj);
                }
                obj->name = name;
                obj->posidx.reserve(1000000);
                obj->texidx.reserve(1000000);
                obj->normidx.reserve(1000000);
            }
            if(getRes(pc, sv, v)){
                objs->pos.push_back(v);
                vid++;
            }
            if(getRes(pc, svt, vt)){
                objs->tex.push_back(vt);
                vtid++;
            }
            if(getRes(pc, svn, v)){
                objs->norm.push_back(v);
                vnid++;
            }
            if(getRes(pc, sf, idx1, idx2, idx3)){
                idx1 -= 1;
                obj->posidx.push_back(idx1[0]); obj->posidx.push_back(idx1[1]); obj->posidx.push_back(idx1[2]);

                if(!idx2.isNull()){
                    idx2 -= 1;
                    obj->texidx.push_back(idx2[0]); obj->texidx.push_back(idx2[1]); obj->texidx.push_back(idx2[2]);
                }
                if(!idx3.isNull()){
                    idx3 -= 1;
                    obj->normidx.push_back(idx3[0]); obj->normidx.push_back(idx3[1]); obj->normidx.push_back(idx3[2]);
                }
            }
        }
        if((id % 5000) == 0){
            mProgress = 1. * id / list.size();
            if(mCallProgress)  mCallProgress(mProgress);
            std::cout << "2 progress " << 1. * id / list.size() << "          \r";
        }
        id++;
    }
    return !objs->empty();
}

void ObjReader::setCallProgress(ObjReader::call_progress fun)
{
    mCallProgress = fun;
}

void ObjReader::set_progress(double v)
{
    mProgress = v;
    if(mCallProgress)  mCallProgress(mProgress);
}
