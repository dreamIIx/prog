#include <iostream>
#include <fstream>
#include <math.h>
#include <math.h>
#include <vector>
#include <exception>
#include <iomanip>

::std::vector<::std::pair<double, double>> methodEuler(double (*foo)(double, double), double y0, double x0, double xn, size_t n)
{
    ::std::vector<::std::pair<double, double>> res;
    res.reserve(n + 1);
    res.emplace_back(::std::make_pair(x0, y0));
    double step = (xn - x0) / n;
    for(size_t i {0}; i < n; ++i)
    {
        res.emplace_back(::std::make_pair(res[i].first + step, res[i].second + step * foo(res[i].first, res[i].second)));
    }
    return res;
}

::std::vector<::std::pair<double, double>> methodRungeKutty(double (*foo)(double, double), double y0, double x0, double xn, size_t n)
{
    ::std::vector<::std::pair<double, double>> res;
    res.reserve(n + 1);
    res.emplace_back(::std::make_pair(x0, y0));
    double step = (xn - x0) / n;
    for(size_t i {0}; i < n; ++i)
    {
        double k1 = foo(res[i].first, res[i].second);
        double k2 = foo(res[i].first + step / 2., res[i].second + (step / 2.) * k1);
        double k3 = foo(res[i].first + step / 2., res[i].second + (step / 2.) * k2);
        double k4 = foo(res[i].first + step, res[i].second + step * k3);
        res.emplace_back(res[i].first + step, res[i].second + ((step) / 6.) * (k1 + 2 * k2 + 2 * k3 + k4));
    }
    return res;
}

::std::vector<::std::pair<double, double>> methodAdams(double (*foo)(double, double), double y0, double x0, double xn, size_t n)
{
    if (n < 3) throw ::std::runtime_error("n < 3");
    ::std::vector<::std::pair<double, double>> res;
    ::std::vector<::std::vector<double>> q;
    double step = (xn - x0) / n;
    res.reserve(n + 1);
    q.reserve(n + 1);
    res.emplace_back(::std::make_pair(x0, y0));
    for(size_t i {0}; i <= n; ++i)
    {
        q.emplace_back(::std::vector<double>());
        q.back().reserve(4ull);
    }
    q.front().emplace_back(res.front().second * step);
    for(size_t i {0}; i < 3; ++i)
    {
        double k1 = foo(res[i].first, res[i].second);
        double k2 = foo(res[i].first + step / 2., res[i].second + (step / 2.) * k1);
        double k3 = foo(res[i].first + step / 2., res[i].second + (step / 2.) * k2);
        double k4 = foo(res[i].first + step, res[i].second + step * k3);
        res.emplace_back(res[i].first + step, res[i].second + ((step) / 6.) * (k1 + 2 * k2 + 2 * k3 + k4));
        q[i + 1].emplace_back(foo(res.back().first, res.back().second) * step);
        q[i].emplace_back(q[i + 1][0] - q[i][0]);
        if (i > 0) q[i - 1].emplace_back(q[i][1] - q[i - 1][1]);
    }
    for(size_t i {3}; i < n; ++i)
    {
        double y_delt = q[i][0] + 0.5 * q[i - 1][1] + (5. / 12.) * q[i - 2][2] + (3. / 8.) * q[i - 3][3];
        res.emplace_back(res[i].first + step, res[i].second + y_delt);
        q[i + 1].emplace_back(foo(res.back().first, res.back().second) * step);
        q[i].emplace_back(q[i + 1][0] - q[i][0]);
        q[i - 1].emplace_back(q[i][1] - q[i - 1][1]);
        q[i - 2].emplace_back(q[i - 1][2] - q[i - 2][2]);
    }
    return res;
}

::std::vector<::std::pair<double, double>> methodAdams_withEuler(double (*foo)(double, double), double y0, double x0, double xn, size_t n)
{
    if (n < 3) throw ::std::runtime_error("n < 3");
    ::std::vector<::std::pair<double, double>> res;
    ::std::vector<::std::vector<double>> q;
    double step = (xn - x0) / n;
    res.reserve(n + 1);
    q.reserve(n + 1);
    res.emplace_back(::std::make_pair(x0, y0));
    for(size_t i {0}; i <= n; ++i)
    {
        q.emplace_back(::std::vector<double>());
        q.back().reserve(4ull);
    }
    q.front().emplace_back(res.front().second * step);
    for(size_t i {0}; i < 3; ++i)
    {
        res.emplace_back(::std::make_pair(res[i].first + step, res[i].second + step * foo(res[i].first, res[i].second)));
        q[i + 1].emplace_back(foo(res.back().first, res.back().second) * step);
        q[i].emplace_back(q[i + 1][0] - q[i][0]);
        if (i > 0) q[i - 1].emplace_back(q[i][1] - q[i - 1][1]);
    }
    for(size_t i {3}; i < n; ++i)
    {
        double y_delt = q[i][0] + 0.5 * q[i - 1][1] + (5. / 12.) * q[i - 2][2] + (3. / 8.) * q[i - 3][3];
        res.emplace_back(res[i].first + step, res[i].second + y_delt);
        q[i + 1].emplace_back(foo(res.back().first, res.back().second) * step);
        q[i].emplace_back(q[i + 1][0] - q[i][0]);
        q[i - 1].emplace_back(q[i][1] - q[i - 1][1]);
        q[i - 2].emplace_back(q[i - 1][2] - q[i - 2][2]);
    }
    return res;
}

