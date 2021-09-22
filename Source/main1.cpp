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
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
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

template<typename T>
const T* find_substr(const T* src, const T* substr);

template<typename T, typename I = ::std::enable_if_t<::std::disjunction_v<::std::is_same<::std::decay_t<T>, char>, ::std::is_same<::std::decay_t<T>, wchar_t>>>>
class dxString
{
private:
    size_t szElem;          // count of 'symbols' in str
    size_t szByte;          // size (in bytes) of str (not whole capacity allocated for it)
    T* str;                 // [szByte]
    size_t* __idx;          // [szByte] (for realization), but in fact [szElem] is enough

    // str is null-terminated array of T
    void __add_utf_str(const T* str) noexcept(false)
    {
        size_t* __temp_idx = new size_t[szElem + ::std::char_traits<T>::length(str)];    // szElem + N >= than enough
		if (szElem) ::std::memcpy(__temp_idx, __idx, szElem * sizeof(size_t));
		delete[] __idx;
		__idx = __temp_idx;
        size_t i = 0ull;
        size_t _lenght = 0ull;
        while(str[i])
        {
            __idx[szElem + _lenght++] = i + szElem;
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
            else/* if (!GETBIT(str[i], 4))*/
            {
                i += 4ull;
            }
        }
        szElem += _lenght;
		szByte += i;
    }

