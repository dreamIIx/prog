#include <iostream>
#include <vector>
#include <math.h>

struct coord
{
    int x;
    int y;

    coord() : x(-1), y(-1) {}
    coord(int ax, int ay) : x(ax), y(ay) {}

};

struct piz_data
{
    int n;
    int e;
    int s;
    int w;
    int cap;
    coord crd;

    piz_data() : crd(), n(0), e(0), s(0), w(0), cap(0) {}
    piz_data(int ax, int ay, int acap) : crd(ax, ay), n(0), e(0), s(0), w(0), cap(acap) {}

};

void incresePizzeria(::std::vector<piz_data>&, ::std::vector<int>&, int&, int, int);

int main()
{
    int map_x;
    int map_y;
    size_t nLoop = 0ull;
    size_t pcount;
    ::std::cin >> map_x;

    ::std::vector<piz_data> vPizzeria;
    // ::std::vector<::std::vector<::std::vector<int>>> vMap;
    // ::std::vector<::std::vector<::std::vector<piz_data*>>> vInfoMap;

    while(map_x)
    {
        ::std::cin >> map_y >> pcount;
        vPizzeria.reserve(pcount);
        // vInfoMap.reserve(2ull);
        // vInfoMap.emplace_back(::std::vector<::std::vector<piz_data*>>());
        // vInfoMap.emplace_back(::std::vector<::std::vector<piz_data*>>());
        for(size_t i {0}; i < pcount; ++i)
        {
            int ax;
            int ay;
            int acap;
            ::std::cin >> ax >> ay >> acap;
            vPizzeria.emplace_back(ax - 1, ay - 1, acap);
            // vInfoMap[0][ay].emplace_back(&vPizzeria.back());
            // vInfoMap[1][ax].emplace_back(&vPizzeria.back());
        }

        int total_cap = map_y * map_x - pcount;
        while(total_cap)
        {
            ::std::vector<::std::pair<::std::pair<coord, ::std::vector<int>>, size_t>> vMapInfo;
            bool is_idle = true;
            for(int y = 0; y < map_y; ++y)
            {
                for(int x = 0; x < map_x; ++x)
                {
                    ::std::vector<int> vCellInfo;
                    size_t nChallengers = 0ull;
                    vCellInfo.reserve(pcount);
                    for(size_t i {0}; i < vPizzeria.size(); ++i)
                    {
                        bool eq_x;
                        bool eq_y;
                        int tmp_x;
                        int tmp_y;
                        if (!(eq_y = ((tmp_y = vPizzeria[i].crd.y) == y)) ^ (eq_x = ((tmp_x = vPizzeria[i].crd.x) == x))) break;
                        if ((eq_x && (tmp_y - (vPizzeria[i].cap + vPizzeria[i].n) <= y && tmp_y + (vPizzeria[i].cap + vPizzeria[i].s) >= y))
                            || (eq_y && (tmp_x - (vPizzeria[i].cap + vPizzeria[i].w) <= x && tmp_x + (vPizzeria[i].cap + vPizzeria[i].e) >= x)))
                        {
                            vCellInfo.emplace_back(1);
                            ++nChallengers;
                        }
                        else
                        {
                            vCellInfo.emplace_back(0);
                        }
                    }
                    if (nChallengers == 1ull)
                    {
                        incresePizzeria(vPizzeria, vCellInfo, total_cap, x, y);
                        is_idle = false;
                    }
                    else if (nChallengers)
                    {
                        vMapInfo.reserve(vMapInfo.capacity() + 1);
                        vMapInfo.emplace_back(::std::make_pair(::std::make_pair(coord(x, y), vCellInfo), nChallengers));
                    }
                }
            }
            if (is_idle)
            {
                for(ptrdiff_t i {0}; i < vMapInfo.size();)
                {
                    for(size_t j {i + 1}; j < vMapInfo.size(); ++j)
                    {
                        if (vMapInfo[i].first.second == vMapInfo[j].first.second)
                        {
                            if (vMapInfo[i].second == 2ull)
                            {
                                incresePizzeria(vPizzeria, vMapInfo[i].first.second, total_cap, vMapInfo[i].first.first.x, vMapInfo[i].first.first.y);
                                incresePizzeria(vPizzeria, vMapInfo[i].first.second, total_cap, vMapInfo[j].first.first.x, vMapInfo[j].first.first.y);
                                vMapInfo.erase(vMapInfo.begin() + j);
                                vMapInfo.erase(vMapInfo.begin() + i);
                            }
                            else
                            {
                                ::std::cout << "here" << ::std::endl;
                            }
                        }
                        ++i;
                    }
                }
            }
        }
        ::std::cout << "Case " << ++nLoop << ":\n";
        for(auto& x : vPizzeria)
        {
            ::std::cout << x.n << ' ' << x.e << ' ' << x.s << ' ' << x.w << '\n';
        }
        ::std::cout << ::std::endl;

        ::std::cin >> map_x;
    }

    return 0;
}

void incresePizzeria(::std::vector<piz_data>& piz, ::std::vector<int>& cellinf, int& totlcap, int x, int y)
{
    for (size_t i {0}; i < cellinf.size(); ++i)
    {
        if (cellinf[i])
        {
            int tmp_x = ::std::abs(piz[i].crd.x - x);
            int tmp_y = ::std::abs(piz[i].crd.y - y);
            if (tmp_x)
            {
                if (tmp_x > 0)  piz[i].w += tmp_x;
                else            piz[i].e += tmp_x;
                --piz[i].cap;
                totlcap -= tmp_x;
            }
            else
            {
                if (tmp_y > 0)  piz[i].n += tmp_y;
                else            piz[i].s += tmp_y;
                --piz[i].cap;
                totlcap -= tmp_y;
            }
            cellinf[i] = 0;
            break;
        }
    }
}