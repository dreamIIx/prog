#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include "./gnuplot-iostream/gnuplot-iostream.h"
#include <boost/qvm/mat_operations.hpp>

#if !defined(defDX_S)
#define defDX_S(x)		#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)		defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__	defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef ERROR_
#if defined(_WIN32)
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
#else
#error This UNIX operating system is not supported
#endif
#else
#error This operating system is not supported
#endif
#endif

#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

#define ISBIT(x,pos)        ( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)       ( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)       ( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)     ( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

struct vec
{
    double x;
    double y;

    vec() : x(0.), y(0.) {}
    vec(double ax, double ay) : x(ax), y(ay) {}

};

struct polynomial
{
    double a;
    double b;
    double c;
    double d;

    polynomial() : a(0.), b(0.), c(0.), d(0.) {}
    polynomial(double aa, double ab, double ac, double ad) : a(aa), b(ab), c(ac), d(ad) {}
};

// since szMATRIX == 64
double det(::std::vector<::std::vector<double>>& mtx, unsigned long long& exc, short count, int k)
{
    double res = 0.;
    short shift = -1;
    for(short i = 0; i < count; ++i)
    {
        if (!ISBIT(exc, i))
        {
            if (shift == -1)    shift = i;
            if (k < count - 1)
            {
                SETBIT(exc, i);
                res += (((i - shift) & 1) ? (-1) : (1)) * mtx[k][i] * det(mtx, exc, count, k + 1);
                UNSETBIT(exc, i);
            }
            else
            {
                return mtx[k][i];
            }
        }
        else
        {
            if (shift == -1)    ++shift;
            ++shift;
        }
    }

    return res;
}

::std::vector<::std::vector<double>> ReverseMatrix(::std::vector<::std::vector<double>> mtx)
{
    //return mtx;
    ER_IF(mtx.size() != mtx.back().size(),, throw ::std::runtime_error("Matrix is not square!"); )
    unsigned long long exc = 0ull;
    ER_IF(det(mtx, exc, mtx.size(), 0) == 0,, throw ::std::runtime_error("Det of matrix = 0!"); )

    ::std::vector<::std::vector<double>> ReverseResult;
    ReverseResult.reserve(mtx.size());
    for(size_t i = 0ull; i < mtx.size(); ++i)
    {
        ReverseResult.emplace_back(::std::vector<double>());
        ReverseResult.back().reserve(mtx.size());
        for(size_t j = 0ull; j < mtx.size(); ++j)
        {
            if (i != j) ReverseResult.back().emplace_back(0.);
            else        ReverseResult.back().emplace_back(1.);
        }
    }
    double kf = 0.;
    for(size_t i = 0ull; i < mtx.size() - 1; ++i)
    {
        for(size_t k = i + 1; k < mtx.size(); ++k)
        {
            if (mtx[k - 1][i] && mtx[k][i])
            {
                kf = -mtx[k][i] / mtx[k - 1][i];
                for(size_t j = 0ull; j < mtx.size(); ++j)
                {
                    ReverseResult[k][j] += ReverseResult[k - 1][j] * kf;
                    mtx[k][j] += mtx[k - 1][j] * kf;
                }
            }
        }
    }
    for(size_t j = 0; j < mtx.size(); ++j)
    {
        ReverseResult.back()[j] /= mtx.back().back();
        mtx.back()[j] /= mtx.back().back();
    }
    for(size_t i = mtx.size() - 1; i > 0ull; --i)
    {
        for(ptrdiff_t k = i - 1; k >= 0; --k)
        {
            if (mtx[k + 1][i] && mtx[k][i])
            {
                kf = -mtx[k][i] / mtx[k + 1][i];
                for(ptrdiff_t j = mtx.size() - 1; j >= 0; --j)
                {
                    ReverseResult[k][j] += ReverseResult[k + 1][j] * kf;
                    mtx[k][j] += mtx[k + 1][j] * kf;
                }
            }
        }
    }
    for(ptrdiff_t j = mtx.size() - 1; j >= 0; --j)
    {
        ReverseResult.front()[j] /= mtx.front().front();
        mtx.front()[j] /= mtx.front().front();
    }
    return ReverseResult;
}

::std::vector<::std::vector<double>> MultMtxOnMtx(const ::std::vector<::std::vector<double>>& w, const ::std::vector<::std::vector<double>>& v)
{
    //return w;
    ER_IF(w.back().size() != v.size(), ::std::cout << "Ill-formed vector!" << ::std::endl;, return ::std::vector<::std::vector<double>>(); )
    ::std::vector<::std::vector<double>> func_result;
    func_result.reserve(w.size());
    for(size_t i {0}; i < w.size(); ++i)
    {
        func_result.emplace_back(::std::vector<double>());
        func_result.back().reserve(v.back().size());
    }
    for(size_t k {0}; k < v.back().size(); ++k)
    {
        for(size_t i {0}; i < w.size(); ++i)
        {
            double temp_res_ = 0.;
            for(size_t j {0}; j < w.back().size(); ++j)
            {
                temp_res_ += w[i][j] * v[j][k];
            }
            func_result[i].emplace_back(temp_res_);
        }
    }
    return func_result;
}

double foo(double);

