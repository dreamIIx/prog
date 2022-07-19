#include <iostream>
#include <array>
#include <vector>
#include <math.h>
#include <algorithm>

#if !defined(defDX_S)
#define defDX_S(x)			#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)			defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__		defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef NOTATION_
#define NOTATION_			::std::cout << "![NTTN]" << (const char*)defDX__FILELINE << ::std::endl;
#define NTTN_ NOTATION_
#endif

#ifndef ERROR_

#if defined(_WIN32)
#define ERROR_				{ ::std::cout << "![EXPT]" << (const char*)defDX__FILELINE << ::std::endl;      \
							throw ::std::exception(); }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				try {																			\
								throw ::std::exception();													\
							} catch (::std::exception& x) {													\
								::std::cout << "![EXPT]" << (const char*)defDX__FILELINE << ::std::endl;	\
							}
#else
#error This UNIX operating system is not supported
#endif
#else
#error This operating system is not supported
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc)		if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc)		if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

struct coord
{
    unsigned x;
    unsigned y;

    coord() : x(-1), y(-1) {}
    coord(int ax, int ay) : x(ax), y(ay) {}

};

struct piz_data
{
    // 0 - n
    // 1 - s
    // 2 - w
    // 3 - e
    ::std::array<unsigned, 4ull> dir;
    unsigned cap;
    unsigned potential;
    coord crd;

    piz_data() : dir(), cap(0), potential(0), crd() {}
    piz_data(int ax, int ay, int acap, int apotential) : dir(), cap(acap), potential(apotential), crd(ax, ay) {}

};

struct spec_point
{
    enum class state : unsigned
    {
        def = 0,
        undef
    };

    state is;

    // 0 - n
    // 1 - s
    // 2 - w
    // 3 - e
    ::std::array<unsigned, 4ull> lengthDir;
    ::std::array<piz_data*, 4ull> targetDir;

    spec_point() : is(state::undef), lengthDir(), targetDir() {}

};

//void increasePizzeria(::std::vector<piz_data>&, ::std::vector<unsigned>&, unsigned&, unsigned, unsigned);
unsigned in_range(const coord&, const piz_data&);

