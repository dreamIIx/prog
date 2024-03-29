#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <type_traits>
#include <exception>
#include <cmath>
#include <cstring>
#include <utility>

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

#define KF_BIG_DATA                 4095
#define _dxBUF_SIZE_INPUT_STREAM    4096
#define __KF                        (static_cast<double>(0.6))
#define __DEFAULT_CAPACITY          (static_cast<size_t>(5ull))

const char* find_substr(const char* src, const char* substr);

template<typename K, typename V>
struct HashElem
{
    bool exist;
    K first;
    V second;

    HashElem() noexcept(true) : exist(false), first(), second() {}

    HashElem(const HashElem<K, V>& inst) noexcept(true)
    {
        *this = inst;
    }

    HashElem(HashElem<K, V>&& inst) noexcept(true)
    {
        *this = ::std::move(inst);
    }

    HashElem<K, V>& operator=(const HashElem<K, V>& inst) noexcept(true)
    {
        exist = inst.exist;
        first = inst.first;
        second = inst.second;
        return *this;
    }

    HashElem<K, V>& operator=(HashElem<K, V>&& inst) noexcept(true)
    {
        exist = ::std::move(inst.exist);
        first = ::std::move(inst.first);
        second = ::std::move(inst.second);
        return *this;
    }

    template<typename _K, typename _V, typename _I = ::std::enable_if_t<::std::conjunction<
        ::std::is_same<K, _K>, ::std::is_same<V, _V>/*, ::std::is_move_constructible<K>, ::std::is_move_constructible<V>*/>::value, void>>
    HashElem(_K&& akey, _V&& avalue) noexcept(true) : exist(true), first(::std::forward<K>(akey)), second(::std::forward<V>(avalue)) {}

    template<typename _K, typename _V, typename _I = ::std::enable_if_t<::std::conjunction<
        ::std::is_same<K, _K>, ::std::is_same<V, _V>/*, ::std::is_copy_constructible<K>, ::std::is_copy_constructible<V>*/>::value, void>>
    HashElem(const _K& akey, const _V& avalue) noexcept(true) : exist(true), first(akey), second(avalue) {}
};

template<typename __K, typename __V,
    typename I = ::std::enable_if_t<::std::conjunction<::std::is_default_constructible<__K>, ::std::is_default_constructible<__V>>::value, void>>
class HashMap
{
    using K = ::std::decay_t<__K>;
    using V = ::std::decay_t<__V>;

protected:
    size_t _size;
    size_t _capacity;
    mutable double KF_OVERFLOW;
    HashElem<K, V>* map;
    ::std::hash<K> _prov_hash;

public:
    enum class _type : unsigned
    {
        base = 0u,
        multi = 1u
    };

    class _Iterator
    {
    public:
        size_t _cap;
        HashElem<K, V>* ptr;

        _Iterator() noexcept(true) : _cap(0ull), ptr(nullptr) {}

        _Iterator(size_t acap, HashElem<K, V>* aptr) noexcept(true) : _cap(acap), ptr(aptr)
        {
            while(_cap > 0 ? !((ptr)->exist) : !!(ptr = nullptr))
            {
                --_cap;
                ++ptr;
            }
        }

        ~_Iterator() noexcept(true)
        {
            _cap = 0ull;
            ptr = nullptr;
        }

        _Iterator operator+(size_t n) noexcept(true)
        {
            if (ptr == nullptr) return _Iterator();
            size_t temp_cap = _cap;
            HashElem<K, V>* temp_ptr = ptr;
            for(size_t i {0}; i < n; ++i)
            {
                while(--temp_cap > 0 ? !((++temp_ptr)->exist) : !!(temp_ptr = nullptr)) {}
            }
            return { temp_cap, temp_ptr };
        }

        _Iterator& operator++() noexcept(true)
        {
            while(--_cap > 0 ? !((++ptr)->exist) : !!(ptr = nullptr)) {}
            return *this;
        }

