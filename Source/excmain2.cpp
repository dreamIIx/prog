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

    Point() : is(state::undef), x(0.), y(0.) {}
    Point(double ax, double ay) : is(state::undef)
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

    Vec2d(double ax, double ay) : x(ax), y(ay) {}
    Vec2d(Point p1, Point p2) : x(p2.x - p1.x), y(p2.y - p1.y) {}

};

struct Straight
{
    double a;
    double b;
    double c;

    Straight(double aa, double ab, double ac)   noexcept(true)  : a(aa), b(ab), c(ac) {}
    Straight(Point f, Point s)                noexcept(true)  : a(s.y - f.y), b(f.x - s.x), c(f.x * (f.y - s.y) + f.y * (s.x - f.x)) {}

};

struct Polygon
{
//private:
//    static size_t _id_section;
//
//public:
    ::std::deque<Point> dPoints;
    //::std::vector<size_t> vSections;

    Polygon() noexcept(true) {}

    /*static size_t getIdSection() noexcept(true)
    {
        return _id_section++;
    }*/

};

//size_t ::Polygon::_id_section(0ull);

struct DataGraphList
{
    unsigned distance;
    ::std::vector<DataGraphList*> neighbors;

    DataGraphList() noexcept(true): distance(-1), neighbors() {}

    ~DataGraphList()
    {
        for(size_t i {0}; i < neighbors.size(); ++i)
        {
            delete neighbors[i];
            neighbors[i] = nullptr;
        }
    }

    DataGraphList* _spec_createDataGraphList(const ::std::vector<Polygon>& vData, size_t target) const noexcept(false)
    {
        DataGraphList* source = new DataGraphList();
        
        for(size_t i {0}; i < vData.size(); ++i)
        {
            for(size_t j {0}; j < vData[i].dPoints.size(); ++j)
            {

            }
        }
    }

};

unsigned _spec_BFS(::std::vector<Polygon> vPolygon, size_t target)
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
        //vPolygon.erase(vPolygon.begin() + queue.front());
        queue.pop();
        for(size_t i {0}; i < cur.dPoints.size(); ++i)
        {
            size_t next_i = i < cur.dPoints.size() - 1 ? i + 1 : 0ull;
            //size_t prev_i = i > 0 ? i - 1 : cur.dPoints.size() - 1;
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
                            //size_t next_k = k < vPolygon[j].dPoints.size() - 1 ? k + 1 : 0ull;
                            if (vPolygon[j].dPoints[k] == cur.dPoints[i])
                            {
                                if (vPolygon[j].dPoints[prev_k] == cur.dPoints[next_i])/* || (vPolygon[j].dPoints[next_k] == cur.dPoints[prev_i]))*/
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
            /*if (cur->neighbors[i]->distance > current_range)
            {
                cur->neighbors[i]->distance = current_range;
                queue.push(cur->neighbors[i]);
            }*/
            /*if ()
            {
                if (min_dist_2_target > ...)
                {
                    min_dist_2_target = ...;
                }
            }*/
        }
    }
    return min_dist_2_target;
}
    
/*Point intersectionOfStraights(Straight line1, Straight line2) noexcept(false) try
{
    return Point((line2.c - line1.c) / (line1.b / line1.a - line2.b / line2.a), (line1.c - line2.c) / (1 - (line2.b * line1.a) / (line2.a * line1.b)) + line1.c);
}
catch(...)
{
    return Point();
}

Point intersectionOfStraights(Point f1, Point s1, Point f2, Point s2) noexcept(false) try
{
    double det = (f1.x - s1.x) * (f2.y - s2.y) - (f1.y - s1.y) * (f2.x - s2.x);
    double delt1 = (f1.x * s1.y - f1.y * s1.x);
    double delt2 = (f2.x * s2.y - f2.y * s2.x);
    double x = (delt1 * (f2.x - s2.x) - (f1.x * s1.x) * delt2) / det;
    double y = (delt1 * (f2.y - s2.y) - (f1.y * s1.y) * delt2) / det;
    return Point(x, y);
}
catch(...)
{
    return Point();
}*/

/*struct Section
{
    Point a;
    Point b;

    Section(Point aa, Point ab) : a(aa), b(ab) {}

};*/

/*::std::pair<size_t, Point> nearestStraightToPoint(::std::vector<Straight>& vS, Point& p) noexcept(false)
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
    return { res_idx, Point((ref.b * (ref.b * p.x - ref.a * p.y) - ref.a * ref.c) / d, (ref.a * (-ref.b * p.x + ref.a * p.y) - ref.b * ref.c) / d) };
}*/

/*inline double lengthOfVector(Point& a, Point& b)
{
    return ::std::sqrt(::std::pow(b.x - a.x, 2.) + ::std::pow(b.y - a.y, 2.));
}*/

Point intersectionOfRayAndStraight(Point, Vec2d, const Straight&) noexcept(false);
Point intersectionOfSectionAndStraight(Point, Point, const Straight&) noexcept(false);
Point intersectionOfRayAndSection(Point, Vec2d, Point, Point) noexcept(false);
void splitVertexes(const Straight&, ::std::vector<Polygon>&);