int main()
{
    unsigned map_x;
    unsigned map_y;
    size_t nLoop = 0ull;
    size_t pcount;
    ::std::cin >> map_x;

    while(map_x)
    {
        ::std::vector<piz_data> vPizzeria;
        ::std::cin >> map_y >> pcount;
        ::std::vector<::std::vector<spec_point>> vMap(map_y, ::std::vector<spec_point>(map_x));
        for(size_t y {0}; y < map_y; ++y)
        {
            for(size_t x {0}; x < map_x; ++x)
            {
                vMap[y][x].lengthDir[0] = y;
                vMap[y][x].lengthDir[1] = map_y - y - 1;
                vMap[y][x].lengthDir[2] = x;
                vMap[y][x].lengthDir[3] = map_x - x - 1;
                for(auto& i : vMap[y][x].targetDir)
                {
                    i = nullptr;
                }
            }
        }
        vPizzeria.reserve(pcount);
        for(size_t i {0}; i < pcount; ++i)
        {
            unsigned ax;
            unsigned ay;
            unsigned acap;
            ::std::cin >> ax >> ay >> acap;
            vPizzeria.emplace_back(ax - 1, ay - 1, acap, acap * 4);
            vMap[ay - 1][ax - 1].is = spec_point::state::def;
            if (ay - 1 >= 0)
            {
                for(ptrdiff_t y {ay - 2}; y >= 0; --y)
                {
                    vMap[y][ax - 1].lengthDir[1] = ay - 2 - y;
                    vMap[y][ax - 1].targetDir[1] = &vPizzeria.back();
                    if (vMap[y][ax - 1].is == spec_point::state::def) break;
                }
            }
            if (ay - 1 < map_y)
            {
                for(size_t y {ay}; y < map_y; ++y)
                {
                    vMap[y][ax - 1].lengthDir[0] = y - ay;
                    vMap[y][ax - 1].targetDir[0] = &vPizzeria.back();
                    if (vMap[y][ax - 1].is == spec_point::state::def) break;
                }
            }
            if (ax - 1 >= 0)
            {
                for(ptrdiff_t x {ax - 2}; x >= 0; --x)
                {
                    vMap[ay - 1][x].lengthDir[3] = ax - 2 - x;
                    vMap[ay - 1][x].targetDir[3] = &vPizzeria.back();
                    if (vMap[ay - 1][x].is == spec_point::state::def) break;
                }
            }
            if (ax - 1 < map_x)
            {
                for(size_t x {ax}; x < map_x; ++x)
                {
                    vMap[ay - 1][x].lengthDir[2] = x - ax;
                    vMap[ay - 1][x].targetDir[2] = &vPizzeria.back();
                    if (vMap[ay - 1][x].is == spec_point::state::def) break;
                }
            }
        }
        
        for(size_t n {0}; n < pcount; ++n)
        {
            for(auto& y : vMap)
            {
                for(auto& x : y)
                {
                    ::std::cout << (x.is == spec_point::state::def ? 1 : 0);
                }
                ::std::cout << std::endl;
            }

            for(size_t k {0}; k < pcount; ++k)
            {
                size_t c;
                for(c = 1ull; c <= ::std::min(vPizzeria[k].cap, vPizzeria[k].crd.y); ++c)
                {
                    if (vMap[vPizzeria[k].crd.y - c][vPizzeria[k].crd.x])   break;
                }
                vPizzeria[k].potential -= vPizzeria[k].cap - c + 1;
                for(c = 1ull; c <= ::std::min(vPizzeria[k].cap, map_y - vPizzeria[k].crd.y - 1); ++c)
                {

                    if (vMap[vPizzeria[k].crd.y + c][vPizzeria[k].crd.x])   break;
                }
                vPizzeria[k].potential -= vPizzeria[k].cap - c + 1;
                for(c = 1ull; c <= ::std::min(vPizzeria[k].cap, vPizzeria[k].crd.x); ++c)
                {
                    if (vMap[vPizzeria[k].crd.y][vPizzeria[k].crd.x - c])   break;
                }
                vPizzeria[k].potential -= vPizzeria[k].cap - c + 1;
                for(c = 1ull; c <= ::std::min(vPizzeria[k].cap, map_x - vPizzeria[k].crd.x - 1); ++c)
                {
                    if (vMap[vPizzeria[k].crd.y][vPizzeria[k].crd.x + c])   break;
                }
                vPizzeria[k].potential -= vPizzeria[k].cap - c + 1;
            }

            ::std::sort(vPizzeria.begin(), vPizzeria.end(),
                [](const piz_data& a, const piz_data& b) -> bool
                {
                    return (static_cast<double>(a.potential) / a.cap < static_cast<double>(b.potential) / b.cap) ||
                        (static_cast<double>(a.potential) / a.cap - static_cast<double>(b.potential) / b.cap < 1e-7 && a.cap > b.cap);
                });
            
            auto it = ::std::partition(vPizzeria.begin(), vPizzeria.end(),
                [&](const piz_data& a) -> bool
                    { return (static_cast<double>(a.potential) / a.cap - static_cast<double>(vPizzeria.front().potential) / vPizzeria.front().cap) < 1e-7; });

            ::std::vector<unsigned> vPath(4ull, 0u);
            ::std::pair<unsigned, double> decr = ::std::make_pair(-1, 2.);
            size_t piz_idx = -1;
            for(auto iter = vPizzeria.begin(); iter != it; ++iter)
            {
                ::std::vector<unsigned> vCurPath(4ull, 0u);
                ::std::pair<unsigned, double> cur_decr = ::std::make_pair(-1, 2.);
                while(iter->cap)
                {
                    piz_data temp = *iter;
                    ::std::vector<::std::pair<unsigned, double>> vInfo(4ull, ::std::make_pair(-1, 2.));
                    for(size_t c {0}; c < 4ull; ++c)
                    {
                        unsigned cur_y = temp.crd.y + (c < 2 ? (temp.dir[c] + 1) * ::std::pow(-1, c + 1) : 0);
                        unsigned cur_x = temp.crd.x + (c < 2 ? 0 : (temp.dir[c] + 1) * ::std::pow(-1, c + 1));
                        if (cur_x < 0 || cur_x >= map_x || cur_y < 0 || cur_y >= map_y) continue;
                        unsigned temp_decr = 0u;
                        for(size_t k {0}; k < pcount; ++k)
                        {
                            if (k == iter - vPizzeria.begin()) continue;
                            if (cur_x == vPizzeria[k].crd.x && cur_y == vPizzeria[k].crd.y) break;
                            temp_decr = in_range(coord(cur_x, cur_y), vPizzeria[k]);
                            vInfo[c].first += temp_decr;
                            vInfo[c].second += (static_cast<double>(temp_decr) / vPizzeria[k].potential) * vPizzeria[k].cap;
                        }
                    }
                    auto it_min = ::std::min_element(vInfo.begin(), vInfo.end(),
                        [](const ::std::pair<unsigned, double>& a, const ::std::pair<unsigned, double>& b) -> bool
                            {
                                return a.first < b.first || (a.first == b.first && a.second < b.second - 1e-7);
                            });
                    ::std::cout << it_min - vInfo.begin() << ::std::endl;
                    ++vCurPath[it_min - vInfo.begin()];
                    cur_decr.first += it_min->first;
                    if (cur_decr.second < it_min->second) cur_decr.second = it_min->second;
                    --(iter->cap);
                }
                if (cur_decr.first < decr.first || (cur_decr.first == decr.first && cur_decr.second < decr.second))
                {
                    ::std::copy(vCurPath.begin(), vCurPath.end(), vPath.begin());
                    //vPath = vCurPath;
                    decr = cur_decr;
                    piz_idx = iter - vPizzeria.begin();
                }
            }

            for(size_t i {0}; i < 4ull; ++i)
            {
                unsigned cur_y = (i < 2 ? vPizzeria[piz_idx].crd.y + vPizzeria[piz_idx].dir[i] * ::std::pow(-1, i + 1) : vPizzeria[piz_idx].crd.y);
                unsigned cur_x = (i < 2 ? vPizzeria[piz_idx].crd.x : vPizzeria[piz_idx].crd.x + vPizzeria[piz_idx].dir[i] * ::std::pow(-1, i + 1));
                for(size_t j {0}; j < vPath[i]; ++j)
                {
                    vMap[cur_y + (i < 2 ? j * ::std::pow(-1, i + 1) : 0)][cur_x + (i >= 2 ? j * ::std::pow(-1, i + 1) : 0)] = 1;
                }
                vPizzeria[piz_idx].dir[i] += vPath[i];
            }
        }

        /*int total_cap = map_y * map_x - pcount;
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
                    for(size_t i {0}; i < pcount; ++i)
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
                        increasePizzeria(vPizzeria, vCellInfo, total_cap, x, y);
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
                                increasePizzeria(vPizzeria, vMapInfo[i].first.second, total_cap, vMapInfo[i].first.first.x, vMapInfo[i].first.first.y);
                                increasePizzeria(vPizzeria, vMapInfo[i].first.second, total_cap, vMapInfo[j].first.first.x, vMapInfo[j].first.first.y);
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
        }*/
        ::std::cout << "Case " << ++nLoop << ":\n";
        for(auto& x : vPizzeria)
        {
            for(auto& y : x.dir)
            {
                ::std::cout << y << ' ';
            }
            ::std::cout << ::std::endl;
        }

        ::std::cin >> map_x;
    }

    return 0;
}

