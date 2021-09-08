#include "fraction.h"

namespace fc
{

//Fraction::Fraction() : num(0), den(1ull) {}

Fraction::Fraction(numerator _n, denominator _d)
{
    ER_IF(_d == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = _n;
        den = _d;
    }
    Fraction::reduction(*this);
}

Fraction::Fraction(const Fraction& instance)
{
    ER_IF(instance.den == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = instance.num;
        den = instance.den;
    }
    Fraction::reduction(*this);
}

/*Fraction::Fraction(Fraction&& instance) noexcept(true)
{
    ER_IF(instance.den == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = ::std::move(instance.num);
        den = ::std::move(instance.den);
    }
}*/

Fraction::~Fraction()
{

}

/*Fraction& Fraction::operator=(Fraction _frac) noexcept(true)
{
    num = _frac.num;
    den = _frac.den;
    return *this;
}*/

Fraction& Fraction::operator=(Fraction&& _frac) noexcept(true)
{
    //::std::cout << _frac.den << ::std::endl;
    num = ::std::forward<decltype(num)>(_frac.num);
    den = ::std::forward<decltype(den)>(_frac.den);
    return *this;
}

Fraction& Fraction::operator=(const Fraction& _frac) noexcept(true)
{
    //::std::cout << _frac.den << ::std::endl;
    num = _frac.num;
    den = _frac.den;
    return *this;
}

Fraction::operator double()
{
    return static_cast<double>(static_cast<double>(num) / static_cast<double>(den));
}

Fraction Fraction::operator+(const Fraction& instance)
{
    Fraction::reduction(const_cast<Fraction&>(instance));
    Fraction temp(num * static_cast<Fraction::numerator>(instance.den) + instance.num  * static_cast<Fraction::numerator>(this->den), den * instance.den);
    Fraction::reduction(temp);

    return temp;
}

Fraction& Fraction::operator+=(const Fraction& instance)
{
    *this = *this + instance;
    return *this;
}

Fraction Fraction::operator-(const Fraction& instance)
{
    Fraction::reduction(const_cast<Fraction&>(instance));
    this->num = this->num * static_cast<Fraction::numerator>(instance.den) - instance.num * static_cast<Fraction::numerator>(this->den);
    this->den *= instance.den;
    Fraction::reduction(*this);

    return *this;
}

Fraction& Fraction::operator-=(const Fraction& instance)
{
    *this = *this - instance;
    return *this;
}

Fraction Fraction::operator*(const Fraction& instance)
{
    Fraction::reduction(const_cast<Fraction&>(instance));
    Fraction temp(num * instance.num, den * instance.den);
    Fraction::reduction(temp);

    return temp;
}

Fraction& Fraction::operator*=(const Fraction& instance)
{
    *this = *this * instance;
    return *this;
}

Fraction Fraction::operator/(const Fraction& instance)
{
    return Fraction::operator*(Fraction::reverse(instance));
}

Fraction& Fraction::operator/=(const Fraction& instance)
{
    *this = *this / instance;
    return *this;
}

Fraction Fraction::reverse(const Fraction instance)
{
    auto res = instance;
    auto temp = res.num;
    //if (temp >= 0)
    //{
        res.num = res.den;
        res.den = temp;
    /*}
    else
    {
        //::std::cout << "!!!" << instance.num << ' ' << instance.den << ::std::endl;
        res.num = -static_cast<Fraction::numerator>(res.den);
        res.den = -temp;
        //::std::cout << "!!!" << instance.num << ' ' << instance.den << ::std::endl;
    }*/

    return res;
}

void Fraction::reduction(Fraction& instance)
{
    if ((instance.num < 0) && (instance.den < 0))
    {
        instance.num = -instance.num;
        instance.den = -instance.den;
    }
    auto temp = dxNOD(instance.num, instance.den);
    instance.num /= temp;
    instance.den /= temp;
}

}