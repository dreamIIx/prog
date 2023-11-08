#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>
#include <algorithm>

::std::vector<int> get_vec_from_stdin(size_t sz, char delim)
{
    ::std::vector<int> res;
    res.reserve(sz);
    int temp;
    for(size_t i {0}; i < sz - 1; ++i)
    {
        ::std::cin >> temp;
        res.emplace_back(temp);
    }
    ::std::string in;
    ::std::getline(::std::cin, in, delim);
    ::std::istringstream iss(in);
    iss >> temp;
    res.emplace_back(temp);
    return res;
}

int main()
{
    size_t n;
    {
        ::std::string in;
        ::std::getline(::std::cin, in, ';');
        ::std::istringstream iss(in);
        iss >> n;
    }
    decltype(auto) a_i = get_vec_from_stdin(n, ';');
    decltype(auto) c_i = get_vec_from_stdin(n, ';');
    decltype(auto) c0_i = get_vec_from_stdin(n, '\n');

    size_t nz = 0ull;
    for(long long i {n - 1}; i >= 0ll; --i)
    {
        if (a_i[i] != 0)
        {
            nz = i + 1;
            break;
        }
    }
    if (!nz)
    {
        ::std::cout << 0 << '\n';
        return 0;
    }
    ::std::vector<int> a_max_i(nz, 1);
    auto it = a_i.rbegin();
    ::std::advance(it, static_cast<long long>(n) - nz);
    ::std::partial_sum(it, a_i.rend(), a_max_i.rbegin());

    ::std::vector<int> s1(a_max_i.front() + 1, 0);
    for(size_t y {0}; y < a_max_i.front(); ++y)
    {
        size_t reverse_y = a_max_i.front() - y;
        s1[y] = reverse_y * c_i.front() + c0_i.front();
    }
    for(size_t k {1}; k < nz; ++k)
    {
        for(long long y {0}; y < a_max_i[k]; ++y)
        {
            int min = s1[y];
            for(size_t x_k {1}; x_k <= a_max_i[k] - y; ++x_k)
            {
                int min_cur = s1[y + x_k] + x_k * c_i[k] + c0_i[k];
                if (min_cur < min) min = min_cur;
            }
            s1[y] = min;
        }
    }
    ::std::cout << s1.front() << '\n';

    return 0;
}