int main()
{
    size_t n;
    ::std::cin >> n;
    ::std::vector<Polygon> vVertexes;
    vVertexes.reserve(1ull);
    vVertexes.emplace_back(/*Polygon()*/);
    vVertexes.back().dPoints.push_back(Point(0, 0));
    vVertexes.back().dPoints.push_back(Point(0, 100));
    vVertexes.back().dPoints.push_back(Point(100, 100));
    vVertexes.back().dPoints.push_back(Point(100, 0));
    /*for(size_t i {0}; i < 4ull; ++i)
    {
        vVertexes.back().vSections.push_back(::Polygon::getIdSection());
    }*/
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
        //int min = vVertexes[i].dPoints.front().weight;
        //int max = vVertexes[i].dPoints.front().weight;
        if (intersectionOfRayAndSection(treasure, Vec2d(1., 0.), vVertexes[i].dPoints.back(), vVertexes[i].dPoints.front())) ++count;
        for(size_t j {0}; j < vVertexes[i].dPoints.size() - 1; ++j)
        {
            //if (min > vVertexes[i].dPoints[j].weight) min = vVertexes[i].dPoints[j].weight;
            //else if (max < vVertexes[i].dPoints[j].weight) max = vVertexes[i].dPoints[j].weight;
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

void splitVertexes(const Straight& line, ::std::vector<Polygon>& vtx)
{
    size_t temp = vtx.size();
    for(size_t i = 0ull; i < temp; ++i)
    {
        Point p1, p2;
        ptrdiff_t _start = -1ll;
        //int min = vtx[i].dPoints.back().weight;
        //int max = vtx[i].dPoints.back().weight;
        if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints.back(), vtx[i].dPoints[0ull], line)))
        {
            //::std::cout << "Intersection! [" << vtx[i].dPoints.back().x << ", " << vtx[i].dPoints.back().y << "]-[" << vtx[i].dPoints[0ull].x << ", " << vtx[i].dPoints[0ull].y << ']' << ::std::endl;
            //p1.weight = vtx[i].dPoints.back().weight;
            _start = 0ull;
            ////min = (max = p1.weight);
        }
        else
        {
            for(size_t j {0}; j < vtx[i].dPoints.size() - 1; ++j)
            {
                //if (min > vtx[i].dPoints[j].weight) min = vtx[i].dPoints[j].weight;
                //else if (max < vtx[i].dPoints[j].weight) max = vtx[i].dPoints[j].weight;
                if ((p1 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
                {
                    ////::std::cout << "Intersection! [" << vtx[i].dPoints[j].x << ", " << vtx[i].dPoints[j].y << "]-[" << vtx[i].dPoints[j + 1].x << ", " << vtx[i].dPoints[j + 1].y << ']' << ::std::endl;
                    _start = j + 1;
                    //p1.weight = vtx[i].dPoints[j].weight;
                    break;
                }
            }
            if (!p1) continue;
        }
        for(size_t j = _start; j < vtx[i].dPoints.size() - 1; ++j)
        {
            //if (min > vtx[i].dPoints[j].weight) min = vtx[i].dPoints[j].weight;
            //else if (max < vtx[i].dPoints[j].weight) max = vtx[i].dPoints[j].weight;
            if ((p2 = intersectionOfSectionAndStraight(vtx[i].dPoints[j], vtx[i].dPoints[j + 1], line)))
            {
                /*for(size_t k = j + 1; k < vtx[i].dPoints.size() - 1; ++k)
                {
                    if (min > vtx[i].dPoints[k].weight) min = vtx[i].dPoints[k].weight;
                    else if (max < vtx[i].dPoints[k].weight) max = vtx[i].dPoints[k].weight;
                }*/
                ////::std::cout << "Intersection! [" << vtx[i].dPoints[j].x << ", " << vtx[i].dPoints[j].y << "]-[" << vtx[i].dPoints[j + 1].x << ", " << vtx[i].dPoints[j + 1].y << ']' << ::std::endl;
                vtx.reserve(vtx.capacity() + 1);
                vtx.emplace_back(/*Polygon()*/);
                vtx.back().dPoints.push_back(p1);
                //vtx.back().vSections.push_back(::Polygon::getIdSection());
                for(size_t k = _start; k < j + 1; ++k)
                {
                    vtx.back().dPoints.push_back(vtx[i].dPoints[k]);
                    //vtx.back().vSections.push_back(vtx[i].vSections[k]);
                }
                vtx.back().dPoints.push_back(p2);
                //size_t commonSideId = ::Polygon::getIdSection();
                //vtx.back().vSections.push_back(commonSideId);
                //p2.weight = vtx[i].dPoints[j].weight;
                vtx[i].dPoints.erase(vtx[i].dPoints.begin() + _start, j < vtx[i].dPoints.size() - 1 ? vtx[i].dPoints.begin() + j + 1 : vtx[i].dPoints.end());
                //vtx[i].vSections.erase(vtx[i].vSections.begin() + _start, j < vtx[i].vSections.size() - 1 ? vtx[i].vSections.begin() + j + 1 : vtx[i].vSections.end());
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p2);
                //vtx[i].vSections.emplace(vtx[i].vSections.begin() + _start, ::Polygon::getIdSection());
                vtx[i].dPoints.emplace(vtx[i].dPoints.begin() + _start, p1);
                //vtx[i].vSections.emplace(vtx[i].vSections.begin() + _start, commonSideId);
                /*if (min == max)
                {
                    vtx.back().dPoints.back().weight = max + 1;
                    vtx[i].dPoints.at(_start).weight = max + 1;
                }
                else
                {
                    vtx.back().dPoints.back().weight = max;
                    vtx[i].dPoints.at(_start).weight = max;
                }*/
                /*for(size_t h {0}; h < vtx.back().dPoints.size(); ++h)
                {
                    ::std::cout << '[' << vtx.back().dPoints[h].x << ',' << vtx.back().dPoints[h].y << "](" << vtx.back().dPoints[h].weight << ")[";
                }
                ::std::cout << ::std::endl;
                for(size_t h {0}; h < vtx[i].dPoints.size(); ++h)
                {
                    ::std::cout << '[' << vtx[i].dPoints[h].x << ',' << vtx[i].dPoints[h].y << "](" << vtx[i].dPoints[h].weight << ")[";
                }
                ::std::cout << ::std::endl;*/
                break;
            }
        }
    }
}