        bool operator!=(const _Iterator& inst) const noexcept(true)
        {
            return (ptr != inst.ptr);
        }

        operator HashElem<K, V>*() const noexcept(true)
        {
            return ptr;
        }

        HashElem<K, V>* operator->()
        {
            return ptr;
        }
    };

    HashMap() : _size(0ull), _capacity(__DEFAULT_CAPACITY), KF_OVERFLOW(__KF), map(new HashElem<K, V>[__DEFAULT_CAPACITY]()) {}
    HashMap(size_t acapacity, double kf_overflow = __KF) noexcept(false) : _size(0ull), _capacity(acapacity), KF_OVERFLOW(kf_overflow), map(new HashElem<K, V>[acapacity]()) {}

    ~HashMap() noexcept(false)
    {
        delete[] map;
        _size = 0ull;
        _capacity = 0ull;
    }
    
    V& operator[](const K& key) noexcept(false)
    {
        auto res = _find(key);
        if (res.second) return res.first->second;
        else return insert(key, V()).second;
    }

    template<typename _K, typename _V,
        typename _I = ::std::enable_if_t<::std::conjunction<::std::is_same<K, ::std::decay_t<_K>>, ::std::is_same<V, ::std::decay_t<_V>>>::value, void>>
    HashElem<K, V>& insert(_K&& key, _V&& value, _type type = _type::base) noexcept(false)
    {
        if (!_capacity)
        {
            _capacity = __DEFAULT_CAPACITY;
            map = new HashElem<K, V>[_capacity]();
        }
        size_t _calced_hash = _hash(key);
        bool placed = false;
        size_t i = _calced_hash;
        do
        {
            if (map[i].exist && (type != _type::base || map[i].first != key))
            {
                if (++i >= _capacity) i = 0ull;
                continue;
            }
            else
            {
                if (type != _type::base || !map[i].exist) ++_size;
                map[i] = HashElem<K, V>(::std::forward<_K>(key), ::std::forward<_V>(value));
                placed = true;
                break;
            }
        } while(i != _calced_hash);
        resize();
        if (!placed)    return insert(::std::forward<_K>(key), ::std::forward<_V>(value));
        return map[_calced_hash];
    }

    void erase(const K& key) noexcept(false)
    {
        auto res = _find(key);
        if (res.second) 
        {
            res.first->exist = false;
            --_size;
        }
        else std::cerr << "[ATTENTION] a pair with the required key is not exist!" << ::std::endl;
    }

    void resize(_type type = _type::base) noexcept(false)
    {
        if (_size >= _capacity * KF_OVERFLOW)
        {
            size_t oldcap = _capacity;
            _capacity *= 2ull;
            HashElem<K, V>* temp = new HashElem<K, V>[_capacity];
            ::std::swap(map, temp);
            for(size_t i {0}; i < oldcap; ++i)
            {
                if (map[i].exist)  temp[i] = map[i];
            }
            _size = 0ull;
            for(size_t i {0}; i < oldcap; ++i)
            {
                if (temp[i].exist)  insert(static_cast<K>(temp[i].first), static_cast<V>(temp[i].second), type);
            }
            delete[] temp;
        }
    }

    double getKF() const noexcept(true)
    {
        return KF_OVERFLOW;
    }

    void setKF(double kf) noexcept(true)
    {
        KF_OVERFLOW = kf;
    }

    size_t size() const noexcept(true)
    {
        return _size;
    }

    void clear() noexcept(false)
    {
        ~HashMap();
    }

    _Iterator begin() const noexcept(true)
    {
        return _Iterator(_capacity, map);
    }

    _Iterator end() const noexcept(true)
    {
        return _Iterator();
    }
    
protected:
    ::std::pair<_Iterator, bool> _find(const K& key) noexcept(false)
    {
        size_t _calced_hash = _hash(key);
        size_t i = _calced_hash;
        do
        {
            if (!map[i].exist || map[i].first != key) 
            {
                if (++i >= _capacity) i = 0ull;
                continue;
            }
            return { _Iterator(_capacity - i, map + i), true };
        } while(i != _calced_hash);
        return { _Iterator(_capacity, map), false };
    }

