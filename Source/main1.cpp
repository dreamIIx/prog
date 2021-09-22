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
#error This UNIX operating system is not supported by dx::NN
#endif
#else
#error This operating system is not supported by dx::NN
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

template<typename T, typename I = ::std::enable_if_t<::std::disjunction_v<::std::is_same<::std::decay_t<T>, char>, ::std::is_same<::std::decay_t<T>, wchar_t>>>>
class dxString
{
    using t_std_string = ::std::conditional_t<::std::is_same_v<T, char>, ::std::string, ::std::wstring>;
private:
    size_t size;
    T* str;

public:
    dxString() noexcept(true) : size(0ull), str(nullptr) {}

    dxString(const dxString<T>& dxstr) noexcept(false) : size(dxstr.size), str(nullptr)
    {
        str = new T[dxstr.size + 1];
        str = ::std::strcpy(str, dxstr.str);
    }

    dxString(dxString<T>&& dxstr) noexcept(false) : size(::std::move(dxstr.size)), str(nullptr)
    {
        str = new T[dxstr.size + 1];
        str = ::std::strcpy(str, dxstr.str);
    }

    dxString(T astr) noexcept(false) : size(1ull), str(nullptr)
    {
        str = new T[size + 1];
        str[0] = astr;
        str[1] = 0;
    }

    explicit dxString(const T* astr) noexcept(false) : size(::std::strlen(astr)), str(nullptr)
    {
        ER_IF(astr == nullptr, ::std::cerr << "arg is nullptr (constructor(const T*))" << ::std::endl;, )
    
        str = new T[size + 1];
        str = ::std::strcpy(str, astr);
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
        t_std_string sstr;
        ::std::getline(_stream, sstr, (T)'\n');
        instance.size = sstr.size();
        instance.str = new T[instance.size + 1];
        //instance.str = ::std::strcpy(instance.str, sstr.c_str());
        instance.str = static_cast<decltype(instance.str)>(::std::memcpy(instance.str, sstr.c_str(), sstr.size()));
        return _stream;
    }

    dxString<T>& operator=(const dxString<T>& dxstr) noexcept(false)
    {
        if (size) delete[] str;
        size = dxstr.size;
        str = new T[dxstr.size + 1];
        str = ::std::strcpy(str, dxstr.str);
    }

    dxString<T>& operator=(dxString<T>&& dxstr) noexcept(false)
    {
        if (size) delete[] str;
        size = ::std::move(dxstr.size);
        str = new T[dxstr.size + 1];
        str = ::std::strcpy(str, dxstr.str);
    }

    dxString<T> operator+(const T* arg) const noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (operator+)!" << ::std::endl;, )
        dxString<T> temp;
        temp.size = size + ::std::strlen(arg);
        temp.str = new T[temp.size + 1];
        temp.str = ::std::strcpy(temp.str, str);
        ::std::strcpy(temp.str + size, arg);
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
            temp.str = ::std::strcpy(temp.str, str);
            ::std::strcpy(temp.str + dsize, arg.str);
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
        ER_IF(pos + n > size, ::std::cerr << "out of range (operator())!" << ::std::endl;, return dxString<T>(); )
        dxString<T> temp;
        temp.size = n;
        temp.str = new T[temp.size + 1];
        temp.str = ::std::strncpy(temp.str, str + pos, temp.size);

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
            ::std::strcpy(temp.str + tempsize * i, str);
        }

        return temp;
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

    dxString<T> operator-(T arg)
    {
        const char *temp = &arg;
        return operator-(temp);
    }

    dxString<T> operator-(const T* arg)
    {
        size_t arg_size = ::std::strlen(arg);
        dxString<T> res;
        res.size = size;
        res.str = new T[res.size + 1];
        const char* cur = str;
        const char* prev = str;
        size_t real_size = 0ull;
        while((cur = ::std::strstr(cur, arg)) != nullptr)
        {
            ::std::strncpy(res.str + real_size, prev, cur - prev);
            real_size = cur - prev;
            cur += arg_size;
            prev = cur;
        }
        ::std::strcpy(res.str + real_size, prev);
        return res;
    }

    dxString<T> operator-(const dxString<T>& arg)
    {
        size_t arg_size = ::std::strlen(arg.str);
        dxString<T> res;
        res.size = size;
        res.str = new T[res.size + 1];
        const char* cur = str;
        const char* prev = str;
        size_t real_size = 0ull;
        while((cur = ::std::strstr(cur, arg.str)) != nullptr)
        {
            ::std::strncpy(res.str + real_size, prev, cur - prev);
            real_size = cur - prev;
            cur += arg_size;
            prev = cur;
        }
        ::std::strcpy(res.str + real_size, prev);
        return res;
    }

    size_t getSize() const noexcept(true)
    {
        return size;
    }

    bool insert(const char* arg, size_t pos)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (insert)" << ::std::endl;, return false; )
        dxString<T> temp;
        temp.size = size + ::std::strlen(arg);
        temp.str = new char[temp.size + 1];
        temp.str = ::std::strcpy(temp.str, str);
        ::std::strcpy(temp.str + size, arg);
        *this = temp;
        return true;
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
    ::std::cout << ((main_str + "hello!)") - dxString("Hel")) - '!' << ::std::endl;
    read >> str_to_delete;
    read >> str_to_replace;
    
    return 0;
}