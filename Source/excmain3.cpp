#include <iostream>
#include <vector>
#include <bitset>
#include <math.h>
#include <type_traits>
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

typedef unsigned	udata_t;
typedef signed		data_t;

// szOfInBits_...
constexpr size_t sz_INT_TYPE_				=	(sizeof(udata_t) * 8);
constexpr size_t sz_HALF_INT_TYPE_			=	(sizeof(udata_t) / 2) * 8;
constexpr size_t sz_QUARTER_INT_TYPE_		=	(sizeof(udata_t) / 4) * 8;
constexpr size_t sz_3_QUARTERS_INT_TYPE_	=	(sz_HALF_INT_TYPE_ + sz_QUARTER_INT_TYPE_);

// shifts
constexpr size_t _SHFT2_X_					=	(sizeof(udata_t) * 8 - _X_);

ptrdiff_t _y_ = _Y_;

using cluster_t = ::std::pair<udata_t, udata_t>;
using vCluster_t = ::std::vector<cluster_t>;

enum clr_idx : size_t
{
    idxRED = 0ull,
    idxGREEN,
    idxBLUE
};

void makeClustersByRow(udata_t, vCluster_t&, udata_t);
void concatClustersRowByRow(udata_t, vCluster_t&, vCluster_t&, udata_t, clr_idx);
inline udata_t decrease_to_1(udata_t);
inline udata_t decrease_at_least_to_1(udata_t);
udata_t sumOfClusters(vCluster_t);
inline char getColorByIndex(clr_idx);
udata_t deleteClustersByIntersectionRow(udata_t&, udata_t);
void shiftClusters(udata_t[][_Y_]);

