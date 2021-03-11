#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define _SZSTRING (size_t)(1024)
#define _SZEXPRE (size_t)(64)

#define _RECURSIVE
#if defined(_RECURSIVE)
#define _NRECCOUNT 16
#endif

typedef struct _RegEx
{
    char** ptr;
    char regex[_SZEXPRE];
    int (**pFunc)(int, char);
    //struct _RegEx* next;
} RegEx;

void regex_init(RegEx*, const char*);
void regex_destruct(RegEx*);
inline int __equal(int, char);
inline int __noteq(int, char);
inline int __isalpha(int, char);
inline int __isdigit(int, char);

int main()
{
    char regexstr[_SZEXPRE];
    size_t K;
    scanf("%zu", &K);
    scanf("%s", regexstr);
    char* pStr[K];
    for(size_t i = 0ul; i < K; ++i)
    {
        pStr[i] = (char*) malloc(_SZSTRING * sizeof(char));
        scanf("%s", pStr[i]);
    }
    RegEx inst1;
    regex_init(&inst1, regexstr);
    /*for(size_t i = 0ull; i < _SZEXPRE; ++i)
    {
        char** ptr = inst1.ptr;
        char** next = ptr + 1;
        while(next != (inst1.ptr + _SZEXPRE + 1))
        {
            size_t j = 0ull;
            while((*ptr + j) != *next)
            {
                printf("%c", *(*ptr + j));
                ++j;
            }
            ++ptr;
            ++next;
        }
    }*/
    size_t yu = 0ull;
    while(inst1.ptr[yu] != NULL)
    {
        printf("%c", *inst1.ptr[yu++]);
    }

    regex_destruct(&inst1);

    for(size_t i = 0ul; i < K; ++i)
    {
        free(pStr[i]);
    }

    return 0;
}

void regex_init(RegEx* inst, const char str[_SZEXPRE])
{
    strcpy(inst->regex, str);
    char* regex = inst->regex;

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
    // just hope they are NULL :/
    ptrdiff_t curLoopCount = -1l;
#endif
    while(*regex)
    {
        // cause' just 2 inc of i below
        if (i > curszPtr - 2)
        {
            curszPtr *= 2;
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
                inst->ptr[i] = regex;
                inst->pFunc[i++] = &__noteq;
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
                    ++pPtr;
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
                    ++pPtr[curLoopCount];
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
    inst->ptr[i] = NULL;
}

void regex_destruct(RegEx* inst)
{
    free(inst->pFunc);
    free(inst->ptr);
}

int regex_match(char* regex, char* inst)
{
    char* pRegex = regex;
    char* pEl = inst;
    while(*pEl)
    {
        if (*pEl != *pRegex) ++pEl;
        else
        {

        }
    }

    return 1;
}

inline int __equal(int _first, char _second)
{
    return (char)_first == _second;
}

inline int __noteq(int _first, char _second)
{
    return (char)_first != _second;
}

inline int __isdigit(int _first, char)
{
    return isdigit(_first);
}

inline int __isalpha(int _first, char)
{
    return isalpha(_first);
}