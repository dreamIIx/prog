#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <malloc.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>

#if defined(__unix__)
#if defined(__linux__)
#include <sys/random.h>
#endif
#elif defined(_WIN32)
#include <windows.h>
#else
    #error This operating system is not supported
#endif

#undef _TURN_OFF_WIN32
#if !defined(_TURN_OFF_WIN32)
#if defined(_WIN32)
#include <profileapi.h>
#endif
#endif

#ifndef ER_IF
#define ER_IF(x, Exc) if ( (x) ) { Exc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, Exc) if ( !(x) ) { Exc }
#endif

#ifndef UNICODE
 #undef _UNICODE
#else
 #ifndef _UNICODE
  #define _UNICODE
 #endif
#endif

#define ISBIT(x,pos)        ( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)       ( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)       ( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)     ( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

#define nMATRIX                     (size_t)(100)
#define szMATRIX                    (size_t)(64)
#define _RAND_MAX_COUNTMTX          (int)(100)
#define _RAND_MAX_DIMMTX            (int)(8)
#define _RAND_nSAMPLE               (int)(101)
#define _RAND_DELT_SAMPLE           (int)(50)
#define _default_input_str          "./input.txt"
#define _default_output_res_str     "./output.txt"
#define _default_output_inf_str    "./inf.txt"

// expected integer type
typedef int mtx_t;

typedef struct
{
    short det;
    short dim;
    short idx;
} datMtx;

typedef struct
{
    long long M;
    long double Dis;
} datSample;

int specCountFunc(mtx_t* mtx, datMtx* dat, void (*sortFunc)(datMtx*, short, int (*)(datMtx, datMtx)), int (*cmp)(datMtx, datMtx),
    datSample* datSample, ptrdiff_t count, char* input_str);
int createInput(mtx_t* mtx, datMtx* dat);
int fillInput(mtx_t* mtx, datMtx* dat, char* input_str);
int saveRes(mtx_t* mtx, datMtx* dat, short count, const char* output_str);
int saveInf(datSample* datSample, short count, char* output_str);
double det(mtx_t* mtx, unsigned long long* exc, short count, int k);
void bubbleSort(datMtx* dat, short n, int (*cmp)(datMtx, datMtx));
void heapify(datMtx* dat, short n, short i, int (*cmp)(datMtx, datMtx));
void heapSort(datMtx* dat, short n, int (*cmp)(datMtx, datMtx));
void swap(datMtx*, datMtx*);
mtx_t _rand();
int _mtx_el_lesser(datMtx, datMtx);
int _mtx_el_greater(datMtx, datMtx);

#if defined(_WIN32)
HCRYPTPROV hProv;
#endif

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "en_US.UTF-8");

#if defined(_WIN32)
	if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
    {
		if (GetLastError() == NTE_BAD_KEYSET)
        {
			ER_IF(!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET), return 0; )
        }
    }
#endif

    // init arrays
    mtx_t* mtx;
    datMtx* dat;
    ER_IFN(mtx = (mtx_t*) malloc(nMATRIX * szMATRIX * szMATRIX * sizeof(mtx_t)), return -1; )
    ER_IFN(dat = (datMtx*) malloc(nMATRIX * sizeof(datMtx)), free(mtx); return -1; )
// for unnecessary job
#define __free { free(mtx); free(dat); }

    // default way to run programm
    ER_IF(argc < 2,
        short count;
        ER_IFN(count = fillInput(mtx, dat, _default_input_str), __free return -1; )
        heapSort(dat, count, &_mtx_el_greater);
        ER_IFN(saveRes(mtx, dat, count, _default_output_res_str), __free return -1; )
        free(mtx);
        free(dat);

        return 0;
    )

    // otherwise it is way to compare efficiency of sorting samples
    ptrdiff_t count = 0l;
    char* idx = argv[1];
    while(isdigit(*idx))
    {
        count *= 10;
        count += *idx - '0';
        ++idx;
    }
    ER_IF(count < 1, __free return -1; )

    datSample* infAlgo;
    ER_IFN(infAlgo = (datSample*) malloc((argc - 2) * sizeof(datSample)), __free return -1; )
    long long min;
    long long max;
    short iMin = 0;
    short iMax = 0;

    printf("Algorithm running time on each dataset(in nanoseconds):\n");
    ER_IF(specCountFunc(mtx, dat, &heapSort, &_mtx_el_greater, infAlgo, count, argv[2]), __free return -1; )
    min = infAlgo[0].M;
    max = infAlgo[0].M;
#if defined(__unix__)
#if defined(__linux__)
    printf("HeapSort#1: %lld %lc %Lf\n", infAlgo[0].M, L'±', infAlgo[0].Dis);
