///
// Pizza
// Compile with:
// g++ -c -Wall -std=c++20 -ftree-vectorize -mavx
//
// Author:  Shibanov Evgeny
// Email:   shiba.zheka65@gmail.com
///

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

    coord() noexcept(true) : x(::std::numeric_limits<unsigned>::max()), y(::std::numeric_limits<unsigned>::max()) {}
    coord(unsigned ax, unsigned ay) noexcept(true) : x(ax), y(ay) {}

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

    piz_data() noexcept(true) : dir({0u, 0u, 0u, 0u}), cap(0u), potential(0u), n(0ull), crd() {}
    piz_data(unsigned ax, unsigned ay, unsigned acap, unsigned apotential, size_t idx) noexcept(true) : dir({0u, 0u, 0u, 0u}), cap(acap), potential(apotential), n(idx), crd(ax, ay) {}

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
    // 1 - w
    // 2 - s
    // 3 - e
    ::std::array<unsigned, CNT_DIRECTIONS> lengthDir;
    ::std::array<T*, CNT_DIRECTIONS> targetDir;

    spec_point() noexcept(true) : is(state::undef), lengthDir(), targetDir() {}

};

struct _spec_cell_data
{
    unsigned count; // damage_count, count
    double kf;      // damage_kf

    _spec_cell_data() noexcept(true) : count(::std::numeric_limits<decltype(count)>::max()), kf(::std::numeric_limits<decltype(kf)>::max()) {}
    _spec_cell_data(unsigned cnt, double kf) noexcept(true) : count(cnt), kf(kf) {}

};

unsigned findOptimalPath(::std::vector<_spec_cell_data>&, _spec_cell_data&, piz_data,
    unsigned, unsigned, const ::std::vector<::std::vector<spec_point<piz_data>>>&);
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
                ::std::fill(vMap[y][x].targetDir.begin(), vMap[y][x].targetDir.end(), nullptr);
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
                            size_t target_direction = ::std::numeric_limits<size_t>::max();
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
            ::std::sort(start_it, end_it,
                [](const piz_data& a, const piz_data& b) -> bool
                {
                    return (static_cast<double>(a.potential) / a.cap < static_cast<double>(b.potential) / b.cap) ||
                        (static_cast<double>(a.potential) / a.cap - static_cast<double>(b.potential) / b.cap < 1e-7 && a.cap > b.cap);
                });

            ::std::stable_sort(vPizzeria.begin(), end_it,
                [](const piz_data& a, const piz_data& b) -> bool
                {
                    return (a.cap > b.cap && b.cap == 0);
                });
            end_it = ::std::partition(vPizzeria.begin(), end_it, [](const piz_data& a) -> bool { return a.cap != 0u; });
            
            auto it = ::std::partition(start_it, end_it,
                [&](const piz_data& a) -> bool
                {
                    return ((static_cast<double>(a.potential) / a.cap - static_cast<double>(vPizzeria.front().get().potential) / vPizzeria.front().get().cap) < 1e-7) &&
                        (a.cap == vPizzeria.front().get().cap);
                });

            ::std::vector<_spec_cell_data> vPath(CNT_DIRECTIONS, {0u, 0.});
            _spec_cell_data decr;
            size_t piz_idx = ::std::numeric_limits<unsigned>::max();
            for(auto iter = start_it; iter != it; ++iter)
            {
                ::std::vector<_spec_cell_data> vCurPath(CNT_DIRECTIONS, {0u, 0.});
                _spec_cell_data cur_decr(0u, 0.);
                
                unsigned temp_res = findOptimalPath(vCurPath, cur_decr, *iter, map_x, map_y, vMap);
                if (!temp_res)
                {
                    if (cur_decr.kf < decr.kf - 1e-7 || (cur_decr.kf - decr.kf < 1e-7 && cur_decr.count < decr.count))
                    {
                        ::std::copy(vCurPath.begin(), vCurPath.end(), vPath.begin());
                        decr = cur_decr;
                        piz_idx = iter - vPizzeria.begin();
                    }
                }
            }
            
            if (piz_idx != ::std::numeric_limits<unsigned>::max())
            {
                for(size_t i {0}; i < CNT_DIRECTIONS; ++i)
                {
                    for(size_t j {1}; j <= vPath[i].count; ++j)
                    {
                        updateMap(vMap,
                            static_cast<ptrdiff_t>(vPizzeria[piz_idx].get().crd.x) +
                                (i & 1 ? (i == 1 ? -vPizzeria[piz_idx].get().dir[i] - j : vPizzeria[piz_idx].get().dir[i] + j) : 0),
                            static_cast<ptrdiff_t>(vPizzeria[piz_idx].get().crd.y) +
                                (i & 1 ? 0 : (i ? vPizzeria[piz_idx].get().dir[i] + j : -vPizzeria[piz_idx].get().dir[i] - j)),
                            map_x, map_y,
                            static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr), static_cast<piz_data*>(nullptr));
                    }
                    vPizzeria[piz_idx].get().dir[i] += vPath[i].count;
                    vPizzeria[piz_idx].get().cap -= vPath[i].count;
                }
                ::std::swap(vPizzeria[piz_idx], *(end_it - 1));
                --end_it;
            }
        }
        
        ::std::sort(vPizzeria.begin(), vPizzeria.end(), [](const piz_data& a, const piz_data& b) -> bool { return a.n < b.n; });
        ::std::cout << "Case " << ++nLoop << ":\n";
        for(auto& x : vPizzeria)
        {
            // n e s w
            ::std::cout << x.get().dir[0] << ' ' << x.get().dir[3] << ' '
                << x.get().dir[2] << ' ' << x.get().dir[1] << ' ' << '[' << x.get().crd.x + 1 << ", " << x.get().crd.y + 1 << "]\n";
        }

        ::std::cin >> map_x;
    }

    return 0;
}

