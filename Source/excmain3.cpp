#include <iostream>
#include <vector>
#include <bitset>
#include <math.h>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <exception>

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

#define ISBIT(x,pos)		( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)		( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)		( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)		( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

constexpr size_t _X_						=	(15ull);
constexpr size_t _Y_						=	(10ull);
constexpr size_t _DIM_						=	(3ull);

typedef uint_least32_t	udata_t;
typedef int_least32_t   data_t;

// szOfInBits_...
constexpr size_t sz_UDATA_TYPE_				=	(sizeof(udata_t) * __CHAR_BIT__);
constexpr size_t sz_HALF_UDATA_TYPE_        =	(sizeof(udata_t) / 2) * __CHAR_BIT__;
constexpr size_t sz_QUARTER_UDATA_TYPE_		=	(sizeof(udata_t) / 4) * __CHAR_BIT__;
constexpr size_t sz_3_QUARTERS_UDATA_TYPE_	=	(sz_HALF_UDATA_TYPE_ + sz_QUARTER_UDATA_TYPE_);

// shifts
constexpr size_t _SHFT2_X_					=	(sizeof(udata_t) * __CHAR_BIT__ - _X_);

ptrdiff_t _y_;

enum clr_idx : ptrdiff_t
{
    none = -1ll,
    idxRED = 0ll,
    idxGREEN,
    idxBLUE
};

struct cluster_t
{
    udata_t lower_mask;
    int y_mask;
    int idx_pivot;
    int y_pivot;
    clr_idx color;
    int size;

    cluster_t() : lower_mask(0u), y_mask(-1), idx_pivot(-1), y_pivot(-1), color(clr_idx::none), size(1) {}

};

using vCluster_t = ::std::vector<cluster_t>;

void deleteCluster(udata_t (&)[][_Y_], vCluster_t&, clr_idx, ptrdiff_t, udata_t);
void makeClustersByRow(udata_t, vCluster_t&, udata_t);
void concatClustersRowByRow(udata_t, vCluster_t&, vCluster_t&, udata_t, clr_idx);
void concatClusters(vCluster_t&, bool (*comp)(const cluster_t&, const cluster_t&));
inline udata_t decrease_to_1(udata_t);
inline udata_t decrease_at_least_to_1(udata_t);
udata_t sumOfClusters(const vCluster_t&);
inline const char getColorByIndex(clr_idx);
udata_t deleteClustersByIntersectionRow(udata_t&, udata_t);
void shiftClusters(udata_t (&)[][_Y_]);

int main()
{
    ::std::conditional<(_X_ <= sz_UDATA_TYPE_), int, void>::type		__err_fallback_1; // _X_ is too large! [not compatible] =(

    size_t count_loop = 0ull;
    ::std::cin >> count_loop;

    for(size_t loop {0}; loop < count_loop; ++loop)
    {
        udata_t Data[_DIM_][_Y_]{0u};
        _y_ = _Y_;
        for(size_t y {0}; y < _Y_; ++y)
        {
            for(size_t x {0}; x < _X_; ++x)
            {
                char temp;
                ::std::cin >> temp;
                if (temp == 'R')
                {
                    SETBIT(Data[idxRED][y], x);
                }
                else if (temp == 'G')
                {
                    SETBIT(Data[idxGREEN][y], x);
                }
                else if (temp == 'B')
                {
                    SETBIT(Data[idxBLUE][y], x);
                }
                else
                {
                    ERROR_
                }
            }
        }
        ::std::cout << "Game " << loop + 1 << ':' << ::std::endl;

        size_t score = 0ull;
        size_t moveNum = 1ull;
        vCluster_t vClusters;
        vCluster_t vTempClusters;
        auto spec_predicat_ = [](const cluster_t& x, const cluster_t& y) -> bool
        {
            if (x.size == y.size)
            {
                if (x.idx_pivot == y.idx_pivot)
                {
                    return (x.y_pivot > y.y_pivot);
                }
                return (x.idx_pivot < y.idx_pivot);
            }
            return (x.size > y.size);
        };
        auto comparator = [](const cluster_t& a, const cluster_t& b) -> bool { return a.lower_mask & b.lower_mask && a.y_mask == b.y_mask && a.color == b.color; };
        auto heap_sort_spec = [&]()
        {
            vClusters.clear();
            if (_y_)
            {
                for(size_t dim_ {0}; dim_ < _DIM_; ++dim_)
                {
                    vTempClusters.clear();
                    makeClustersByRow(Data[dim_][_Y_ - _y_], vTempClusters, 0ull);
                    for(size_t y {_Y_ - _y_ + 1}; y < _Y_; ++y)
                    {
                        concatClustersRowByRow(Data[dim_][y], vTempClusters, vClusters, y, static_cast<clr_idx>(dim_));
                    }
                    for(size_t i {0}; i < vTempClusters.size(); ++i)
                    {
                        vClusters.reserve(vClusters.capacity() + 1);
                        vClusters.emplace_back(vTempClusters[i]);
                        vClusters.back().color = static_cast<clr_idx>(dim_);
                        vClusters.back().y_mask = _Y_;
                    }
                    concatClusters(vClusters, comparator);
                }
                ::std::make_heap(vClusters.begin(), vClusters.end(), spec_predicat_);
                ::std::sort_heap(vClusters.begin(), vClusters.end(), spec_predicat_);
            }
        };

        heap_sort_spec();

        while(vClusters.size() && vClusters.front().size != 1)
        {
            ptrdiff_t cur_idx = vClusters.front().y_mask - 1;
            clr_idx cur_color = vClusters.front().color;
            udata_t data_intersect = vClusters.front().lower_mask;
            deleteCluster(Data, vClusters, cur_color, cur_idx, data_intersect);
            shiftClusters(Data);
            size_t delt_score = static_cast<udata_t>(::std::pow(vClusters.front().size - 2, 2u));
            score += delt_score;
            ::std::cout << "Move " << moveNum << " at (" << _Y_ - (vClusters.front().y_pivot) << ','
                << vClusters.front().idx_pivot + 1 << "): removed " << vClusters.front().size
                << " balls of color " << getColorByIndex(vClusters.front().color) << ", got " << delt_score << " points." << ::std::endl;
            
            heap_sort_spec();
            ++moveNum;
        }
        if (!vClusters.size()) score += 1000ull;
        ::std::cout << "Final score: " << score << ", with " << sumOfClusters(vClusters) << " balls remaining." << ::std::endl;
    }

    return 0;
}