    size_t _hash(const K& key) const noexcept(true)
    {
        return _prov_hash(key) % _capacity;
    }
};

template<typename __T, typename I = ::std::enable_if_t<::std::is_default_constructible_v<__T>, void>>
class Array
{
    using T = ::std::decay_t<__T>;
private:
    size_t _size;
    size_t _capacity;
    T** arr;

public:
    class _Iterator
    {
    public:
        size_t _cap;
        T** ptr;

        _Iterator() noexcept(true) : _cap(0ull), ptr(nullptr) {}
        _Iterator(size_t acap, T** aptr) noexcept(true) : _cap(acap), ptr(aptr) {}

        ~_Iterator() noexcept(true)
        {
            _cap = 0ull;
            ptr = nullptr;
        }

        _Iterator operator+(size_t n) noexcept(true)
        {
            if ((_cap -= n) <= 0) return { 0ull, nullptr };
            return { _cap, (ptr + n) };
        }

        _Iterator& operator++() noexcept(true)
        {
            if (_cap - 1 <= 0)  ptr = nullptr;
            else
            {
                --_cap;
                ++ptr;
            }
            return *this;
        }

        bool operator!=(const _Iterator& inst) const noexcept(true)
        {
            return (ptr != inst.ptr);
        }

        operator T*() const noexcept(true)
        {
            return *ptr;
        }

        T* operator->()
        {
            return *ptr;
        }
    };

    _Iterator begin() const noexcept(true)
    {
        return _Iterator(_size, arr);
    }

    _Iterator end() const noexcept(true)
    {
        return _Iterator();
    }

    Array() noexcept(true);
    Array(const Array<T>& inst) noexcept(false);
    Array(Array<T>&& inst) noexcept(false);
    Array(size_t count, T data) noexcept(::std::is_copy_constructible_v<T>);
    ~Array() noexcept(false);
    Array<T>& operator=(const Array<T>& inst) noexcept(false);
    Array<T>& operator=(Array<T>&& inst) noexcept(false);

    T& operator[](size_t __idx) noexcept(false);
    T& back() noexcept(false);
    void reserve(size_t acapacity) noexcept(false);
    void shrink_to_size() noexcept(false);
    template<typename U>
    void emplace_back(U&& __instance)
    noexcept(::std::conjunction_v<
                ::std::is_same<::std::decay_t<U>, T>,
                ::std::disjunction<
                    ::std::conjunction<::std::is_lvalue_reference<U>, ::std::is_copy_constructible<U>>,
                    ::std::conjunction<::std::is_rvalue_reference<U>, ::std::is_move_constructible<U>>>>);
    template<typename U>
    void push_back(U&& __instance)
    noexcept(::std::conjunction_v<
                ::std::is_same<::std::decay_t<U>, T>,
                ::std::disjunction<
                    ::std::conjunction<::std::is_lvalue_reference<U>, ::std::is_copy_constructible<U>>,
                    ::std::conjunction<::std::is_rvalue_reference<U>, ::std::is_move_constructible<U>>>>);
    void erase(size_t __idx) noexcept(false);
    void clear() noexcept(false);
    size_t size() const noexcept(true);
    size_t capacity() const noexcept(true);
};

template<typename __K, typename __V>
class MultiHashMap : public HashMap<__K, __V>
{
    using K = ::std::decay_t<__K>;
    using V = ::std::decay_t<__V>;

public:
    using _Iterator = typename HashMap<K, V>::_Iterator;

    MultiHashMap() : HashMap<K, V>() {}
    MultiHashMap(size_t acapacity, double kf_overflow = __KF) noexcept(false) : HashMap<K, V>(acapacity, kf_overflow) {}

