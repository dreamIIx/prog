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

    bool operator==(const Point& p) noexcept(true)
    {
        return (p.is == is) && (abs(p.x - x) <= 1e-7) && (abs(p.y - y) <= 1e-7);
    }

};

struct Vec2d
{
    double x;
    double y;

    Vec2d(double ax, double ay) noexcept(true) : x(ax), y(ay) {}
    Vec2d(Point p1, Point p2)   noexcept(true) : x(p2.x - p1.x), y(p2.y - p1.y) {}

};

struct Straight
{
    double a;
    double b;
    double c;

    Straight(double aa, double ab, double ac)   noexcept(true)  : a(aa), b(ab), c(ac) {}
    Straight(Point f, Point s)                  noexcept(true)  : a(s.y - f.y), b(f.x - s.x), c(f.x * (f.y - s.y) + f.y * (s.x - f.x)) {}

};

struct Polygon
{
    ::std::deque<Point> dPoints;

    Polygon() noexcept(true) {}

};

Point intersectionOfRayAndStraight(Point, Vec2d, const Straight&) noexcept(false);
Point intersectionOfSectionAndStraight(Point, Point, const Straight&) noexcept(false);
Point intersectionOfRayAndSection(Point, Vec2d, Point, Point) noexcept(false);
void splitVertexes(const Straight&, ::std::vector<Polygon>&);
unsigned _spec_BFS(const ::std::vector<Polygon>&, size_t);

int main()
{
    size_t n;
    ::std::cin >> n;
    ::std::vector<Polygon> vVertexes;
    vVertexes.reserve(1ull);
    vVertexes.emplace_back();
    vVertexes.back().dPoints.push_back(Point(0, 0));
    vVertexes.back().dPoints.push_back(Point(0, 100));
    vVertexes.back().dPoints.push_back(Point(100, 100));
    vVertexes.back().dPoints.push_back(Point(100, 0));
    for(size_t i {0}; i < n; ++i)
    {
        int x1, x2, y1, y2;
        ::std::cin >> x1 >> y1 >> x2 >> y2;
        splitVertexes(Straight(Point(static_cast<double>(x1), static_cast<double>(y1)), Point(static_cast<double>(x2), static_cast<double>(y2))), vVertexes);
    }
    Point treasure;
    {
        double x, y;
        ::std::cin >> x >> y;
        treasure.x = x;
        treasure.y = y;
    }
    for(size_t i {0}; i < vVertexes.size(); ++i)
    {
        int count = 0;
        if (intersectionOfRayAndSection(treasure, Vec2d(1., 0.), vVertexes[i].dPoints.back(), vVertexes[i].dPoints.front())) ++count;
        for(size_t j {0}; j < vVertexes[i].dPoints.size() - 1; ++j)
        {
            if (intersectionOfRayAndSection(treasure, Vec2d(1., 0.), vVertexes[i].dPoints[j], vVertexes[i].dPoints[j + 1])) ++count;
        }
        if (count == 1)
        {
            ::std::cout << _spec_BFS(vVertexes, i) << ::std::endl;
            break;
        }
    }

    return 0;
}

Point intersectionOfRayAndStraight(Point p, Vec2d v, const Straight& line) noexcept(false) try
{
    double res = (-line.a * p.x - line.b * p.y - line.c) / (line.a * v.x + line.b * v.y);
    if (res > 0.) return Point(p.x + v.x * res, p.y + v.y * res);
    return Point();
}
catch(...)
{
    return Point();
}

Point intersectionOfSectionAndStraight(Point p1, Point p2, const Straight& line) noexcept(false) try
{
    double res1 = (-line.a * p1.x - line.b * p1.y - line.c) / (line.a * (p2.x - p1.x) + line.b * (p2.y - p1.y));
    double res2 = (-line.a * p2.x - line.b * p2.y - line.c) / (line.a * (p1.x - p2.x) + line.b * (p1.y - p2.y));
    if (res1 > 0. && res2 > 0.) return Point(p1.x + (p2.x - p1.x) * res1, p1.y + (p2.y - p1.y) * res1);
    return Point();
}
catch(...)
{
    return Point();
}

