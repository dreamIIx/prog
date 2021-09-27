#include <iostream>
#include <fstream>
#include <cstring>
#include <type_traits>
#include <limits>

#if !defined(defDX_S)
#define defDX_S(x)		#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)		defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__	defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef ERROR_

#if defined(_WIN32)
#define ERROR_				{ ::std::cout << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; 	\
							throw ::std::exception(); }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				try {																			\
                            	throw ::std::exception();													\
							} catch (::std::exception& x) {													\
								::std::cout << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl;	\
							}
#else
#error This UNIX operating system is not supported
#endif
#else
#error This operating system is not supported
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

#define ISBIT(x,pos)        ( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)       ( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)       ( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)     ( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

#define _dxBUF_SIZE_INPUT_STREAM 4096
#define _dxREALLOC_MEM_WAY(x) ((x) = (x) * 2)

template<typename T>
const T* find_substr(const T* src, const T* substr);

template<typename T, typename I = ::std::enable_if_t<::std::disjunction_v<::std::is_same<::std::decay_t<T>, char>, ::std::is_same<::std::decay_t<T>, wchar_t>>>>
class dxString
{
private:
    size_t size;
    size_t capacity;
    T* str;

public:
    dxString() noexcept(true) : size(0ull), capacity(0ull), str(nullptr) {}