unsigned findOptimalPath(::std::vector<_spec_cell_data>& vCurPath, _spec_cell_data& cur_decr, piz_data temp,
    unsigned map_x, unsigned map_y, const ::std::vector<::std::vector<spec_point<piz_data>>>& vMap)
{
    if(temp.cap)
    {
        ::std::vector<_spec_cell_data> vInfo(CNT_DIRECTIONS, {::std::numeric_limits<unsigned>::max(), ::std::numeric_limits<double>::max()});
        for(size_t c {0}; c < CNT_DIRECTIONS; ++c)
        {
            vInfo[c].count = 0u;
            vInfo[c].kf = 0.;
            int cur_y = static_cast<ptrdiff_t>(temp.crd.y) + (c & 1 ? 0 : static_cast<ptrdiff_t>(temp.dir[c] + 1) * (c ? 1 : -1));
            int cur_x = static_cast<ptrdiff_t>(temp.crd.x) + (c & 1 ? static_cast<ptrdiff_t>(temp.dir[c] + 1) * (c == 1 ? -1 : 1) : 0);
            if (cur_x < 0 || static_cast<decltype(map_x)>(cur_x) >= map_x ||
                cur_y < 0 || static_cast<decltype(map_y)>(cur_y) >= map_y || vMap[cur_y][cur_x].is == spec_point<piz_data>::state::def)
            {
                vInfo[c].count = ::std::numeric_limits<unsigned>::max();
                continue;
            }
            for(size_t k = {0}; k < CNT_DIRECTIONS; ++k)
            {
                if (c == (k - 2) % 4) continue;
                if (vMap[cur_y][cur_x].targetDir[k])
                {
                    unsigned temp_cap = ::std::min(vMap[cur_y][cur_x].targetDir[k]->cap,
                        vMap[cur_y][cur_x].lengthDir[k] + 1 + vMap[cur_y][cur_x].lengthDir[(k - 2) % 4]);
                    int diff = static_cast<int>(temp_cap) - vMap[cur_y][cur_x].lengthDir[k];
                    if (diff > 0)
                    {
                        vInfo[c].count += diff;
                        double res_potential = vMap[cur_y][cur_x].targetDir[k]->potential - diff;
                        if (res_potential)      res_potential = static_cast<double>(vMap[cur_y][cur_x].targetDir[k]->cap) / res_potential;
                        else                    res_potential = ::std::numeric_limits<double>::max();
                        if (res_potential > 1.)
                        {
                            vInfo[c].count = ::std::numeric_limits<unsigned>::max();
                            continue;
                        }
                        vInfo[c].kf += (static_cast<double>(vMap[cur_y][cur_x].targetDir[k]->cap) * diff) / vMap[cur_y][cur_x].targetDir[k]->potential;
                    }
                }
            }
        }
        for(size_t u {0}; u < CNT_DIRECTIONS; ++u)
        {
            auto it_min = ::std::min_element(vInfo.begin(), vInfo.end(),
                [](const _spec_cell_data& a, const _spec_cell_data& b) -> bool
                    {
                        return a.count < b.count || (a.count == b.count && a.kf < b.kf - 1e-7);
                    });
            size_t idx = it_min - vInfo.begin();
            if (vInfo[idx].count != ::std::numeric_limits<unsigned>::max())
            {
                int cur_y = static_cast<ptrdiff_t>(temp.crd.y) + (idx & 1 ? 0 : static_cast<ptrdiff_t>(temp.dir[idx] + 1) * (idx ? 1 : -1));
                int cur_x = static_cast<ptrdiff_t>(temp.crd.x) + (idx & 1 ? static_cast<ptrdiff_t>(temp.dir[idx] + 1) * (idx == 1 ? -1 : 1) : 0);
                auto _spec_update_potential = [&](int increaser) -> void
                {
                    vCurPath[idx].count += increaser;
                    temp.dir[idx] += increaser;
                    temp.cap -= increaser;
                    cur_decr.count += vInfo[idx].count;
                    cur_decr.kf += vInfo[idx].kf;
                    for(size_t k = {0}; k < CNT_DIRECTIONS; ++k)
                    {
                        if (idx == (k - 2) % 4) continue;
                        if (vMap[cur_y][cur_x].targetDir[k])
                        {
                            unsigned temp_cap = ::std::min(vMap[cur_y][cur_x].targetDir[k]->cap,
                                vMap[cur_y][cur_x].lengthDir[k] + 1 + vMap[cur_y][cur_x].lengthDir[(k - 2) % 4]);
                            int diff = static_cast<int>(temp_cap) - vMap[cur_y][cur_x].lengthDir[k];
                            if (diff > 0) vMap[cur_y][cur_x].targetDir[k]->potential -= diff * increaser;
                        }
                    }
                };
                _spec_update_potential(1);
                vInfo[idx].count = findOptimalPath(vCurPath, cur_decr, temp, map_x, map_y, vMap);
                if (vInfo[idx].count != ::std::numeric_limits<unsigned>::max())
                {
                    temp.cap = vInfo[idx].count;
                    break;
                }
                _spec_update_potential(-1);
            }
            else return ::std::numeric_limits<unsigned>::max();
        }
    }
    return temp.cap;
};

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
    if (static_cast<decltype(map_y)>(ay + 1) < map_y)
    {
        for(size_t y = 1ull + ay; y < map_y; ++y)
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
    if (static_cast<decltype(map_x)>(ax + 1) < map_x)
    {
        for(size_t x = 1ull + ax; x < map_x; ++x)
        {
            vMap[ay][x].lengthDir[1] = x - ax - 1;
            vMap[ay][x].targetDir[1] = ptr1;
            if (vMap[ay][x].is == spec_point<T>::state::def) break;
        }
    }
}