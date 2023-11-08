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
            ::std::stack<::std::pair<int, int>> sOnBranch;
            sOnBranch.push({cur - 1, cur - 1});
            int upperBound = cur;
            for(int i = 1; i < n; ++i)
            {
                ::std::cin >> cur;
                if (broken)
                {
                    // in order not to throw numbers in the input stream after a breakdown, print the answer at the end
                    continue;
                }
                if (cur == upperBound + 1) upperBound = cur;
                else if (cur > upperBound + 1)
                {
                    sOnBranch.push({cur - 1, cur - upperBound - 1});
                    upperBound = cur;
                }
                else if (cur < sOnBranch.top().first)
                {
                    broken = true;
                }
                else if (cur == sOnBranch.top().first)
                {
                    --sOnBranch.top().first;
                    --sOnBranch.top().second;
                }
                if (!sOnBranch.empty() && sOnBranch.top().second == 0) sOnBranch.pop();
            }
            if (broken)     ::std::cout << "No" << ::std::endl;
            else            ::std::cout << "Yes" << ::std::endl;
            broken = false;
        }
        ::std::cout << ::std::endl;
    }

    return 0;
}