    dxString(const T* astr) noexcept(false)
    {
        ER_IF(astr == nullptr, ::std::cerr << "arg is nullptr (constructor(const T*))" << ::std::endl;, )
        size = __get_real_size(astr);
        capacity = ::std::char_traits<T>::length(astr) + 1;
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, astr, capacity);
    }

    dxString(const dxString<T>& dxstr) noexcept(false) : size(dxstr.size), capacity(dxstr.capacity)
    {
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, dxstr.str, size + 1);
    }

    dxString(dxString<T>&& dxstr) noexcept(false) : size(::std::move(dxstr.size)), capacity(::std::move(dxstr.capacity))
    {
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, dxstr.str, size + 1);
        dxstr.str = nullptr;
    }

    [[deprecated]]
    dxString(T astr) noexcept(false) : size(1ull), capacity(2ull)
    {
        str = new T[2ull];
        str[0] = astr;
        str[1] = 0;
    }

    ~dxString() noexcept(false)
    {
        delete[] str;
        size = 0ull;
        capacity = 0ull;
    }

    // str is null-terminated array of T
    size_t __get_real_size(const T* str)
    {
        size_t i = 0ull;
        size_t _lenght = 0ull;
        while(str[i])
        {
            if (!GETBIT(str[i], 8))
            {
                ++i;
            }
            else if (!GETBIT(str[i], 6))
            {
                i += 2ull;
            }
            else if (!GETBIT(str[i], 5))
            {
                i += 3ull;
            }
            else if (!GETBIT(str[i], 4))
            {
                i += 4ull;
            }
            ++_lenght;
        }
        return _lenght;
    }

    void __reallocate_mem(size_t add_cpcty = 0ull) noexcept(false)
    {
        if (!add_cpcty)     _dxREALLOC_MEM_WAY(capacity);
        else                capacity += add_cpcty;
        T* tempstr = new T[capacity];
        if (size)   ::std::char_traits<T>::copy(tempstr, str, size + 1);               // +1 for '\0'
        delete[] str;                                                               // well defined by standart for nullptr
        str = tempstr;
    }

    // reads and appends following 'line' from file (until '\n' is encountered)
    template<typename R>
    friend ::std::enable_if_t<::std::disjunction_v<
        ::std::conjunction<::std::is_same<T, char>, ::std::is_same<R, ::std::ifstream>>,
        ::std::conjunction<::std::is_same<T, wchar_t>, ::std::is_same<R, ::std::wifstream>>>, R&>
    operator>>(R& _stream, dxString<T>& instance) noexcept(false)
    {
        //if (instance.size) delete[] instance.str;
        size_t maxszbuf = _dxBUF_SIZE_INPUT_STREAM;                     // temporary size
        T* temp = new T[maxszbuf + 1];
        _stream.getline(temp, maxszbuf, (T)'\n');
        maxszbuf = ::std::char_traits<T>::length(temp);                 // using an already useless variable
        if (instance.size + maxszbuf >= instance.capacity) instance.__reallocate_mem(instance.size + maxszbuf - instance.capacity + 1);
        instance.str = ::std::char_traits<T>::copy(instance.str + instance.size, temp, maxszbuf + 1);
        instance.size += instance.__get_real_size(temp);
        delete[] temp;
        return _stream;
    }

    template<typename R>
    friend ::std::enable_if_t<::std::disjunction_v<
        ::std::conjunction<::std::is_same<T, char>, ::std::is_same<R, ::std::ostream>>,
        ::std::conjunction<::std::is_same<T, wchar_t>, ::std::is_same<R, ::std::wostream>>,
        ::std::conjunction<::std::is_same<T, char>, ::std::is_same<R, ::std::ofstream>>,
        ::std::conjunction<::std::is_same<T, wchar_t>, ::std::is_same<R, ::std::wofstream>>>, ::std::decay_t<R>&>
    operator<<(R& _stream, const dxString<T>& instance) noexcept(false)
    {
        return static_cast<::std::decay_t<R>&>(_stream << instance.str);
    }

    dxString<T>& operator=(const T* astr) noexcept(false)
    {
        if (size) delete[] str;
        size = __get_real_size(astr);
        capacity = ::std::char_traits<T>::length(astr) + 1;
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, astr, capacity);
        return *this;
    }

    dxString<T>& operator=(const dxString<T>& dxstr) noexcept(false)
    {
        if (&dxstr == this) return *this;
        if (size) delete[] str;
        size = dxstr.size;
        capacity = dxstr.capacity;
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, dxstr.str, size + 1);
        return *this;
    }

    dxString<T>& operator=(dxString<T>&& dxstr) noexcept(false)
    {
        if (&dxstr == this) return *this;
        if (size) delete[] str;
        size = ::std::move(dxstr.size);
        capacity = ::std::move(dxstr.capacity);
        str = new T[capacity];
        str = ::std::char_traits<T>::copy(str, dxstr.str, size + 1);
        dxstr.str = nullptr;
        return *this;
    }

    dxString<T> operator+(const T* arg) const noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (operator+)!" << ::std::endl;, )
        dxString<T> temp;
        temp.size = size + __get_real_size(arg);
        temp.capacity += ::std::char_traits<T>::length(arg) + 1;
        temp.str = new T[temp.capacity];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, size + 1);
        ::std::char_traits<T>::copy(temp.str + size, arg, ::std::char_traits<T>::length(arg) + 1);
        return temp;
    }

    dxString<T>& operator+=(const T* arg) noexcept(false)
    {
        *this = this->operator+(arg);
        return *this;
    }

    dxString<T> operator+(const dxString<T>& arg) const noexcept(false)
    {
        dxString<T> temp;
        temp.size = size + arg.size;
        temp.capacity = temp.size + 1;
        temp.str = new T[temp.capacity];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, size + 1);
        ::std::char_traits<T>::copy(temp.str + size, arg.str, arg.size + 1);
        return temp;
    }

    dxString<T>& operator+=(const dxString<T>& arg) noexcept(false)
    {
        *this = this->operator+(arg);
        return *this;
    }

    dxString<T> operator()(size_t pos, size_t n) const noexcept(false)
    {
        ER_IF(pos >= size, ::std::cerr << "out of range (operator())!" << ::std::endl;, return dxString<T>(); )
        dxString<T> temp;
        temp.size = (pos + n > size ? size - pos : n);
        temp.capacity = temp.size + 1;
        temp.str = new T[temp.capacity];
        temp.str = ::std::char_traits<T>::copy(temp.str, str + pos, temp.capacity);
        return temp;
    }

    dxString<T> operator*(size_t n) const noexcept(false)
    {
        dxString<T> temp;
        temp.size = size * n;
        temp.capacity = temp.size + 1;
        temp.str = new T[temp.capacity];
        for(size_t i {0}; i < n; ++i)
        {
            ::std::char_traits<T>::copy(temp.str + size * i, str, ::std::char_traits<T>::length(str) + 1);
        }
        return temp;
    }

    dxString<T>& operator*=(size_t n) const noexcept(false)
    {
        *this = this->operator*(n);
        return *this;
    }

    operator const T*() const noexcept(false)
    {
        return const_cast<const T*>(str);
    }

    T* operator[](size_t n) const noexcept(false)
    {
        ER_IF(n >= size, ::std::cerr << "out of range (operator[])!" << ::std::endl;, return 0; )
        return (str + n);
    }

    dxString<T> operator-(const T* arg) const noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(arg);
        dxString<T> res;
        res.size = size;
        res.capacity = res.size + 1;
        res.str = new T[res.capacity];
        const T* cur = str;
        const T* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, arg)) != nullptr)
        {
            ::std::char_traits<T>::copy(res.str + real_size, prev, cur - prev);
            real_size += cur - prev;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<T>::copy(res.str + real_size, prev, ::std::char_traits<T>::length(prev) + 1);
        return res;
    }

    dxString<T>& operator-=(const T* arg) noexcept(false)
    {
        *this = this->operator-(arg);
        return *this;
    }

    dxString<T> operator-(const dxString<T>& arg) const noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(arg.str);
        dxString<T> res;
        res.size = size;
        res.capacity = res.size + 1;
        res.str = new T[res.capacity];
        const T* cur = str;
        const T* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, arg.str)) != nullptr)
        {
            ::std::char_traits<T>::copy(res.str + real_size, prev, cur - prev);
            real_size += cur - prev;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<T>::copy(res.str + real_size, prev, ::std::char_traits<T>::length(prev) + 1);
        return res;
    }

    dxString<T>& operator-=(const dxString<T>& arg) noexcept(false)
    {
        *this = this->operator-(arg);
        return *this;
    }

    size_t getSize() const noexcept(true)
    {
        return size;
    }

    bool insert(const T* arg, size_t pos) noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (insert)" << ::std::endl;, return false; )
        dxString<T> temp;
        temp.size = size + __get_real_size(arg);
        temp.capacity = temp.size + ::std::char_traits<T>::length(arg) + 1;
        temp.str = new T[temp.capacity];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, size + 1);
        ::std::char_traits<T>::move(temp.str + pos + ::std::char_traits<T>::length(arg), str + pos, size - pos + 1);
        ::std::char_traits<T>::copy(temp.str + pos, arg, ::std::char_traits<T>::length(arg));
        *this = temp;
        return true;
    }

    dxString& replace_substr(const T* s2find, const T* s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[capacity * (arg_size2 - arg_size)]; // approximate
        else                                                                            res = new T[capacity];
        const T* cur = str;
        const T* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, s2find)) != nullptr)
        {
            ::std::char_traits<T>::copy(res + real_size, prev, cur - prev);
            real_size += cur - prev;
            ::std::char_traits<T>::copy(res + real_size, s2replace, arg_size2);
            real_size += arg_size2;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<T>::copy(res + real_size, prev, ::std::char_traits<T>::length(prev) + 1);
        *this = dxString(res);
        delete[] res;
        return *this;
    }

    dxString& replace_substr(const dxString<T>& s2find, const dxString<T>& s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find.str);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace.str);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[capacity * (arg_size2 - arg_size)]; // approximate
        else                                                                            res = new T[capacity];
        const T* cur = str;
        const T* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, s2find.str)) != nullptr)
        {
            ::std::char_traits<T>::copy(res + real_size, prev, cur - prev);
            real_size += cur - prev;
            ::std::char_traits<T>::copy(res + real_size, s2replace.str, arg_size2);
            real_size += arg_size2;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<T>::copy(res + real_size, prev, ::std::char_traits<T>::length(prev) + 1);
        *this = dxString(res);
        delete[] res;
        return *this;
    }

};

int main()
{
    dxString<char> main_str;
    dxString<char> str_to_delete;
    dxString<char> str_to_replace;
    ::std::ifstream read("./input.txt");
    ER_IFN(read.is_open(),, return 1; )
    read >> main_str;
    read >> str_to_delete;
    read >> str_to_replace;

    ::std::ofstream write("./output.txt");
    main_str.replace_substr(str_to_delete, str_to_replace);
    write << main_str << ::std::endl;
    
    return 0;
}

template<typename T>
const T* find_substr(const T* src, const T* substr)
{
    size_t match = 0ull;
    size_t iter = 0ull;
    size_t arg_size = ::std::char_traits<T>::length(substr);
    while(*(src + iter))
    {
        if (!::std::char_traits<T>::compare(src + iter, substr + match, 1ull))
        {
            ++match;
            ++iter;
            if (match < arg_size)   continue;
            return (src + (iter - arg_size));
        }
        match = 0ull;
        ++iter;
    }
    return nullptr;
}