Point intersectionOfRayAndSection(Point p, Vec2d v, Point p1, Point p2) noexcept(false) try
{
    Point temp;
    if (intersectionOfSectionAndStraight(p1, p2, Straight(p, Point(p.x + v.x, p.y + v.y))) &&
        (temp = intersectionOfRayAndStraight(p, v, Straight(p1, p2)))) return temp;
    return Point();
}
catch(...)
{
    return Point();
}

void splitVertexes(const Straight& line, ::std::vector<Polygon>& vtx) noexcept(false)
{
    size_t temp = vtx.size();
    for(size_t i = 0ull; i < temp; ++i)
    {
        Point p1, p2;
        ptrdiff_t _start = -1ll;
        if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints.back(), vtx[i].dPoints[0ull], line)))
        {
            _start = 0ull;
        }
        else
        {
            for(size_t j {0}; j < vtx[i].dPoints.size() - 1; ++j)
            {
                if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
                {
                    _start = j + 1;
                    break;
                }
            }
            if (!p1) continue;
        }
        for(size_t j = _start; j < vtx[i].dPoints.size() - 1; ++j)
        {
            if ((p2 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
            {
                vtx.reserve(vtx.capacity() + 1);
                vtx.emplace_back();
                vtx.back().dPoints.push_back(p1);
                for(size_t k = _start; k < j + 1; ++k)
                {
                    vtx.back().dPoints.push_back(vtx[i].dPoints[k]);
                }
                vtx.back().dPoints.push_back(p2);
                vtx[i].dPoints.erase(vtx[i].dPoints.begin() + _start, j < vtx[i].dPoints.size() - 1 ? vtx[i].dPoints.begin() + j + 1 : vtx[i].dPoints.end());
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p2);
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p1);
                break;
            }
        }
    }
}

unsigned _spec_BFS(const ::std::vector<Polygon>& vPolygon, size_t target) noexcept(false)
{
    unsigned min_dist_2_target = -1;
    ::std::queue<size_t> queue;
    ::std::vector<unsigned> vDist2Polygon(vPolygon.size(), -1);
    queue.push(target);
    vDist2Polygon[target] = 0;
    while(!queue.empty())
    {
        size_t cur_idx = queue.front();
        unsigned current_range = vDist2Polygon[cur_idx] + 1;
        Polygon cur = vPolygon[cur_idx];
        queue.pop();
        for(size_t i {0}; i < cur.dPoints.size(); ++i)
        {
            size_t next_i = i < cur.dPoints.size() - 1 ? i + 1 : 0ull;
            if (((cur.dPoints[i].x != cur.dPoints[next_i].x) || ((cur.dPoints[i].x != 0) && (cur.dPoints[i].x != 100)))
                && ((cur.dPoints[i].y != cur.dPoints[next_i].y) || ((cur.dPoints[i].y != 0) && (cur.dPoints[i].y != 100))))
            {
                for(size_t j {0}; j < vPolygon.size(); ++j)
                {
                    if (cur_idx != j)
                    {
                        for(size_t k {0}; k < vPolygon[j].dPoints.size(); ++k)
                        {
                            size_t prev_k = k > 0 ? k - 1 : vPolygon[j].dPoints.size() - 1;
                            if (vPolygon[j].dPoints[k] == cur.dPoints[i])
                            {
                                if (vPolygon[j].dPoints[prev_k] == cur.dPoints[next_i])
                                {
                                    if (vDist2Polygon[j] > current_range)
                                    {
                                        vDist2Polygon[j] = current_range;
                                        queue.push(j);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (min_dist_2_target > current_range)
                {
                    if (current_range == 1) return current_range;
                    min_dist_2_target = current_range;
                }
            }
        }
    }
    return min_dist_2_target;
}