void deleteCluster(udata_t (&Data)[][_Y_], vCluster_t& vClusters, clr_idx cur_color, ptrdiff_t cur_idx, udata_t data_intersect)
{
    if(data_intersect & Data[cur_color][cur_idx])
    {
        data_intersect = deleteClustersByIntersectionRow(Data[cur_color][cur_idx], data_intersect);
        if (cur_idx > _Y_ - _y_)    deleteCluster(Data, vClusters, cur_color, cur_idx - 1, data_intersect);
        if (cur_idx < _Y_ - 1)          deleteCluster(Data, vClusters, cur_color, cur_idx + 1, data_intersect);
    }
}

void makeClustersByRow(udata_t row, vCluster_t& cl, udata_t y)
{
    bool found = false;
    size_t first = 0ull;
    size_t second;
    for(size_t x {0}; x < _X_; ++x)
    {
        if (GETBIT(row, _X_ - 1 - x))
        {
            if (!found)
            {
                first = x;
                cl.reserve(cl.capacity() + 1);
                cl.emplace_back();
                found = true;
            }
            else ++cl.back().size;
        }
        else
        {
            if (found)
            {
                second = x;
                cl.back().lower_mask = ((row << (_SHFT2_X_ + first)) >> (_SHFT2_X_ + first + (_X_ - second))) << (_X_ - second);
                cl.back().idx_pivot = decrease_at_least_to_1(cl.back().lower_mask);
                cl.back().y_pivot = y;
                cl.back().y_mask = y;
                found = false;
            }
        }
    }
    if (found)
    {
        second = _X_;
        cl.back().lower_mask = ((row << (_SHFT2_X_ + first)) >> (_SHFT2_X_ + first + (_X_ - second))) << (_X_ - second);
        cl.back().idx_pivot = decrease_at_least_to_1(cl.back().lower_mask);
        cl.back().y_pivot = y;
        cl.back().y_mask = y;
    }
}

void concatClustersRowByRow(udata_t row, vCluster_t& cl, vCluster_t& whole_cl, udata_t cur_y, clr_idx color)
{
    vCluster_t new_cl;
    makeClustersByRow(row, new_cl, cur_y);
    for(ptrdiff_t i {0}; i < cl.size(); ++i)
    {
        udata_t temp_mask = 0u;
        for(size_t j {0}; j < new_cl.size(); ++j)
        {
            if (cl[i].lower_mask & new_cl[j].lower_mask)
            {
                temp_mask |= new_cl[j].lower_mask;
                cl[i].size += new_cl[j].size;
                new_cl[j].size = 0;
                if (cl[i].idx_pivot == new_cl[j].idx_pivot)
                {
                    if (cl[i].y_pivot < new_cl[j].y_pivot)
                    {
                        cl[i].y_pivot = new_cl[j].y_pivot;
                    }
                }
                else if (cl[i].idx_pivot > new_cl[j].idx_pivot)
                {
                    cl[i].idx_pivot = new_cl[j].idx_pivot;
                    cl[i].y_pivot = new_cl[j].y_pivot;
                }
                cl[i].y_mask = cur_y;
            }
        }
        if (temp_mask) cl[i].lower_mask = temp_mask;
        else
        {
            whole_cl.reserve(whole_cl.capacity() + 1);
            whole_cl.emplace_back(cl[i]);
            whole_cl.back().color = color;
            whole_cl.back().y_mask = cur_y;
            cl.erase(cl.begin() + i);
            --i;
        }
    }
    for(size_t i {0}; i < new_cl.size(); ++i)
    {
        if (new_cl[i].size)
        {
            cl.reserve(cl.capacity() + 1);
            cl.emplace_back(new_cl[i]);
            cl.back().color = color;
        }
    }
    concatClusters(cl, [](const cluster_t& a, const cluster_t& b) -> bool { return a.lower_mask & b.lower_mask; });
    concatClusters(whole_cl,
        [](const cluster_t& a, const cluster_t& b) -> bool { return a.lower_mask & b.lower_mask && a.y_mask == b.y_mask && a.color == b.color; });
}

