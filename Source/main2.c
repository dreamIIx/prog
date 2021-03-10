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
    //struct _RegEx* next;
} RegEx;

void regex_init(RegEx*, const char*);
void regex_destruct(RegEx*);

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
    size_t i = 0ull;
    int state = 0;
#if !defined(_RECURSIVE)
    int loopcount = 0;
    char** pPtr = NULL;
#else
    int* pLoopCount = (int*) calloc(_NRECCOUNT, sizeof(int));
    char*** pPtr = (char***) malloc(_NRECCOUNT * sizeof(char**));
    ptrdiff_t curLoopCount = -1l;
#endif
    while(*regex)
    {
        // cause' just 2 inc of i below
        if (i > curszPtr - 2)
        {
            curszPtr *= 2;
            inst->ptr = (char**) realloc(inst->ptr, curszPtr * sizeof(char*));
        }

        if (!state)
        {
            if (*regex == '[')
            {
                state = 1;
            }
            else if ((*regex == '\\') || (*regex == '~'))
            {
                inst->ptr[i++] = regex;
                regex += 2;
            }
            else if (*regex != ')')
            {
                inst->ptr[i++] = regex++;
            }
            else
            {
#if !defined(_RECURSIVE)
                if (loopcount && pPtr != NULL)
                {
                    ++pPtr;
                    char** curPtr = &inst->ptr[i - 1];
                    for(int k = 0; k < loopcount - 1; ++k)
                    {
                        size_t idx = 0ull;
                        while((pPtr + idx) - 1 != curPtr)
                        {
                            inst->ptr[i++] = *(pPtr + idx++);
                        }
                    }
                }
                regex += 2;
                loopcount = 0;
#else
                if (pLoopCount[curLoopCount] && pPtr[curLoopCount] != NULL)
                {
                    ++pPtr[curLoopCount];
                    char** curPtr = &inst->ptr[i - 1];
                    for(int k = 0; k < pLoopCount[curLoopCount] - 1; ++k)
                    {
                        size_t idx = 0ull;
                        while((pPtr[curLoopCount] + idx) - 1 != curPtr)
                        {
                            inst->ptr[i++] = *(pPtr[curLoopCount] + idx++);
                        }
                    }
                }
                regex += 2;
                pLoopCount[curLoopCount] = 0;
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
                printf("pLoopCount = %d\n", pLoopCount[curLoopCount]);
#if defined(_RECURSIVE)
                ++curLoopCount;
#endif
                // regex is ptr to '['
                ++regex;
                while(isdigit(*regex))
                {
#if !defined(_RECURSIVE)
                    loopcount *= 10;
                    loopcount += (int)(*regex - '0');
#else
                    pLoopCount[curLoopCount] *= 10;
                    pLoopCount[curLoopCount] += (int)(*regex - '0');
#endif
                    ++regex;
                }
                // regex is ptr to '*'
                regex += 2;
                state = 0;
                // granted reg-ex symbol
#if !defined(_RECURSIVE)
                pPtr = &inst->ptr[i - 1];
#else
                pPtr[curLoopCount] = &inst->ptr[i - 1];
#endif
            }
        }
    }
#if defined(_RECURSIVE)
    free(pLoopCount);
    free(pPtr);
#endif
    inst->ptr[i] = NULL;
}

void regex_destruct(RegEx* inst)
{
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