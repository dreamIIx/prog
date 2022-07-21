#include <iostream>
#include <array>
#include <vector>
#include <math.h>
#include <algorithm>
#include <memory>
#include <numeric>

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

#define CNT_DIRECTIONS (4ull)

struct coord
{
    unsigned x;
    unsigned y;

    coord() : x(-1), y(-1) {}
    coord(unsigned ax, unsigned ay) : x(ax), y(ay) {}

};

struct piz_data
{
    // 0 - n
    // 1 - w
    // 2 - s
    // 3 - e
    ::std::array<unsigned, 4ull> dir;
    unsigned cap;
    unsigned potential;
    size_t n;
    coord crd;

    piz_data() : dir({0u, 0u, 0u, 0u}), cap(0u), potential(0u), n(0ull), crd() {}
    piz_data(unsigned ax, unsigned ay, unsigned acap, unsigned apotential, size_t idx) : dir({0u, 0u, 0u, 0u}), cap(acap), potential(apotential), n(idx), crd(ax, ay) {}

};

template<typename T>
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
    ::std::array<unsigned, CNT_DIRECTIONS> lengthDir;
    ::std::array<T*, CNT_DIRECTIONS> targetDir;

    spec_point() : is(state::undef), lengthDir(), targetDir() {}

};

template<typename T>
void updateMap(::std::vector<::std::vector<spec_point<T>>>& vMap, int, int, unsigned, unsigned,
    T* ptr0, T* ptr1 = nullptr, T* ptr2 = nullptr, T* ptr3 = nullptr);

