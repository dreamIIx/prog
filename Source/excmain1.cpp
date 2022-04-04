#include <iostream>
#include <stack>
#include <chrono>

int main()
{
    bool broken = false;
    int n;
    while(::std::cin >> n && n)
    {
        int cur;
        while(::std::cin >> cur && cur)
        {
	        ::std::chrono::time_point<::std::chrono::system_clock> startTime = ::std::chrono::system_clock::now();
            ::std::stack<::std::pair<int, int>> sI;
            sI.push({cur, 0});
            for(int i = 1; i < n; ++i)
            {
                ::std::cin >> cur;
                if (broken)
                {
                    continue;
                }
                if (cur > sI.top().first + 1)
                {
                    sI.push({cur, 0});
                }
                else if (cur == sI.top().first + 1)
                {
                    ++sI.top().first;
                    ++sI.top().second;
                }
                else if (cur == sI.top().first - sI.top().second - 1)
                {
                    ++sI.top().second;
                }
                else
                {
                    while (cur < sI.top().first - sI.top().second - 1)
                    {
                        auto [edge, shift] = sI.top();
                        sI.pop();
                        if (sI.top().first + shift + 1 == edge && cur <= sI.top().first - sI.top().second - 1)
                        {
                            sI.top().first = edge;
                            sI.top().second += shift + 1;
                            continue;
                        }
                        else broken = true;
                    }
                    ++sI.top().second;
                }
            }
            ::std::cout << "time = " << ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::system_clock::now() - startTime).count() << ::std::endl;
            if (broken)     ::std::cout << "No" << ::std::endl;
            else            ::std::cout << "Yes" << ::std::endl;
            broken = false;
        }
    }

    return 0;
}