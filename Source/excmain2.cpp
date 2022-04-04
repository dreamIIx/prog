#include <iostream>
#include <vector>
#include <deque>
#include <chrono>
#include <math.h>

struct _Point
{
    enum class state : unsigned
    {
        def = 0,
        undef
    };

    int weight;
    state is;
    double x;
    double y;

    _Point() : weight(0), is(state::undef), x(0.), y(0.) {}
    _Point(double ax, double ay) : weight(0), is(state::undef)
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

};

struct Straight
{
    double a;
    double b;
    double c;

    Straight(double aa, double ab, double ac)   noexcept(true)  : a(aa), b(ab), c(ac) {}
    Straight(_Point f, _Point s)                noexcept(true)  : a(s.y - f.y), b(f.x - s.x), c(f.x * (f.y - s.y) + f.y * (s.x - f.x)) {}

};
    
/*_Point intersectionOfStraights(Straight line1, Straight line2) noexcept(false) try
{
    return _Point((line2.c - line1.c) / (line1.b / line1.a - line2.b / line2.a), (line1.c - line2.c) / (1 - (line2.b * line1.a) / (line2.a * line1.b)) + line1.c);
}
catch(...)
{
    return _Point();
}

_Point intersectionOfStraights(_Point f1, _Point s1, _Point f2, _Point s2) noexcept(false) try
{
    double det = (f1.x - s1.x) * (f2.y - s2.y) - (f1.y - s1.y) * (f2.x - s2.x);
    double delt1 = (f1.x * s1.y - f1.y * s1.x);
    double delt2 = (f2.x * s2.y - f2.y * s2.x);
    double x = (delt1 * (f2.x - s2.x) - (f1.x * s1.x) * delt2) / det;
    double y = (delt1 * (f2.y - s2.y) - (f1.y * s1.y) * delt2) / det;
    return _Point(x, y);
}
catch(...)
{
    return _Point();
}*/

struct Vec2d
{
    double x;
    double y;

    Vec2d(double ax, double ay) : x(ax), y(ay) {}
    Vec2d(_Point p1, _Point p2) : x(p2.x - p1.x), y(p2.y - p1.y) {}

};

struct Section
{
    _Point a;
    _Point b;

    Section(_Point aa, _Point ab) : a(aa), b(ab) {}

};

struct Vertex
{
    ::std::deque<_Point> dPoints;

    Vertex() {}

};

_Point intersectionOfRayAndStraight(_Point p, Vec2d v, Straight line) noexcept(false) try
{
    double res = (-line.a * p.x - line.b * p.y - line.c) / (line.a * v.x + line.b * v.y);
    if (res > 0.) return _Point(p.x + v.x * res, p.y + v.y * res);
    return _Point();
}
catch(...)
{
    return _Point();
}

_Point intersectionOfSectionAndStraight(_Point p1, _Point p2, Straight line) noexcept(false) try
{
    double res1 = (-line.a * p1.x - line.b * p1.y - line.c) / (line.a * (p2.x - p1.x) + line.b * (p2.y - p1.y));
    double res2 = (-line.a * p2.x - line.b * p2.y - line.c) / (line.a * (p1.x - p2.x) + line.b * (p1.y - p2.y));
    if (res1 > 0. && res2 > 0.) return _Point(p1.x + (p2.x - p1.x) * res1, p1.y + (p2.y - p1.y) * res1);
    return _Point();
}
catch(...)
{
    return _Point();
}

_Point intersectionOfRayAndSection(_Point p, Vec2d v, _Point p1, _Point p2) noexcept(false) try
{
    _Point temp;
    if (intersectionOfSectionAndStraight(p1, p2, Straight(p, _Point(p.x + v.x, p.y + v.y))) &&
        (temp = intersectionOfRayAndStraight(p, v, Straight(p1, p2)))) return temp;
    return _Point();
}
catch(...)
{
    return _Point();
}

