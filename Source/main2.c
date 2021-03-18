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

#define _RECURSIVE
#if defined(_RECURSIVE)
#define _NRECCOUNT 16
#endif

typedef struct _RegEx
{
    char** ptr;
    char regex[_SZEXPRE];
    int (**pFunc)(int, char);
    size_t size;

} RegEx;

typedef struct __frame
{
    char* El;
    char** Symb;
    int (**Ex)(int, char);

} _frame, *_pframe;

int regex_init(RegEx*, const char*);
void regex_destruct(RegEx*);
int regex_match(char**, char***, int (***)(int, char), _pframe*, int);
char** __specfind(char**);
int spec_resize(size_t, char***, int (***)(int, char));
void spec_add_nts(char**, int);
int __equal(int, char);
int __noteq(int, char);
int __isalpha(int, char);
int __isdigit(int, char);

int main()
{
    size_t K;
    char regexstr[_SZEXPRE];
    scanf("%s", regexstr);
    RegEx inst1;
    ER_IFN(regex_init(&inst1, regexstr), regex_destruct(&inst1); return -1; )

    scanf("%zu", &K);
    char str[_SZSTRING];
    // approximate ~(K * 2)
    char resStr[K * 2];
    char* pResStr = resStr;
    unsigned char nMatch = 0u;
    int res = 0;
    for(size_t i = 0ul; i < K; ++i)
    {
        scanf("%s", str);
        char* ppStr = str;
        char** ppSymb = inst1.ptr;
        int (**ppEx)(int, char) = inst1.pFunc;
        if ((res = regex_match(&ppStr, &ppSymb, &ppEx, NULL, 0)) == 1)
        {
            spec_add_nts(&pResStr, i);
            ++nMatch;
        }
        else if (res == -1)
        {
            regex_destruct(&inst1);
            return -1;
        }
    }
    *pResStr = '\0';
    if (!nMatch) printf("none");
    else printf("%s", resStr);

    regex_destruct(&inst1);
    return 0;
}

int regex_init(RegEx* inst, const char str[_SZEXPRE])
{
    char* regex = strcpy(inst->regex, str);

    size_t curszPtr = _SZEXPRE;
    ER_IFN(inst->ptr = (char**) malloc(curszPtr * sizeof(char*)), return 0; )
    ER_IFN(inst->pFunc = (int (**)(int, char)) malloc(curszPtr * sizeof(int (*)(int, char))), return 0; )
    size_t i = 0ull;
    int state = 0;
#if !defined(_RECURSIVE)
    unsigned int loopcount = 0;
    char** pPtr = NULL;
    int (**ppFunc)(int, char) = NULL;
#else
    unsigned int* vLoopCount = NULL;
    char*** pPtr = NULL;
    int (***ppFunc)(int, char) = NULL;
    ER_IFN(vLoopCount = (unsigned int*) calloc(_NRECCOUNT, sizeof(unsigned int)), return 0; )
    ER_IFN(pPtr = (char***) malloc(_NRECCOUNT * sizeof(char**)), return 0; )
    ER_IFN(ppFunc = (int (***)(int, char)) malloc(curszPtr * sizeof(int (**)(int, char))), return 0; )
    ptrdiff_t curLoopCount = -1l;
#endif
    while(*regex)
    {
        if (i > curszPtr - 2)
        {
            curszPtr += _SZEXPRE;
            ER_IFN(spec_resize(curszPtr, &inst->ptr, &inst->pFunc), return 0; )
        }

        if (!state)
        {
            if (*regex == '[')
            {
                state = 1;
            }
            else if (*regex == '\\')
            {
                inst->ptr[i] = regex++;
                inst->pFunc[i++] = (*(regex++) == 'D') ? &__isalpha : &__isdigit;
            }
            else if (*regex == '~')
            {
                inst->ptr[i] = regex + 1;
                inst->pFunc[i++] = &__noteq;
                regex += 2;
            }
            else if (*regex == '<')
            {
                inst->ptr[i] = regex++;
                // duplicate for architecture
                inst->pFunc[i] = inst->pFunc[i - 1];
                ++i;
            }
            else if (*regex == '>')
            {
                inst->ptr[i] = regex;
                // duplicate for architecture
                inst->pFunc[i] = inst->pFunc[i - 1];
                ++i;
                regex += 2;
            }
            else if (*regex != ')')
            {
                inst->ptr[i] = regex++;
                inst->pFunc[i++] = &__equal;
            }
            else
            {
#if !defined(_RECURSIVE)
                if (loopcount /*unnecessary but ->*/ && pPtr && ppFunc)
                {
                    if (i > curszPtr - loopcount - 1)
                    {
                        curszPtr += i - loopcount + 2;
                        ER_IFN(spec_resize(curszPtr, &inst->ptr, &inst->pFunc), return 0; )
                    }

                    ++pPtr;
                    ++ppFunc;
                    char** curPtr = &inst->ptr[i - 1];
                    for(unsigned int k = 0; k < loopcount - 1; ++k)
                    {
                        size_t idx = 0ull;
                        while((pPtr + idx) - 1 != curPtr)
                        {
                            inst->ptr[i] = *(pPtr + idx);
                            inst->pFunc[i++] = *(ppFunc + idx++);
                        }
                    }
                }
                regex += 2;
                loopcount = 0;
                pPtr = NULL;
                ppFunc = NULL;
#else
                if (vLoopCount[curLoopCount] /*unnecessary but ->*/ && pPtr[curLoopCount] && ppFunc[curLoopCount])
                {
                    if (i > curszPtr - vLoopCount[curLoopCount] - 1)
                    {
                        curszPtr += i - vLoopCount[curLoopCount] + 2;
                        ER_IFN(spec_resize(curszPtr, &inst->ptr, &inst->pFunc), return 0; )
                    }

                    ++pPtr[curLoopCount];
                    ++ppFunc[curLoopCount];
                    char** curPtr = &inst->ptr[i - 1];
                    for(unsigned int k = 0; k < vLoopCount[curLoopCount] - 1; ++k)
                    {
                        size_t idx = 0ull;
                        while((pPtr[curLoopCount] + idx) - 1 != curPtr)
                        {
                            inst->ptr[i] = *(pPtr[curLoopCount] + idx);
                            inst->pFunc[i++] = *(ppFunc[curLoopCount] + idx++);
                        }
                    }
                }
                regex += 2;
                vLoopCount[curLoopCount] = 0;
                pPtr[curLoopCount] = NULL;
                ppFunc[curLoopCount] = NULL;
                --curLoopCount;
#endif
            }
        }
        else
        {
            // template:
            // [loopcount*(...)]
            if (state == 1)
            {
#if defined(_RECURSIVE)
                ++curLoopCount;
#endif
                // regex is ptr to '['
                ++regex;
                while(isdigit(*regex))
                {
#if !defined(_RECURSIVE)
                    loopcount *= 10;
                    loopcount += (unsigned int)(*regex - '0');
#else
                    vLoopCount[curLoopCount] *= 10;
                    vLoopCount[curLoopCount] += (unsigned int)(*regex - '0');
#endif
                    ++regex;
                }
                // regex is ptr to '*'
                regex += 2;
                state = 0;
                // granted reg-ex symbol
#if !defined(_RECURSIVE)
                pPtr = &inst->ptr[i - 1];
                ppFunc = &inst->pFunc[i - 1];
#else
                pPtr[curLoopCount] = &inst->ptr[i - 1];
                ppFunc[curLoopCount] = &inst->pFunc[i - 1];
#endif
            }
        }
    }
#if defined(_RECURSIVE)
    free(vLoopCount);
    free(pPtr);
    free(ppFunc);
#endif
    // granted <elem>[i] exists
    inst->ptr[i] = NULL;
    inst->pFunc[i] = NULL;
    inst->size = curszPtr;

    return 1;
}

