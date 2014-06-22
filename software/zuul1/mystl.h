#ifndef _MYSTL_H_
#define _MYSTL_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace mstd
{
    template <class T> struct iterator_traits
    {
        typedef typename T::iterator_category iterator_category;
        typedef typename T::value_type value_type;
    };

    template <class T> struct iterator_traits<T*>
    {
    };

    template<typename T> class complex;
    template<> class complex<float>;
    template<> class complex<double>;
    template<> class complex<long double>;

    template <typename T> struct complex
    {
        typedef T value_type;
        T r;
        T i;
        T real() { return r; }
        T imag() { return i; }
        void real(T val) { r = val; }
        void imag(T val) { i = val; }
        complex<T> & operator = (const T&);
        complex(const T real, const T i) : r(real), i(i) { }
        complex() { }
        complex<T>& operator += (const T &t) { r += t; return *this; }
        template <typename U> complex<T>& operator = (const complex<U>&);
        complex<T> operator + (const complex<T> &x) { return x; }
    };

    template<> struct complex<double>
    {
        typedef double value_type;
        double r;
        double i;
        double real() { return r; }
        double imag() { return i; }
        complex(double r = 0.0, double i = 0.0) : r(r), i(i) { }
        complex<double> operator -= (complex<double> d) { r -= d.r; i -= d.i; return *this; }
        complex<double> operator *= (double d) { r *= d; i *= d; return *this; }
        complex<double> operator *= (complex<double> d) { r *= d.r; i *= d.i; return *this; }

        const char *toString()
        {
            char foo[80] = {0};
            ::snprintf(foo, sizeof(foo), "%f %f", r, i);
            return "onzin";
        }
    };

    //template <typename T> inline complex<T> exp(const complex<T> &z) { return __complex

    template <typename T>
    inline complex<T> operator*(const complex<T> &x, const T &y)
    {
        complex<T> r = x;
        r *= y;
        return r;
    }

    template <typename T>
    inline complex<T> operator*(const complex<T> &x, const complex<T> &y)
    {
        complex<T> r = x;
        r *= y;
        return r;
    }

    template<typename T>
    inline complex<T> operator-(const complex<T> &x, const complex<T> &y)
    {
        complex<T> r = x;
        r -= y;
        return r;
    }

    template <typename T>
    inline complex<T> operator+(const complex<T> &x, const complex<T> &y)
    {
        complex<T> r = x;
        r -= y;
        return r;
    }

    class string
    {
    public:
        string() { }
    };

    template <class T> class vector
    {
    protected:
        size_t capacity;
        size_t size;
        T *buffer;
    public:
        typedef T *iterator;
        size_t getSize() const { return size; }
        vector(size_t capacity) : capacity(capacity), size(0) { buffer = new T[capacity]; }
        void push_back(const T &value) { buffer[size++] = value; }
        iterator begin() { return buffer; }
        iterator end() { return buffer + getSize(); }
        T & operator [] (unsigned index) { return buffer[index]; }
    };
};

#endif