int main()
{
    unsigned map_x;
    unsigned map_y;
    size_t nLoop = 0ull;
    size_t pcount;
    ::std::cin >> map_x;

    while(map_x)
    {
        ::std::cin >> map_y >> pcount;
        ::std::unique_ptr<piz_data> _data_storage(new piz_data[pcount]());
        ::std::vector<::std::reference_wrapper<piz_data>> vPizzeria;
        ::std::vector<::std::vector<spec_point<piz_data>>> vMap(map_y, ::std::vector<spec_point<piz_data>>(map_x));
        for(size_t y {0}; y < map_y; ++y)
        {
            for(size_t x {0}; x < map_x; ++x)
            {
                vMap[y][x].lengthDir[0] = y;
                vMap[y][x].lengthDir[1] = x;
                vMap[y][x].lengthDir[2] = map_y - y - 1;
                vMap[y][x].lengthDir[3] = map_x - x - 1;
                for(auto& i : vMap[y][x].targetDir)
                {
                    i = nullptr;
                }
            }
        }
        for(size_t i {0}; i < pcount; ++i)
        {
            unsigned ax;
            unsigned ay;
            unsigned acap;
            ::std::cin >> ax >> ay >> acap;
            vPizzeria.push_back(_data_storage.get()[i]);
            vPizzeria[i].get().crd.x = ax - 1;
            vPizzeria[i].get().crd.y = ay - 1;
            vPizzeria[i].get().cap = acap;
            vPizzeria[i].get().potential = 0u;
            vPizzeria[i].get().n = i;
            updateMap(vMap, ax - 1, ay - 1, map_x, map_y, &vPizzeria[i].get(), &vPizzeria[i].get(), &vPizzeria[i].get(), &vPizzeria[i].get());
        }

        auto start_it = vPizzeria.begin();
        auto end_it = vPizzeria.end();
        
        for(auto iter_main = start_it; iter_main != end_it;)
        {
            for(auto iter = start_it; iter != end_it; ++iter)
            {
                iter->get().potential = 0u;
                for(size_t i {0}; i < CNT_DIRECTIONS; ++i)
                {
                    iter->get().potential += ::std::min(
                        vMap[iter->get().crd.y + (i & 1 ? 0 : (i ? iter->get().dir[i] : -iter->get().dir[i]))]
                            [iter->get().crd.x + (i & 1 ? (i == 1 ? -iter->get().dir[i] : iter->get().dir[i]) : 0)].lengthDir[i], iter->get().cap);
                }
            }

            ::std::sort(start_it, end_it,
                [](const piz_data& a, const piz_data& b) -> bool
                {
                    return (static_cast<double>(a.potential) / a.cap < static_cast<double>(b.potential) / b.cap) ||
                        (static_cast<double>(a.potential) / a.cap - static_cast<double>(b.potential) / b.cap < 1e-7 && a.cap > b.cap);
                });

            bool is_idle = false;
            while(!is_idle)
            {
                is_idle = true;
                for(auto& y : vMap)
                {
                    for(auto& x : y)
                    {
                        if (x.is == spec_point<piz_data>::state::undef)
                        {
                            size_t count_free_directions = 0ull;
                            size_t target_direction = -1;
                            for(size_t i {0}; i < CNT_DIRECTIONS; ++i)
                            {
                                if (x.targetDir[i])
                                {
                                    if (x.targetDir[i]->cap <= x.lengthDir[i]) ++count_free_directions;
                                    else target_direction = i;
                                }
                                else ++count_free_directions;
                            }
                            if (count_free_directions == 3ull)
                            {
                                unsigned chage_range = x.lengthDir[target_direction] + 1;
                                auto& target = *x.targetDir[target_direction];
                                size_t rtd = (target_direction - 2) % 4;
                                for(size_t i {1}; i <= chage_range; ++i)
                                {
                                    updateMap(vMap,
                                        static_cast<ptrdiff_t>(target.crd.x) + (rtd & 1 ? (rtd == 1 ? -target.dir[rtd] - i : target.dir[rtd] + i) : 0),
                                        static_cast<ptrdiff_t>(target.crd.y) + (rtd & 1 ? 0 : (rtd ? target.dir[rtd] + i : -target.dir[rtd] - i)),
                                        map_x, map_y,
                                        static_cast<piz_data*>(target_direction == 0 ? &target : nullptr),
                                        static_cast<piz_data*>(target_direction == 1 ? &target : nullptr),
                                        static_cast<piz_data*>(target_direction == 2 ? &target : nullptr),
                                        static_cast<piz_data*>(target_direction == 3 ? &target : nullptr));
                                }
                                target.dir[rtd] += chage_range;
                                target.cap -= chage_range;
                                is_idle = false;
                            }
                        }
                    }
                }
            }
            ::std::stable_sort(start_it, vPizzeria.end(),
                [](const piz_data& a, const piz_data& b) -> bool
                {
                    return (a.cap > b.cap && b.cap == 0) || (a.cap < b.cap && a.cap == 0);
                });
            end_it = ::std::partition(vPizzeria.begin(), vPizzeria.end(), [](const piz_data& a) -> bool { return a.cap != 0u; });
            
            auto it = ::std::partition(start_it, end_it,
                [&](const piz_data& a) -> bool
                    { return ((static_cast<double>(a.potential) / a.cap - static_cast<double>(vPizzeria.front().get().potential) / vPizzeria.front().get().cap) < 1e-7) &&
                        (a.cap == vPizzeria.front().get().cap); });

            ::std::vector<unsigned> vPath(CNT_DIRECTIONS, 0u);
            ::std::pair<unsigned, double> decr = ::std::make_pair(-1, -1.);
            size_t piz_idx = -1;
            for(auto iter = start_it; iter != it; ++iter)
            {
                ::std::vector<unsigned> vCurPath(CNT_DIRECTIONS, 0u);
                ::std::pair<unsigned, double> cur_decr = ::std::make_pair(0u, -1.);
                piz_data temp = *iter;
                while(temp.cap)
                {
                    ::std::vector<::std::pair<unsigned, double>> vInfo(CNT_DIRECTIONS, ::std::make_pair(0u, -1.));
                    for(size_t c {0}; c < CNT_DIRECTIONS; ++c)
                    {
                        int cur_y = static_cast<ptrdiff_t>(temp.crd.y) + (c & 1 ? 0 : (temp.dir[c] + 1) * (c ? 1 : -1));
                        int cur_x = static_cast<ptrdiff_t>(temp.crd.x) + (c & 1 ? (temp.dir[c] + 1) * (c == 1 ? -1 : 1) : 0);
                        if (cur_x < 0 || cur_x >= map_x || cur_y < 0 || cur_y >= map_y || vMap[cur_y][cur_x].is == spec_point<piz_data>::state::def)
                        {
                            vInfo[c].first = -1;
                            continue;
                        }
                        for(size_t k {0}; k < CNT_DIRECTIONS; ++k)
                        {
                            if (c == (k - 2) % 4) continue;
                            if (vMap[cur_y][cur_x].targetDir[k])
                            {
                                unsigned temp_cap = ::std::min(vMap[cur_y][cur_x].targetDir[k]->cap,
                                    vMap[cur_y][cur_x].lengthDir[k] + 1 + vMap[cur_y][cur_x].lengthDir[(k - 2) % 4]);
                                int diff = temp_cap - vMap[cur_y][cur_x].lengthDir[k];
                                if (diff > 0)
                                {
                                    vInfo[c].first += diff;
                                    double temp_kf = (static_cast<double>(diff) / vMap[cur_y][cur_x].targetDir[k]->potential) * vMap[cur_y][cur_x].targetDir[k]->cap;
                                    if (vInfo[c].second < temp_kf) vInfo[c].second = temp_kf;
                                }
                            }
                        }
                    }
                    auto it_min = ::std::min_element(vInfo.begin(), vInfo.end(),
                        [](const ::std::pair<unsigned, double>& a, const ::std::pair<unsigned, double>& b) -> bool
                            {
                                return a.first < b.first || (a.first == b.first && a.second < b.second - 1e-7);
                            });
                    ++vCurPath[it_min - vInfo.begin()];
                    ++temp.dir[it_min - vInfo.begin()];
                    --temp.cap;
                    cur_decr.first += it_min->first;
                    if (cur_decr.second < it_min->second) cur_decr.second = it_min->second;
                }
                if (cur_decr.first < decr.first || (cur_decr.first == decr.first && cur_decr.second < decr.second))
                {
                    ::std::copy(vCurPath.begin(), vCurPath.end(), vPath.begin());
                    decr = cur_decr;
                    piz_idx = iter - vPizzeria.begin();
                }
            }
            
            if (piz_idx != -1)
            {
                for(size_t i {0}; i < CNT_DIRECTIONS; ++i)
                {
                    for(size_t j {1}; j <= vPath[i]; ++j)
                    {
                        updateMap(vMap,
                            static_cast<ptrdiff_t>(vPizzeria[piz_idx].get().crd.x) +
                                (i & 1 ? (i == 1 ? -vPizzeria[piz_idx].get().dir[i] - j : vPizzeria[piz_idx].get().dir[i] + j) : 0),
                            static_cast<ptrdiff_t>(vPizzeria[piz_idx].get().crd.y) +
                                (i & 1 ? 0 : (i ? vPizzeria[piz_idx].get().dir[i] + j : -vPizzeria[piz_idx].get().dir[i] - j)),
                            map_x, map_y,
                            static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr));
                    }
                    vPizzeria[piz_idx].get().dir[i] += vPath[i];
                    vPizzeria[piz_idx].get().cap -= vPath[i];
                }
                ::std::swap(vPizzeria[piz_idx], *(end_it - 1));
                --end_it;
            }
        }
        
        ::std::sort(vPizzeria.begin(), vPizzeria.end(), [](const piz_data& a, const piz_data& b) -> bool { return a.n < b.n; });
        ::std::cout << "Case " << ++nLoop << ":\n";
        for(auto& x : vPizzeria)
        {
            ::std::cout << x.get().dir[0] << ' ' << x.get().dir[3] << ' '
                << x.get().dir[2] << ' ' << x.get().dir[1] << ' ' << '[' << x.get().crd.x + 1 << ", " << x.get().crd.y + 1 << "]\n";
        }

        ::std::cin >> map_x;
    }

    return 0;
}

