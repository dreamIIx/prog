#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <cmath>

#define ISBIT(x,pos)        ( ( (x) & ( 0x1 << (pos) ) ) != 0 )
#define GETBIT(x,pos)       ( (x) & ( 0x1 << (pos) ) )
#define GETBITS(x,y,pos)	( (x) & ( y << (pos) ) )
#define SETBIT(x,pos)       ( (x) |= ( 0x1 << (pos) ) )
#define UNSETBIT(x,pos)     ( (x) &= (~( 0x1 << (pos) ) ) )
#define SETBITS(x,y,pos)	( (x) |= ( y << (pos) ) )
#define UNSETBITS(x,y,pos)	( (x) &= (~( y << (pos) ) ) )

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

template<typename T>
::std::string nts(T);

class Matrix
{
public:
    bool completed;
    size_t size; // is N
    ::std::vector<::std::vector<::fc::Fraction>> start_mtx; // N x (N + 1)
    ::std::vector<::std::vector<::fc::Fraction>> mtx; // N x (N + 1)
    ::std::vector<::std::vector<::fc::Fraction>> inv_mtx; // N x N
    ::std::vector<::fc::Fraction> res; // N

    Matrix() noexcept(true) : completed(false), size(0u) {}

    Matrix(const char* file) noexcept(false)
    {
        ::std::ifstream read(file);
        ER_IFN(read.is_open(), ::std::cout << "Error to read " << file << ::std::endl;, return; )
        
        read >> size;
        mtx.reserve(size);
        inv_mtx.reserve(size);
        res.reserve(size);
        for(size_t i {0}; i < size; ++i)
        {
            mtx.emplace_back(::std::vector<::fc::Fraction>());
            mtx.back().reserve(size + 1);
            inv_mtx.emplace_back(::std::vector<::fc::Fraction>());
            inv_mtx.back().reserve(size);
            res.emplace_back(0ll, 1ll);
        }

        for(size_t i {0}; i < size; ++i)
        {
            for(size_t j {0}; j < size + 1; ++j)
            {
                ::std::string read_symbs;
                read >> read_symbs;
                mtx[i].emplace_back(read_symbs);
                if ((i == j) && j != size) inv_mtx[i].emplace_back(1ull, 1ull);
                else inv_mtx[i].emplace_back();
            }
        }
        start_mtx = mtx;
        completed = true;
    }

    void __spec_input_mtx()
    {
        size = 3ull;
        mtx.reserve(size);
        inv_mtx.reserve(size);
        res.reserve(size);
        for(size_t i {0}; i < size; ++i)
        {
            mtx.emplace_back(::std::vector<::fc::Fraction>());
            mtx.back().reserve(size + 1);
            inv_mtx.emplace_back(::std::vector<::fc::Fraction>());
            inv_mtx.back().reserve(size);
            res.emplace_back(0ll, 1ll);
        }

        for(size_t i {0}; i < size; ++i)
        {
            for(size_t j {0}; j < size; ++j)
            {
                double temp_exp = exp(-0.1 * ::std::llabs(i - j));
                auto temp_str = nts(temp_exp);
                mtx[i].emplace_back(temp_str);
                if (i == j) inv_mtx[i].emplace_back(1ll, 1ull);
                else inv_mtx[i].emplace_back();
            }
        }
        for(size_t i {0}; i < size; ++i)
        {
            mtx[i].emplace_back(i, 1ull);
        }
        start_mtx = mtx;
        completed = true;
    }

    static ::std::vector<::std::vector<::fc::Fraction>> MultMtxOnMtx(::std::vector<::std::vector<::fc::Fraction>>& w, ::std::vector<::std::vector<::fc::Fraction>>& v) noexcept(false)
    {
        ER_IF(w.size() != v.size(), ::std::cout << "Ill-formed vector!" << ::std::endl;, return ::std::vector<::std::vector<::fc::Fraction>>(); )
        ::std::vector<::std::vector<::fc::Fraction>> func_result;
        func_result.reserve(w.size());
        for(size_t i {0}; i < w.size(); ++i)
        {
            func_result.emplace_back(::std::vector<::fc::Fraction>());
            func_result.back().reserve(v[0].size());
        }
        for(size_t k {0}; k < v[0].size(); ++k)
        {
            for(size_t i {0}; i < w.size(); ++i)
            {
                ::fc::Fraction temp_res_;
                for(size_t j {0}; j < w.size(); ++j)
                {
                    temp_res_ += w[i][j] * v[j][k];
                }
                func_result[i].emplace_back(temp_res_);
            }
        }
        return func_result;
    }