::std::pair<size_t, _Point> nearestStraightToPoint(::std::vector<Straight>& vS, _Point& p) noexcept(false)
{
    size_t res_idx = 0ull;
    double distance = ::std::fabs(vS.front().a * p.x + vS.front().b * p.y + vS.front().c) / ::std::sqrt(::std::pow(vS.front().a, 2.) + ::std::pow(vS.front().b, 2.));
    for(size_t i = 1ull; i < vS.size(); ++i)
    {
        double tempDist = ::std::fabs(vS[i].a * p.x + vS[i].b * p.y + vS[i].c) / ::std::sqrt(::std::pow(vS[i].a, 2.) + ::std::pow(vS[i].b, 2.));
        if (distance > tempDist)
        {
            distance = tempDist;
            res_idx = i;
        }
    }
    auto& ref = vS.at(res_idx);
    double d = ::std::pow(ref.a, 2.) + ::std::pow(ref.b, 2.);
    return { res_idx, _Point((ref.b * (ref.b * p.x - ref.a * p.y) - ref.a * ref.c) / d, (ref.a * (-ref.b * p.x + ref.a * p.y) - ref.b * ref.c) / d) };
}

inline double lengthOfVector(_Point& a, _Point& b)
{
    return ::std::sqrt(::std::pow(b.x - a.x, 2.) + ::std::pow(b.y - a.y, 2.));
}

void splitVertexes(Straight&& line, ::std::vector<Vertex>& vtx)
{
    size_t temp = vtx.size();
    for(size_t i = 0ull; i < temp; ++i)
    {
        _Point p1, p2;
        ptrdiff_t _start = -1ll;
        int min = vtx[i].dPoints[vtx[i].dPoints.size() - 1].weight;
        int max = vtx[i].dPoints[vtx[i].dPoints.size() - 1].weight;
        if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints[vtx[i].dPoints.size() - 1], vtx[i].dPoints[0ull], line)))
        {
            //::std::cout << "Intersection! [" << vtx[i].dPoints[vtx[i].dPoints.size() - 1].x << ", " << vtx[i].dPoints[vtx[i].dPoints.size() - 1].y << "]-[" << vtx[i].dPoints[0ull].x << ", " << vtx[i].dPoints[0ull].y << ']' << ::std::endl;
            p1.weight = vtx[i].dPoints[vtx[i].dPoints.size() - 1].weight;
            _start = 0ull;
            //min = (max = p1.weight);
        }
        else
        {
            for(size_t j = 0ull; j < vtx[i].dPoints.size() - 1; ++j)
            {
                if (min > vtx[i].dPoints[j].weight) min = vtx[i].dPoints[j].weight;
                else if (max < vtx[i].dPoints[j].weight) max = vtx[i].dPoints[j].weight;
                if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
                {
                    //::std::cout << "Intersection! [" << vtx[i].dPoints[j].x << ", " << vtx[i].dPoints[j].y << "]-[" << vtx[i].dPoints[j + 1].x << ", " << vtx[i].dPoints[j + 1].y << ']' << ::std::endl;
                    _start = j + 1;
                    p1.weight = vtx[i].dPoints[j].weight;
                    break;
                }
            }
            if (!p1) continue;
        }
        for(size_t j = _start; j < vtx[i].dPoints.size() - 1; ++j)
        {
            if (min > vtx[i].dPoints[j].weight) min = vtx[i].dPoints[j].weight;
            else if (max < vtx[i].dPoints[j].weight) max = vtx[i].dPoints[j].weight;
            if ((p2 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
            {
                for(size_t k = j + 1; k < vtx[i].dPoints.size() - 1; ++k)
                {
                    if (min > vtx[i].dPoints[k].weight) min = vtx[i].dPoints[k].weight;
                    else if (max < vtx[i].dPoints[k].weight) max = vtx[i].dPoints[k].weight;
                }
                //::std::cout << "Intersection! [" << vtx[i].dPoints[j].x << ", " << vtx[i].dPoints[j].y << "]-[" << vtx[i].dPoints[j + 1].x << ", " << vtx[i].dPoints[j + 1].y << ']' << ::std::endl;
                vtx.reserve(vtx.capacity() + 1);
                vtx.emplace_back(Vertex());
                vtx.back().dPoints.push_back(p1);
                for(size_t k = _start; k < j + 1; ++k)
                {
                    vtx.back().dPoints.push_back(vtx[i].dPoints[k]);
                }
                vtx.back().dPoints.push_back(p2);
                p2.weight = vtx[i].dPoints[j].weight;
                vtx[i].dPoints.erase(vtx[i].dPoints.begin() + _start, j < vtx[i].dPoints.size() - 1 ? vtx[i].dPoints.begin() + j + 1 : vtx[i].dPoints.end());
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p2);
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p1);
                if (min == max)
                {
                    vtx.back().dPoints.back().weight = max + 1;
                    vtx[i].dPoints.at(_start).weight = max + 1;
                }
                else
                {
                    vtx.back().dPoints.back().weight = max;
                    vtx[i].dPoints.at(_start).weight = max;
                }
                for(size_t h = 0ull; h < vtx.back().dPoints.size(); ++h)
                {
                    ::std::cout << '[' << vtx.back().dPoints[h].x << ',' << vtx.back().dPoints[h].y << "](" << vtx.back().dPoints[h].weight << ")[";
                }
                ::std::cout << ::std::endl;
                for(size_t h = 0ull; h < vtx[i].dPoints.size(); ++h)
                {
                    ::std::cout << '[' << vtx[i].dPoints[h].x << ',' << vtx[i].dPoints[h].y << "](" << vtx[i].dPoints[h].weight << ")[";
                }
                ::std::cout << ::std::endl;
                break;
            }
        }
    }
}