int main()
{
    ::std::conditional<(_X_ <= sz_HALF_INT_TYPE_), int, void>::type		__err_fallback_1; // _X_ is too large! [not compatible] =(
    ::std::conditional<(_Y_ <= sz_INT_TYPE_), int, void>::type			__err_fallback_2; // _Y_ is too large! [not compatible] =(

    size_t count_loop = 0ull;
    ::std::cin >> count_loop;

    for(size_t loop {0}; loop < count_loop; ++loop)
    {
        ::std::cout << "Game " << loop + 1 << ':' << ::std::endl;
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

        size_t score = 0ull;
        size_t moveNum = 1ull;
        vCluster_t vClusters;
        vCluster_t vTempClusters;
        auto spec_predicat_ = [](cluster_t x, cluster_t y)
        {
            if ((x.second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_ == (y.second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_)
            {
                if (((x.second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) == ((y.second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_))
                {
                    return ((x.first << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) > ((y.first << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_);
                }
                return (((x.second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) < ((y.second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_));
            }
            return (x.second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_ > (y.second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_;
        };
        auto heap_sort_spec = [&]()
        {
            vClusters.clear();
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
                    vClusters.emplace_back(::std::make_pair((dim_ << sz_3_QUARTERS_INT_TYPE_) | (_Y_ << sz_HALF_INT_TYPE_) | vTempClusters[i].first, vTempClusters[i].second));
                }
            }
            ::std::make_heap(vClusters.begin(), vClusters.end(), spec_predicat_);
            ::std::sort_heap(vClusters.begin(), vClusters.end(), spec_predicat_);
        };

        heap_sort_spec();

        while(vClusters.size() && (vClusters.front().second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_ != 1)
        {
            size_t row_i = 1ull;
            udata_t data_tersect = vClusters.front().first & Data[vClusters.front().first >> sz_3_QUARTERS_INT_TYPE_][((vClusters.front().first << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) - row_i];
            while(data_tersect)
            {
                data_tersect = deleteClustersByIntersectionRow(Data[vClusters.front().first >> sz_3_QUARTERS_INT_TYPE_][((vClusters.front().first << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) - row_i++], data_tersect);
                data_tersect &= Data[vClusters.front().first >> sz_3_QUARTERS_INT_TYPE_][((vClusters.front().first << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) - row_i];
            }
            shiftClusters(Data);
            size_t delt_score = static_cast<udata_t>(::std::pow(((vClusters.front().second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_) - 2, 2u));
            score += delt_score;
            ::std::cout << "Move " << moveNum << " at (" << _Y_ - (vClusters.front().second >> sz_3_QUARTERS_INT_TYPE_) << ','
                << ((vClusters.front().second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) + 1 << "): removed " << ((vClusters.front().second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_)
                << " balls of color " << getColorByIndex(static_cast<clr_idx>(vClusters.front().first >> sz_3_QUARTERS_INT_TYPE_)) << ", got " << delt_score << " points." << ::std::endl;

            heap_sort_spec();
            ++moveNum;
        }
        if (!vClusters.size()) score += 1000ull;
        ::std::cout << "Final score: " << score << ", with " << sumOfClusters(vClusters) << " balls remaining." << ::std::endl;
    }

    return 0;
}

void makeClustersByRow(udata_t row, vCluster_t& cl, udata_t prev_y)
{
    bool found = false;
    size_t first = 0;
    size_t second = 0;
    for(size_t x {0}; x < _X_; ++x)
    {
        if (GETBIT(row, _X_ - 1 - x))
        {
            if (!found)
            {
                first = x;
                cl.reserve(cl.capacity() + 1);
                cl.emplace_back(::std::make_pair(0, 1));
                found = true;
            }
            else ++cl.back().second;
        }
        else
        {
            if (found)
            {
                second = x;
                cl.back().first = ((row << (_SHFT2_X_ + first)) >> (_SHFT2_X_ + first + (_X_ - second))) << (_X_ - second);
                cl.back().second |= decrease_at_least_to_1(cl.back().first) << sz_HALF_INT_TYPE_;
                cl.back().second |= prev_y << sz_3_QUARTERS_INT_TYPE_;
                found = false;
            }
        }
    }
    if (found)
    {
        second = _X_;
        cl.back().first = ((row << (_SHFT2_X_ + first)) >> (_SHFT2_X_ + first + (_X_ - second))) << (_X_ - second);
        cl.back().second |= decrease_at_least_to_1(cl.back().first) << sz_HALF_INT_TYPE_;
        cl.back().second |= prev_y << sz_3_QUARTERS_INT_TYPE_;
        found = false;
    }
}

void concatClustersRowByRow(udata_t row, vCluster_t& cl, vCluster_t& whole_cl, udata_t prev_y, clr_idx color)
{
    vCluster_t new_cl;
    makeClustersByRow(row, new_cl, prev_y);
    for(ptrdiff_t i {0}; i < cl.size(); ++i)
    {
        udata_t temp = 0;
        for(size_t j {0}; j < new_cl.size(); ++j)
        {
            if (cl[i].first & new_cl[j].first)
            {
                temp |= new_cl[j].first;
                cl[i].second += (new_cl[j].second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_;
                new_cl[j].second >>= sz_HALF_INT_TYPE_;
                if (((cl[i].second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) >= (new_cl[j].second << sz_3_QUARTERS_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_)
                {
                    cl[i].second = ((cl[i].second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_) | (new_cl[j].second << sz_HALF_INT_TYPE_);
                }
                new_cl[j].second <<= sz_HALF_INT_TYPE_;
            }
        }
        if (temp) cl[i].first = temp;
        else
        {
            whole_cl.reserve(whole_cl.capacity() + 1);
            whole_cl.emplace_back(::std::make_pair((color << sz_3_QUARTERS_INT_TYPE_) | (prev_y << sz_HALF_INT_TYPE_) | cl[i].first, cl[i].second));
            cl.erase(cl.begin() + i);
            --i;
        }
    }
    for(size_t i {0}; i < new_cl.size(); ++i)
    {
        if ((new_cl[i].second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_)
        {
            cl.reserve(cl.capacity() + 1);
            cl.emplace_back(new_cl[i]);
        }
    }
    for(size_t i {0}; i < cl.size(); ++i)
    {
        for(size_t j {i + 1}; j < cl.size(); ++j)
        {
            if (cl[i].first & cl[j].first)
            {
                cl[i].second += (cl[j].second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_;
                if (((cl[i].second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) > ((cl[j].second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_))
                {
                    cl[i].second = ((cl[i].second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_) | ((cl[j].second >> sz_HALF_INT_TYPE_) << sz_HALF_INT_TYPE_);
                }
                else if (((cl[i].second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_) == ((cl[j].second << sz_QUARTER_INT_TYPE_) >> sz_3_QUARTERS_INT_TYPE_))
                {
                    if ((cl[i].second >> sz_3_QUARTERS_INT_TYPE_) < (cl[j].second >> sz_3_QUARTERS_INT_TYPE_))
                    {
                        cl[i].second = ((cl[i].second << sz_QUARTER_INT_TYPE_) >> sz_QUARTER_INT_TYPE_) | ((cl[j].second >> sz_3_QUARTERS_INT_TYPE_) << sz_3_QUARTERS_INT_TYPE_);
                    }
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

udata_t sumOfClusters(vCluster_t cl)
{
    udata_t res = 0u;
    for(auto& x : cl)
    {
        res += (x.second << sz_HALF_INT_TYPE_) >> sz_HALF_INT_TYPE_;
    }
    return res;
}

inline char getColorByIndex(clr_idx x)
{
    char static_table[_DIM_]{ 'R', 'G', 'B' };
    return static_table[x];
}

udata_t deleteClustersByIntersectionRow(udata_t& row, udata_t irow)
{
    udata_t res_row = 0u;
    vCluster_t new_cl;
    makeClustersByRow(row, new_cl, 0ull);
    for(ptrdiff_t i = new_cl.size() - 1; i >= 0; --i)
    {
        if (new_cl[i].first & irow)
        {
            row &= ~new_cl[i].first;
            res_row |= new_cl[i].first;
            new_cl.erase(new_cl.begin() + i);
            if (!new_cl.size()) break;
        }
    }
    return res_row;
}

void shiftClusters(udata_t data[][_Y_])
{
    udata_t map[_X_]{0u};
    bool flag = true;
    for(ptrdiff_t y {_Y_ - _y_ + 1}; y < _Y_; ++y)
    {
        udata_t temp_y = (~(data[0][y] ^ data[1][y] ^ data[2][y]) << (_SHFT2_X_ + (_X_ - _X_))) >> (_SHFT2_X_ + (_X_ - _X_));
        if ((temp_y == ((1 << _X_) - 1)) && flag)
        {
            if (_y_ == 10) --_y_;
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
        if (map[x] == ((1 << _y_) - 2) << (_Y_ - _y_))
        {
            for(size_t y = {_Y_ - _y_}; y < _Y_; ++y)
            {
                for(size_t i {0}; i < 3ull; ++i)
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
                data_t lower_pos = decrease_to_1(vMap_X_clusters[i].first) + _Y_ - _y_;
                data_t upper_pos = decrease_at_least_to_1(vMap_X_clusters[i].first) + _Y_ - _y_;
                for(size_t y {_Y_ - _y_}; y < upper_pos; ++y)
                {
                    for(size_t i {0}; i < 3ull; ++i)
                    {
                        if (GETBIT(data[i][upper_pos - y - 1], _X_ - 1 - x))
                        {
                            SETBIT(data[i][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[(i + 1) % 3][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[(i + 2) % 3][lower_pos - y], _X_ - 1 - x);
                            UNSETBIT(data[i][upper_pos - y - 1], _X_ - 1 - x);
                            break;
                        }
                    }
                }
            }
        }
    }
}