template<typename T>
void updateMap(::std::vector<::std::vector<spec_point<T>>>& vMap, int ax, int ay, unsigned map_x, unsigned map_y, T* ptr0, T* ptr1, T* ptr2, T* ptr3)
{
    vMap[ay][ax].is = spec_point<T>::state::def;
    if (ay > 0)
    {
        for(ptrdiff_t y {ay - 1}; y >= 0; --y)
        {
            vMap[y][ax].lengthDir[2] = ay - 1 - y;
            vMap[y][ax].targetDir[2] = ptr2;
            if (vMap[y][ax].is == spec_point<T>::state::def) break;
        }
    }
    if (ay + 1 < map_y)
    {
        for(size_t y {ay + 1}; y < map_y; ++y)
        {
            vMap[y][ax].lengthDir[0] = y - ay - 1;
            vMap[y][ax].targetDir[0] = ptr0;
            if (vMap[y][ax].is == spec_point<T>::state::def) break;
        }
    }
    if (ax > 0)
    {
        for(ptrdiff_t x {ax - 1}; x >= 0; --x)
        {
            vMap[ay][x].lengthDir[3] = ax - 1 - x;
            vMap[ay][x].targetDir[3] = ptr3;
            if (vMap[ay][x].is == spec_point<T>::state::def) break;
        }
    }
    if (ax + 1 < map_x)
    {
        for(size_t x {ax + 1}; x < map_x; ++x)
        {
            vMap[ay][x].lengthDir[1] = x - ax - 1;
            vMap[ay][x].targetDir[1] = ptr1;
            if (vMap[ay][x].is == spec_point<T>::state::def) break;
        }
    }
}