int main()
{
    size_t n;
    ::std::cin >> n;
    ::std::vector<Vertex> vVertexes;
    vVertexes.reserve(1ull);
    vVertexes.emplace_back(Vertex());
    vVertexes.back().dPoints.push_back(_Point(0, 0));
    vVertexes.back().dPoints.push_back(_Point(0, 100));
    vVertexes.back().dPoints.push_back(_Point(100, 100));
    vVertexes.back().dPoints.push_back(_Point(100, 0));
    for(size_t i = 0ull; i < n; ++i)
    {
        int x1, x2, y1, y2;
        ::std::cin >> x1 >> y1 >> x2 >> y2;
        splitVertexes(Straight(_Point(static_cast<double>(x1), static_cast<double>(y1)), _Point(static_cast<double>(x2), static_cast<double>(y2))), vVertexes);
    }
    _Point treasure;
    {
        double x, y;
        ::std::cin >> x >> y;
        treasure.x = x;
        treasure.y = y;
    }
    for(size_t i = 0ull; i < vVertexes.size(); ++i)
    {
        int count = 0;
        int min = vVertexes[i].dPoints[0].weight;
        int max = vVertexes[i].dPoints[0].weight;
        if (intersectionOfRayAndSection(treasure, Vec2d(1., 0.), vVertexes[i].dPoints[vVertexes[i].dPoints.size() - 1], vVertexes[i].dPoints[0])) ++count;
        for(size_t j = 0ull; j < vVertexes[i].dPoints.size() - 1; ++j)
        {
            if (min > vVertexes[i].dPoints[j].weight) min = vVertexes[i].dPoints[j].weight;
            else if (max < vVertexes[i].dPoints[j].weight) max = vVertexes[i].dPoints[j].weight;
            if (intersectionOfRayAndSection(treasure, Vec2d(1., 0.), vVertexes[i].dPoints[j], vVertexes[i].dPoints[j + 1])) ++count;
        }
        if (count & 1)
        {
            ::std::cout << min + 1 << ::std::endl;
            break;
        }
    }

    return 0;
}