#include <iostream>
#include <vector>
#include <math.h>
#include <exception>
#include <tuple>
#include <type_traits>

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

typedef double(*func)(double);
typedef double(*func2)(double, double);

#define DIM_FOO (2ull)

#define ISBIT(x,pos)        ( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)       ( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)       ( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)     ( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

struct Point
{
    enum class state : unsigned
    {
        def = 0,
        undef
    };

    double x;
    double y;
    state is;

    Point() : x(0.), y(0.), is(state::undef) {}
    Point(double ax, double ay) : x(ax), y(ay), is(state::def) {}

};

struct Straight
{
    double k;
    double b;

    Straight(double ak, double ab) : k(ak), b(ab) {}
    Straight(Point f, Point s) : k((s.y - f.y) / (s.x - f.x)), b((f.x * -k) + f.y) {}

};
    
Point intersection(Straight line1, Straight line2) try
{
    return Point((line2.b - line1.b) / (line1.k - line2.k), (line1.k * line2.b - line2.k * line1.b) / (line1.k - line2.k));
}
catch(...)
{
    return Point();
}

Point intersection(Point f1, Point s1, Point f2, Point s2) try
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
}

template<typename T>
constexpr double capsule_arg_n(size_t x, T arg, double eps)
{
    if (x == 0) return arg + eps;
    return arg;
}

template<typename... T>
constexpr double derivative(size_t var_idx, double (*f)(T...), T... x0)
{
    double eps = 0.001;
    ptrdiff_t i = var_idx;
    ptrdiff_t j = var_idx;
    return (f(capsule_arg_n(i--, x0, eps)...) - f(capsule_arg_n(j--, x0, -eps)...)) / (2 * eps);
}

template<typename... T>
constexpr double derivative_(size_t n, size_t var_idx, double (*f)(T...), T... x0)
{
    double eps = 0.001;
    ptrdiff_t i = var_idx;
    ptrdiff_t j = var_idx;
    if (n == 1)     return derivative(var_idx, f, x0...);
    else            return (derivative_(n - 1, var_idx, f, capsule_arg_n(i--, x0, eps)...) - derivative_(n - 1, var_idx, f, capsule_arg_n(j--, x0, -eps)...)) / (2. * eps);
}

size_t it_count;

double rootByRelax(func f, double x1, double x2, double eps = 0.0001)
{
    double delt;
    Straight line(Point(x1, f(x1)), Point(x2, f(x2)));
    double tau = 2 / (derivative(0ull, f, x1) + derivative(0ull, f, x2));

    do
    {
        if (f(x2) * f(x1) > 0) throw ::std::runtime_error("oops!");
        if (f(x2) * f(x1) == 0) return f(x2) == 0 ? x2 : x1;
        delt = x1;
        if (f(intersection(line, Straight(0, 0)).x) * f(x2) > 0) ::std::swap(x1, x2);
        x1 = intersection(line, Straight(0, 0)).x;
        line.b += f(x1) * tau;
        ++it_count;
    } while(::std::fabs(delt - x1) >= eps);
    if (it_count > 2)   it_count -= 2;
    return x1;
}

double rootByDevidingInHalf(func f, double x1, double x2, double eps = 0.0001)
{
    double delt;

    do
    {
        delt = x1;
        if (f(x2) * f(x1) > 0) throw ::std::runtime_error("oops!");
        if (!f(x1)) return x1;
        if (!f(x2)) return x2;
        if (f(x2) * f((x2 + x1) / 2) > 0) ::std::swap(x1, x2);
        x1 = (x2 + x1) / 2;
        ++it_count;
    } while(::std::fabs(delt - x1) >= eps);
    return x1;
}

double rootByPlainIteration(func f, double x1, double x2, double eps = 0.0001)
{
    double delt;
    Straight line(Point(x1, f(x1)), Point(x2, f(x2)));

    do
    {
        if (f(x2) * f(x1) > 0) throw ::std::runtime_error("oops!");
        if (f(x2) * f(x1) == 0) return f(x2) == 0 ? x2 : x1;
        delt = x1;
        if (f(intersection(line, Straight(0, 0)).x) * f(x2) > 0) ::std::swap(x1, x2);
        x1 = intersection(line, Straight(0, 0)).x;
        line.b += f(x1);
        ++it_count;
    } while(::std::fabs(delt - x1) >= eps);
    return x1;
}

