#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#define __N 255

typedef int_least32_t mtx_t;

typedef struct dat
{
    mtx_t min;
    mtx_t max;
    mtx_t average;
} _data;

typedef struct pair_short
{
    short _first;
    short _second;
} pairShort;

int main()
{
    size_t x, y;

    scanf("%zu %zu", &x, &y);
    mtx_t mtx[__N][__N];
    _data inf[__N];
    pairShort points[__N * __N];
    size_t szPoints = 0ull;

    for(size_t i = 0ull; i < x; ++i)
    {
        inf[i].average = 0;
    }

    for (size_t i = 0ull; i < y; ++i)
    {
        for (size_t j = 0ull; j < x; ++j)
        {
            scanf("%i", &mtx[i][j]);
            inf[j].average += mtx[i][j];
        }
    }

    for (size_t i = 0ull; i < y; ++i)
    {
        inf[i].min = mtx[i][0];
    }
    for(size_t i = 0ull; i < x; ++i)
    {
        inf[i].max = mtx[0][i];
    }

    for (size_t i = 0ull; i < y; ++i)
    {
        for (size_t j = 0ull; j < x; ++j)
        {
            if (mtx[i][j] < inf[i].min) inf[i].min = mtx[i][j];
            if (mtx[i][j] > inf[j].max) inf[j].max = mtx[i][j];
        }
    }

    unsigned int count = 0u;
    for (size_t i = 0ull; i < y; ++i)
    {
        for (size_t j = 0ull; j < x; ++j)
        {
            if ((mtx[i][j] == inf[i].min) && (mtx[i][j] == inf[j].max))
            {
                points[szPoints]._first = i;
                points[szPoints]._second = j;
                ++szPoints;
                ++count;
                mtx[i][j] = (mtx_t)(inf[j].average / y);
            }
        }
    }

    printf("%i\n", count);
    for (size_t i = 0ull; i < y; ++i)
    {
        for (size_t j = 0ull; j < x; ++j)
        {
            printf("%i ", mtx[i][j]);
        }
        printf("\n");
    }

    return 0;
}