#endif
#elif defined(_WIN32)
    printf("HeapSort#1: %I64d %C %l\n", infAlgo[0].M, L'±', infAlgo[0].Dis);
#else
    #error This operating system is not supported
#endif

    for(int i = 3; i < argc; ++i)
    {
        ER_IF(specCountFunc(mtx, dat, &heapSort, &_mtx_el_greater, &infAlgo[i - 2], count, argv[i]), __free return -1; )
        if (min > infAlgo[i - 2].M)
        {
            iMin = i;
            min = infAlgo[i - 2].M;
        }
        if (max < infAlgo[i - 2].M)
        {
            iMax = i;
            max = infAlgo[i - 2].M;
        }
#if defined(__unix__)
#if defined(__linux__)
        printf("HeapSort#%d: %lld %lc %Lf\n", i - 1, infAlgo[i - 2].M, L'±', infAlgo[i - 2].Dis);
#endif
#elif defined(_WIN32)
        printf("HeapSort#%d: %I64d %C %l\n", i - 1, infAlgo[i - 2].M, L'±', infAlgo[i - 2].Dis);
#else
    #error This operating system is not supported
#endif
    }
#if defined(__unix__)
#if defined(__linux__)
    printf("Sorting worked slower on %hd dataset[Average time = %lld]\n", iMax, max);
    printf("Sorting worked all the fastest on %hd dataset[Average time = %lld]", iMin, min);
#endif
#elif defined(_WIN32)
    printf("Sorting worked slower on %hd dataset[Average time = %I64d]\n", iMax, max);
    printf("Sorting worked all the fastest on %hd dataset[Average time = %I64d]", iMin, min);
#else
    #error This operating system is not supported
#endif

    printf("Algorithm running time on each dataset(in nanoseconds):\n");
    ER_IF(specCountFunc(mtx, dat, &bubbleSort, &_mtx_el_greater, infAlgo, count, argv[2]), __free return -1; )
    min = infAlgo[0].M;
    max = infAlgo[0].M;
#if defined(__unix__)
#if defined(__linux__)
    printf("BubbleSort#1: %lld %lc %Lf\n", infAlgo[0].M, L'±', infAlgo[0].Dis);
#endif
#elif defined(_WIN32)
    printf("BubbleSort#1: %I64d %C %l\n", infAlgo[0].M, L'±', infAlgo[0].Dis);
#else
    #error This operating system is not supported
#endif

    for(int i = 3; i < argc; ++i)
    {
        ER_IF(specCountFunc(mtx, dat, &bubbleSort, &_mtx_el_greater, &infAlgo[i - 2], count, argv[i]), __free return -1; )
        if (min > infAlgo[i - 2].M)
        {
            iMin = i;
            min = infAlgo[i - 2].M;
        }
        if (max < infAlgo[i - 2].M)
        {
            iMax = i;
            max = infAlgo[i - 2].M;
        }
#if defined(__unix__)
#if defined(__linux__)
        printf("BubbleSort#%d: %lld %lc %Lf\n", i - 1, infAlgo[i - 2].M, L'±', infAlgo[i - 2].Dis);
#endif
#elif defined(_WIN32)
        printf("BubbleSort#%d: %I64d %C %l\n", i - 1, infAlgo[i - 2].M, L'±', infAlgo[i - 2].Dis);
#else
    #error This operating system is not supported
#endif
    }
#if defined(__unix__)
#if defined(__linux__)
    printf("Sorting worked slower on %hd dataset[Average time = %lld]\n", iMax, max);
    printf("Sorting worked all the fastest on %hd dataset[Average time = %lld]", iMin, min);
#endif
#elif defined(_WIN32)
    printf("Sorting worked slower on %hd dataset[Average time = %I64d]\n", iMax, max);
    printf("Sorting worked all the fastest on %hd dataset[Average time = %I64d]", iMin, min);
#else
    #error This operating system is not supported
#endif
    saveInf(infAlgo, argc - 2, _default_output_inf_str);

    free(mtx);
    free(dat);
    free(infAlgo);

    return 0;
}

int specCountFunc(mtx_t* mtx, datMtx* dat, void (*sortFunc)(datMtx*, short, int (*)(datMtx, datMtx)), int (*cmp)(datMtx, datMtx),
    datSample* datSample, ptrdiff_t count, char* input_str)
{
#if defined(_TURN_OFF_WIN32)
    return 0;
}
#else
#if defined(__unix__)
    #if defined(__linux__)
    struct timespec start;
    struct timespec end;
    long long diffsec;
    long long diffnsec;
    #endif