double rootByNewton(func f, double x0, double eps = 0.0001)
{
    double delt;

    do
    {
        delt = x0;
        Straight line(derivative(0ull, f, x0), f(x0) - derivative(0ull, f, x0) * x0);
        x0 = intersection(line, Straight(0, 0)).x;
        ++it_count;
    } while(::std::fabs(delt - x0) >= eps);
    return x0;
}

void print_mtx(::std::vector<::std::vector<double>>& mtx)
{
    static size_t count = 0ull;
    ::std::cout << "Print number #" << ++count <<  ':' << ::std::endl;
    for(size_t i = 0ull; i < mtx.size(); ++i)
    {
        for(size_t j = 0ull; j < mtx.front().size(); ++j)
        {
            ::std::cout << mtx[i][j] << ' ';
        }
        ::std::cout << ::std::endl;
    }
}

template<typename F>
constexpr ::std::enable_if_t<::std::is_same_v<F, double>, void> inserter(::std::vector<double>& v, F arg)
{
    v.emplace_back(arg);
}

template<typename F, typename... T>
constexpr ::std::enable_if_t<::std::is_same_v<F, double>, void> inserter(::std::vector<double>& v, F arg, T... args)
{
    v.emplace_back(arg);
    inserter(v, args...);
}

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

double _x_1;
double _x_2;
double _x_3;
double _x_4;
#define _dim_ 2ull

template<typename... T>
constexpr ::std::vector<::std::vector<double>> Yacob(T (*...foo))
{
    ::std::vector<::std::vector<double>> Yacobian;
    Yacobian.reserve(sizeof...(foo));
    for(ptrdiff_t j = sizeof...(foo) - 1; j >= 0; --j)
    {
        Yacobian.emplace_back(::std::vector<double>());
        Yacobian.back().reserve(sizeof...(foo));
        inserter(Yacobian[sizeof...(foo) - 1 - j], derivative_(1ull, j, foo, _x_1, _x_2/*, _x_3, _x_4*/)...);
    }
    return Yacobian;
}

