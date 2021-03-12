#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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

void regex_init(RegEx*, const char*);
void regex_destruct(RegEx*);
int regex_match(RegEx*, char*);
void klini_match(char**, char***, int (***)(int, char), _pframe*);
int __equal(int, char);
int __noteq(int, char);
int __isalpha(int, char);
int __isdigit(int, char);

int main()
{
    size_t K;
    char regexstr[_SZEXPRE];
    scanf("%s", regexstr);
    scanf("%zu", &K);
    char* pStr[K];
    for(size_t i = 0ul; i < K; ++i)
    {
        pStr[i] = (char*) malloc(_SZSTRING * sizeof(char));
        scanf("%s", pStr[i]);
    }
    RegEx inst1;
    regex_init(&inst1, regexstr);
/*
    size_t yu = 0ull;
    while(inst1.ptr[yu] != NULL)
    {
        printf("%c", *inst1.ptr[yu]);
        //printf("%p\n", inst1.pFunc[yu]);
        //printf("%d\n", inst1.pFunc[yu](*inst1.ptr[yu], *inst1.ptr[yu]));
        ++yu;
    }
*/
    unsigned char nMatch = 0u;
    for(size_t i = 0ull; i < K; ++i)
    {
        if (regex_match(&inst1, pStr[i]))
        {
            printf("%zu ", i);
            ++nMatch;
        }
    }
    if (!nMatch) printf("none");

    regex_destruct(&inst1);

    for(size_t i = 0ul; i < K; ++i)
    {
        free(pStr[i]);
    }

    return 0;
}

void regex_init(RegEx* inst, const char str[_SZEXPRE])
{
    char* regex = strcpy(inst->regex, str);

    size_t curszPtr = _SZEXPRE;
    inst->ptr = (char**) malloc(curszPtr * sizeof(char*));
    inst->pFunc = (int (**)(int, char)) malloc(curszPtr * sizeof(int (*)(int, char)));
    size_t i = 0ull;
    int state = 0;
#if !defined(_RECURSIVE)
    unsigned int loopcount = 0;
    char** pPtr = NULL;
    int (**ppFunc)(int, char) = NULL;
#else
    unsigned int* vLoopCount = (unsigned int*) calloc(_NRECCOUNT, sizeof(unsigned int));
    char*** pPtr = (char***) malloc(_NRECCOUNT * sizeof(char**));
    int (***ppFunc)(int, char) = (int (***)(int, char)) malloc(curszPtr * sizeof(int (**)(int, char)));
    ptrdiff_t curLoopCount = -1l;
#endif
    while(*regex)
    {
        if (i > curszPtr - 2)
        {
            curszPtr += _SZEXPRE;
            inst->ptr = (char**) realloc(inst->ptr, curszPtr * sizeof(char*));
            inst->pFunc = (int (**)(int, char)) realloc(inst->pFunc, curszPtr * sizeof(int (*)(int, char)));
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
                if (*(regex++) == 'D')  inst->pFunc[i++] = &__isalpha; // check up condition
                else                    inst->pFunc[i++] = &__isdigit;
                //++regex;
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
                if (loopcount /*unnecessary but ->*/ && pPtr != NULL && ppFunc != NULL)
                {
                    if (i > curszPtr - loopcount - 1)
                    {
                        curszPtr += i - loopcount + 2;
                        inst->ptr = (char**) realloc(inst->ptr, curszPtr * sizeof(char*));
                        inst->pFunc = (int (**)(int, char)) realloc(inst->pFunc, curszPtr * sizeof(int (*)(int, char)));
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
                if (vLoopCount[curLoopCount] /*unnecessary but ->*/ && pPtr[curLoopCount] != NULL && ppFunc[curLoopCount] != NULL)
                {
                    if (i > curszPtr - vLoopCount[curLoopCount] - 1)
                    {
                        curszPtr += i - vLoopCount[curLoopCount] + 2;
                        inst->ptr = (char**) realloc(inst->ptr, curszPtr * sizeof(char*));
                        inst->pFunc = (int (**)(int, char)) realloc(inst->pFunc, curszPtr * sizeof(int (*)(int, char)));
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
}

void regex_destruct(RegEx* inst)
{
    free(inst->pFunc);
    free(inst->ptr);
}

// match = 1; 0 otherwise
int regex_match(RegEx* regex, char* str)
{
    _pframe backtracers = (_pframe) malloc(_NBKTRCS * sizeof(_pframe));
    // granted backtracers[0].<all> = NULL
    backtracers[0].El = NULL;
    backtracers[0].Symb = NULL;
    backtracers[0].Ex = NULL;
    _pframe curFrame = backtracers;
    
    char* curEl = str;
    while(*curEl)
    {
        char** curSymb = regex->ptr;
        int (**curEx)(int, char) = regex->pFunc;
        /*if (**curSymb == '<')   klini_match(&curEl, &curSymb, &curEx, &curFrame);
        else if ((*curEx++)(*curEl, **curSymb++))
        {*/
        //char* pcurEl = curEl + 1;
        char* pcurEl = curEl;
        int flag = 1;
        while(*pcurEl && *curSymb)
        {
            if (**curSymb == '<')   klini_match(&pcurEl, &curSymb, &curEx, &curFrame);
            else if (!(*curEx++)(*pcurEl++, **curSymb++))
            {
                if (curFrame->El)
                {
                    pcurEl = curFrame->El;
                    curSymb = curFrame->Symb;
                    curEx = curFrame->Ex;
                    curFrame->El = NULL;
                    curFrame->Symb = NULL;
                    curFrame->Ex = NULL;
                    --curFrame;
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
        }
        if (flag && !*curSymb) return 1;
        //}
        ++curEl;
    }

    free(backtracers);
    return 0;
}

// get ptr to previous (actually current) _pframe, to write to current (next)
void klini_match(char** pStr, char*** pSymb, int (***pEx)(int, char), _pframe* aframe)
{
    char* curEl = *pStr;
    char** curSymb;
    int (**curEx)(int, char);
    char** endSymb = *pSymb;
    int (**endEx)(int, char) = *pEx;
    _pframe pFrame = *aframe;
    while(*curEl)
    {
        curSymb = *pSymb + 1;
        curEx = *pEx + 1;
        while(*curEl && *curSymb && **curSymb != '>')
        {
            if (**curSymb == '<')   klini_match(&curEl, &curSymb, &curEx, aframe);
            else if (!(*curEx++)(*curEl++, **curSymb++))
            {
                break;
            }
        }
        if (*curSymb)
        {
            if (**curSymb == '>')
            {
                ++pFrame;
                ++*aframe; // inc aframe outside
                pFrame->El = *pStr;
                pFrame->Symb = curSymb + 1;
                pFrame->Ex = curEx + 1;

                endSymb = curSymb;
                endEx = curEx;
                *pStr = curEl; // reset pStr outside
            }
        }
    }
    *pSymb = endSymb + 1; // reset pSymb outside
    *pEx = endEx + 1; // reset pEx outside
}

inline int __equal(int _first, char _second)
{
    return (char)_first == _second;
}

inline int __noteq(int _first, char _second)
{
    return (char)_first != _second;
}

inline int __isdigit(int _first, char _second)
{
    return isdigit(_first);
}

inline int __isalpha(int _first, char _second)
{
    return isalpha(_first);
}