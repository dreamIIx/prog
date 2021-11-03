#include <iostream>
#include <iomanip>
#include <fstream>
#include <type_traits>
#include <exception>
#include <functional>
#include <cmath>

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

#define KF_BIG_DATA 4095

template<typename __T, typename I = ::std::enable_if_t<::std::is_default_constructible_v<__T>, void>>
class Array
{
    using T = ::std::decay_t<__T>;
private:
    size_t _size;
    size_t _capacity;
    T** arr;

public:
    Array() noexcept(true) : _size(0ull), _capacity(0ull), arr(nullptr) {}

    Array(const Array<T>& inst) noexcept(false)
    {
        *this = inst;
    }
    
    Array(size_t count, T data) noexcept(::std::is_copy_constructible_v<T>) : _size(0ull), _capacity(count), arr(new T*[count])
    {
        for(size_t i {0}; i < count; ++i)
        {
            arr[i] = new T(data);
        }
        _size = count;
    }

    ~Array() noexcept(false)
    {
        for(size_t i {0}; i < _size; ++i)
        {
            delete arr[i];
        }
        delete[] arr;
        _size = 0ull;
        _capacity = 0ull;
    }

    Array<T>& operator=(const Array<T>& inst) noexcept(false)
    {
        _size = inst._size;
        _capacity = inst._capacity;
        arr = new T*[_capacity];
        for(size_t i {0}; i < _size; ++i)
        {
            arr[i] = new T(*inst.arr[i]);
        }
        return *this;
    }

    T& operator[](size_t __idx) noexcept(false)
    {
        ER_IF(__idx >= _size,, throw ::std::out_of_range("out of range in Array::operator[]"); )
        return *arr[__idx];
    }

    T& back() noexcept(false)
    {
        ER_IF(!_size,, throw ::std::range_error("Array is empty"); )
        return *arr[_size - 1];
    }

    void reserve(size_t acapacity) noexcept(false)
    {
        if (acapacity <= _capacity) return;
        T** temp = new T*[acapacity];
        for(size_t i {0}; i < _size; ++i)
        {
            temp[i] = arr[i];
        }
        delete[] arr;
        arr = temp;
        _capacity = acapacity;
    }

    void shrink_to_size() noexcept(false)
    {
        size_t countOfNonNullptrPtr = 0ull;
        for(size_t i {0}; i < _size; ++i)
        {
            if (arr[i] != nullptr)
            {
                ++countOfNonNullptrPtr;
            }
        }
        if (countOfNonNullptrPtr >= KF_BIG_DATA)    _capacity = countOfNonNullptrPtr + 1;
        else                                        _capacity = countOfNonNullptrPtr * 2;
        T** temp = new T*[_capacity];
        size_t j = 0ull;
        for(size_t i {0}; i < countOfNonNullptrPtr; ++i)
        {
            while (arr[j] == nullptr)   ++j;
            temp[i] = arr[j++];
        }
        delete[] arr;
        arr = temp;
        _size = countOfNonNullptrPtr;
    }

    template<typename U>
    void emplace_back(U&& __instance)
    noexcept(::std::conjunction_v<
                ::std::is_same<::std::decay_t<U>, T>,
                ::std::disjunction<
                    ::std::conjunction<::std::is_lvalue_reference<U>, ::std::is_copy_constructible<U>>,
                    ::std::conjunction<::std::is_rvalue_reference<U>, ::std::is_move_constructible<U>>>>)
    {
        arr[_size++] = new T(::std::forward<T>(__instance));
    }

    template<typename U>
    void push_back(U&& __instance)
    noexcept(::std::conjunction_v<
                ::std::is_same<::std::decay_t<U>, T>,
                ::std::disjunction<
                    ::std::conjunction<::std::is_lvalue_reference<U>, ::std::is_copy_constructible<U>>,
                    ::std::conjunction<::std::is_rvalue_reference<U>, ::std::is_move_constructible<U>>>>)
    {
        if (_size == _capacity)
        {
            if (!_capacity) reserve(1ull);
            else            reserve(_capacity * 2);
        }
        emplace_back(::std::forward<T>(__instance));
    }