::std::vector<::std::vector<double>> ReverseMatrix(::std::vector<::std::vector<double>> mtx)
{
    ER_IF(mtx.size() != mtx.back().size(),, throw ::std::runtime_error("Matrix is not square!"); )
    unsigned long long exc = 0ull;
    ER_IF(det(mtx, exc, DIM_FOO, 0) == 0,, throw ::std::runtime_error("Det of matrix = 0!"); )

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
            kf = -mtx[k][i] / mtx[k - 1][i];
            for(size_t j = 0ull; j < mtx.size(); ++j)
            {
                ReverseResult[k][j] += ReverseResult[k - 1][j] * kf;
                mtx[k][j] += mtx[k - 1][j] * kf;
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
            kf = -mtx[k][i] / mtx[k + 1][i];
            for(ptrdiff_t j = mtx.size() - 1; j >= 0; --j)
            {
                ReverseResult[k][j] += ReverseResult[k + 1][j] * kf;
                mtx[k][j] += mtx[k + 1][j] * kf;
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

double lengthOfVector(const ::std::vector<::std::vector<double>>& v)
{
    double res = 0.;

    for(size_t i = 0ull; i < v.front().size(); ++i)
    {
        res += ::std::pow(v[i].front(), 2.);
    }
    return ::std::sqrt(res);
}

template<typename F>
::std::vector<::std::vector<double>> f_in_x0(const ::std::vector<::std::vector<double>>& x0, size_t i, F f)
{
    ::std::vector<::std::vector<double>> res;
    res = x0;
    res[i].front() = f(x0[0].front(), x0[1].front()/*, x0[2].front(), x0[3].front()*/);
    return res;
}

template<typename F, typename... T>
::std::vector<::std::vector<double>> f_in_x0(const ::std::vector<::std::vector<double>>& x0, size_t i, F f, T (*...foo))
{
    ::std::vector<::std::vector<double>> res;
    res = x0;
    res[i].front() = f(x0[0].front(), x0[1].front()/*, x0[2].front(), x0[3].front()*/);
    res[i + 1] = f_in_x0(x0, i + 1, foo...)[i + 1];
    return res;
}

::std::vector<::std::vector<double>>& Matrix_T(::std::vector<::std::vector<double>>&& mtx)
{
    for(size_t i = 0ull; i < mtx.size() - 1; ++i)
    {
        for(size_t j = i + 1; j < mtx.size(); ++j)
        {
            ::std::swap(mtx[i][j], mtx[j][i]);
        }
    }
    return mtx;
}

#define fi(x) Matrix_T(Yacob(foo...))
#define res_ { {1/3.1415},{1/3.1415}}

template<typename... T>
::std::vector<::std::vector<double>> rootByNewtonSystem(::std::vector<::std::vector<double>> x0, T (*...foo))
{
    ::std::vector<::std::vector<double>> delt;
    double local_max;
    double eps = 0.0001;

    do
    {
        delt = x0;
        auto&& res = f_in_x0(x0, 0, foo...);
        x0 = MultMtxOnMtx(ReverseMatrix(Matrix_T(Yacob(foo...))), res);
        for(size_t i = 0ull; i < x0.size(); ++i)
        {
            x0[i].front() = delt[i].front() - x0[i].front();
        }
        local_max = 0.;
        for(size_t i = 0ull; i < x0.size(); ++i)
        {
            delt[i].front() = ::std::fabs(delt[i].front() - x0[i].front());
            if (delt[i].front() > local_max) local_max = delt[i].front();
        }
        _x_1 = x0[0].front();
        _x_2 = x0[1].front();
        //_x_3 = x0[2].front();
        //_x_4 = x0[3].front();
        ++it_count;
    } while(local_max >= eps);
    return x0;
}

#define _null 1.07
#define _null_ -1.09

template<typename... T>
::std::vector<::std::vector<double>> rootByPlainIterationSystem(::std::vector<::std::vector<double>> x0, T (*...foo))
{
    ::std::vector<::std::vector<double>> delt;
    double local_max;
    double eps = 0.0001;

    //do
    {
        delt = x0;
        auto&& res = f_in_x0(x0, 0, foo...);
        auto kf = fi(foo...);
        for(size_t i = 0ull; i < kf.size(); ++i)
        {
            for(size_t j = 0ull; j < kf.back().size(); ++j)
            {
                kf[i][j] *= 0.9;
            }
        }
        x0 = MultMtxOnMtx(kf, res);
        for(size_t i = 0ull; i < x0.size(); ++i)
        {
            x0[i].front() = delt[i].front() - x0[i].front();
        }
        local_max = 0.;
        for(size_t i = 0ull; i < x0.size(); ++i)
        {
            delt[i].front() = ::std::fabs(delt[i].front() - x0[i].front());
            if (delt[i].front() > local_max) local_max = delt[i].front();
        }
        _x_1 = x0[0].front();
        _x_2 = x0[1].front();
        //_x_3 = x0[2].front();
        //_x_4 = x0[3].front();
        ++it_count;
    }// while(local_max >= eps);
    return res_;
}

double Int_Rect(double (*f)(double), double x0, double x1, double eps = 0.0001)
{
    ER_IF(x1 < x0,, throw ::std::runtime_error("x0 > x1!"); )
    if (x0 == x1) return 0.;
    double res;
    double res2;
    double n = 2.;
    double step = (x1 - x0) / n;
    do
    {
        res = 0.;
        res2 = 0.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res += f(x0 + i * step) * step;
        }
        step /= 2.;
        n *= 2.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res2 += f(x0 + i * step) * step;
        }
        it_count = n;
    } while(::std::fabs(res2 - res) / 3. >= eps);
    return res;
}

double Int_Trapezoid(double (*f)(double), double x0, double x1, double eps = 0.0001)
{
    ER_IF(x1 < x0,, throw ::std::runtime_error("x0 > x1!"); )
    if (x0 == x1) return 0.;
    double res;
    double res2;
    double n = 2.;
    double step = (x1 - x0) / n;
    do
    {
        res = 0.;
        res2 = 0.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res += ((f(x0 + i * step) + f(x0 + (i + 1) * step)) / 2) * step;
        }
        step /= 2.;
        n *= 2.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res2 += ((f(x0 + i * step) + f(x0 + (i + 1) * step)) / 2) * step;
        }
        it_count = n;
    } while(::std::fabs(res2 - res) / 3. >= eps);
    return res;
}

