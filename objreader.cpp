#include "objreader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <functional>
#include <map>
//#include <hash_map>

#include <QHash>
#include <QStringList>

using namespace std;

typedef std::vector< std::string > ListString;
typedef std::vector< std::vector<char> > ListVector;
typedef std::vector<char*> ListPChar;
typedef std::vector< int > ListInt;

#if defined(_MSC_VER) && _MSC_VER <= 1600
typedef std::function<void(double)> signal_progress;
#else
typedef void(*signal_progress(double));
#endif

inline void split(const std::string& text, const std::string &ref, ListString& res)
{
    res.resize(0);
    size_t pos = 0, prev = 0;
    while((pos = text.find(ref, prev)) != text.npos){
        res.push_back(text.substr(prev, pos - prev));
        pos += ref.length();
        prev = pos;
    }
    res.push_back(text.substr(prev));
}

inline void split(const char* text, const char *end, char ref, ListString& res)
{
    res.resize(0);
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

inline void split(const char* text, const char *end, char ref, ListPChar& res)
{
    res.resize(0);
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

//void split(const char* text, const char *end, char ref, ListString& res)
//{
//    res.resize(0);
//    QString qs = QString::fromAscii(text, (int)(end - text));
//    QStringList sl = qs.split(QChar(ref));
//    for(int i = 0; i < sl.size(); ++i){
//        res.push_back(sl[i].toStdString());
//    }
//}

inline void split(const std::string& text, char ref, ListString& res)
{
    res.resize(0);
    size_t pos = 0, prev = 0;
    while((pos = text.find(ref, prev)) != text.npos){
        res.push_back(text.substr(prev, pos - prev));
        pos += 1;
        prev = pos;
    }
    res.push_back(text.substr(prev));
}

inline void vsplit(const std::vector<char>& text, char ref, std::vector<int> &posdel, signal_progress fun = nullptr)
{
    posdel.resize(0);

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
inline void getVTN(const char *begin, const char* end, T &A, T &B, T&C)
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

inline bool getResO(const ListPChar& pc, std::string& res)
{
    if(pc.size() < 3)
        return false;
    res = std::string(pc[1], pc[2] - pc[1]);
    return true;
}

inline bool getResO(const QStringList& pc, QString& res)
{
    if(pc.size() < 2)
        return false;
    res = pc[1];
    return true;
}

template< typename T>
inline bool getRes(const ListPChar& pc, T& res)
{
    for(size_t i = 1; i < pc.size() - 1; ++i){
        res[i - 1] = std::atof(pc[i]);
    }
    return true;
}

template< typename T>
inline bool getRes(const QStringList& pc, T& res)
{
    for(size_t i = 1; i < pc.size(); ++i){
        res[i - 1] = pc[i].toFloat();
    }
    return true;
}

inline bool getRes(const ListPChar& pc, Vec3f& res)
{
    res[0] = std::atof(pc[1]);
    res[1] = std::atof(pc[2]);
    res[2] = std::atof(pc[3]);
    return true;
}

inline bool getRes(const QStringList& pc, Vec3f& res)
{
    res[0] = pc[1].toFloat();
    res[1] = pc[2].toFloat();
    res[2] = pc[3].toFloat();
    return true;
}

template< typename T>
inline bool getRes(const ListPChar& pc, T& res1, T& res2, T& res3)
{
    int res[96] = {0};

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

template< typename T>
inline void Fn(vector<T>& res1, const vector<int> res[10], int i)
{
    if(res[i].size() == 4){
        res1.push_back(res[i][0]);
        res1.push_back(res[i][1]);
        res1.push_back(res[i][2]);

        res1.push_back(res[i][2]);
        res1.push_back(res[i][3]);
        res1.push_back(res[i][0]);
    }else{
        printf("count pnt %d", res[i].size());
    }
}

template< typename T>
inline bool getRes(const ListPChar& pc, vector<T>& res1, vector<T>& res2, vector<T>& res3)
{
    //int res[512] = {0};
    vector<int> res[10];

    char* f = const_cast<char*>(pc[1]);
    char* end = const_cast<char*>(pc.back());
    f++;
    string s(f, end - f);
    ListString r, r0;
    split(s, ' ', r);
    for(int i = 0; i < r.size(); ++i){
        split(r[i], '/', r0);
        if(r0.size()){
            for(int j = 0; j < r0.size(); ++j){
                string& s1 = r0[j];
                if(!s1.empty())
                    res[j].push_back(std::atoi(s1.c_str()) - 1);
            }
        }else{
            res[0].push_back(std::atoi(r[i].c_str()) - 1);
        }
    }
    if(res[0].size() == 3){
        int r = res[0].size();
        int r1 = res1.size();
        for(int i = 0; i < res[0].size(); ++i){
            res1.push_back(res[0][i]);
        }
        for(int i = 0; i < res[1].size(); ++i){
            res2.push_back(res[1][i]);
        }
        for(int i = 0; i < res[2].size(); ++i){
            res3.push_back(res[2][i]);
        }
    }else{
        Fn(res1, res, 0);
        Fn(res2, res, 1);
        Fn(res3, res, 2);
    }
//    char* prev = f;
//    int id = 0;
//    while(f != end){
//        if(*f == '/' || *f == ' '){
//            res[id++] = std::atoi(prev);
//            prev = f + 1;
//        }
//        f++;
//    }
//    res[id++] = std::atoi(prev);
//    res1[0] = res[0];
//    res2[0] = res[1];
//    res3[0] = res[2];

//    res1[1] = res[3];
//    res2[1] = res[4];
//    res3[1] = res[5];

//    res1[2] = res[6];
//    res2[2] = res[7];
//    res3[2] = res[8];
    return true;
}

template< typename T>
inline bool getRes(const QStringList& pc, vector<T>& res1, vector<T>& res2, vector<T>& res3)
{
    //int res[512] = {0};
    vector<int> res[10];
    QStringList sl;

    if(pc.size() == 3 + 1){
        for(int i = 1; i < pc.size(); ++i){
            sl = pc[i].split('/');
            if(sl.size() == 0){
                res1.push_back(pc[i].toUInt() -1 );
            }else{
                if(sl.size() == 3){
                    if(!sl[0].isEmpty())res1.push_back(sl[0].toUInt() - 1);
                    if(!sl[1].isEmpty())res2.push_back(sl[1].toUInt() - 1);
                    if(!sl[2].isEmpty())res3.push_back(sl[2].toUInt() - 1);
                }
            }
        }
    }else{
        if(pc.size() == 4 + 1){
            for(int i = 1; i < pc.size(); ++i){
                sl = pc[i].split('/');
                if(sl.size() == 0){
                    res[0].push_back(pc[i].toFloat() - 1);
                }else{
                    if(sl.size() == 3){
                        if(!sl[0].isEmpty())res[0].push_back(sl[0].toUInt() - 1);
                        if(!sl[0].isEmpty())res[1].push_back(sl[1].toUInt() - 1);
                        if(!sl[0].isEmpty())res[2].push_back(sl[2].toUInt() - 1);
                    }
                }
            }
            Fn(res1, res, 0);
            Fn(res2, res, 1);
            Fn(res3, res, 2);
        }
    }
    return true;
}

inline tuple<Vec3u, Vec3u, Vec3u> getTriangle(const ListString& ls)
{
    Vec3u res1, res2, res3;
    if(ls.size() < 4)
        return tuple<Vec3u, Vec3u, Vec3u>(res1, res2, res3);

    getVTN(ls[1].c_str(), ls[1].c_str() + ls[1].length(), res1[0], res1[0], res1[0]);
    getVTN(ls[2].c_str(), ls[2].c_str() + ls[2].length(), res1[1], res1[1], res1[1]);
    getVTN(ls[3].c_str(), ls[3].c_str() + ls[3].length(), res1[2], res1[2], res1[2]);

    return tuple<Vec3u, Vec3u, Vec3u>(res1, res2, res3);
}

//////////////////////////////

ObjReader::ObjReader()
{

}

bool ObjReader::loadObject(const std::string &fileName, Objects *objs)
{
    std::fstream file(fileName.c_str(), std::ios_base::in | std::ios_base::binary);
    if(!file.is_open() || !objs)
        return false;

    file.seekg (0, file.end);
    size_t size = file.tellg();
    file.seekg (0, file.beg);
    std::vector<char> data;
    data.resize(size);
    file.read(&data[0], size);
    file.close();

    return loadFromBuffer(data, objs);
}

bool ObjReader::loadObject(QIODevice *device, Objects *objs)
{
    std::vector<char> data;
    data.resize(device->size());
    device->read(&data[0], data.size());
    return loadFromBuffer(data, objs);
}

string getSId(const ListPChar& pc)
{
    if(pc.size() < 2)
        return string();
    string s(pc[0], pc[1] - pc[0]);
    return s;
}

bool ObjReader::loadFromBuffer(const std::vector<char> &data, Objects *objs)
{
    //std::vector<int> list;
    QStringList list;
#if 0
    auto fn = std::bind(&ObjReader::set_progress, this, std::placeholders::_1);
    vsplit(data, '\n', list, fn);
#else
    list = QString::fromLatin1(&data[0], data.size()).split('\n');
    //vsplit(data, '\n', list, NULL);
#endif

    Obj* obj;
    size_t id = 0;
    unsigned vid = 0, vtid = 0, vnid = 0;

    QStringList pc;
    //ListPChar pc;
    Vec3f v;
    Vec2f vt;
    //Vec3u idx1, idx2, idx3;
    QString name;

    const QString so = "o";
    const QString sv = "v";
    const QString svt = "vt";
    const QString svn = "vn";
    const QString sf = "f";

    objs->pos.reserve(1000);
    objs->tex.reserve(1000);
    objs->norm.reserve(1000);

//    string sid;
    enum ID{EO = 1, EV, EVT, EVN, EF};
    QHash<QString, int> ids;
    ids[so] = EO;
    ids[sv] = EV;
    ids[svt] = EVT;
    ids[svn] = EVN;
    ids[sf] = EF;
//    map<char, int> idc;
//    idc[so[0]] = EO;
//    idc[sv[0]] = EV;
//    idc[svt[0]] = EVT;
//    idc[svn[0]] = EVN;
//    idc[sf[0]] = EF;

    for(size_t i = 0; i < list.size() - 1; ++i){
        QString& li = list[i];
        char cid = li[0].toLatin1();
        pc = li.split(' ');
        //split(&data[0] + list[i] + 1, &data[0] + list[i + 1], ' ', pc);
        if(cid != '#' && ids.contains(pc[0])){
            //sid = getSId(pc);
            //cout << sid.c_str() << endl;
            //int eid = ids[sid];
            int eid = ids[pc[0]];

            //split(&data[0] + list[i] + 1, &data[0] + list[i + 1], ' ', pc);
            switch (eid) {
            case EO:
                if(getResO(pc, name)){
                    obj = objs->getInstance();
                    if(obj){
                        objs->push(obj);
                    }
                    obj->name = name;
                    obj->posidx.reserve(1000);
                    obj->texidx.reserve(1000);
                    obj->normidx.reserve(1000);
                }
                break;
            case EV:
                getRes(pc, v);
                objs->pos.push_back(v);
                vid++;
                break;
            case EVT:
                if(getRes(pc, vt)){
                    objs->tex.push_back(vt);
                    vtid++;
                }
                break;
            case EVN:
                if(getRes(pc, v)){
                    objs->norm.push_back(v);
                    vnid++;
                }
                break;
            case EF:
                getRes(pc, obj->posidx, obj->texidx, obj->normidx);
    //                idx1 -= 1;
    //                obj->posidx.push_back(idx1[0]); obj->posidx.push_back(idx1[1]); obj->posidx.push_back(idx1[2]);

    //                if(!idx2.isNull()){
    //                    idx2 -= 1;
    //                    obj->texidx.push_back(idx2[0]); obj->texidx.push_back(idx2[1]); obj->texidx.push_back(idx2[2]);
    //                }
    //                if(!idx3.isNull()){
    //                    idx3 -= 1;
    //                    obj->normidx.push_back(idx3[0]); obj->normidx.push_back(idx3[1]); obj->normidx.push_back(idx3[2]);
    //                }
    //            }
                break;
            default:
                break;
            }
        }
        if((id % 5000) == 0){
            size_t list_size = list.size();
            mProgress = 1. * id / list_size;
#if _MSC_VER > 1600
            if(mCallProgress)  mCallProgress(mProgress);
#endif
            std::cout << "2 progress " << 1. * id / list_size << "          \r\n";
        }
        id++;
    }
    return !objs->empty();
}


#ifdef __GNUC__
#  include <features.h>
#  if __GNUC_PREREQ(4,0)
//      If  gcc_version >= 4.0
void ObjReader::setCallProgress(ObjReader::call_progress fun)
{
    mCallProgress = fun;
}

void ObjReader::set_progress(double v)
{
    mProgress = v;
    if(mCallProgress)  mCallProgress(mProgress);
}
#  elif __GNUC_PREREQ(3,2)
//       If gcc_version >= 3.2
#  else
//       Else
#  endif
#else
#if _MSC_VER > 1600
void ObjReader::setCallProgress(ObjReader::call_progress fun)
{
    mCallProgress = fun;
}

void ObjReader::set_progress(double v)
{
    mProgress = v;
    if(mCallProgress)  mCallProgress(mProgress);
}
#endif
#endif