void concatClusters(vCluster_t& cl, bool (*comp)(const cluster_t&, const cluster_t&))
{
    for(size_t i {0}; i < cl.size(); ++i)
    {
        for(ptrdiff_t j {i + 1}; j < cl.size(); ++j)
        {
            if (comp(cl[i], cl[j]))
            {
                cl[i].lower_mask |= cl[j].lower_mask;
                cl[i].size += cl[j].size;
                if (cl[i].idx_pivot == cl[j].idx_pivot)
                {
                    if (cl[i].y_pivot < cl[j].y_pivot)
                    {
                        cl[i].y_pivot = cl[j].y_pivot;
                    }
                }
                else if (cl[i].idx_pivot > cl[j].idx_pivot)
                {
                    cl[i].idx_pivot = cl[j].idx_pivot;
                    cl[i].y_pivot = cl[j].y_pivot;
                }
                cl.erase(cl.begin() + j);
                --j;
            }
        }
    }
}

inline udata_t decrease_to_1(udata_t x)
{
    udata_t res = 0u;
    while(x != 1)
    {
        x >>= 1;
        ++res;
    }
    return res;
}

inline udata_t decrease_at_least_to_1(udata_t x)
{
    udata_t res = 0u;
    while(!(x & 1))
    {
        x >>= 1;
        ++res;
    }
    return res;
}

udata_t sumOfClusters(const vCluster_t& cl)
{
    return ::std::accumulate(cl.begin(), cl.end(), static_cast<udata_t>(0u),
        [](udata_t sum, const cluster_t& a) -> udata_t { return sum + static_cast<udata_t>(a.size); });
}

inline const char getColorByIndex(clr_idx x)
{
    const char static_table[_DIM_]{ 'R', 'G', 'B' };
    return static_table[x];
}

udata_t deleteClustersByIntersectionRow(udata_t& row, udata_t irow)
{
    udata_t res_row = 0u;
    vCluster_t new_cl;
    makeClustersByRow(row, new_cl, 0ull);
    for(ptrdiff_t i = new_cl.size() - 1; i >= 0; --i)
    {
        if (new_cl[i].lower_mask & irow)
        {
            row &= ~new_cl[i].lower_mask;
            res_row |= new_cl[i].lower_mask;
            new_cl.erase(new_cl.begin() + i);
            if (!new_cl.size()) break;
        }
    }
    return res_row;
}

void shiftClusters(udata_t (&data)[][_Y_])
{
    udata_t map[_X_]{0u};
    bool flag = true;
    for(ptrdiff_t y {_Y_ - _y_}; y < _Y_; ++y)
    {
        udata_t temp_y = (~(data[0][y] ^ data[1][y] ^ data[2][y]) << (_SHFT2_X_)) >> (_SHFT2_X_);
        if ((temp_y == ((1 << _X_) - 1)) && flag)
        {
            --_y_;
            continue;
        }
        else flag = false;
        for(size_t x {0}; x < _X_; ++x)
        {
            if (GETBIT(temp_y, _X_ - 1 - x))  SETBIT(map[x], y);
        }
    }
    for(size_t x {0}; x < _X_; ++x)
    {
        if (map[x] == ((1 << _y_) - 1) << (_Y_ - _y_))
        {
            for(size_t y = {_Y_ - _y_}; y < _Y_; ++y)
            {
                for(size_t i {0}; i < _DIM_; ++i)
                {
                    if (data[i][y])
                    {
                        data[i][y] = ((data[i][y] >> (_X_ - x)) << (_X_ - x - 1)) |
                            (data[i][y] << (_SHFT2_X_ + x) >> (_SHFT2_X_ + x));
                    }
                }
            }
        }
        else if(map[x])
        {
            vCluster_t vMap_X_clusters;
            makeClustersByRow(map[x], vMap_X_clusters, 0ull);
            for(ptrdiff_t i {vMap_X_clusters.size() - 1}; i >= 0; --i)
            {
                data_t lower_pos = decrease_to_1(vMap_X_clusters[i].lower_mask) + _Y_ - _y_;
                data_t upper_pos = decrease_at_least_to_1(vMap_X_clusters[i].lower_mask) + _Y_ - _y_;
                for(size_t y {_Y_ - _y_}; y < upper_pos; ++y)
                {
                    for(size_t j {0}; j < _DIM_; ++j)
                    {
                        if (GETBIT(data[j][upper_pos - y - 1], _X_ - 1 - x))
                        {
                            SETBIT(data[j][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[(j + 1) % _DIM_][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[(j + 2) % _DIM_][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[j][upper_pos - y - 1], _X_ - 1 - x);
                            break;
                        }
                    }
                }
            }
        }
    }
}