///
// Railroads
// Compile with:
// g++ -c -Wall -std=c++20 -ftree-vectorize -mavx
//
// Author:  Shibanov Evgeny
// Email:   shiba.zheka65@gmail.com
///

#include <iostream>
#include <stack>

int main()
{
    bool broken = false;
    int n;
    while(::std::cin >> n && n)
    {
        int cur;
        while(::std::cin >> cur && cur)
        {
            ::std::stack<::std::pair<int, int>> sI;
            sI.push({cur, 0});
            for(int i = 1; i < n; ++i)
            {
                ::std::cin >> cur;
                if (broken)
                {
                    // in order not to throw numbers in the input stream after a breakdown, print the answer at the end
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
                    // if the current cluster is over, check if the transition to the previous one is correct
                    while (cur < sI.top().first - sI.top().second - 1)
                    {
                        auto [edge, shift] = sI.top();
                        sI.pop();
                        if (!sI.empty() && sI.top().first + shift + 1 == edge && cur <= sI.top().first - sI.top().second - 1)
                        {
                            sI.top().first = edge;
                            sI.top().second += shift + 1;
                            continue;
                        }
                        else
                        {
                            broken = true;
                            break;
                        }
                    }
                    ++sI.top().second;
                }
            }
            if (broken)     ::std::cout << "No" << ::std::endl;
            else            ::std::cout << "Yes" << ::std::endl;
            broken = false;
        }
        ::std::cout << ::std::endl;
    }

    return 0;
}