int main()
{
    auto foo = [](double x, double y) -> double { return ::std::pow(y, 2. / 3.); };
    auto foo_explicit = [](double x) -> double { return 0.; };
    double x0 = 0.;
    double xn = 10.;
    double y0 = 0.;
    size_t n = 100;
    auto res1 = methodEuler(foo, y0, x0, xn, n);
    auto res2 = methodRungeKutty(foo, y0, x0, xn, n);
    auto res3 = methodAdams_withEuler(foo, y0, x0, xn, n);

    ::std::vector<::std::pair<double, double>> res_explicit;
    res_explicit.reserve(n + 1);
    double step = (xn - x0) / n;
    for(size_t i {0}; i <= n; ++i)
    {
        res_explicit.emplace_back(::std::make_pair(x0 + i * step, foo_explicit(x0 + i * step)));
    }

    ::std::vector<::std::vector<double>> err;
    err.reserve(n + 1);
    for(size_t i {0}; i <= n; ++i)
    {
        err.emplace_back(::std::vector<double>());
        err.back().reserve(3ull);
        err.back().emplace_back(::std::fabs((res1[i].second - res_explicit[i].second) / res_explicit[i].second));
        err.back().emplace_back(::std::fabs((res2[i].second - res_explicit[i].second) / res_explicit[i].second));
        err.back().emplace_back(::std::fabs((res3[i].second - res_explicit[i].second) / res_explicit[i].second));
    }

    ::std::ofstream write("./graph.txt");
    if (!write.is_open()) throw ::std::runtime_error("file is not open!");
    for(size_t i {0}; i <= n; ++i)
    {
        write << res_explicit[i].first << ' ' << res1[i].second <<  ' ' << res2[i].second <<  ' ' << res3[i].second << ' ' << res_explicit[i].second
            << ' ' << err[i][0] << ' ' << err[i][1] << ' ' << err[i][2] << ::std::endl;
        /*::std::cout.width(0);
        ::std::cout << 'x';
        ::std::cout.width(3);
        ::std::cout << ::std::left << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].first;
        ::std::cout.width(0);
        ::std::cout << 'y';
        ::std::cout.width(3);
        ::std::cout << ::std::left << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << res1[i].second;
        ::std::cout.width(16);
        ::std::cout << res2[i].second;
        ::std::cout.width(16);
        ::std::cout << res3[i].second;
        ::std::cout.width(16);
        ::std::cout.width(0);
        ::std::cout << '[';
        ::std::cout << res_explicit[i].second;
        ::std::cout.width(0);
        ::std::cout << ']' << ::std::endl;*/

    }
    //::std::cout << "-----------------" << ::std::endl;

    double max_err1 = 0.;
    double max_err2 = 0.;
    double max_err3 = 0.;
    for(size_t i {0}; i <= n; ++i)
    {
        if (max_err1 < err[i][0]) max_err1 = err[i][0];
        if (max_err2 < err[i][1]) max_err2 = err[i][1];
        if (max_err3 < err[i][2]) max_err3 = err[i][2];
        /*::std::cout.width(0);
        ::std::cout << 'x';
        ::std::cout.width(3);
        ::std::cout << ::std::left << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].first;
        ::std::cout.width(0);
        ::std::cout << "relative_error";
        ::std::cout.width(3);
        ::std::cout << ::std::left << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << err[i][0];
        ::std::cout.width(16);
        ::std::cout << err[i][1];
        ::std::cout.width(16);
        ::std::cout << err[i][2] << ::std::endl;
        ::std::cout.width(0);*/
    }
    //::std::cout << "-----------------" << ::std::endl;
    ::std::cout << "max_relative_error = ";
    ::std::cout.width(16);
    ::std::cout << max_err1;
    ::std::cout.width(16);
    ::std::cout << max_err2;
    ::std::cout.width(16);
    ::std::cout << max_err3 << ::std::endl;
    ::std::cout.width(0);

#if defined(__unix__)
#if defined(__linux__)
    system("python ./Source/draw_graph.py");
#endif
#elif defined(_WIN32)
    system("py ./Source/draw_graph.py");
#else
    #error This operating system is not supported
#endif

    /*for(size_t i {0}; i < res2.size(); ++i)
    {
        ::std::cout.width(0);
        ::std::cout << 'x';
        ::std::cout.width(3);
        ::std::cout << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].first;
        ::std::cout.width(0);
        ::std::cout << 'y';
        ::std::cout.width(3);
        ::std::cout << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].second << ::std::endl;
    }
    ::std::cout << "-----------------" << ::std::endl;

    for(size_t i {0}; i < res3.size(); ++i)
    {
        ::std::cout.width(0);
        ::std::cout << 'x';
        ::std::cout.width(3);
        ::std::cout << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].first;
        ::std::cout.width(0);
        ::std::cout << 'y';
        ::std::cout.width(3);
        ::std::cout << i;
        ::std::cout.width(0);
        ::std::cout << " = ";
        ::std::cout.width(16);
        ::std::cout << ::std::left << res1[i].second << ::std::endl;
    }
    ::std::cout << "-----------------" << ::std::endl;*/

    return 0;
}