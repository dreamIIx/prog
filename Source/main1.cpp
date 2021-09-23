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

#define _dxBUF_SIZE_INPUT_STREAM 4096

template<typename T>
const T* find_substr(const T* src, const T* substr);

template<typename T, typename I = ::std::enable_if_t<::std::disjunction_v<::std::is_same<::std::decay_t<T>, char>, ::std::is_same<::std::decay_t<T>, wchar_t>>>>
class dxString
{
private:
    size_t size;
    T* str;

public:
    dxString() noexcept(true) : size(0ull), str(nullptr) {}

    dxString(const dxString<T>& dxstr) noexcept(false) : size(dxstr.size), str(nullptr)
    {
        str = new T[dxstr.size + 1];
        str = ::std::char_traits<T>::copy(str, dxstr.str, ::std::char_traits<T>::length(dxstr.str) + 1);
    }

    dxString(dxString<T>&& dxstr) noexcept(false) : size(::std::move(dxstr.size)), str(nullptr)
    {
        str = new T[dxstr.size + 1];
        str = ::std::char_traits<T>::move(str, dxstr.str, ::std::char_traits<T>::length(dxstr.str) + 1);
    }

    dxString(T astr) noexcept(false) : size(1ull), str(nullptr)
    {
        str = new T[2];
        str[0] = astr;
        str[1] = 0;
    }

    explicit dxString(const T* astr) noexcept(false) : size(::std::char_traits<T>::length(astr)), str(nullptr)
    {
        ER_IF(astr == nullptr, ::std::cerr << "arg is nullptr (constructor(const T*))" << ::std::endl;, )
    
        str = new T[size + 1];
        str = ::std::char_traits<T>::copy(str, astr, ::std::char_traits<T>::length(astr) + 1);
    }

    ~dxString() noexcept(false)
    {
        delete[] str;
    }

    template<typename R>
    friend ::std::enable_if_t<::std::disjunction_v<
        ::std::conjunction<::std::is_same<T, char>, ::std::is_same<R, ::std::ifstream>>,
        ::std::conjunction<::std::is_same<T, wchar_t>, ::std::is_same<R, ::std::wifstream>>>, R&>
    operator>>(R& _stream, dxString<T>& instance) noexcept(false)
    {
        if (instance.size) delete[] instance.str;
        size_t maxszbuf = _dxBUF_SIZE_INPUT_STREAM;                         // temporary size
        T* temp = new T[maxszbuf + 1];
        _stream.getline(temp, ::std::numeric_limits<::std::streamsize>::max(), (T)'\n');
        instance.size = ::std::char_traits<T>::length(temp);
        instance.str = new T[instance.size + 1];
        instance.str = ::std::char_traits<T>::copy(instance.str, temp, ::std::char_traits<T>::length(temp) + 1);
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

    dxString<T>& operator=(const dxString<T>& dxstr) noexcept(false)
    {
        if (size) delete[] str;
        size = dxstr.size;
        str = new T[dxstr.size + 1];
        str = ::std::char_traits<T>::copy(str, dxstr.str, ::std::char_traits<T>::length(dxstr.str) + 1);
        return *this;
    }

    dxString<T>& operator=(dxString<T>&& dxstr) noexcept(false)
    {
        if (size) delete[] str;
        size = ::std::move(dxstr.size);
        str = new T[dxstr.size + 1];
        str = ::std::char_traits<T>::move(str, dxstr.str, ::std::char_traits<T>::length(dxstr.str) + 1);
        return *this;
    }

    dxString<T> operator+(const T* arg) const noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (operator+)!" << ::std::endl;, )
        dxString<T> temp;
        temp.size = size + ::std::char_traits<T>::length(arg);
        temp.str = new T[temp.size + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, ::std::char_traits<T>::length(str) + 1);
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
        if (arg.size)
        {
            size_t dsize = size - arg.size;
            temp.size = size + arg.size;
            temp.str = new T[temp.size + 1];
            temp.str = ::std::char_traits<T>::copy(temp.str, str, ::std::char_traits<T>::length(str) + 1);
            ::std::char_traits<T>::copy(temp.str + dsize, arg.str, ::std::char_traits<T>::length(arg.str) + 1);
        }
        else
        {
            temp = *this;
        }
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
        temp.size = (pos + n > size ? size : n);
        temp.str = new T[temp.size + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str + pos, temp.size + 1);

        return temp;
    }

    dxString<T> operator*(size_t n) const noexcept(false)
    {
        dxString<T> temp;
        size_t tempsize = size;
        temp.size = size * n;
        temp.str = new T[temp.size + 1];
        for(size_t i {0}; i < n; ++i)
        {
            ::std::char_traits<T>::copy(temp.str + tempsize * i, str, ::std::char_traits<T>::length(str) + 1);
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

    T operator[](size_t n) const noexcept(false)
    {
        ER_IF(n >= size, ::std::cerr << "out of range (operator[])!" << ::std::endl;, return 0; )
        return str[n];
    }

    dxString<T> operator-(const T* arg) const noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(arg);
        dxString<T> res;
        res.size = size;
        res.str = new T[res.size + 1];
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
        res.str = new T[res.size + 1];
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
        temp.size = size + ::std::char_traits<T>::length(arg);
        temp.str = new char[temp.size + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, ::std::char_traits<T>::length(str) + 1);
        ::std::char_traits<T>::copy(temp.str + size, arg, ::std::char_traits<T>::length(arg) + 1);
        *this = temp;
        return true;
    }

    dxString& replace_substr(const T* s2find, const T* s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[size * (arg_size2 - arg_size)]; // approximate
        else                        res = new T[size];
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
        return *this;
    }

    dxString& replace_substr(const dxString<T>& s2find, const dxString<T>& s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find.str);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace.str);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[size * (arg_size2 - arg_size)]; // approximate
        else                        res = new T[size];
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
    ::std::cout << main_str << ::std::endl;
    dxString temp = ((main_str + "hello!)") - dxString("Hel"));
    ::std::cout << temp << ::std::endl;
    temp -= "!";
    ::std::cout << temp << ::std::endl;
    temp = temp - (temp - "lo,");
    ::std::cout << temp << ::std::endl;
    read >> str_to_delete;
    ::std::cout << str_to_delete << ::std::endl;
    read >> str_to_replace;
    ::std::cout << str_to_replace * 6 << ::std::endl;
    ::std::cout << str_to_replace << ::std::endl;
    ::std::ofstream write("./input.txt");
    main_str.replace_substr(str_to_delete, str_to_replace);
    write << main_str << ::std::endl;
    main_str.replace_substr("Goodbye", "World");
    write << main_str << ::std::endl;
    ::std::cout << main_str(3, 30) << ::std::endl;
    ::std::cout << main_str[14] << ::std::endl;
    ::std::cout << main_str - "o" << ::std::endl;
    
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