    void erase(size_t __idx) noexcept(false)
    {
        ER_IF(__idx >= _size, ::std::cerr << "out of range in Array::erase()" << ::std::endl;, return; )
        delete arr[__idx];
        arr[__idx] = nullptr;
    }

    void clear() noexcept(false)
    {
        ~Array();
    }

    size_t size() const noexcept(true)
    {
        return _size;
    }
    
    size_t capacity() const noexcept(true)
    {
        return _capacity;
    }
};

struct vect;

struct coord
{
    signed x;
    signed y;

    coord() noexcept(true) : x(0), y(0) {}
    coord(signed _x, signed _y) noexcept(true) : x(_x), y(_y) {}

    bool operator==(const coord&) noexcept(true);
    coord operator+(const vect&) noexcept(true);
    coord& operator+=(const vect&) noexcept(true);
};

struct vect
{
    signed x;
    signed y;
    
    vect() noexcept(true) : x(0), y(0) {}
    vect(signed _x, signed _y) noexcept(true) : x(_x), y(_y) {}
    vect(coord c1, coord c2) noexcept(true) : x(c2.x - c1.x), y (c2.y - c1.y) {}
    // as if { c2 - coord() }
    vect(coord c) noexcept(true) : x(c.x), y (c.y) {}

    bool operator==(const vect& instance) noexcept(true)
    {
        return (this->x == instance.x) && (this->y == instance.y);
    }

    vect operator+(const vect& instance) noexcept(true)
    {
        return { this->x + instance.x, this->y + instance.y };
    }

    vect operator+(const coord& instance) noexcept(true)
    {
        return { this->x + instance.x, this->y + instance.y };
    }

    vect& operator+=(const vect& instance) noexcept(true)
    {
        *this = this->operator+(instance);
        return *this;
    }

    vect& operator+=(const coord& instance) noexcept(true)
    {
        *this = this->operator+(instance);
        return *this;
    }

    vect operator*(signed n) noexcept(true)
    {
        return { this->x * n, this->y * n };
    }

    vect operator*=(signed n) noexcept(true)
    {
        *this = this->operator*(n);
        return *this;
    }
};

bool coord::operator==(const coord& instance) noexcept(true)
{
    return (this->x == instance.x) && (this->y == instance.y);
}

coord coord::operator+(const vect& instance) noexcept(true)
{
    return { this->x + instance.x, this->y + instance.y };
}

coord& coord::operator+=(const vect& instance) noexcept(true)
{
    *this = this->operator+(instance);
    return *this;
}

class Animal
{
    friend class Simulation;
public:
    coord pos;
    vect direct;
    unsigned speed;
    unsigned constancy;
    unsigned cur_constancy;
    unsigned age;

protected:
    enum class __type : signed
    {
        __Hyena = -2,
        __Wolf,
        __None,
        __Rabbit = 1
    };

public:
    Animal() noexcept(true) {}
    virtual ~Animal() noexcept(true) {}
    virtual __type getType() = 0;

    virtual __type getForbidType() noexcept(true)
    {
        return __type::__None;
    };

protected:
    virtual void move(size_t x, size_t y) noexcept(true)
    {
        pos += direct * speed;
        if (pos.x < 0)                                      pos.x = x - -pos.x % x;
        else if (pos.x >= static_cast<decltype(pos.x)>(x))  pos.x %= x;
        if (pos.y < 0)                                      pos.y = y - -pos.y % y;
        else if (pos.y >= static_cast<decltype(pos.y)>(y))  pos.y %= y;
        ++cur_constancy;
        if (cur_constancy >= constancy)
        {
            cur_constancy = 0;
            ::std::swap(direct.x, direct.y);
            direct.x = -direct.x;
        }
    }

    virtual bool reproduction() = 0;
    virtual void engine(::Array<Animal*>&, size_t) = 0;
    virtual bool extinction() = 0;
    virtual Animal* born() = 0;

