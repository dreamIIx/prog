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
#define _SZ4KLINI ((_SZEXPRE / 3) + 1) // motivated by: <>*<>*<>*...

int regex_match(char**, char**, unsigned char);
char* __specfind(char*);
void spec_add_nts(char**, int);
int __equal(int, char);
void __free(void*, void*, void*);

int main()
{
    size_t K;   
    char* regexstr = NULL;
    ER_IFN(regexstr = (char*) malloc(_SZEXPRE * sizeof(char)), return -1; )
    scanf("%s", regexstr);
    scanf("%zu", &K);
    char* str;
    ER_IFN(str = (char*) malloc(_SZSTRING * sizeof(char)), free(regexstr); return -1; )
    // approximate ~(K * 2)
    char resStr[K * 5];
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
            printf("%zu ", i);
        }
    }
    *pResStr = '\0';
    if (!nMatch) printf("none");

    free(str);
    free(regexstr);
    
    return 0;
}

int regex_match(char** regex, char** str, unsigned char rec)
{
    char* pStr = *str;
    char** startkliniEl = NULL;
    char** startklini = NULL;
    char** endklini = NULL;
    ER_IFN(startkliniEl = (char**) malloc(_SZ4KLINI * sizeof(char*)), return -1; )
    ER_IFN(startklini = (char**) malloc(_SZ4KLINI * sizeof(char*)), free(startkliniEl); return -1; )
    ER_IFN(endklini = (char**) malloc(_SZ4KLINI * sizeof(char*)), __free(startkliniEl, startklini, NULL); return -1; )
    char* fregex = *regex;

    ptrdiff_t kidx = 0;
    ptrdiff_t curKidx = -1;
    while(*pStr != '\0')
    {
        char* curSymb = fregex;
        char* curEl = pStr;
        if (kidx > 0)
        {
            curSymb = startklini[curKidx];
            curEl = startkliniEl[curKidx];
            curKidx = --kidx;
        }
        while(*curEl != '\0' && *curSymb != '\0')
        {
            if (*curSymb == '~')
            {
                ++curSymb;
                if (__equal(*curSymb, *curEl))
                {
                    break;
                }
                ++curSymb;
                ++curEl;
            }
            else if (*curSymb == '\\')
            {
                ++curSymb;
                if (*curSymb == 'D')
                {
                    if (!isalpha(*curEl))
                    {
                        break;
                    }
                }
                else
                {
                    if (!isdigit(*curEl))
                    {
                        break;
                    }
                }
                ++curSymb;
                ++curEl;
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
                if (kidx != curKidx)
                {
                    startklini[kidx] = curSymb;
                    startkliniEl[kidx] = curEl;
                    ER_IFN(endklini[kidx] = __specfind(curSymb + 1), __free(startkliniEl, startklini, endklini); return 0; )
                    endklini[kidx] += 2;
                    curKidx = kidx;
                    if (*endklini[kidx] != '\0' && *endklini[kidx] != '>')
                    {
                        ++kidx;
                    }
                    else
                    {
                        *str = curEl;
                        __free(startkliniEl, startklini, endklini);
                        return 1;
                    }
                    curSymb = endklini[curKidx];
                }
                else
                {
                    curSymb = startklini[curKidx] + 1;
                    curEl = startkliniEl[curKidx];
                    int tobreak = 0;
                    while(!regex_match(&curSymb, &curEl, 2))
                    {
                        if (--kidx < 0)
                        {
                            tobreak = 1;
                            --curKidx;
                            break;
                        }
                        else
                        {
                            --curKidx;
                            curSymb = startklini[curKidx] + 1;
                            curEl = startkliniEl[curKidx];
                        }
                    }
                    ++kidx;
                    if (tobreak) break;
                    startkliniEl[curKidx] = curEl;
                    curSymb = endklini[curKidx];
                }
            }
            // rec > 0 (can only do this, due to correctly granted input)
            else if (*curSymb == ')')
            {
                *regex = curSymb + 2;
                *str = curEl;
                __free(startkliniEl, startklini, endklini);
                return 1;
            }
            else if (*curSymb == '>')
            {
                *str = curEl;
                __free(startkliniEl, startklini, endklini);
                return 1;
            }
            // end rec > 0
            else
            {
                if (!__equal(*curSymb, *curEl))
                {
                    break;
                }
                ++curSymb;
                ++curEl;
            }
        }
        if (*curSymb == '\0')
        {
            __free(startkliniEl, startklini, endklini);
            return 1;
        }
        else if (*curSymb == ')')
        {
                *regex = curSymb + 2;
                *str = curEl;
                __free(startkliniEl, startklini, endklini);
                return 1;
        }   
        else if (*curSymb == '>')
        {
                *str = curEl;
                __free(startkliniEl, startklini, endklini);
                return 1;
        }
        if (kidx <= 0)
        {
            if (rec)
            {
                __free(startkliniEl, startklini, endklini);
                return 0;
            }
            else ++pStr;
        }
    }

    __free(startkliniEl, startklini, endklini);
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

void __free(void* first, void* second, void* third)
{
    free(first);
    free(second);
    free(third);
}