void regex_destruct(RegEx* inst)
{
    free(inst->pFunc);
    free(inst->ptr);
}

// get ptr to previous (actually current) _pframe, to write to current (next)
int regex_match(char** pStr, char*** pSymb, int (***pEx)(int, char), _pframe* aframe, int req)
{
    // cause' chosen architecture, kbacktracers size will staticly lesser than _NBKTRCS (i.e without relloc)
    _pframe backtracers = NULL;
    ER_IFN(backtracers = (_pframe) malloc(_NBKTRCS * sizeof(_pframe)), return -1; )
    // granted kbacktracers[0].<all> = NULL
    backtracers[0].El = NULL;
    backtracers[0].Symb = NULL;
    backtracers[0].Ex = NULL;
    _pframe curFrame = backtracers;

    char* curEl = *pStr;
    char** curSymb = *pSymb;
    int (**curEx)(int, char) = *pEx;
    char** endSymb = NULL;
	int (**endEx)(int, char) = NULL;
	if (req)
	{
		endSymb = __specfind(*pSymb);
		endEx = *pEx + (endSymb - *pSymb);
	}
    while(*curEl)
    {
        while(*curEl && *curSymb)
        {
            if (**curSymb == '<')
            {
                ++curSymb;
                ++curEx;
                regex_match(&curEl, &curSymb, &curEx, &curFrame, 1);
            }
            else if (**curSymb == '>')
            {
				// this lines can do only regex_match<req == 1>
				++*aframe; // inc aframe outside
				(*aframe)->El = *pStr;
				(*aframe)->Symb = endSymb + 1;
				(*aframe)->Ex = endEx + 1;
					
                curSymb = *pSymb;
                curEx = *pEx;
                *pStr = curEl; // reset pStr outside
            }
            else if (!(*curEx++)(*curEl++, **curSymb++))
            {
                if (curFrame->El)
                {
                    curEl = curFrame->El;
                    curSymb = curFrame->Symb;
                    curEx = curFrame->Ex;
                    curFrame->El = NULL;
                    curFrame->Symb = NULL;
                    curFrame->Ex = NULL;
                    --curFrame;
                }
                else
                {
                    if (req)
                    {
                        *pSymb = endSymb + 1; // reset pSymb outside
                        *pEx = endEx + 1; // reset pEx outside
                        return 0;
                    }
                    else break;
                }
            }
            if (!*curSymb) return 1;
        }
        ++*pStr;
        curSymb = *pSymb;
        curEx = *pEx;
        curEl = *pStr;
    }

    free(backtracers);
    return 0;
}

char** __specfind(char** begin)
{
    int count = 1;
    char** res = NULL;
    char** cur = begin;
    while (*cur && count)
    {
        if (**cur == '>')
        {
            res = cur;
            --count;
        }
        else if (**cur == '<')    ++count;
        ++cur;
    }
    return res;
}

// very specialized method :(
int spec_resize(size_t new_size, char*** a1, int (***a2)(int, char))
{
    ER_IFN(*a1 = (char**) realloc(*a1, new_size * sizeof(char*)), return 0; )
    ER_IFN(*a2 = (int (**)(int, char)) realloc(*a2, new_size * sizeof(int (*)(int, char))), return 0; )
    return 1;
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

inline int __noteq(int _first, char _second)
{
    return !__equal(_first, _second);
}

inline int __isdigit(int _first, char _second)
{
    return isdigit(_first);
}

inline int __isalpha(int _first, char _second)
{
    return isalpha(_first);
}