void interpolation(::std::string filename)
{
    ::std::ifstream read(filename);
    if(!read.is_open()) throw ::std::runtime_error("file not open");

    ::std::vector<vec> data;
    ::std::vector<polynomial> res;
    ::std::vector<::std::vector<double>> c;
    ::std::vector<::std::vector<double>> y;
    ::std::vector<::std::vector<double>> graph;
    size_t n = 0ull;
    read >> n;
    c.reserve(n - 1);
    y.reserve(n - 1);
    res.reserve(n - 2);
    graph.reserve(n - 2);
    for(size_t i = 0ull; i < n; ++i)
    {
        vec temp;
        read >> temp.x >> temp.y;
        data.reserve(data.capacity() + 1);
        data.emplace_back(temp);
    }
    read.close();

    double h = data[1].x - data[0].x;
    for(size_t i = 0ull; i < n - 2; ++i)
    {
        y.emplace_back(::std::vector<double>());
        y.back().reserve(1ull);
        y.back().emplace_back((data[i + 2].y - data[i + 1].y) / h - (data[i + 1].y - data[i].y) / h);
        c.emplace_back(::std::vector<double>());
        c.back().reserve(n - 1);
        for(size_t j = 0ull; j < n - 2; ++j)
        {
            if (i == j)                                                             c.back().emplace_back((1. / 6) * 4. * h);
            else if (static_cast<ptrdiff_t>(i) - static_cast<ptrdiff_t>(j) == -1)    c.back().emplace_back((1. / 6) * h);
            else if (static_cast<ptrdiff_t>(i) - static_cast<ptrdiff_t>(j) == 1)   c.back().emplace_back((1. / 6) * h);
            else                                                                    c.back().emplace_back(0.);
        }
    }
    //c = ::boost::qvm::inverse(c);
    c = ReverseMatrix(c);
    auto res_vector = MultMtxOnMtx(c, y);

    res.emplace_back(polynomial());
    res.back().a = data[0].y;
    for(size_t i = 0ull; i < n - 2; ++i)
    {
        ::std::cout << res_vector[i].front() << ::std::endl;
        /*res.emplace_back(polynomial());
        res.back().a = data[i].y;
        res.back().c = res_vector[i].front();
        res.back().d = (res_vector[i].front() - res_vector[static_cast<ptrdiff_t>(i) - 1 < 0 ? 0 : i - 1].front()) / 3. * (data[i + 1].x - data[i].x);
        res.back().b = (res.back().a - res[static_cast<ptrdiff_t>(i) - 1 < 0 ? 0 : i - 1].a) / (data[i + 1].x - data[i].x) -
            (data[i + 1].x - data[i].x) * (2. * res.back().c + res_vector[static_cast<ptrdiff_t>(i) - 1 < 0 ? 0 : i - 1].front()) / 3.;*/
        /*::std::cout << res.back().a << ::std::endl;
        ::std::cout << res.back().b << ::std::endl;
        ::std::cout << res.back().c << ::std::endl;
        ::std::cout << res.back().d << ::std::endl;*/
        size_t count = 100ul;
        double step = (data[i + 1].x - data[i].x) / count;
        for(size_t j = 0ull; j < count; ++j)
        {
            double x = data[i].x + j * step;
            graph.emplace_back(::std::vector<double>());
            graph.back().reserve(3ull);
            graph.back().emplace_back(x);
            /*graph.back().emplace_back(res.back().a + res.back().b * (x - data[i].x) + res.back().c * ::std::pow(x - data[i].x, 2.)
                + res.back().d * ::std::pow(x - data[i].x, 3.));*/
            graph.back().emplace_back(data[i].y * ((data[i + 1].x - x) / (data[i + 1].x - data[i].x)) + data[i + 1].y * ((x - data[i].x) / (data[i + 1].x - data[i].x))
                + res_vector[i].front() * ((::std::pow(data[i + 1].x - x, 3.) - ::std::pow(data[i + 1].x - data[i].x, 2.) * (data[i + 1].x - x))
                / (6. * (data[i].x - x)))
                + res_vector[i].front() * ((::std::pow(x - data[i].x, 3.) - ::std::pow(data[i + 1].x - data[i].x, 2.) * (x - data[i].x))
                / (6. * (data[i].x - x))));
            graph.back().emplace_back(foo(x));
        }
    }

    ::std::ofstream write("output_graph.txt");
    if (!write.is_open()) throw ::std::runtime_error("file not open");
    for(size_t i = 0ull; i < graph.size(); ++i)
    {
        for(size_t j = 0ull; j < graph[i].size(); ++j)
        {
            write << graph[i][j] << ' ';
        }
        write << ::std::endl;
    }
    write.close();

	//Gnuplot gp;
    //gp << "plot \"output_graph.txt\" u 1:2 w line, \"output_graph.txt\" u 1:3 w line linecolor 3";
    ::std::ofstream com("./commands.txt");
    if (!com.is_open()) throw ::std::runtime_error("file not open");
    // \'output_graph.txt\' u 1:2 w dots, 
    com << "gnuplot -e \"set size 1.0, 1.0; plot \'output_graph.txt\' u 1:3 w lines title \'input\', \
        \'input_graph.txt\' u 1:2 smooth csplines title \'output\' \" -pause" << ::std::endl;
    com.close();

    system("./commands.txt");
}

double foo(double x)
{
    return ::std::pow(x, 3.);
}

int main()
{
    double range1 = 0.;
    double range2 = 1.;
    size_t count = 6ull;
    double step = (range2 - range1) / count;
    ::std::ofstream write("input_graph.txt");
    if (!write.is_open()) throw ::std::runtime_error("file not open");
    write << count << ::std::endl;
    for(size_t i = 0ull; i < count; ++i)
    {
        double x = range1 + i * step;
        write << x <<  ' ' << foo(x) << ::std::endl;
    }
    write.close();

    interpolation("input_graph.txt");

    return 0;
}