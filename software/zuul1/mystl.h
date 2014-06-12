#ifndef _MYSTL_H_
#define _MYSTL_H_

namespace mstd
{
    template <class T> struct iterator_traits
    {
        typedef typename T::iterator_category iterator_category;
        typedef typename T::value_type value_type;
    };

    template <class T> struct iterator_traits<T*>
    {
        //typedef random_access_iterator_tag iterator_category;
    };

    class string
    {
    public:
        string() { }
        //string(const char *input) { }
    };

    template <class T> class vector
    {
    private:
        size_t size;
        size_t capacity;
        T *buffer;
    public:
        typedef T *iterator;
        size_t getSize() const { return size; }
        vector(size_t capacity) : capacity(capacity), size(0) { buffer = new T[capacity]; }
        void push_back(const T &value) { buffer[size++] = value; }
        iterator begin() { return buffer; }
        iterator end() { return buffer + getSize(); }
    };
};

#endif