#elif defined(_WIN32)
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    LARGE_INTEGER diffmcsec;
    LARGE_INTEGER freq;
#else
    #error This operating system is not supported
#endif

    datSample->M = 0;
    datSample->Dis = 0.;
    long double* sample;
    ER_IFN(sample = (long double*) malloc(count * sizeof(long double)), free(sample); return -1; )

    for(ptrdiff_t i = 0l; i < count; ++i)
    {
        short nMtx;
        if (strcmp(input_str, "rand") != 0)
        {
            ER_IFN(nMtx = fillInput(mtx, dat, input_str), free(sample); return -1; )
        }
        else
        {
            ER_IFN(nMtx = createInput(mtx, dat), free(sample); return -1; )
        }
        
#if defined(__unix__)
    #if defined(__linux__)
        //ER_IFN(timespec_get(&start, TIME_UTC), free(sample); return -1; )
        ER_IF(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start), free(sample); return -1; )
    #endif
#elif defined(_WIN32)
        ER_IFN(QueryPerformanceFrequency(&freq), free(sample); return -1; )
        ER_IFN(QueryPerformanceCounter(&start), free(sample); return -1; )
#else
    #error This operating system is not supported
#endif
        sortFunc(dat, nMtx, cmp);
                
#if defined(__unix__)
    #if defined(__linux__)
        //ER_IFN(timespec_get(&end, TIME_UTC), free(sample); return -1; )
        ER_IF(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end), free(sample); return -1; )
        ER_IFN(saveRes(mtx, dat, nMtx, _default_output_res_str), free(sample); return -1; )
        diffsec = (long long)end.tv_sec - start.tv_sec;
        diffnsec = (long long)end.tv_nsec - start.tv_nsec;
        diffsec *= 1000000000;
        //diffnsec /= 1000;
        diffsec += diffnsec;
        sample[i] = (long double)diffsec;
        datSample->M += diffsec;
    #endif
#elif defined(_WIN32)
    ER_IFN(QueryPerformanceCounter(&end), free(sample); return -1; )
    diffmcsec.HighPart = end.HighPart - start.HighPart;
    diffmcsec.HighPart *= 1000000;
    diffmcsec.HighPart /= freq.HighPart;
    sample[i] = (long double)diffmcsec.HighPart;
    datSample->M += diffmcsec.HighPart;
#else
    #error This operating system is not supported
#endif
    }

    datSample->M /= count;
    for(ptrdiff_t i = 0l; i < count; ++i)
    {
        sample[i] = powl(sample[i] - (long double) datSample->M, 2);
        datSample->Dis += sample[i];
    }
    datSample->Dis /= count - 1;
    datSample->Dis = sqrt(datSample->Dis);
    
    free(sample);
    
    return 0;
}
#endif

int createInput(mtx_t* mtx, datMtx* dat)
{
    short count = _rand() % _RAND_MAX_COUNTMTX + 1;
    //printf("Count rand matrix: %hd[", count);
    for(short k = 0; k < count; ++k)
    {
        short curDim = _rand() % _RAND_MAX_DIMMTX + 1;
        //printf("%hd, ", curDim);
        dat[k].dim = curDim;
        for(short i = 0; i < curDim; ++i)
        {
            for(short j = 0; j < curDim; ++j)
            {
                mtx[k * szMATRIX * szMATRIX + i * szMATRIX + j] = (mtx_t) (_rand() % _RAND_nSAMPLE - _RAND_DELT_SAMPLE);
            }
        }
        dat[k].idx = k;
        unsigned long long exc = 0ull;
        dat[k].det = det(mtx + k * szMATRIX * szMATRIX, &exc, curDim, 0);
    }
    //printf("]\n");

    return count;
}