    bool GaussForward() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return false; )
        size_t idx_swapedline = 1ull;
        for(size_t i {1}; i < size; ++i)
        {
            if (mtx[i - 1][i - 1].num == 0)
            {
                if (idx_swapedline >= size) idx_swapedline = i;
                ::std::swap(mtx[idx_swapedline], mtx[i - 1]);
                ::std::swap(inv_mtx[idx_swapedline++], inv_mtx[i - 1]);
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
                        if (j != size) inv_mtx[k][j] -= inv_mtx[i - 1][j] * diff;
                    }
                }
            }
        }
        mtx[size - 1][size] /= mtx[size - 1][size - 1];
        for(size_t i {0}; i < size; ++i)
        {
            inv_mtx[size - 1][i] /= mtx[size - 1][size - 1];
        }
        mtx[size - 1][size - 1] = ::fc::Fraction(1, 1);
        return true;
    }

    bool GaussForwardReverse() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return false; )
        ptrdiff_t idx_swapedline = size - 2;
        for(ptrdiff_t i {size - 2}; i >= 0; --i)
        {
            if (mtx[i + 1][i + 1].num == 0)
            {
                if (idx_swapedline >= size) idx_swapedline = i;
                ::std::swap(mtx[idx_swapedline], mtx[i + 1]);
                ::std::swap(inv_mtx[idx_swapedline--], inv_mtx[i + 1]);
                ++i;
                continue;
            }

            for(ptrdiff_t k {i}; k >= 0; --k)
            {
                ::fc::Fraction diff(::fc::Fraction::reverse(mtx[i + 1][i + 1]));
                diff *= mtx[k][i + 1];
                for(size_t j {0}; j < size + 1; ++j)
                {
                    if (diff.num)
                    {
                        mtx[k][j] -= mtx[i + 1][j] * diff;
                        if (j != size) inv_mtx[k][j] -= inv_mtx[i + 1][j] * diff;
                    }
                }
            }
        }
        return true;
    }

    bool GaussWithPivotForward() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return false; )
        size_t idx_line2swap;
        size_t idx_swapedline = 1ull;
        for(size_t i {1}; i < size; ++i)
        {
            idx_line2swap = i - 1;
            for(size_t p {i}; p < size; ++p)
            {
                if (mtx[p][i - 1] > mtx[idx_line2swap][i - 1]) idx_line2swap = p;
            }
            if (idx_line2swap != i - 1)
            {
                ::std::swap(mtx[idx_line2swap], mtx[i - 1]);
                ::std::swap(inv_mtx[idx_line2swap], inv_mtx[i - 1]);
            }
            if (mtx[i - 1][i - 1].num == 0)
            {
                if (idx_swapedline >= size) idx_swapedline = i;
                ::std::swap(mtx[idx_swapedline], mtx[i - 1]);
                ::std::swap(inv_mtx[idx_swapedline++], inv_mtx[i - 1]);
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
                        if (j != size) inv_mtx[k][j] -= inv_mtx[i - 1][j] * diff;
                    }
                }
            }
        }
        mtx[size - 1][size] /= mtx[size - 1][size - 1];
        for(size_t i {0}; i < size; ++i)
        {
            inv_mtx[size - 1][i] /= mtx[size - 1][size - 1];
        }
        mtx[size - 1][size - 1] = ::fc::Fraction(1, 1);
        return true;
    }

    bool GaussBackward() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return false; )
        for(size_t i {0}; i < size; ++i)
        {
            for(size_t j {0}; j < size + 1; ++j)
            {
                if (i != j) mtx[i][j] /= mtx[i][i];
                if (j != size) inv_mtx[i][j] /= mtx[i][i];
            }
            mtx[i][i].num = 1ll;
            mtx[i][i].den = 1ull;
        }

        for(ptrdiff_t i = static_cast<ptrdiff_t>(size - 1); i >= 0; --i)
        {
            ::fc::Fraction temp_res(0ll, 1ll);
            for(ptrdiff_t j {i + 1}; j < size; ++j)
            {
                temp_res += mtx[i][j] * res[j];
            }
            auto temp = mtx[i][size] - temp_res;
            if (res[i].num == 0)    res[i] = temp;
        }
        return true;
    }

    // if max szMATRIX == 64
    ::fc::Fraction det(unsigned long long exc = 0ull, int k = 0, int _spec = 1)
    {
        ::fc::Fraction temp_res;
        int shift = -1;
        for(int i = 0; i < size; ++i)
        {
            if (!ISBIT(exc, i))
            {
                if (shift == -1)    shift = i;
                if (k < size - 1)
                {
                    SETBIT(exc, i);
                    temp_res += (((i - shift) & 1) ? (::fc::Fraction(-1, 1)) : (::fc::Fraction(1, 1))) * mtx[k][i] * det(exc, k + 1, 0);
                    UNSETBIT(exc, i);
                }
                else
                {
                    return mtx[k][i];
                }
            }
            else
            {
                if (shift == -1)    ++shift;
                ++shift;
            }
        }
        ::fc::Fraction::reduction(temp_res);

        return temp_res;
    }

    // true = 1
    // false = 0
    // error = -1
    int isNullOnDiag() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return -1; )
        for(size_t i {0}; i < size; ++i)
        {
            if (mtx[i][i] == ::fc::Fraction(0, 1)) return 1;
        }
        return 0;
    }

    void printMtx() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return; )
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

    void printInvMtx() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return; )
        for(size_t i = 0ull; i < size; ++i)
        {
            for(size_t j = 0ull; j < size; ++j)
            {
                ::std::cout << inv_mtx[i][j].num << '/';
                ::std::cout << inv_mtx[i][j].den << ' ';
            }
            ::std::cout << ::std::endl;
        }
    }

    void printRes() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return; )
        for(size_t i = 0ull; i < size; ++i)
        {
            ::std::cout << res[i].num << '/';
            ::std::cout << res[i].den << ' ';
        }
        ::std::cout << ::std::endl;
    }

    void printResDouble() noexcept(false)
    {
        ER_IFN(completed, ::std::cout << "Matrix is incompleted!" << ::std::endl;, return; )
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
    if (argc <= 1)  filename = "CP_input.txt";
    else            filename = argv[1];

    Matrix main_mtx(filename.c_str());
    //Matrix main_mtx;
    //main_mtx.__spec_input_mtx();
    ::std::cout << "Input matrix:" << ::std::endl;
    main_mtx.printMtx();

    /*::std::vector<::std::vector<::fc::Fraction>> vector_x;
    vector_x.reserve(3ull);
    for(size_t i {0}; i < 3ull; ++i)
    {
        vector_x.emplace_back(::std::vector<::fc::Fraction>());
    }
    vector_x[0].emplace_back(1ll, 1ull);
    for(size_t i {1}; i < 3; ++i)
    {
        vector_x[i].emplace_back();
    }
    auto res_b = main_mtx.MultMtxOnVector(vector_x);
    ::std::cout << "\nResult B:" << ::std::endl;
    for(size_t i {0}; i < res_b.size(); ++i)
    {
        for(size_t j {0}; j < res_b.back().size(); ++j)
        {
            ::std::cout << res_b[i][j].num << '/' << res_b[i][j].den << ' ';
        }
        ::std::cout << ::std::endl;
    }
    for(size_t i {0}; i < main_mtx.size; ++i)
    {
        main_mtx.mtx[i][main_mtx.size] = res_b[i][0];
    }
    ::std::cout << "2 matrix:" << ::std::endl;
    main_mtx.printMtx();*/

    auto res_det = main_mtx.det();
    ::std::cout << "\nDeterminant: " << res_det << " [" << res_det.num << '/' << res_det.den << ']' << ::std::endl;
    ER_IFN(main_mtx.GaussForward(), ::std::cout << "An error has occured!" << ::std::endl;, return 1; )
    ER_IFN(main_mtx.GaussForwardReverse(), ::std::cout << "An error has occured!" << ::std::endl;, return 1; )
    ER_IFN(main_mtx.GaussBackward(), ::std::cout << "An error has occured!" << ::std::endl;, return 1; )
    else
    {
        ::std::cout << "\nMatrix:" << ::std::endl;
        main_mtx.printMtx();
        ::std::cout << "\nInverse matrix:" << ::std::endl;
        main_mtx.printInvMtx();
        ::std::cout << "\nResults:" << ::std::endl;
        main_mtx.printRes();
        main_mtx.printResDouble();
    }
    
    main_mtx.mtx = main_mtx.start_mtx;
    auto res_b2 = Matrix::MultMtxOnMtx(main_mtx.mtx, main_mtx.inv_mtx);
    ::std::cout << "\nResult A*A-1:" << ::std::endl;
    for(size_t i {0}; i < res_b2.size(); ++i)
    {
        for(size_t j {0}; j < res_b2.back().size(); ++j)
        {
            ::std::cout << res_b2[i][j].num << '/' << res_b2[i][j].den << ' ';
        }
        ::std::cout << ::std::endl;
    }

    return 0;
}

template<typename T>
::std::string nts(T arg)
{
	::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

	ss << arg;

	::std::string s = ss.str();

	return s;
}