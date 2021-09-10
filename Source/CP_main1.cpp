#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <cmath>

#if !defined(defDX_S)
#define defDX_S(x)		#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)		defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__	defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef ERROR_

#if defined(_WIN32)
#define ERROR_				{ ::std::cout << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; 	\
							throw ::std::exception(); }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				try {																			\
                            	throw ::std::exception();													\
							} catch (::std::exception& x) {													\
								::std::cout << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl;	\
							}
#else
#error This UNIX operating system is not supported by dx::NN
#endif
#else
#error This operating system is not supported by dx::NN
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

#include "fraction.h"

class Matrix
{
public:
    size_t size; // is N
    ::std::vector<::std::vector<::fc::Fraction>> mtx; // N x (N + 1)
    ::std::vector<::fc::Fraction> res; // N

    Matrix() : size(0u) {}

    Matrix(const char* file)
    {
        ::std::ifstream read(file);
        ER_IFN(read.is_open(), ::std::cout << "Error to read " << file << ::std::endl;, return; )
        
        read >> size;
        mtx.reserve(size);
        res.reserve(size);
        for(size_t i {0}; i < size; ++i)
        {
            mtx.emplace_back(::std::vector<::fc::Fraction>());
            mtx.back().reserve(size + 1);
            res.emplace_back(0ll, 1ll);
        }

        for(size_t i {0}; i < size; ++i)
        {
            for(size_t j {0}; j < size + 1; ++j)
            {
                ::std::string read_symbs;
                read >> read_symbs;
                if (read_symbs.find('/') != ::std::string::npos) 
                {
                    size_t slashpos = read_symbs.find('/');
                    mtx[i].emplace_back(static_cast<::fc::Fraction::numerator>(::std::atoll(read_symbs.substr(0, slashpos).c_str())),
                        static_cast<::fc::Fraction::denominator>(::std::atoll(read_symbs.substr(slashpos + 1).c_str())));
                }
                else if (read_symbs.find('.') != ::std::string::npos)
                {
                    size_t dotpos = read_symbs.find('.');
                    double mult = ::std::pow(10, read_symbs.size() - dotpos - 1);
                    signed long long temp_num = ::std::atoll(read_symbs.substr(0, dotpos).c_str()) * mult;
                    mtx[i].emplace_back(static_cast<::fc::Fraction::numerator>(temp_num + ::std::atoll(read_symbs.substr(dotpos + 1).c_str())),
                        static_cast<::fc::Fraction::denominator>(mult));
                }
                else
                {
                    mtx[i].emplace_back(static_cast<::fc::Fraction::numerator>(::std::atoll(read_symbs.c_str())),
                        static_cast<::fc::Fraction::denominator>(1ll));
                }
            }
        }
    }

    bool Gauss()
    {
        size_t idx_swapedline = 1ull;
        for(size_t i {1}; i < size; ++i)
        {
            if (mtx[i - 1][i - 1].num == 0)
            {
                ER_IF(idx_swapedline >= size, ::std::cout << "Null on diagonal!" << ::std::endl;, return false; )
                ::std::swap(mtx[idx_swapedline++], mtx[i - 1]);
                --i;
                continue;
            }

            for(size_t k {i}; k < size; ++k)
            {
                ::fc::Fraction diff(::fc::Fraction::reverse(mtx[i - 1][i - 1]));
                diff *= mtx[k][i - 1];
                for(size_t j {0}; j < size + 1; ++j)
                {
                    if (diff.num)
                    {
                        mtx[k][j] -= mtx[i - 1][j] * diff;
                    }
                }
            }
        }
        mtx[size - 1][size] /= mtx[size - 1][size - 1];
        mtx[size - 1][size - 1] = ::fc::Fraction(1, 1);

        for(size_t i {0}; i < size; ++i)
        {
            for(size_t j {i + 1}; j < size + 1; ++j)
            {
                mtx[i][j] /= mtx[i][i];
            }
            mtx[i][i].num = 1;
            mtx[i][i].den = 1;
        }

        for(ptrdiff_t i = static_cast<ptrdiff_t>(size - 1); i >= 0; --i)
        {
            ::fc::Fraction temp_res(0ll, 1ll);
            for(ptrdiff_t j {i + 1}; j < size; ++j)
            {
                temp_res += mtx[i][j] * res[j];
            }
            if (res[i].num == 0)    res[i] = mtx[i][size] - temp_res;
            else ER_IF(res[i] != mtx[i][size] - temp_res, ::std::cout << "Inconsistent system of linear equations, since result of X[" << i << ']' << ::std::endl;, return false; )
        }
        return true;
    }

    bool isNullOnDiag()
    {
        for(size_t i {0}; i < size; ++i)
        {
            if (mtx[i][i] == ::fc::Fraction(0, 1)) return true;
        }
        return false;
    }

    void printMtx()
    {
        for(size_t i = 0ull; i < size; ++i)
        {
            for(size_t j = 0ull; j < size + 1; ++j)
            {
                ::std::cout << mtx[i][j].num << '/';
                ::std::cout << mtx[i][j].den << ' ';
            }
            ::std::cout << ::std::endl;
        }
    }

    void printRes()
    {
        for(size_t i = 0ull; i < size; ++i)
        {
            ::std::cout << res[i].num << '/';
            ::std::cout << res[i].den << ' ';
        }
        ::std::cout << ::std::endl;
    }

    void printResDouble()
    {
        for(size_t i = 0ull; i < size; ++i)
        {
            ::std::cout << res[i] << ' ';
        }
        ::std::cout << ::std::endl;
    }

};


int main(int argc, char** argv)
{
    ::std::string filename;
    if (argc <= 1)   filename = "CP_input.txt";
    else            filename = argv[1];

    Matrix main_mtx(filename.c_str());
    ::std::cout << "Matrix:" << ::std::endl;
    main_mtx.printMtx();

    ER_IFN(main_mtx.Gauss(), ::std::cout << "An error has occured!" << ::std::endl;, )
    else
    {
        ::std::cout << "Results:" << ::std::endl;
        main_mtx.printRes();
        main_mtx.printResDouble();
    }

    return 0;
}