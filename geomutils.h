#ifndef GEOMUTILS_H
#define GEOMUTILS_H

#include <vector>
#include <memory>
#include <iterator>
#include <string>

template< typename T, int Count = 3 >
struct Vec_{
    T data[Count];

    Vec_(){
        for(int i = 0; i < Count; ++i){
            data[i] = 0;
        }
    }
    Vec_(const Vec_<T, Count> &cp){
        for(int i = 0; i < Count; ++i){
            data[i] = cp.data[i];
        }
    }
    Vec_(T x){
        for(int i = 0; i < Count; ++i){
            data[i] = x;
        }
    }
    Vec_(T x1, T x2){
        data[0] = x1;
        data[1] = x2;
        for(int i = 2; i < Count; ++i){
            data[i] = 0;
        }
    }
    Vec_(T x1, T x2, T x3){
        data[0] = x1;
        data[1] = x2;
        data[2] = x3;
        for(int i = 3; i < Count; ++i){
            data[i] = 0;
        }
    }
    Vec_(T x1, T x2, T x3, T x4){
        data[0] = x1;
        data[1] = x2;
        data[2] = x3;
        data[3] = x4;
        for(int i = 4; i < Count; ++i){
            data[i] = 0;
        }
    }

    bool isNull() const{
        for(int i = 0; i < Count; ++i){
            if(data[i]) return false;
        }
        return true;

    }

    T dot(const Vec_<T, Count>& v) const{
        T res = 0;
        for(int i = 0; i < Count; ++i){
            res += data[i] * v.data[i];
        }
        return res;
    }
    T length2() const{
        T res = 0;
        for(int i = 0; i < Count; ++i){
            res += data[i] * data[i];
        }
        return (res);
    }
    T length() const{
        return sqrt(length2());
    }
    void normalize(){
        T len = length2();
        if(len){
            len = 1 / sqrt(len);
            for(int i = 0; i < Count; ++i){
                data[i] *= len;
            }
        }
    }
    Vec_<T, Count> normalized() const{
        Vec_<T, Count> ret = *this;
        ret.normalize();
        return ret;
    }
    Vec_<T, Count>& operator +=(const Vec_<T, Count>& v){
        for(int i = 0; i < Count; ++i)  data[i] += v.data[i];
        return *this;
    }
    Vec_<T, Count>& operator -=(const Vec_<T, Count>& v){
        for(int i = 0; i < Count; ++i)  data[i] -= v.data[i];
        return *this;
    }
    Vec_<T, Count>& operator *=(const Vec_<T, Count>& v){
        for(int i = 0; i < Count; ++i)  data[i] *= v.data[i];
        return *this;
    }
    Vec_<T, Count>& operator /=(const Vec_<T, Count>& v){
        for(int i = 0; i < Count; ++i)  data[i] /= v.data[i];
        return *this;
    }
    Vec_<T, Count>& operator +=(T v){
        for(int i = 0; i < Count; ++i)  data[i] += v;
        return *this;
    }
    Vec_<T, Count>& operator -=(T v){
        for(int i = 0; i < Count; ++i)  data[i] -= v;
        return *this;
    }
    Vec_<T, Count>& operator *=(T v){
        for(int i = 0; i < Count; ++i)  data[i] *= v;
        return *this;
    }
    Vec_<T, Count>& operator /=(T v){
        for(int i = 0; i < Count; ++i)  data[i] /= v;
        return *this;
    }
    T& operator[](int id){
        return data[id];
    }
    T operator[](int id) const{
        return data[id];
    }
};

template< typename T, int Count = 3 >
Vec_<T, Count> operator +(const Vec_<T, Count>& v1, const Vec_<T, Count>& v2){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] + v2.data[i];
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator -(const Vec_<T, Count>& v1, const Vec_<T, Count>& v2){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] - v2.data[i];
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator *(const Vec_<T, Count>& v1, const Vec_<T, Count>& v2){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] * v2.data[i];
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator /(const Vec_<T, Count>& v1, const Vec_<T, Count>& v2){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] / v2.data[i];
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator +(const Vec_<T, Count>& v1, T v){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] + v;
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator -(const Vec_<T, Count>& v1, T v){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] - v;
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator *(const Vec_<T, Count>& v1, T v){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] * v;
    return res;
}

template< typename T, int Count = 3 >
Vec_<T, Count> operator /(const Vec_<T, Count>& v1, T v){
    Vec_<T, Count> res;
    for(int i = 0; i < Count; ++i)  res.data[i] = v1.data[i] / v;
    return res;
}

typedef Vec_<float, 2> Vec2f;
typedef Vec_<float, 3> Vec3f;
typedef Vec_<float, 4> Vec4f;

typedef Vec_<int,   3> Vec3i;
typedef Vec_<unsigned,   3> Vec3u;

typedef std::vector<Vec2f> vectorvecf2;
typedef std::vector<Vec3f> vectorvecf3;
typedef std::vector<Vec3i> vectorveci3;
typedef std::vector<Vec3u> vectorvecu3;
typedef std::vector<unsigned> vectorvecu;

class Objects;

class Obj{
public:
    std::string name;
    vectorvecu posidx;
    vectorvecu texidx;
    vectorvecu normidx;

    Objects* parent = nullptr;

    Obj(Objects* parent): parent(parent){}
    virtual bool empty() const;
    virtual void clear();

    uint32_t type() const;
    void setType(uint32_t tp);
private:
    uint32_t mType = 0;
};

typedef std::shared_ptr<Obj> PObj;

class Objects{
public:
    struct Iterator{
        typedef Obj *PtrObj;
        using iterator_category = std::forward_iterator_tag;
        using diference_type = std::ptrdiff_t;
        using value_type = PObj;
        using pointer = PObj*;
        using reference = PObj;

        Iterator(pointer ptr): mPtr(ptr){}

        reference operator*()const { return *mPtr; }
        pointer operator->(){ return mPtr; }
        Iterator& operator++() { mPtr++; return *this; }
//        Iterator& operator++(int){ Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator==(const Iterator &a, const Iterator &b){ return a.mPtr == b.mPtr; }
        friend bool operator!=(const Iterator &a, const Iterator &b){ return a.mPtr != b.mPtr; }

    private:
        pointer mPtr;

    };

    Iterator begin() { return Iterator(&mObjs.front()); }
    Iterator end()   { return Iterator(&mObjs.back()); }

    vectorvecf3 pos;
    vectorvecf2 tex;
    vectorvecf3 norm;

    Objects();
    virtual ~Objects();
    virtual Obj *getInstance();

    size_t count() const;
    bool empty() const;
    void push(Obj* obj);
    Obj *get(size_t index);
    void clear();

protected:
    std::vector<PObj> mObjs;
    size_t mId = 0;
};

#endif // GEOMUTILS_H