unsigned in_range(const coord& source, const piz_data& target)
{
    if (source.x == target.crd.x)
    {
        if (source.y < target.crd.y && source.y >= target.crd.y - target.dir[0] - target.cap) return source.y - (target.crd.y - target.cap - target.dir[1]) + 1;
        if (source.y > target.crd.y && source.y <= target.crd.y + target.dir[1] + target.cap) return target.crd.y + target.cap + target.dir[0] - source.y + 1;
    }
    else if (source.y == target.crd.y)
    {
        if (source.x < target.crd.x && source.x >= target.crd.x - target.dir[2] - target.cap) return source.x - (target.crd.x - target.cap - target.dir[3]) + 1;
        if (source.x > target.crd.x && source.x <= target.crd.x + target.dir[3] + target.cap) return target.crd.x + target.cap + target.dir[2] - source.x + 1;
    }
    return 0;
}

/*void increasePizzeria(::std::vector<piz_data>& piz, ::std::vector<unsigned>& cellinf, unsigned& totlcap, unsigned x, unsigned y)
{
    for (size_t i {0}; i < cellinf.size(); ++i)
    {
        if (cellinf[i])
        {
            unsigned tmp_x = ::std::abs(piz[i].crd.x - x);
            unsigned tmp_y = ::std::abs(piz[i].crd.y - y);
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
}*/