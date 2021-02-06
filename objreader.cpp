#include "objreader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>

typedef std::vector< std::string > ListString;
typedef std::vector< std::vector<char> > ListVector;
typedef std::vector< int > ListInt;

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

void vsplit(const std::vector<char>& text, char ref, std::vector<int> &posdel)
{
    posdel.clear();

    posdel.push_back(-1);
    for(size_t i = 0; i < text.size(); ++i){
        if(text[i] == ref){
            posdel.push_back(i);
        }
        if((i % 50000) == 0){
            std::cout << "1 progress " << 1. * i / text.size() << "          \r";
        }
    }
    if(posdel.back() != text.size() - 1){
        posdel.push_back(text.size());
    }
}

std::tuple<Vec3u, Vec3u, Vec3u> getTriangle(const ListString& ls)
{
    Vec3u res1, res2, res3;
    if(ls.size() < 4)
        return {res1, res2, res3};
    ListString ls1;
    split(ls[1], '/', ls1);
    res1[0] = std::stol(ls1[0]);
    if(!ls1[1].empty())  res2[0] = std::stol(ls1[1]);
    if(!ls1[2].empty())  res3[0] = std::stol(ls1[2]);

    split(ls[2], '/', ls1);
    res1[1] = std::stol(ls1[0]);
    if(!ls1[1].empty())  res2[1] = std::stol(ls1[1]);
    if(!ls1[2].empty())  res3[1] = std::stol(ls1[2]);

    split(ls[3], '/', ls1);
    res1[2] = std::stol(ls1[0]);
    if(!ls1[1].empty())  res2[2] = std::stol(ls1[1]);
    if(!ls1[2].empty())  res3[2] = std::stol(ls1[2]);
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
    ListString ls;
    vsplit(data, '\n', list);

    Obj* obj;
    float x, y, z;
    size_t id = 0;

    vectorveci3 vi, vti, vni;

    unsigned vid = 0, vtid = 0, vnid = 0;

    for(size_t i = 0; i < list.size() - 1; ++i){
        split(data.data() + list[i] + 1, data.data() + list[i + 1], ' ', ls);
        if(!ls.empty()){
            if(ls[0] == "o"){
                obj = objs->getInstance();
                if(obj){
                    objs->push(obj);
                }
                obj->name = ls[1];
                obj->clear();
            }
            if(ls[0] == "v"){
                x = std::stof(ls[1]);
                y = std::stof(ls[2]);
                z = std::stof(ls[3]);
                objs->pos.push_back(Vec3f(x, y, z));
                vid++;
            }
            if(ls[0] == "vt"){
                x = std::stof(ls[1]);
                y = std::stof(ls[2]);
                objs->tex.push_back(Vec2f(x, y));
                vtid++;
            }
            if(ls[0] == "vn"){
                x = std::stof(ls[1]);
                y = std::stof(ls[2]);
                z = std::stof(ls[3]);
                objs->norm.push_back(Vec3f(x, y, z));
                vnid++;
            }
            if(ls[0] == "f"){
                Vec3u idx1, idx2, idx3;
                std::tie<Vec3u>(idx1, idx2, idx3) = getTriangle(ls);
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
            std::cout << "2 progress " << 1. * id / list.size() << "          \r";
        }
        id++;
    }
    return !objs->empty();
}