    void aging() noexcept(true)
    {
        ++age;
    }
};

class Rabbit : public Animal
{
public:
    Rabbit(coord apos, coord d, unsigned cntcy) noexcept(true)
    {
        pos = apos;
        direct = d;
        speed = 1;
        constancy = cntcy;
        cur_constancy = 0u;
        age = 0u;
    }

    bool reproduction() noexcept(true) override
    {
        return (age == 5 || age == 10);
    }

    void engine(::Array<Animal*>&, size_t) {}

    bool extinction() noexcept(true) override
    {
        if (age >= 10) return true;
        return false;
    }

    Animal* born() noexcept(false) override
    {
        return new Rabbit(*this);
    }

    __type getType() noexcept(true) override
    {
        return __type::__Rabbit;
    }
};

class Wolf : public Animal
{
public:
    unsigned satiety;

    Wolf(coord apos, coord d, unsigned cntcy) noexcept(true)
    {
        pos = apos;
        direct = d;
        speed = 2;
        constancy = cntcy;
        cur_constancy = 0u;
        age = 0u;
        satiety = 0u;
    }

    virtual ~Wolf() noexcept(true) override {}

    virtual bool reproduction() noexcept(true) override
    {
        return (satiety >= 2);
    }

    void __spec_engine(::Array<Animal*>& arrUnit, size_t i) noexcept(false)
    {
        for(size_t j {0}; j < arrUnit.size(); ++j)
        {
            if (i != j && arrUnit[j] != nullptr && arrUnit[j]->getType() != getForbidType())
            {
                if (arrUnit[j]->pos == pos)
                {
                    delete arrUnit[j];
                    arrUnit[j] = nullptr;
                    ++satiety;
                }
            }
        }
    }

    void engine(::Array<Animal*>& arrUnit, size_t i) noexcept(false) override
    {
        __spec_engine(arrUnit, i);
    }

    virtual bool extinction() noexcept(true) override
    {
        return (age >= 15);
    }

    Animal* born() noexcept(false) override
    {
        satiety = 0u;
        return new Wolf(*this);
    }

    virtual __type getType() noexcept(true) override
    {
        return __type::__Wolf;
    }

    virtual __type getForbidType() noexcept(true) override
    {
        return __type::__Wolf;
    }
};

class Hyena : public Wolf
{
public:
    unsigned satiety;

    Hyena(coord apos, coord d, unsigned cntcy) noexcept(true) : Wolf(apos, d, cntcy) {}
    ~Hyena() noexcept(true) override {}

    void engine(::Array<Animal*>& arrUnit, size_t i) noexcept(false) override
    {
        if (satiety < 2)
        {   
            __spec_engine(arrUnit, i);
        }
    }

    Animal* born() noexcept(false) override
    {
        satiety = 0u;
        return new Hyena(*this);
    }

    __type getType() noexcept(true) override
    {
        return __type::__Hyena;
    }
};

class Simulation
{
private:
    const size_t sizex;
    const size_t sizey;
    ::Array<Animal*> arrUnit;

public:
    Simulation() noexcept(true) : sizex(0ull), sizey(0ull), arrUnit() {}
    Simulation(size_t asizex, size_t asizey) noexcept(true) : sizex(asizex), sizey(asizey), arrUnit() {}

    void addUnit(Animal* inst) noexcept(false)
    {
        arrUnit.push_back(inst);
    }