int fillInput(mtx_t* mtx, datMtx* dat, char* input_str)
{
    FILE* file;
    ER_IFN(file = fopen(input_str, "r"), return 0; )
    short count;
    ER_IF(fscanf(file, "%hd", &count) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
    for(short k = 0; k < count; ++k)
    {
        short curDim;
        ER_IF(fscanf(file, "%hd", &curDim) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
        dat[k].dim = curDim;
        for(short i = 0; i < curDim; ++i)
        {
            for(short j = 0; j < curDim; ++j)
            {
                ER_IF(fscanf(file, "%i", &mtx[k * szMATRIX * szMATRIX + i * szMATRIX + j]) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
            }
        }
        dat[k].idx = k;
        unsigned long long exc = 0ull;
        dat[k].det = det(mtx + k * szMATRIX * szMATRIX, &exc, curDim, 0);
    }
    ER_IF(fclose(file) == EOF, return 0; )

    return count;
}

int saveRes(mtx_t* mtx, datMtx* dat, short count, const char* output_str)
{
    FILE* file;
    ER_IFN(file = fopen((output_str ? output_str : _default_output_res_str), "w"), return 0; )
    for(short k = 0; k < count; ++k)
    {
        short dim = dat[k].dim;
        for(short i = 0; i < dim; ++i)
        {
            for(short j = 0; j < dim; ++j)
            {
                ER_IF(fprintf(file, "%i ", mtx[dat[k].idx * szMATRIX * szMATRIX + i * szMATRIX + j]) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
            }
            ER_IF(fprintf(file, "\n") == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
        }
    }
    ER_IF(fclose(file) == EOF, return 0; )

    return 1;
}

int saveInf(datSample* datSample, short count, char* output_str)
{
    FILE* file;
    ER_IFN(file = fopen((output_str ? output_str : _default_output_inf_str), "w"), return 0; )
    for(short k = 0; k < count; ++k)
    {
#if defined(__unix__)
#if defined(__linux__)
        ER_IF(fprintf(file, "%hd: M = %lld, Dis = %Lf\n", k, datSample[k].M, datSample[k].Dis) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
#endif
#elif defined(_WIN32)
        ER_IF(fprintf(file, "%hd: M = %I64d, Dis = %l\n", k, datSample[k].M, datSample[k].Dis) == EOF, ER_IF(fclose(file) == EOF, return 0; ) return 0; )
#else
    #error This operating system is not supported
#endif
    }
    ER_IF(fclose(file) == EOF, return 0; )

    return 1;
}

// since szMATRIX == 64
double det(mtx_t* mtx, unsigned long long* exc, short count, int k)
{
    double res = 0.;
    short shift = -1;
    for(short i = 0; i < count; ++i)
    {
        if (!ISBIT(*exc, i))
        {
            if (shift == -1)    shift = i;
            if (k < count - 1)
            {
                SETBIT(*exc, i);
                res += (((i - shift) & 1) ? (-1) : (1)) * mtx[k * szMATRIX + i] * det(mtx, exc, count, k + 1);
                UNSETBIT(*exc, i);
            }
            else
            {
                return mtx[k * szMATRIX + i];
            }
        }
        else
        {
            if (shift == -1)    ++shift;
            ++shift;
        }
    }

    return res;
}

void heapify(datMtx* dat, short n, short i, int (*cmp)(datMtx, datMtx))
{
    
    short largest;
    short left;
    short right;
    
    largest = i;

    do
    {
        // first time useless
        swap(&dat[i], &dat[largest]);

        //largest = i;
        i = largest;
        left = 2 * i + 1;
        right = 2 * i + 2;

        if (left < n)
        {
            if (cmp(dat[left], dat[largest])) largest = left;
        }
        if (right < n)
        {
            if (cmp(dat[right], dat[largest])) largest = right;
        }
    } while(largest != i);
}

void bubbleSort(datMtx* dat, short n, int (*cmp)(datMtx, datMtx))
{
    for(size_t i = 0ul; i < n; ++i)
    {
        for(size_t j = 0ul; j < n - i - 1; ++j)
        {
            if (cmp(dat[j], dat[j + 1]))
            {
                swap(&dat[i], &dat[j]);
            }
        }
    }
}

void heapSort(datMtx* dat, short n, int (*cmp)(datMtx, datMtx))
{
    for (short i = n / 2 - 1; i >= 0; --i)
    {
        heapify(dat, n, i, cmp);
    }
 
    for (short i = n - 1; i > 0; --i)
    {
        swap(&dat[0], &dat[i]);
        heapify(dat, i, 0, cmp);
    }
}

void swap(datMtx* a, datMtx* b)
{
    datMtx c = *a;
    *a = *b;
    *b = c;
}

mtx_t _rand()
{      
#if defined(__unix__)
#if defined(__linux__)
    mtx_t res = 0x0;
    getrandom(&res, sizeof(mtx_t), GRND_RANDOM);
    return res >= 0 ? res : -res;
#endif
#elif defined(_WIN32)
    BYTE Buf1 = 0b0;
    mtx_t res = 0b0;

    for(size_t i = 0ul; i < sizeof(mtx_t); ++i)
    {
        if (CryptGenRandom(hProv, (DWORD)(sizeof(BYTE)), &Buf1))
        {
            res += (mtx_t)Buf1;
            res <<= 0b1000;
        }
    }
    return res;
#else
    #error This operating system is not supported
#endif
}

inline int _mtx_el_lesser(datMtx a, datMtx b)
{
    return a.det < b.det;
}

inline int _mtx_el_greater(datMtx a, datMtx b)
{
    return a.det > b.det;
}
