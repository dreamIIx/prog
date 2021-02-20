#include <stdio.h>

#define __N 255

typedef int mtx_4t;

#define int4_MAX (mtx_4t)(0x7FFFFFFF)
#define int4_MIN (mtx_4t)(-0x7FFFFFFF)

typedef struct dat
{
    mtx_4t min;
    mtx_4t max;
    mtx_4t average;
} _data;

int main()
{
    size_t x, y;

    scanf("%Iu %Iu", &x, &y);
    //printf("%Iu %Iu\n", x, y);
    mtx_4t mtx[__N][__N];
    _data inf[__N];

    for (size_t i = 0ull; i < x; ++i)
    {
        inf[i].max = int4_MIN;
        inf[i].average = 0;
    }

    for (size_t i = 0ull; i < y; ++i)
    {
        inf[i].min = int4_MAX;
        for (size_t j = 0ull; j < x; ++j)
        {
            scanf("%i", &mtx[i][j]);
            inf[j].average += mtx[i][j];
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
                ++count;
                mtx[i][j] = (int)(inf[j].average / y);
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

    //system("pause");
    return 0;
}