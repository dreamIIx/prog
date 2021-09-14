#ifndef FRAC_H
#define FRAC_H

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <string.h>

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

namespace fc
{

template<class T1, class T2>
T2 dxNOD(T1 a, T2 b)
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while(a && b)
    {
        if (a > b) a = a % b;
        else b = b % a;
    }
    return a + b;
}

class Fraction
{
public:
    using numerator = signed long long;
    using denominator = unsigned long long;
    //using numerator_type = ::std::vector<numerator>;
    //using denominator_type = ::std::vector<denominator>;

    numerator num;
    denominator den;

    Fraction();
    explicit Fraction(numerator numerator, denominator denominator);
    Fraction(const Fraction& instance);
    Fraction(::std::string&) noexcept(false);
    //Fraction(Fraction&& instance) noexcept(true);
    ~Fraction();
    
    //Fraction& operator=(Fraction instance) noexcept(true);
    Fraction& operator=(Fraction&& instance) noexcept(true);
    Fraction& operator=(const Fraction& instance) noexcept(true);
    Fraction& operator=(::std::string&) noexcept(false);
    operator double();
    Fraction operator+(const Fraction& instance);
    Fraction& operator+=(const Fraction& instance);
    Fraction operator-(const Fraction& instance);
    Fraction& operator-=(const Fraction& instance);
    Fraction operator*(const Fraction& instance);
    Fraction& operator*=(const Fraction& instance);
    Fraction operator/(const Fraction& instance);
    Fraction& operator/=(const Fraction& instance);

    static Fraction reverse(const Fraction& instance);
    Fraction& reverse();
    static void reduction(Fraction& instance);
};

}

#endif