    void engine(size_t loopsCount) noexcept(false)
    {
        for(size_t loop {0}; loop < loopsCount; ++loop)
        {
            for(size_t i {0}; i < arrUnit.size(); ++i)
            {
                arrUnit[i]->move(sizex, sizey);
            }
            for(size_t i {0}; i < arrUnit.size(); ++i)
            {
                if (arrUnit[i] != nullptr)
                {
                    arrUnit[i]->engine(arrUnit, i);
                }
            }
            for(size_t i {0}; i < arrUnit.size(); ++i)
            {
                if (arrUnit[i] == nullptr)  arrUnit.erase(i);
            }
            arrUnit.shrink_to_size();
            for(size_t i {0}; i < arrUnit.size(); ++i)
            {
                arrUnit[i]->aging();
            }
            size_t szUnit = arrUnit.size();
            for(size_t i {0}; i < szUnit; ++i)
            {
                if (arrUnit[i]->reproduction())
                {
                    Animal* newUnit = arrUnit[i]->born();
                    newUnit->age = 0ull;
                    addUnit(newUnit);
                }
            }
            for(size_t i {0}; i < arrUnit.size(); ++i)
            {
                if (arrUnit[i]->extinction())
                {
                    delete arrUnit[i];
                    arrUnit[i] = nullptr;
                    arrUnit.erase(i);
                }
            }
            arrUnit.shrink_to_size();
        }
    }

    void printField(::std::ostream& os) noexcept(false)
    {
        ::Array<coord> field;
        ::Array<int> numOnCell;
        field.reserve(arrUnit.size());
        numOnCell.reserve(arrUnit.size());
        for(size_t i {0}; i < arrUnit.size(); ++i)
        {
            bool found = false;
            for(size_t j {0}; j < field.size(); ++j)
            {
                if (arrUnit[i]->pos.x == field[j].x && arrUnit[i]->pos.y == field[j].y)
                {
                    numOnCell[j] += static_cast<signed>(arrUnit[i]->getType());
                    found = true;
                }
            }
            if (!found)
            {
                field.emplace_back(arrUnit[i]->pos);
                numOnCell.emplace_back(static_cast<signed>(arrUnit[i]->getType()));
            }
        }
        for(ptrdiff_t y {0}; y < sizey; ++y)
        {
            for(ptrdiff_t x {0}; x < sizex; ++x)
            {
                bool exist = false;
                for(size_t i {0}; i < field.size(); ++i)
                {
                    if (field[i].x == x && field[i].y == y)
                    {
                        exist = true;
                        os << numOnCell[i];
                    }
                }
                if (!exist) os << '#';
            }
            os << ::std::endl;
        }
    }
    
    ~Simulation() noexcept(false)
    {
        for(size_t i {0}; i < arrUnit.size(); ++i)
        {
            delete arrUnit[i];
        }
    }
};

void addUnit2Simulation(::std::ifstream& read, Simulation& sm, size_t count, int flag);

int main()
{
    ::std::ifstream read("input.txt");
    ER_IFN(read.is_open(), ::std::cerr << "cannot open the file" << ::std::endl;, return 1; )

    size_t ax;
    size_t ay;
    read >> ax;
    read >> ay;
    Simulation test(ax, ay);
    size_t count, szR, szW;
    read >> count;
    read >> szR;
    read >> szW;
    addUnit2Simulation(read, test, szR, 0);
    addUnit2Simulation(read, test, szW, 1);
    test.engine(count);
    test.printField(::std::cout);

    return 0;
}

void addUnit2Simulation(::std::ifstream& read, Simulation& sm, size_t count, int flag)
{
    for(size_t i {0}; i < count; ++i)
    {
        coord tempPos;
        coord tempDir;
        size_t direction;
        size_t c;
        read >> tempPos.x;
        read >> tempPos.y;
        read >> direction;
        read >> c;
        switch(direction)
        {
        case 0:
        {
            tempDir.x = 0;
            tempDir.y = -1;
            break;
        }
        case 1:
        {
            tempDir.x = 1;
            tempDir.y = 0;
            break;
        }
        case 2:
        {
            tempDir.x = 0;
            tempDir.y = 1;
            break;
        }
        case 3:
        {
            tempDir.x = -1;
            tempDir.y = 0;
            break;
        }
        default:
        {
            ERROR_
            break;
        }
        }
        Animal* temp = nullptr;
        switch(flag)
        {
        case 0:
        {
            temp = new Rabbit(tempPos, tempDir, c);
            break;
        }
        case 1:
        {
            temp = new Wolf(tempPos, tempDir, c);
            break;
        }
        default:
        {
            ERROR_
            break;
        }
        }
        sm.addUnit(temp);
    }
}