    template<typename _K, typename _V,
        typename _I = ::std::enable_if_t<::std::conjunction<::std::is_same<K, ::std::decay_t<_K>>, ::std::is_same<V, ::std::decay_t<_V>>>::value, void>>
    inline HashElem<K, V>& insert(_K&& key, _V&& value) noexcept(false)
    {
        return this->insert(::std::forward(key), ::std::forward(value), 1);
    }

    void erase(const K& key) noexcept(false)
    {
        while(true)
        {
            auto res = _find(key);
            if (res.second)
            {
                res.first->exist = false;
                --this->_size;
            }
            else break;
        }
    }

    void resize() noexcept(false)
    {
        this->resize(1);
    }

    Array<HashElem<K, V>> getElemsByKey(K key)
    {
        Array<HashElem<K, V>> res;
        res.reserve(this->_size);
        size_t _calced_hash = _hash(key);
        for(; _calced_hash < this->_capacity; ++_calced_hash)
        {
            if (this->map[_calced_hash].exist && this->map[_calced_hash].first == key)
            {
                res.emplace_back(this->map[_calced_hash]);
            }
        }
        res.shrink_to_size();
        return res;
    }

    size_t countElemByKey(K key)
    {
        size_t res = 0ull;
        size_t _calced_hash = _hash(key);
        for(; _calced_hash < this->_capacity; ++_calced_hash)
        {
            if (this->map[_calced_hash].exist && this->map[_calced_hash].first == key) ++res;
        }
        return res;
    }
};

template<typename T1,typename T2, typename... Args_T>
void __mA(::std::istream& is, Args_T... args)
{
    HashMap<T1, T2> map(args...);
    ::std::string str;
    T1 arg1;
    T2 arg2;
    size_t n;
    is >> n;
    for(size_t i {0}; i < n; ++i)
    {
        is >> str;
        is >> arg1;
        if (str == "A")
        {
            is >> arg2;
            map.insert(::std::move(arg1), ::std::move(arg2));
        }
        else if (str == "R")
        {
            map.erase(::std::move(arg1));
        }
        else throw ::std::runtime_error("[EXP] unexpected parametr!");
    }
    ::std::ofstream out("output.txt");
    out << map.size() << ' ';
    size_t idxVal = 0ull;
    T2* checked_values = new T2[map.size()];
    for(auto& x : map)
    {
        bool found = false;
        for(size_t i {0}; i < idxVal; ++i)
        {
            if (x.second == checked_values[i])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            checked_values[idxVal++] = x.second;
        }
    }
    out << idxVal << ::std::endl;
    delete[] checked_values;
}

template<typename T1, typename... Args_T>
void _mA(::std::istream& is, const ::std::string& s2, Args_T... args)
{
    if (s2 == "I")
    {
        __mA<T1, int>(is, args...);
    }
    else if (s2 == "D")
    {
        __mA<T1, double>(is, args...);
    }
    else if (s2 == "S")
    {
        __mA<T1, ::std::string>(is, args...);
    }
}

template<typename... Args_T>
void mA(::std::istream& is, const ::std::string& s1, const ::std::string& s2, Args_T... args)
{
    if (s1 == "I")
    {
        _mA<int>(is, s2, args...);
    }
    else if (s1 == "D")
    {
        _mA<double>(is, s2, args...);
    }
    else if (s1 == "S")
    {
        _mA<::std::string>(is, s2, args...);
    }
}

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
    ::std::hash<char> _prov_hash;

    // _str is null-terminated array of char
    void __add_str(const char* _str) noexcept(false);
    void __shrink_to_size();
    void __init(size_t _szelem, size_t _szbyte) noexcept(false);

