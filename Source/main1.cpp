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

const char* find_substr(const char* src, const char* substr);

class dxString
{
public:
    enum class __type : signed char
    {
        ANSII = 0,
        UTF8
    };
    
private:
    mutable __type type;
    size_t szElem;          // count of 'symbols' in str
    size_t szByte;          // size (in bytes) of str (not whole capacity allocated for it)
    char* str;              // [szByte]
    size_t* __idx;          // [szByte] (for realization), but in fact [szElem] is enough

    // _str is null-terminated array of char
    void __add_str(const char* _str) noexcept(false)
    {
        size_t* __temp_idx = new size_t[szElem + ::std::char_traits<char>::length(_str)];    // szElem + N >= than enough
        if (szElem) ::std::memcpy(__temp_idx, __idx, szElem * sizeof(size_t));
        delete[] __idx;
        __idx = __temp_idx;
        size_t i = 0ull;
        size_t _lenght = 0ull;
        while(_str[i])
        {
            __idx[szElem + _lenght++] = i + szElem;
            switch(type)
            {
            case __type::ANSII:
            {
                ++i;
                break;
            }
            case __type::UTF8:
            {
                if (!GETBIT(_str[i], 8))
                {
                    ++i;
                }
                else if (!GETBIT(_str[i], 6))
                {
                    i += 2ull;
                }
                else if (!GETBIT(_str[i], 5))
                {
                    i += 3ull;
                }
                else
                {
                    i += 4ull;
                }
                break;
            }
            default:
            {
                ERROR_
                throw ::std::bad_exception();
            }
            }
        }
        szElem += _lenght;
	    szByte += i;
    }

    void __shrink_to_size()
    {
        szByte = 0ull;
        szElem = 0ull;
        __add_str(str);
        char* temp = new char[szByte + 1];
        temp = ::std::char_traits<char>::copy(temp, str, szByte + 1);
        delete[] str;
        str = temp;
    }