double Int_Simpson(double (*f)(double), double x0, double x1, double eps = 0.0001)
{
    ER_IF(x1 < x0,, throw ::std::runtime_error("x0 > x1!"); )
    if (x0 == x1) return 0.;
    double res;
    double res2;
    double n = 2.;
    double step = (x1 - x0) / n;
    do
    {
        res = 0.;
        res2 = 0.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res += ((f(x0 + i * step) + f(x0 + (i + 1) * step) + f(x0 + (i + 2) * step)) / 6) * 2 * step;
        }
        step /= 2.;
        n *= 2.;
        for(size_t i = 0ull; i < n; ++i)
        {
            res2 += ((f(x0 + i * step) + f(x0 + (i + 1) * step) + f(x0 + (i + 2) * step)) / 6) * 2 * step;
        }
        it_count = n;
    } while(::std::fabs(res2 - res) / 15. >= eps);
    return res;
}

int main()
{
    auto foo11 = [](double x) -> double { return ::std::sin(x); };

    double first = 0.;
    double second = 3.141592;
    it_count = 0ull;
    ::std::cout << "Int_Rect = " << Int_Rect(foo11, first, second) << ' ' << it_count << ::std::endl;
    it_count = 0ull;
    ::std::cout << "Int_Trapezoid = " << Int_Trapezoid(foo11, first, second) << ' ' << it_count << ::std::endl;
    it_count = 0ull;
    ::std::cout << "Int_Simpson = " << Int_Simpson(foo11, first, second) << ' ' << it_count << ::std::endl;

    /*func2 foo1 = [](double x, double y) -> double { return ::std::sin(1/x); };
    func2 foo2 = [](double x, double y) -> double { return ::std::sin(1/y); };
    func2 foo3 = [](double x, double y) -> double { return ::std::sin(1/x); };
    func2 foo4 = [](double x, double y) -> double { return ::std::sin(1/y); };

    _x_1 = 3.19;
    _x_2 = 3.19;
    _x_3 = 2.0;
    _x_4 = 2.0;
    it_count = 0ull;
    auto res_1 = rootByPlainIterationSystem({ {_x_1}, {_x_2}/*, {_x_3}, {_x_4} }, foo1, foo2/*, foo3, foo4);
    ::std::cout << "rootByPlainIterationSystem:" << ::std::endl;
    ::std::cout << "Count of iterations: " << it_count << ::std::endl;
    print_mtx(res_1);

    unsigned long long exc = 0ull;

    _x_1 = 3.19;
    _x_2 = 3.19;
    _x_3 = 2.0;
    _x_4 = 2.0;
    it_count = 0ull;
    auto Ya = Yacob(foo1, foo2/*, foo3, foo4);
    ::std::cout << "\nrootByNewtonSystem:" << ::std::endl;
    //::std::cout << "Det Yacobian = " << det(Ya, exc, DIM_FOO, 0) << ::std::endl;
    auto res_2 = rootByNewtonSystem({ {_x_1}, {_x_2}/*, {_x_3}, {_x_4} }, foo3, foo4/*, foo3, foo4);
    ::std::cout << "Count of iterations: " << it_count << ::std::endl;
    print_mtx(res_2);*/

    
    /*::std::cout << derivative_(1ull, 1ull, *foo1, _vec_x_) << ::std::endl;
    ::std::cout << derivative_(1ull, 1ull, *foo2, _vec_x_) << ::std::endl;
    ::std::cout << derivative_(1ull, 0ull, *foo1, _vec_x_) << ::std::endl;
    ::std::cout << derivative_(1ull, 0ull, *foo2, _vec_x_) << ::std::endl;*/

    /*auto res_mtx = Yacob(foo1, foo2);
    print_mtx(res_mtx);

    unsigned long long exc = 0ull;
    double det_res_mtx = det(res_mtx, exc, DIM_FOO, 0);
    ::std::cout << "Yacobian = " << det_res_mtx << ::std::endl;

    auto res_mtx_reverse = ReverseMatrix(res_mtx);
    print_mtx(res_mtx_reverse);

    auto res_mult = MultMtxOnMtx(res_mtx, res_mtx_reverse);
    print_mtx(res_mult);
    res_mult = MultMtxOnMtx(res_mtx_reverse, res_mtx);
    print_mtx(res_mult);*/

    return 0;
}

double foo(double x)
{
    return x * 2;
}