public:
    dxString(__type atype = __type::ANSII) noexcept(true);
    dxString(const char* astr, __type atype = __type::ANSII) noexcept(false);
    dxString(const dxString& dxstr) noexcept(false);
    dxString(dxString&& dxstr) noexcept(false);
    ~dxString() noexcept(false);

    // special for ex3
    // reads and appends following 'line' from file (until '\n' is encountered)
    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_istream<_C, _T>, C<_C, _T>>,
        ::std::is_same<char, _C>>, C<_C, _T>&>
    operator>>(C<_C, _T>& _stream, dxString& instance) noexcept(false)
    {
        instance.~dxString();
        size_t maxszbuf = _dxBUF_SIZE_INPUT_STREAM;                     // temporary szElem
        char* temp = new char[maxszbuf + 1];
        _stream.getline(temp, maxszbuf, '\n');
        instance += temp;
        instance.__shrink_to_size();
        instance = instance.delWhite();
        return _stream;
    }

    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_ostream<_C, _T>, C<_C, _T>>,
        ::std::is_same<_C, char>>, C<_C, _T>&>
    operator<<(C<_C, _T>& _stream, dxString& instance) noexcept(::std::is_lvalue_reference_v<C<_C, _T>>);
    dxString& operator=(const char* astr) noexcept(false);
    template<typename C>
    ::std::enable_if_t<::std::is_same_v<::std::decay_t<C>, dxString>, ::std::decay_t<C>&>
    operator=(C&& dxstr) noexcept(false);
    dxString& operator=(const dxString& dxstr) noexcept(false);

    // special for ex3
    bool operator==(const char* str) noexcept(false)
    {
        return this->operator==(dxString(str));
    }

    // special for ex3
    bool operator==(const dxString& dxstr) noexcept(false)
    {
        if (type != dxstr.type) return false;
        if (szByte != dxstr.szByte) return false;
        if (szElem != dxstr.szElem) return false;
        for(size_t i {0}; i < szByte; ++i)
        {
            if (str[i] != dxstr.str[i]) return false;
        }
        return true;
    }

    dxString operator+(const char* arg) const noexcept(false);
    dxString& operator+=(const char* arg) noexcept(false);
    dxString operator+(const dxString& arg) const noexcept(false);
    dxString& operator+=(const dxString& arg) noexcept(false);
    dxString operator*(size_t n) const noexcept(false);
    dxString& operator*=(size_t n) noexcept(false);
    dxString operator-(const char* arg) const noexcept(false);

    // special for ex3
    dxString delWhite() const noexcept(false)
    {
        size_t arg_size = 1ull;
        dxString res;
        res.__init(szElem, szByte);
        const char* cur = str;
        const char* prev = str;
        size_t real_size = 0ull;
        while((cur = find_substr(cur, " ")) == str || (cur == str + szByte - 1))
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

    dxString& operator-=(const char* arg) noexcept(false);
    dxString operator-(const dxString& arg) const noexcept(false);
    dxString& operator-=(const dxString& arg) noexcept(false);
    // renamed special for ex3 (it was operator())
    dxString getSubstr(size_t pos, size_t n) const noexcept(false);
    char& operator[](size_t n) const noexcept(false);
    operator const char*() const noexcept(false);

    // special for ex3
    size_t operator()() const noexcept(false)
    {
        const size_t prime = /*92821ull*/ 31ull;
        size_t res = 0ull;
        for(size_t i {0}; i < szByte; ++i)
        {
            res += _prov_hash(str[i]) * ::std::pow(prime, i);
        }
        res += ::std::pow(prime, szByte);
        return res;
    }

    bool insert(const char* arg, size_t pos) noexcept(false);
    dxString& replace_substr(const char* s2find, const char* s2replace) noexcept(false);
    dxString& replace_substr(const dxString& s2find, const dxString& s2replace) noexcept(false);
    __type getType() const noexcept(true);
    void setType(__type atype) const noexcept(true);
    size_t size() const noexcept(true);
    size_t length() const noexcept(true);
};

int main()
{
    ::std::string s1, s2;
    ::std::ifstream read("input.txt");
    read >> s1;
    read >> s2;
    ER_IFN(read.is_open(), ::std::cerr << "[ERR] cannot open the file!" << ::std::endl;, return 1; )
    mA(read, s1, s2, 10);

    return 0;
}