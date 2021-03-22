#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifndef ER_IF
#define ER_IF(x, Exc) if ( (x) ) { Exc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, Exc) if ( !(x) ) { Exc }
#endif

#define _SZSTRING (size_t)(1024)
#define _SZEXPRE (size_t)(64)
#define _NBKTRCS (size_t)(128)

int regex_match(char**, char**, unsigned char);
char* __specfind(char*);
void spec_add_nts(char**, int);
int __equal(int, char);

int main()
{
    size_t K;
    char* regexstr = NULL;
    ER_IFN(regexstr = (char*) malloc(_SZEXPRE * sizeof(char)), return -1; )
    scanf("%s", regexstr);

    scanf("%zu", &K);
    char* str;
    ER_IFN(str = (char*) malloc(_SZSTRING * sizeof(char)), return -1; )
    // approximate ~(K * 2)
    char resStr[K * 2];
    char* pResStr = resStr;
    unsigned char nMatch = 0u;
    int res = 0;
    for(size_t i = 0ul; i < K; ++i)
    {
        scanf("%s", str);
        if ((res = regex_match(&regexstr, &str, 0)))
        {
            spec_add_nts(&pResStr, i);
            ++nMatch;
        }
        printf("%i\n", res);
    }
    *pResStr = '\0';
    if (!nMatch) printf("none");
    else printf("%s", resStr);

    return 0;
}

int regex_match(char** regex, char** str, unsigned char rec)
{
    char* pStr = *str;
    char* startkliniEl = NULL;
    char* startklini = NULL;
    char* endklini = NULL;
    char* fregex = *regex;
    //if (rec == 2) fregex += 1;
    int klinifound = 0;
    while(*pStr != '\0')
    {
        char* curSymb = fregex;
        char* curEl = pStr;
        if (klinifound)
        {
            curSymb = startklini;
            curEl = startkliniEl;
        }
        while(*curEl  != '\0' && *curSymb != '\0')
        {
            if (isalpha(*curSymb))
            {
                if (!__equal(*curSymb++, *curEl++))
                {
                    break;
                }
            }
            else if (*curSymb == '~')
            {
                if (__equal(*++curSymb, *curEl++))
                {
                    break;
                }
                ++curSymb;
            }
            else if (*curSymb == '\\')
            {
                ++curSymb;
                if (*curSymb == 'D')
                {
                    if (!isalpha(*curEl++))
                    {
                        break;
                    }
                }
                else
                {
                    if (!isdigit(*curEl++))
                    {
                        break;
                    }
                }
                ++curSymb;
            }
            else if (*curSymb == '[')
            {
                unsigned int count = 0;
                ++curSymb;
                while(isdigit(*curSymb))
                {
                    count *= 10;
                    count += (unsigned int)(*curSymb - '0');
                    ++curSymb;
                }
                char* pSymb = NULL;
                int tobreak = 0;
                for (unsigned int i = 0; i < count; ++i)
                {
                    pSymb = curSymb + 2;
                    if (!regex_match(&pSymb, &curEl, 1))
                    {
                        tobreak = 1;
                        break;
                    }
                }
                if (tobreak) break;
                curSymb = pSymb;
            }
            else if (*curSymb == '<')
            {
                if (!klinifound)
                {
                    startklini = curSymb;
                    startkliniEl = curEl;
                    ER_IFN(endklini = __specfind(curSymb + 1) + 2, return 0; )
                    if (*endklini != '\0')
                    {
                        klinifound = 1;
                    }
                    else
                    {
                        ++curSymb;
                        if (regex_match(&curSymb, &curEl, 2))
                        {
                            startkliniEl = curEl;
                            klinifound = 1;
                        }
                    }
                    curSymb = endklini;
                }
                else
                {
                    klinifound = 0;
                    curSymb = startklini + 1;
                    curEl = startkliniEl;
                    if (regex_match(&curSymb, &curEl, 2))
                    {
                        startkliniEl = curEl;
                        klinifound = 1;
                        curSymb = endklini;
                    }
                    else
                    {
                        curSymb = endklini;
                        break;
                    }
                }
            }
            else if (*curSymb == ')')
            {
                if (rec)
                {
                    *regex = curSymb + 2;
                    *str = curEl;
                    return 1;
                }
            }
            else if (*curSymb++ == '>')
            {
                if (rec)
                {
                    *str = curEl;
                    return 1;
                }
            }
        }
        if (*curSymb == '\0') return 1;
        if (!klinifound)
        {
            if (rec) return 0;
            else ++pStr;
        }
    }
    
    return 0;
}

char* __specfind(char* begin)
{
    int count = 1;
    char* res = NULL;
    char* cur = begin;
    while (cur && count)
    {
        if (*cur == '>')
        {
            res = cur;
            --count;
        }
        else if (*cur == '<')    ++count;
        ++cur;
    }
    return res;
}

void spec_add_nts(char** astr, int num)
{
    int radix = 10;
    while(num / radix > 0)    radix *= 10;

    // better than while(n / (radix * 10) > 0)    radix *= 10;
    radix /= 10;
    while(radix > 0)
    {
        **astr = '0' + (num / radix);
        num %= radix;
        radix /= 10;
        ++*astr;
    }
    **astr = ' ';
    ++*astr;
}

inline int __equal(int _first, char _second)
{
    return (char)_first == _second;
}