    void __shrink_to_size()
    {
        szByte = 0ull;
        szElem = 0ull;
        __add_utf_str(str);
        T* temp = new T[szByte + 1];
        temp = ::std::char_traits<T>::copy(temp, str, szByte + 1);
        delete[] str;
        str = temp;
    }

public:
    dxString() noexcept(true) : szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr) {}

    dxString(const T* astr) noexcept(false) : szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = astr;
    }

    dxString(const dxString<T>& dxstr) noexcept(false) : szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = dxstr;
    }

    dxString(dxString<T>&& dxstr) noexcept(false) : szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = ::std::move(dxstr);
    }

    [[deprecated]]
    dxString(T astr) noexcept(false) : szElem(1ull), szByte(2ull)
    {
        str = new T[2ull];
        str[0] = astr;
        str[1] = 0;
    }

    ~dxString() noexcept(false)
    {
        delete[] __idx;
        delete[] str;
        szElem = 0ull;
        szByte = 0ull;
    }

    // reads and appends following 'line' from file (until '\n' is encountered)
    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_istream<_C, _T>, C<_C, _T>>,
        ::std::is_same<T, _C>>, C<_C, _T>&>
    operator>>(C<_C, _T>& _stream, dxString<T>& instance) noexcept(false)
    {
        size_t maxszbuf = _dxBUF_SIZE_INPUT_STREAM;                     // temporary szElem
        T* temp = new T[maxszbuf + 1];
        _stream.getline(temp, maxszbuf, (T)'\n');
        size_t old_capacity = instance.szByte;
        instance.__add_utf_str(temp);
        T* tempstr = new T[instance.szByte + 1];
        if (instance.szElem)    ::std::char_traits<T>::copy(tempstr, instance.str, old_capacity);
        ::std::char_traits<T>::copy(tempstr + old_capacity, temp, ::std::char_traits<T>::length(temp) + 1);
        delete[] instance.str;                                          // well defined by standart for nullptr
        instance.str = tempstr;
        delete[] temp;
        return _stream;
    }

    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_ostream<_C, _T>, C<_C, _T>>,
        ::std::is_same<T, _C>>, C<_C, _T>&>
    operator>>(C<_C, _T>& _stream, dxString<T>& instance) noexcept(false)
    {
        return static_cast<C<_C, _T>&>(_stream << instance.str);
    }

    dxString<T>& operator=(const T* astr) noexcept(false)
    {
        ER_IF(astr == nullptr, ::std::cerr << "arg is nullptr (operator=(const T*))" << ::std::endl;, )
        if (szElem)
        {
            delete[] __idx;
            delete[] str;
        }
		this->__add_utf_str(astr);
        str = new T[szByte + 1];
        str = ::std::char_traits<T>::copy(str, astr, szByte + 1);
        return *this;
    }

    dxString<T>& operator=(const dxString<T>& dxstr) noexcept(false)
    {
        if (&dxstr == this) return *this;
        if (szElem)
        {
            delete[] __idx;
            delete[] str;
        }
        szElem = dxstr.szElem;
        szByte = dxstr.szByte;
        str = new T[szByte + 1];
        str = ::std::char_traits<T>::copy(str, dxstr.str, szByte + 1);
        __idx = new size_t[szElem];
        ::std::memcpy(__idx, dxstr.__idx, szElem * sizeof(size_t));
        return *this;
    }

    dxString<T>& operator=(dxString<T>&& dxstr) noexcept(false)
    {
        if (&dxstr == this) return *this;
        if (szElem)
        {
            delete[] __idx;
            delete[] str;
        }
        szElem = ::std::move(dxstr.szElem);
        szByte = ::std::move(dxstr.szByte);
        str = new T[szByte + 1];
        str = ::std::char_traits<T>::copy(str, dxstr.str, szByte + 1);
        __idx = new size_t[szElem];
        ::std::memcpy(__idx, dxstr.__idx, szElem * sizeof(size_t));
        return *this;
    }

    dxString<T> operator+(const T* arg) const noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (operator+)!" << ::std::endl;, return *this; )
        dxString<T> temp;
        temp.szElem = szElem;
        temp.szByte = szByte;
        temp.__idx = new size_t[szElem];
        ::std::memcpy(temp.__idx, __idx, szElem * sizeof(size_t));
        temp.__add_utf_str(arg);
        temp.str = new T[temp.szByte + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, szByte);
        ::std::char_traits<T>::copy(temp.str + szByte, arg, temp.szByte - szByte + 1);
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
        temp.szElem = szElem;
        temp.szByte = szByte;
        temp.__idx = new size_t[szElem];
        ::std::memcpy(temp.__idx, __idx, szElem * sizeof(size_t));
        temp.__add_utf_str(arg.str);
        temp.str = new T[temp.szByte + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str, szByte);
        ::std::char_traits<T>::copy(temp.str + szByte, arg.str, temp.szByte - szByte + 1);
        return temp;
    }

    dxString<T>& operator+=(const dxString<T>& arg) noexcept(false)
    {
        *this = this->operator+(arg);
        return *this;
    }

    dxString<T> operator*(size_t n) const noexcept(false)
    {
        dxString<T> temp;
        temp.szElem = szElem * n;
        temp.szByte = szByte * n;
        temp.str = new T[temp.szByte + 1];
        temp.__idx = new size_t[temp.szElem];
        for(size_t i {0}; i < n; ++i)
        {
            ::std::char_traits<T>::copy(temp.str + szByte * i, str, szByte + 1);
            ::std::memcpy(temp.__idx + szByte * i, __idx, szByte * sizeof(size_t));
            for(size_t j {0}; j < szByte; ++j)
            {
                temp.__idx[szByte * i + j] += szByte * i;
            }
        }
        return temp;
    }

    dxString<T>& operator*=(size_t n) const noexcept(false)
    {
        *this = this->operator*(n);
        return *this;
    }

    dxString<T> operator-(const T* arg) const noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(arg);
        dxString<T> res;
        res.szElem = szElem;
        res.szByte = szByte;
        res.str = new T[res.szByte + 1];
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
        res.__shrink_to_size();
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
        res.szElem = szElem;
        res.szByte = szByte;
        res.str = new T[res.szByte + 1];
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
        res.__shrink_to_size();
        return res;
    }

    dxString<T>& operator-=(const dxString<T>& arg) noexcept(false)
    {
        *this = this->operator-(arg);
        return *this;
    }

    dxString<T> operator()(size_t pos, size_t n) const noexcept(false)
    {
        ER_IF(pos >= szElem, ::std::cerr << "out of range (operator())!" << ::std::endl;, return ::dxString<T>(); )
        dxString<T> temp;
        if (pos + n > szElem)
        {
            temp.szElem = szElem - pos;
            temp.szByte = szByte - __idx[pos];
        }
        else
        {
            temp.szElem = n;
            temp.szByte = __idx[pos + n] - __idx[pos];
        }
        temp.str = new T[temp.szByte + 1];
        temp.str = ::std::char_traits<T>::copy(temp.str, str + __idx[pos], temp.szByte);
        temp.__idx = new size_t[temp.szElem];
        ::std::memcpy(temp.__idx, __idx + pos, temp.szElem * sizeof(size_t));
        for(size_t j {0}; j < temp.szElem; ++j)
        {
            temp.__idx[j] -= pos;
        }
        temp.str[temp.szByte] = 0;
        return temp;
    }

    T& operator[](size_t n) const noexcept(false)
    {
        ER_IF(n >= szElem,, throw ::std::out_of_range("out of range (operator[])!"); )
        return *(str + __idx[n]);
    }

    operator const T*() const noexcept(false)
    {
        return const_cast<const T*>(str);
    }

    bool insert(const T* arg, size_t pos) noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (insert)" << ::std::endl;, return false; )
        ER_IF(pos > szElem, ::std::cerr << "pos > 'size of str' (insert)" << ::std::endl;, return false; )
        dxString<T> temp;
        temp.szElem = szElem;
        temp.szByte = szByte;
        temp.__idx = new size_t[szElem];
        ::std::memcpy(temp.__idx, __idx, szElem * sizeof(size_t));
        temp.__add_utf_str(arg);
        temp.str = new T[temp.szByte + 1];
        size_t shift = 0ull;
        if (pos == szElem)  shift = szByte;
        else                shift = temp.__idx[pos + 1] - 1;
        temp.str = ::std::char_traits<T>::copy(temp.str, str, szByte + 1);
        ::std::char_traits<T>::move(temp.str + shift + ::std::char_traits<T>::length(arg), str + shift, szByte - shift);
        ::std::char_traits<T>::copy(temp.str + shift, arg, ::std::char_traits<T>::length(arg));
        *this = temp;
        return true;
    }

    dxString& replace_substr(const T* s2find, const T* s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[szByte * (arg_size2 - arg_size)]; // approximate
        else                                                                            res = new T[szByte];
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
        this->__shrink_to_size();
        delete[] res;
        return *this;
    }

    dxString& replace_substr(const dxString<T>& s2find, const dxString<T>& s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<T>::length(s2find.str);
        size_t arg_size2 = ::std::char_traits<T>::length(s2replace.str);
        T* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new T[szByte * (arg_size2 - arg_size)]; // approximate
        else                                                                            res = new T[szByte];
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
        this->__shrink_to_size();
        delete[] res;
        return *this;
    }

    size_t size() const noexcept(true)
    {
        return szElem;
    }

    size_t length() const noexcept(true)
    {
        return szElem;
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
    write << main_str;
    
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