    void __init(size_t _szelem, size_t _szbyte) noexcept(false)
    {
        szElem = _szelem;
        szByte = _szbyte;
        str = new char[szByte + 1];
        __idx = new size_t[szElem];
    }

public:
    dxString(__type atype = __type::ANSII) noexcept(true) : type(atype), szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr) {}

    dxString(const char* astr, __type atype = __type::ANSII) noexcept(false) : type(atype), szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = astr;
    }

    dxString(const dxString& dxstr) noexcept(false) : type(dxstr.type), szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = const_cast<dxString&>(dxstr);
    }

    dxString(dxString&& dxstr) noexcept(false) : type(dxstr.type), szElem(0ull), szByte(0ull), str(nullptr), __idx(nullptr)
    {
        *this = ::std::move(dxstr);
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
        ::std::is_same<char, _C>>, C<_C, _T>&>
    operator>>(C<_C, _T>& _stream, dxString& instance) noexcept(false)
    {
        size_t maxszbuf = _dxBUF_SIZE_INPUT_STREAM;                     // temporary szElem
        char* temp = new char[maxszbuf + 1];
        _stream.getline(temp, maxszbuf, (char)'\n');
        instance += temp;
        instance.__shrink_to_size();
        return _stream;
    }

    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_ostream<_C, _T>, C<_C, _T>>,
        ::std::is_same<_C, char>>, C<_C, _T>&>
    operator<<(C<_C, _T>& _stream, dxString& instance) noexcept(::std::is_lvalue_reference_v<C<_C, _T>>)
    {
        return static_cast<C<_C, _T>&>(_stream << instance.str);
    }

    dxString& operator=(const char* astr) noexcept(false)
    {
        ER_IF(astr == nullptr, ::std::cerr << "arg is nullptr (operator=(const char*))" << ::std::endl;, )
        if (szElem)
        {
            delete[] __idx;
            delete[] str;
        }
	    this->__add_str(astr);
        str = new char[szByte + 1];
        str = ::std::char_traits<char>::copy(str, astr, szByte + 1);
        return *this;
    }

    template<typename C>
    ::std::enable_if_t<::std::is_same_v<::std::decay_t<C>, dxString>, ::std::decay_t<C>&>
    operator=(C&& dxstr) noexcept(false)
    {
        if (&dxstr == this) return *this;
        if (szElem)
        {
            delete[] __idx;
            delete[] str;
        }
        __init(::std::forward<decltype(dxstr.szElem)>(dxstr.szElem), ::std::forward<decltype(dxstr.szByte)>(dxstr.szByte));
        str = ::std::char_traits<char>::copy(str, dxstr.str, szByte + 1);
        ::std::memcpy(__idx, dxstr.__idx, szElem * sizeof(size_t));
        return *this;
    }

    dxString& operator=(const dxString&) = delete;      // for well substitution of operator=(dxString&)

    dxString operator+(const char* arg) const noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (operator+)!" << ::std::endl;, return *this; )
        dxString temp;
        temp.__init(szElem, szByte + ::std::char_traits<char>::length(arg));
        temp.szByte -= ::std::char_traits<char>::length(arg); // crutch
        ::std::memcpy(temp.__idx, __idx, szElem * sizeof(size_t));
        temp.__add_str(arg);
        temp.str = ::std::char_traits<char>::copy(temp.str, str, szByte);
        ::std::char_traits<char>::copy(temp.str + szByte, arg, temp.szByte - szByte + 1);
        return temp;
    }

    dxString& operator+=(const char* arg) noexcept(false)
    {
        *this = this->operator+(arg);
        return *this;
    }

    dxString operator+(const dxString& arg) const noexcept(false)
    {
        return this->operator+(arg.str);
    }

    dxString& operator+=(const dxString& arg) noexcept(false)
    {
        *this = this->operator+(arg.str);
        return *this;
    }

    dxString operator*(size_t n) const noexcept(false)
    {
        dxString temp;
        temp.__init(szElem * n, szByte * n);
        for(size_t i {0}; i < n; ++i)
        {
            ::std::char_traits<char>::copy(temp.str + szByte * i, str, szByte + 1);
            ::std::memcpy(temp.__idx + szByte * i, __idx, szByte * sizeof(size_t));
            for(size_t j {0}; j < szByte; ++j)
            {
                temp.__idx[szByte * i + j] += szByte * i;
            }
        }
        return temp;
    }

    dxString& operator*=(size_t n) noexcept(false)
    {
        *this = this->operator*(n);
        return *this;
    }

    dxString operator-(const char* arg) const noexcept(false)
    {
        size_t arg_size = ::std::char_traits<char>::length(arg);
        dxString res;
        res.__init(szElem, szByte);
        const char* cur = str;
        const char* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, arg)) != nullptr)
        {
            ::std::char_traits<char>::copy(res.str + real_size, prev, cur - prev);
            real_size += cur - prev;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<char>::copy(res.str + real_size, prev, ::std::char_traits<char>::length(prev) + 1);
        res.__shrink_to_size();
        return res;
    }

    dxString& operator-=(const char* arg) noexcept(false)
    {
        *this = this->operator-(arg);
        return *this;
    }

    dxString operator-(const dxString& arg) const noexcept(false)
    {
        return this->operator-(arg.str);
    }

    dxString& operator-=(const dxString& arg) noexcept(false)
    {
        *this = this->operator-(arg.str);
        return *this;
    }

    dxString operator()(size_t pos, size_t n) const noexcept(false)
    {
        ER_IF(pos >= szElem, ::std::cerr << "out of range (operator())!" << ::std::endl;, return ::dxString(type); )
        dxString temp;
        if (pos + n > szElem)
        {
            temp.__init(szElem - pos, szByte - __idx[pos]);
        }
        else
        {
            temp.__init(n, __idx[pos + n] - __idx[pos]);
        }
        temp.str = ::std::char_traits<char>::copy(temp.str, str + __idx[pos], temp.szByte);
        ::std::memcpy(temp.__idx, __idx + pos, temp.szElem * sizeof(size_t));
        for(size_t j {0}; j < temp.szElem; ++j)
        {
            temp.__idx[j] -= pos;
        }
        temp.str[temp.szByte] = 0;
        return temp;
    }

    char& operator[](size_t n) const noexcept(false)
    {
        ER_IF(n >= szElem,, throw ::std::out_of_range("out of range (operator[])!"); )
        return *(str + __idx[n]);
    }

    operator const char*() const noexcept(false)
    {
        return const_cast<const char*>(str);
    }

    bool insert(const char* arg, size_t pos) noexcept(false)
    {
        ER_IF(arg == nullptr, ::std::cerr << "arg is nullptr (insert)" << ::std::endl;, return false; )
        ER_IF(pos > szElem, ::std::cerr << "pos > 'size of str' (insert)" << ::std::endl;, return false; )
        dxString temp;
        temp.__init(szElem, szByte + ::std::char_traits<char>::length(arg));
        temp.szByte -= ::std::char_traits<char>::length(arg); // crutch
        ::std::memcpy(temp.__idx, __idx, szElem * sizeof(size_t));
        temp.__add_str(arg);
        size_t shift = 0ull;
        if (pos == szElem)  shift = szByte;
        else                shift = temp.__idx[pos + 1] - 1;
        temp.str = ::std::char_traits<char>::copy(temp.str, str, szByte + 1);
        ::std::char_traits<char>::move(temp.str + shift + ::std::char_traits<char>::length(arg), str + shift, szByte - shift);
        ::std::char_traits<char>::copy(temp.str + shift, arg, ::std::char_traits<char>::length(arg));
        *this = temp;
        return true;
    }

    dxString& replace_substr(const char* s2find, const char* s2replace) noexcept(false)
    {
        size_t arg_size = ::std::char_traits<char>::length(s2find);
        size_t arg_size2 = ::std::char_traits<char>::length(s2replace);
        char* res;
        if (static_cast<ptrdiff_t>(arg_size2) - static_cast<ptrdiff_t>(arg_size) > 0)   res = new char[szByte * (arg_size2 - arg_size)]; // approximate
        else                                                                            res = new char[szByte];
        const char* cur = str;
        const char* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, s2find)) != nullptr)
        {
            ::std::char_traits<char>::copy(res + real_size, prev, cur - prev);
            real_size += cur - prev;
            ::std::char_traits<char>::copy(res + real_size, s2replace, arg_size2);
            real_size += arg_size2;
            cur += arg_size;
            prev = cur;
        }
        ::std::char_traits<char>::copy(res + real_size, prev, ::std::char_traits<char>::length(prev) + 1);
        *this = dxString(res, type);
        this->__shrink_to_size();
        delete[] res;
        return *this;
    }

    dxString& replace_substr(const dxString& s2find, const dxString& s2replace) noexcept(false)
    {
        return replace_substr(s2find.str, s2replace.str);
    }

    __type getType() const noexcept(true)
    {
        return type;
    }

    void setType(__type atype) const noexcept(true)
    {
        if (type != atype)
        {
            type = atype;
            this->~dxString();
        }
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
    dxString main_str(::dxString::__type::UTF8);
    dxString str_to_delete(::dxString::__type::UTF8);
    dxString str_to_replace(::dxString::__type::UTF8);
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

const char* find_substr(const char* src, const char* substr)
{
    size_t match = 0ull;
    size_t iter = 0ull;
    size_t arg_size = ::std::char_traits<char>::length(substr);
    while(*(src + iter))
    {
        if (!::std::char_traits<char>::compare(src + iter, substr + match, 1ull))
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