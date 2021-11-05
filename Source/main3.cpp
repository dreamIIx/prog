#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <type_traits>
#include <exception>

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

#define __KF (static_cast<double>(0.60))

template<typename __K, typename __V,
    typename I = ::std::enable_if_t<::std::conjunction<::std::is_default_constructible<__K>, ::std::is_default_constructible<__V>>::value, void>>
class HashMap
{
    using K = ::std::decay_t<__K>;
    using V = ::std::decay_t<__V>;

    struct Pair
    {
        bool exist;
        K first;
        V second;

        Pair() noexcept(true) : exist(false), first(), second() {}

        Pair(const Pair& inst) noexcept(true)
        {
            *this = inst;
        }

        Pair(Pair&& inst) noexcept(true)
        {
            *this = inst;
        }

        Pair& operator=(const Pair& inst) noexcept(true)
        {
            exist = inst.exist;
            first = inst.first;
            second = inst.second;
            return *this;
        }

        Pair& operator=(Pair&& inst) noexcept(true)
        {
            exist = ::std::move(inst.exist);
            first = ::std::move(inst.first);
            second = ::std::move(inst.second);
            return *this;
        }

        template<typename _K, typename _V, typename _I = ::std::enable_if_t<::std::conjunction<
            ::std::is_same<K, _K>, ::std::is_same<V, _V>/*, ::std::is_move_constructible<K>, ::std::is_move_constructible<V>*/>::value, void>>
        Pair(_K&& akey, _V&& avalue) noexcept(true) : exist(true), first(::std::forward<K>(akey)), second(::std::forward<V>(avalue)) {}

        template<typename _K, typename _V, typename _I = ::std::enable_if_t<::std::conjunction<
            ::std::is_same<K, _K>, ::std::is_same<V, _V>/*, ::std::is_copy_constructible<K>, ::std::is_copy_constructible<V>*/>::value, void>>
        Pair(const _K& akey, const _V& avalue) noexcept(true) : exist(true), first(akey), second(avalue) {}
    };
    size_t _size;
    size_t _capacity;
    Pair* map;
    ::std::hash<K> _prov_hash;

public:
    class _Iterator
    {
    public:
        size_t _cap;
        Pair* ptr;

        _Iterator() noexcept(true) : _cap(0ull), ptr(nullptr) {}

        _Iterator(size_t acap, Pair* aptr) noexcept(true) : _cap(acap), ptr(aptr)
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
            Pair* temp_ptr = ptr;
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

        operator Pair*() const noexcept(true)
        {
            return ptr;
        }

        Pair* operator->()
        {
            return ptr;
        }

    };

    HashMap() = delete; // _capacity = 0 not allowed

    HashMap(size_t acapacity) noexcept(false) : _size(0ull), _capacity(acapacity), map(new Pair[acapacity]) {}

    template<typename _K, typename _V,
        typename _I = ::std::enable_if_t<::std::conjunction<::std::is_same<K, ::std::decay_t<_K>>, ::std::is_same<V, ::std::decay_t<_V>>>::value, void>>
    Pair& insert(_K&& key, _V&& value) noexcept(false)
    {
        ER_IF(!_capacity,, throw ::std::length_error("[EXC] HashMap capacity is null!"); )
        auto _calced_hash = _hash(key);
        bool placed = false;
        for(; _calced_hash < _capacity; ++_calced_hash)
        {
            if (map[_calced_hash].exist && map[_calced_hash].first != key) continue;
            else
            {
                if (!map[_calced_hash].exist) ++_size;
                map[_calced_hash] = Pair(::std::forward<_K>(key), ::std::forward<_V>(value));
                placed = true;
                break;
            }
        }
        resize();
        if (!placed)    return insert(::std::forward<_K>(key), ::std::forward<_V>(value));
        return map[_calced_hash];
    }
    
    V& operator[](const K& key) noexcept(false)
    {
        return _find(key).second;
    }

    void erase(const K& key) noexcept(false)
    {
        try
        {
            _find(key).exist = false;
            --_size;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void resize() noexcept(false)
    {
        if (_size >= _capacity * __KF)
        {
            size_t oldcap = _capacity;
            _capacity *= 2ull;
            Pair* temp = new Pair[_capacity];
            ::std::swap(map, temp);
            for(size_t i {0}; i < oldcap; ++i)
            {
                if (map[i].exist)  temp[i] = map[i];
            }
            _size = 0ull;
            for(size_t i {0}; i < oldcap; ++i)
            {
                if (temp[i].exist)  insert(static_cast<K>(temp[i].first), static_cast<V>(temp[i].second));
            }
            delete[] temp;
        }
    }

    ~HashMap() noexcept(false)
    {
        delete[] map;
        _size = 0ull;
        _capacity = 0ull;
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
    
private:
    Pair& _find(const K& key) noexcept(/*::std::equality_comparable<K>*/false)
    {
        auto _calced_hash = _hash(key);
        for(; _calced_hash < _capacity; ++_calced_hash)
        {
            if (!map[_calced_hash].exist || map[_calced_hash].first != key) continue;
            return map[_calced_hash];
        }
        throw ::std::runtime_error("[EXC] a pair with the required key is not exist!");
    }

    size_t _hash(const K& key) const noexcept(true)
    {
        return _prov_hash(key) % _capacity;
    }
};

template<typename T1,typename T2, typename... Args_T>
void _mA(::std::istream& is, Args_T... args)
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
    typename HashMap<T1, T2>::_Iterator iter = map.begin();
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

template<typename... Args_T>
void mA(::std::istream& is, ::std::string& s1, ::std::string s2, Args_T... args)
{
    if (s1 == "I")
    {
        if (s2 == "I")
        {
            _mA<int, int>(is, 10);
        }
        else if (s2 == "D")
        {
            _mA<int, double>(is, 10);
        }
        else if (s2 == "S")
        {
            _mA<int, ::std::string>(is, 10);
        }
    }
    else if (s1 == "D")
    {
        if (s2 == "I")
        {
            _mA<double, int>(is, 10);
        }
        else if (s2 == "D")
        {
            _mA<double, double>(is, 10);
        }
        else if (s2 == "S")
        {
            _mA<double, ::std::string>(is, 10);
        }
    }
    else if (s1 == "S")
    {
        if (s2 == "I")
        {
            _mA<::std::string, int>(is, 10);
        }
        else if (s2 == "D")
        {
            _mA<::std::string, double>(is, 10);
        }
        else if (s2 == "S")
        {
            _mA<::std::string, ::std::string>(is, 10);
        }
    }
}

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