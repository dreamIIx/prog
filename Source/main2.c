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
    size_t size;

} RegEx;

void regex_init(RegEx*, const char*);
void regex_destruct(RegEx*);
int regex_match(RegEx*, char*);
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
    unsigned char nMatch = 0u;
    for(size_t i = 0ull; i < K; ++i)
    {
        if (regex_match(&inst1, pStr[i]))
        {
            printf("%zu ", i + 1);
        }
        printf("\n");
    }
    if (!nMatch) printf("none");

    size_t yu = 0ull;
    while(inst1.ptr[yu] != NULL)
    {
        printf("%c\n", *inst1.ptr[yu]);
        printf("%p\n", inst1.pFunc[yu]);
        printf("%d", inst1.pFunc[yu](*inst1.ptr[yu], *inst1.ptr[yu]));
        ++yu;
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
    char* curEl = str;
    while(*curEl)
    {
        char** curSymb = regex->ptr;
        int (**curEx)(int, char) = regex->pFunc[0];
        if ((*curEx)(*curEl, **curSymb))
        {
            char* pcurEl = curEl + 1;
            ++curSymb;
            while(*pcurEl && *curSymb != NULL)
            {
                //printf("*curSymb = %c\n", *curSymb);
                //printf("curEx = %p\n", curEx);
                if (!(*curEx)(*pcurEl, **curSymb))
                {
                    printf("break;\n");
                    break;
                }
                ++curEx;
                ++pcurEl;
                ++curSymb;
            }
            if (*curSymb == NULL) return 1;
        }
        ++curEl;
    }

    return 0;
}

inline int __equal(int _first, char _second)
{
    printf("%c == %c\n", _first, _second);
    return (char)_first == _second;
}

inline int __noteq(int _first, char _second)
{
    printf("%c != %c\n", _first, _second);
    return (char)_first != _second;
}

inline int __isdigit(int _first, char _second)
{
    printf("%c isdigit %c\n", _first, _second);
    return isdigit(_first);
}

inline int __isalpha(int _first, char _second)
{
    printf("%c is alpha %c\n", _first, _second);
    return isalpha(_first);
}