#include "fraction.h"

namespace fc
{

Fraction::Fraction() : num(0), den(1ull) {}

Fraction::Fraction(numerator _n, denominator _d)
{
    ER_IF(_d == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = ((_n < 0) ^ (_d < 0)) ? -::std::llabs(_n) : ::std::llabs(_n);
        den = ::std::llabs(_d);
    }
    Fraction::reduction(*this);
}

Fraction::Fraction(const Fraction& instance)
{
    ER_IF(instance.den == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = ((instance.num < 0) ^ (instance.den < 0)) ? -::std::llabs(instance.num) : ::std::llabs(instance.num);
        den = ::std::llabs(instance.den);
    }
    Fraction::reduction(*this);
}

Fraction::Fraction(::std::string& str) noexcept(false)
{
    num = 0ll;
    den = 1ull;
    if (str.find('/') != ::std::string::npos)
    {
        size_t slashpos = str.find('/');
        auto t_num = ::std::atoll(str.substr(0, slashpos).c_str());
        auto t_den = ::std::atoll(str.substr(slashpos + 1).c_str());
        num = ((t_num < 0) ^ (t_den < 0)) ? -::std::llabs(t_num) : ::std::llabs(t_num);
        den = ::std::llabs(t_den);
    }
    else if (str.find('.') != ::std::string::npos)
    {
        size_t dotpos = str.find('.');
        size_t epos = str.find('e') != ::std::string::npos ? str.find('e') : str.size();
        denominator t_den = ::std::pow(10, epos - dotpos - 1);
        signed long long temp_num = ::std::atoll(str.substr(0, dotpos).c_str()) * t_den;
        numerator t_num = temp_num + ::std::atoll(str.substr(dotpos + 1, epos - dotpos - 1).c_str());
        num = ((t_num < 0) ^ (t_den < 0)) ? -::std::llabs(t_num) : ::std::llabs(t_num);
        den = t_den;
    }
    else
    {
        num = ::std::atoll(str.c_str());
    }
    if (str.find('e') != ::std::string::npos)
    {
        size_t epos = str.find('e');
        denominator t_den = ::std::pow(10, ::std::atoll(str.substr(epos + 2).c_str()));
        if (!strncmp(str.substr(epos + 1, 1).c_str(), "+", 1))
        {
            *this *= ::fc::Fraction(t_den, 1ll);
        }
        else
        {
            *this /= ::fc::Fraction(t_den, 1ll);
        }
    }
    Fraction::reduction(*this);
}

Fraction::Fraction(Fraction&& instance) noexcept(true)
{
    ER_IF(instance.den == 0ull, ::std::cout << "denominator is 0" << ::std::endl;, )
    else
    {
        num = ::std::move(instance.num);
        den = ::std::move(instance.den);
    }
}

Fraction::~Fraction()
{

}

Fraction& Fraction::operator=(const Fraction& _frac) noexcept(true)
{
    num = _frac.num;
    den = _frac.den;
    return *this;
}

Fraction& Fraction::operator=(Fraction&& _frac) noexcept(true)
{
    num = ::std::forward<decltype(num)>(_frac.num);
    den = ::std::forward<decltype(den)>(_frac.den);
    return *this;
}

Fraction& Fraction::operator=(::std::string& str) noexcept(false)
{
    num = 0ll;
    den = 1ull;
    if (str.find('/') != ::std::string::npos)
    {
        size_t slashpos = str.find('/');
        auto t_num = ::std::atoll(str.substr(0, slashpos).c_str());
        auto t_den = ::std::atoll(str.substr(slashpos + 1).c_str());
        num = ((t_num < 0) ^ (t_den < 0)) ? -::std::llabs(t_num) : ::std::llabs(t_num);
        den = ::std::llabs(t_den);
    }
    else if (str.find('.') != ::std::string::npos)
    {
        size_t dotpos = str.find('.');
        size_t epos = str.find('e') != ::std::string::npos ? str.find('e') : str.size();
        denominator t_den = ::std::pow(10, epos - dotpos - 1);
        signed long long temp_num = ::std::atoll(str.substr(0, dotpos).c_str()) * t_den;
        numerator t_num = temp_num + ::std::atoll(str.substr(dotpos + 1, epos - dotpos - 1).c_str());
        num = ((t_num < 0) ^ (t_den < 0)) ? -::std::llabs(t_num) : ::std::llabs(t_num);
        den = t_den;
    }
    else
    {
        num = ::std::atoll(str.c_str());
    }
    if (str.find('e') != ::std::string::npos)
    {
        size_t epos = str.find('e');
        denominator t_den = ::std::pow(10, ::std::atoll(str.substr(epos + 2).c_str()));
        if (!strncmp(str.substr(epos + 1, 1).c_str(), "+", 1))
        {
            *this *= ::fc::Fraction(t_den, 1ll);
        }
        else
        {
            *this /= ::fc::Fraction(t_den, 1ll);
        }
    }
    Fraction::reduction(*this);
    return *this;
}

Fraction::operator double()
{
    return static_cast<double>(static_cast<double>(num) / static_cast<double>(den));
}

Fraction Fraction::operator+(const Fraction& instance)
{
    Fraction::reduction(const_cast<Fraction&>(instance));
    Fraction temp(num * static_cast<Fraction::numerator>(instance.den) + instance.num  * static_cast<Fraction::numerator>(den), den * instance.den);
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
    Fraction temp(num * static_cast<Fraction::numerator>(instance.den) - instance.num  * static_cast<Fraction::numerator>(den), den * instance.den);
    Fraction::reduction(temp);

    return temp;
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

Fraction Fraction::reverse(const Fraction& instance)
{
    Fraction res = instance;
    bool flag = false;
    if (res.num < 0)
    {
        res.num = -res.num;
        flag = true;
    }
    auto temp = res.num;
    res.num = (flag) ? -static_cast<numerator>(res.den) : static_cast<numerator>(res.den);
    res.den = temp;

    return res;
}

Fraction& Fraction::reverse()
{
    bool flag = false;
    if (num < 0)
    {
        num = -num;
        flag = true;
    }
    auto temp = num;
    num = (flag) ? -static_cast<numerator>(den) : static_cast<numerator>(den);
    den = temp;

    return *this;
}

void Fraction::reduction(Fraction& instance)
{
    bool flag = false;
    if (instance.num < 0)
    {
        instance.num = -instance.num;
        flag = true;
    }
    auto temp = dxNOD(instance.num, instance.den);
    instance.num /= temp;
    instance.den /= temp;
    if (flag) instance.num = -instance.num;
}

}