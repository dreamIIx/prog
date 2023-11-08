///
// TreasureHunt
// Compile with:
// g++ -c -Wall -std=c++20 -ftree-vectorize -mavx
//
// Author:  Shibanov Evgeny
// Email:   shiba.zheka65@gmail.com
///

#include <iostream>
#include <vector>
#include <queue>
#include <math.h>
#include <type_traits>
#include <limits>

#define _EPS 1e-8

// simplified special class for this task
struct Point
{
    enum class state : unsigned
    {
        def = 0,
        undef
    };

    state is;
    double x;
    double y;

    Point() noexcept(true) : is(state::undef), x(0.), y(0.) {}
    Point(double ax, double ay) noexcept(true) : is(state::undef)
    {
        if (ax >= 0 && ax <= 100 && ay >= 0 && ay <= 100)
        {
            x = ax;
            y = ay;
            is = state::def;
        }
    }

    operator bool() noexcept(true)
    {
        return (is == state::def) ? true : false;
    }

    bool operator==(const ::Point& p) noexcept(true)
    {
        return (p.is == is) && (abs(p.x - x) <= 1e-7) && (abs(p.y - y) <= 1e-7);
    }

};

// simplified special class for this task
class Vector3D
{
public:
    enum class _state : int
    {
        undef = -1,
        def = 1
    };

    _state state;
    double x;
    double y;
    // it is need to defined a cross product
    double z;

    Vector3D() noexcept(true) : state(_state::undef) {}
    Vector3D(double ax, double ay, double az) noexcept(true) : state(_state::def), x(ax), y(ay), z(az) {}

    operator bool() const noexcept(true)
    {
        return (state == _state::def) && (::std::abs(x) > _EPS || ::std::abs(y) > _EPS || ::std::abs(z) > _EPS);
    }

    ::Vector3D operator-() const noexcept(true)
    {
        return ::Vector3D(-x, -y, -z);
    }

    ::Vector3D operator+(const ::Vector3D& v) const noexcept(true)
    {
        return ::Vector3D(x + v.x, y + v.y, z + v.z);
    }

    ::Vector3D operator-(const ::Vector3D& v) const noexcept(true)
    {
        return this->operator+(-v);
    }

    bool operator==(const ::Vector3D& v) const noexcept(true)
    {
        return ::std::abs(v.x - x) < _EPS && ::std::abs(v.y - y) < _EPS && ::std::abs(v.z - z) < _EPS;
    }

    // scalar-product
    ::Vector3D operator*(double a) const noexcept(true)
    {
        return ::Vector3D(x * a, y * a, z * a);
    }

    inline static double dotProd(const ::Vector3D& v1, const ::Vector3D& v2) noexcept(true)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline static double norm2(const ::Vector3D& v) noexcept(true)
    {
        return dotProd(v, v);
    }

    inline static ::Vector3D crossProd(const ::Vector3D& v1, const ::Vector3D& v2) noexcept(true)
    {
        return ::Vector3D((v1.y * v2.z - v1.z * v2.y), -(v1.x * v2.z - v1.z * v2.x), (v1.x * v2.y - v1.y * v2.x));
    }

    inline static int isColinear(const ::Vector3D& v1, const ::Vector3D& v2) noexcept(true)
    {
        return v1 && v2 && !::Vector3D::crossProd(v1, v2);
    }

};

template<typename T>
concept Vector3D_t = ::std::is_same_v<::std::decay_t<T>, ::Vector3D>;

// simplified special class for this task
class Segment3D
{
public:
    ::Vector3D start;
    ::Vector3D end;

    Segment3D() = delete;
    Segment3D(Vector3D_t auto&& a, Vector3D_t auto&& b) noexcept(true) : start(::std::forward<decltype(a)>(a)), end(::std::forward<decltype(b)>(b)) {}

    bool operator==(const ::Segment3D& s) const noexcept(true)
    {
        return (start == s.start) && (end == s.end);
    }

};

// simplified method for non-zero segments
::Vector3D intersectOf2Segments(const ::Segment3D& s1, const ::Segment3D& s2) noexcept(true);

int main()
{
    size_t n;
    ::std::cin >> n;
    ::std::vector<::Segment3D> vSeg;
    vSeg.reserve(n);
    for(size_t i {0}; i < n; ++i)
    {
        int x1, x2, y1, y2;
        ::std::cin >> x1 >> y1 >> x2 >> y2;
        vSeg.emplace_back(::Vector3D(static_cast<double>(x1), static_cast<double>(y1), 0.), ::Vector3D(static_cast<double>(x2), static_cast<double>(y2), 0.));
    }
    double x, y;
    ::std::cin >> x >> y;
    ::Point treasure(x, y);
    size_t res_count = ::std::numeric_limits<decltype(res_count)>::max();
    for(size_t i {0}; i < 100ull; ++i)
    {
        size_t cur_count = 1ull;
        for(size_t j {0}; j < vSeg.size(); ++j)
        {
            if (intersectOf2Segments(
                ::Segment3D(::Vector3D(treasure.x, treasure.y, 0.), ::Vector3D(static_cast<double>(i), 0., 0.)),
                vSeg[j]))   ++cur_count;
        }
        if (cur_count < res_count) res_count = cur_count;
    }
    for(size_t i {0}; i < 100ull; ++i)
    {
        size_t cur_count = 1ull;
        for(size_t j {0}; j < vSeg.size(); ++j)
        {
            if (intersectOf2Segments(
                ::Segment3D(::Vector3D(treasure.x, treasure.y, 0.), ::Vector3D(100., static_cast<double>(i), 0.)),
                vSeg[j]))   ++cur_count;
        }
        if (cur_count < res_count) res_count = cur_count;
    }
    for(ptrdiff_t i {100}; i > 0ll; --i)
    {
        size_t cur_count = 1ull;
        for(size_t j {0}; j < vSeg.size(); ++j)
        {
            if (intersectOf2Segments(
                ::Segment3D(::Vector3D(treasure.x, treasure.y, 0.), ::Vector3D(static_cast<double>(i), 100., 0.)),
                vSeg[j]))   ++cur_count;
        }
        if (cur_count < res_count) res_count = cur_count;
    }
    for(ptrdiff_t i {100}; i > 0ll; --i)
    {
        size_t cur_count = 1ull;
        for(size_t j {0}; j < vSeg.size(); ++j)
        {
            if (intersectOf2Segments(
                ::Segment3D(::Vector3D(treasure.x, treasure.y, 0.), ::Vector3D(0., static_cast<double>(i), 0.)),
                vSeg[j]))   ++cur_count;
        }
        if (cur_count < res_count) res_count = cur_count;
    }

    ::std::cout << res_count;

    return 0;
}

// simplified method for non-zero complanar segments
::Vector3D intersectOf2Segments(const Segment3D& s1, const Segment3D& s2) noexcept(true)
{
    ::Vector3D a1(s2.start - s1.start);
    //::Vector3D a2(s2.end - s1.start);
    ::Vector3D v1(s1.end - s1.start);
    ::Vector3D v2(s2.end - s2.start);
    // for non-colinear segments (zero-segments are not coliear)
    if (!::Vector3D::isColinear(v1, v2))
    {
        double s = ::Vector3D::dotProd(::Vector3D::crossProd(a1, v2), ::Vector3D::crossProd(v1, v2)) /
            ::Vector3D::norm2(::Vector3D::crossProd(v1, v2));
        double t = ::Vector3D::dotProd(::Vector3D::crossProd(a1, v1), ::Vector3D::crossProd(v1, v2)) /
            ::Vector3D::norm2(::Vector3D::crossProd(v1, v2));
        if ((s > -_EPS && s - 1. < _EPS) && (t > -_EPS && t - 1. < _EPS))
        {
            return ::Vector3D(s1.start + v1 * s);
        }
    }
    return ::Vector3D();
}