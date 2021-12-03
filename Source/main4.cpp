#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <deque>
#include <vector>
#include <exception>
#include <memory>
#include <cctype>
#include <algorithm>

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
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				{ ::std::cerr << "![EXCPT]" << (const char*)defDX__FILELINE << ::std::endl; }
#else
#error This UNIX operating system is not supported
#endif
#else
#error This operating system is not supported
#endif
#endif

#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

::std::pair<::std::string, size_t> extract(const ::std::string& str, size_t offset = 0ull);
template<typename T, typename I = ::std::enable_if_t<::std::is_fundamental_v<T>, void>>
::std::string nts(const T& example);

class Expression
{
    friend class Interpreter;
public:
    using id_type = ::std::string;
    using integer = int;

    struct env_pair
    {
        id_type id;
        Expression* expr;

        env_pair(id_type aid, Expression* ae) noexcept(true) : id(aid), expr(ae) {}

        template<typename T = id_type>
        bool operator==(T str)
        {
            return id == str;
        }
    };

    using env_type = ::std::deque<env_pair>;

protected:
    ::std::shared_ptr<env_type> spEnv;

public:
    Expression() = delete;

    Expression(::std::shared_ptr<env_type> aspEnv) noexcept(true)
    {
        if (aspEnv == nullptr)  spEnv = ::std::make_shared<env_type>();
        else                    spEnv = aspEnv;
    }

    Expression(const Expression& instance) = delete;
    Expression(Expression& instance) noexcept(true) : spEnv(instance.spEnv) {}
    Expression(Expression&& instance) noexcept(true) : spEnv(::std::move(instance.spEnv)) {}

    virtual Expression* eval() = 0;
    virtual ::std::string print() const noexcept(true) = 0;

    virtual Expression* fromEnv() const noexcept(false)
    {
        throw ::std::runtime_error("inappropriate expression");
        return nullptr;     // useless, but clear
    }

    virtual integer getValue() const noexcept(false)
    {
        throw ::std::runtime_error("inappropriate expression");
        return integer();   // useless, but clear
    }

    virtual ~Expression() noexcept(false) {}

};

class Value : public Expression
{
    integer val;

public:
    Value(integer aval, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true) : Expression(aspEnv), val(aval) {}

    Expression* eval() noexcept(false) override
    {
        return this;
    }

    integer getValue() const noexcept(false) override
    {
        return val;
    }

    ::std::string print() const noexcept(true) override
    {
        return ::std::string("(val " + nts(val)) + ")";
    }

    ~Value() {}

};

class Variable : public Expression
{
    friend class Call;
    id_type id;

public:
    Variable(id_type& aid, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true) :
        Expression(aspEnv), id(aid) {}

    Expression* eval() noexcept(false) override
    {
        return this->fromEnv();
    }

    Expression* fromEnv() const noexcept(false) override
    {
        auto it = spEnv->rbegin();
        for(; it != spEnv->rend(); ++it)
        {
            if (it->id == id)   return it->expr;
        }
        throw ::std::runtime_error("not found!");
    }

    ::std::string print() const noexcept(true) override
    {
        return ::std::string("(var " + id) + ")";
    }

    ~Variable() {}

};

class Addition : public Expression
{
    Expression* first;
    Expression* second;

public:
    Addition(Expression* af, Expression* as, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true) :
        Expression(aspEnv), first(af), second(as) {}

    Expression* eval() noexcept(false) override
    {
        return new Value(first->eval()->getValue() + second->eval()->getValue(), spEnv);
    }

    ::std::string print() const noexcept(true) override
    {
        return "(add " + first->print() + second->print() + ")";
    }

    ~Addition() noexcept(false)
    {
        delete second;
        delete first;
    }

};

class Condition : public Expression
{
    Expression* first;
    Expression* second;
    Expression* then_cond;
    Expression* else_cond;

public:
    Condition(Expression* af, Expression* as, Expression* athen, Expression* aelse, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true) :
        Expression(aspEnv), first(af), second(as), then_cond(athen), else_cond(aelse) {}

    Expression* eval() noexcept(false) override
    {
        if (first->eval()->getValue() > second->eval()->getValue()) return then_cond->eval();
        return else_cond->eval();
    }

    ::std::string print() const noexcept(true) override
    {
        ::std::string res = "(if " + first->print();
        res += " " + second->print();
        res += " then " + then_cond->print();
        res += " else " + else_cond->print() + ")";
        return res;
    }

    ~Condition() noexcept(false)
    {
        delete else_cond;
        delete then_cond;
        delete second;
        delete first;
    }

};

class Let : public Expression
{
    id_type id;
    Expression* expr;
    Expression* body;

public:
    Let(id_type aid, Expression* aexpr, Expression* abody, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true)
        : Expression(aspEnv), id(aid), expr(aexpr), body(abody) {}

    Expression* eval() noexcept(false) override
    {
        ER_IF(body == nullptr,, throw ::std::runtime_error("body is not initialized!"); )
        spEnv->push_back(env_pair(id, expr->eval()));
        Expression* res = body->eval();
        spEnv->erase(::std::find_if(spEnv->begin(), spEnv->end(), [this](env_pair& x){ return x.id == id; }), spEnv->end());
        return res;
    }

    ::std::string print() const noexcept(true) override
    {
        ::std::string res = "(let " + id;
        res += " = " + expr->print();
        res += " in " + body->print() + ")";
        return res;
    }

    ~Let() noexcept(false)
    {
        delete body;
    }

};

class Function : public Expression
{
    friend class Call;
    id_type id;
    Expression* callable_body;

public:
    Function(id_type aid, Expression* aexpr, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true) :
        Expression(aspEnv), id(aid), callable_body(aexpr) {}

    Expression* eval() noexcept(true) override
    {
        return this;
    }

    ::std::string print() const noexcept(true) override
    {
        return ::std::string("(function " + id) + " " + callable_body->print() + ")";
    }

    ~Function() noexcept(false)
    {
        delete callable_body;
    }

};

class Call : public Expression
{
    Expression* func_expr;
    Expression* arg_expr;

public:
    Call(Expression* afunc_expr, Expression* aarg_expr, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(false) :
        Expression(aspEnv), func_expr(afunc_expr), arg_expr(aarg_expr) {}

    Expression* eval() noexcept(false) override
    {
        spEnv->push_back(env_pair(dynamic_cast<Function*>(func_expr->eval())->id, arg_expr->eval()));
        Expression* res = dynamic_cast<Function*>(func_expr->eval())->callable_body->eval();
        spEnv->erase(::std::find_if(spEnv->begin(), spEnv->end(), [this](env_pair& x){ return x.id == dynamic_cast<Function*>(func_expr->eval())->id; }),
            spEnv->end());
        return res;
    }

    ::std::string print() const noexcept(true) override
    {
        return ::std::string("(call ") + func_expr->print() + " " + arg_expr->print() + ")";
    }

    ~Call() noexcept(false)
    {
        delete arg_expr;
        delete func_expr;
    }

};

class Arr : public Expression
{
    friend class AtExpr;
    ::std::vector<Expression*> arr;

public:
    Arr(::std::vector<Expression*>&& rrarr, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true)
        : Expression(aspEnv), arr(::std::move(rrarr)) {}

    Expression* eval() noexcept(false) override
    {
        ::std::vector<Expression*> temp;
        for(auto& x : arr)
        {
            temp.reserve(temp.capacity() + 1);
            temp.emplace_back(x->eval());
        }
        return new Arr(::std::move(temp));
    }

    ::std::string print() const noexcept(true) override
    {
        ::std::string res = "(arr ";
        for(auto& x : arr)
        {
            res += " " + x->print();
        }
        res += ")";
        return res;
    }

    ~Arr() noexcept(false)
    {
        for(auto& x : arr)
        {
            delete x;
        }
    }

};

class Generator : public Expression
{
    Expression* count;
    Expression* functor;

public:
    Generator(Expression* acount, Expression* afoo, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true)
        : Expression(aspEnv), count(acount), functor(afoo) {}

    Expression* eval() noexcept(false) override
    {
        ::std::vector<Expression*> temp;
        for(decltype(count->eval()->getValue()) i {0}; i < count->eval()->getValue(); ++i)
        {
            Expression* expr = (new Call(functor, new Value(i), spEnv))->eval();
            temp.reserve(temp.capacity() + 1);
            temp.emplace_back(expr);
        }
        return new Arr(::std::move(temp));
    }

    ::std::string print() const noexcept(true) override
    {
        return "(gen " + count->print() + " " + functor->print() + ")";
    }

    ~Generator() noexcept(false)
    {
        delete functor;
        delete count;
    }

};
class AtExpr : public Expression
{
    Expression* arr;
    Expression* idx;

public:
    AtExpr(Expression* rrarr, Expression* aidx, ::std::shared_ptr<env_type> aspEnv = nullptr) noexcept(true)
        : Expression(aspEnv), arr(rrarr), idx(aidx) {}

    Expression* eval() noexcept(false) override
    {
        Expression* getArr = arr->eval();
        return dynamic_cast<Arr*>(getArr)->arr[idx->eval()->getValue()]->eval(); // need to eval() result (???)
    }

    ::std::string print() const noexcept(true) override
    {
        return "(at " + arr->print() + " " + idx->print() + ")";
    }

    ~AtExpr() noexcept(false)
    {
        delete idx;
        delete arr;
    }

};

class Interpreter
{
public:
    using id_type = typename ::Expression::id_type;
    using env_type = typename ::Expression::env_type;
    using integer = typename ::Expression::integer;

private:
    ::std::string str;
    Expression* expr;

public:
    Interpreter(const ::std::string& source_file_name)
    {
        ::std::ifstream read(source_file_name);
        ::std::string temp;
        ER_IFN(read.is_open(), ::std::cerr << "source_name = " << source_file_name << ::std::endl;, throw ::std::runtime_error("cannot open the file"); )
        while(read >> temp) { str.append(temp); str.append(" "); }
        read.close(); // damn, forgot to close in the past (in previous exercises)
        ::std::shared_ptr<env_type> ptr_ = ::std::make_shared<env_type>();
        expr = parse(str, ptr_);
    }

    void evalExpr(const ::std::string& res_file_name)
    {
        ::std::ofstream write(res_file_name);
        ER_IFN(write.is_open(), ::std::cerr << "source_name = " << res_file_name << ::std::endl;, throw ::std::runtime_error("cannot open the file"); )
        try
        {
            expr = expr->eval();
            write << expr->print() << ::std::endl;
        }
        catch(...)
        {
            write << "ERROR" << ::std::endl;
        }
        write.close();
    }

    ~Interpreter() noexcept(false)
    {
        delete expr;
    }

    Expression* parse(std::string& astr, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        // awful
        static ::std::string scall = "call";
        static ::std::string sfunc = "func";
        static ::std::string slet = "let";
        static ::std::string scond = "if";
        static ::std::string sadd = "add";
        static ::std::string svar = "var";
        static ::std::string sval = "val";
        static ::std::string sarr = "arr";
        static ::std::string sgen = "gen";
        static ::std::string satexpr = "at";
        size_t _off = astr.find('(');
        if (_off != ::std::string::npos)
        {
            ::std::string::iterator it;
            if ((it = ::std::find_if(astr.begin(), astr.end(), static_cast<int(*)(int)>(::std::isalpha))) != astr.end())
            {
                if (::std::search(it, astr.end(), scall.begin(), scall.end()) == it) return call_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), sfunc.begin(), sfunc.end()) == it) return func_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), slet.begin(), slet.end()) == it) return let_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), scond.begin(), scond.end()) == it) return cond_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), sadd.begin(), sadd.end()) == it) return add_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), svar.begin(), svar.end()) == it) return var_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), sval.begin(), sval.end()) == it) return val_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), sarr.begin(), sarr.end()) == it) return arr_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), sgen.begin(), sgen.end()) == it) return gen_construct(extract(astr), ptr);
                else if (::std::search(it, astr.end(), satexpr.begin(), satexpr.end()) == it) return atexpr_construct(extract(astr), ptr);
            }
        }
        return nullptr;
    }

    ///
    // due to for not to dive into recursion, I have not written same-part function for those (to avoid unnecessary calls)
    ///
    Expression* call_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        return new Call(f, s, ptr);
    }

    Expression* func_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        id_type id = getID(apair.first);
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        return new Function(id, f, ptr);
    }

    Expression* gen_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        return new Generator(f, s, ptr);
    }

    Expression* atexpr_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        return new AtExpr(f, s, ptr);
    }
    ///
    // end
    ///

    // <id_type> requires to be convertible to ::std::string & have to has method size()
    Expression* let_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        id_type id = getID(apair.first);
        /*// slow code (even if compiler optimize it)
        ::std::string check_str = apair.first.substr(apair.first.find(static_cast<::std::string>(id)) + id.size(),
            apair.first.find(" ", apair.first.find(static_cast<::std::string>(id)) + id.size()) - apair.first.find(static_cast<::std::string>(id)) + id.size() + 1);
        // insufficient verification, nevertheless at least something
        ER_IFN(check_str.find_first_not_of(" =") == ::std::string::npos,, throw ::std::runtime_error("invalid formated"); )*/
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        /*// slow code (even if compiler optimize it)
        check_str = apair.first.substr(fres.second, apair.first.find_first_of(" \n", fres.second + 1) - fres.second + 1);
        // insufficient verification, nevertheless at least something
        //ER_IF(check_str.find("in") == ::std::string::npos,, throw ::std::runtime_error("invalid formated"); )*/
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        return new Let(id, f, s, ptr);
    }

    Expression* cond_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        /*// slow code (even if compiler optimize it)
        ::std::string check_str = apair.first.substr(fres.second, apair.first.find_first_of(" \n", fres.second + 1) - fres.second + 1);
        // insufficient verification, nevertheless at least something
        ER_IF(check_str.find("then") == ::std::string::npos,, throw ::std::runtime_error("invalid formated"); )*/
        fres = extract(apair.first, fres.second + 1);
        Expression* thenc = parse(fres.first, ptr);
        /*// slow code (even if compiler optimize it)
        check_str = apair.first.substr(fres.second, apair.first.find_first_of(" \n", fres.second + 1) - fres.second + 1);
        // insufficient verification, nevertheless at least something
        ER_IF(check_str.find("else") == ::std::string::npos,, throw ::std::runtime_error("invalid formated"); )*/
        fres = extract(apair.first, fres.second + 1);
        Expression* elsec = parse(fres.first, ptr);
        return new Condition(f, s, thenc, elsec, ptr);
    }

    Expression* add_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        auto fres = extract(apair.first, 1ull);
        Expression* f = parse(fres.first, ptr);
        fres = extract(apair.first, fres.second + 1);
        Expression* s = parse(fres.first, ptr);
        return new Addition(f, s, ptr);
    }

    Expression* var_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        id_type id = getID(apair.first, [](int i) -> int { return i == ')'; });
        return new Variable(id, ptr);
    }

    // <id_type> have to has method c_str()
    Expression* val_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        ::std::string str = getID(apair.first, [](int i) -> int { return i == ')'; },
            static_cast<int(*)(int)>(::std::isspace), [](int i) -> int { return i == '-' || ::std::isdigit(i); });
        integer val = ::std::atoi(str.c_str());
        return new Value(val, ptr);
    }

    // at least one expression must to be in <arr ...> expr
    Expression* arr_construct(::std::pair<::std::string, size_t> apair, ::std::shared_ptr<env_type> ptr) const noexcept(false)
    {
        ::std::vector<Expression*> vpExpr;
        auto fres = extract(apair.first, 1ull);
        Expression* expr = parse(fres.first, ptr);
        vpExpr.reserve(vpExpr.capacity() + 1);
        vpExpr.emplace_back(expr);
        try
        {
            while(true)
            {
                fres = extract(apair.first, fres.second + 1);
                expr = parse(fres.first, ptr);
                vpExpr.reserve(vpExpr.capacity() + 1);
                vpExpr.emplace_back(expr);
            }
        }
        catch(...) {}
        return new Arr(::std::move(vpExpr), ptr);
    }

    id_type getID(::std::string astr,
        int(*end_predicate_)(int) = static_cast<int(*)(int)>(::std::isspace),
        int(*start_predicate_)(int) = static_cast<int(*)(int)>(::std::isspace),
        int(*main_predicate_)(int) = [](int i) -> int { return i == '_' || ::std::isalpha(i); }) const noexcept(false)
    {
        ::std::string::iterator start = ::std::find_if(astr.begin(), astr.end(), start_predicate_);
        ::std::string::iterator it = ::std::find_if(start + 1, astr.end(), main_predicate_);
        ::std::string::iterator temp;
        return astr.substr(it - astr.begin(), (temp = ::std::find_if(it, astr.end(), end_predicate_)) - it);
    }

};

int main()
{
    Interpreter test("./input.txt");
    test.evalExpr("./output.txt");

    return 0;
}

::std::pair<::std::string, size_t> extract(const ::std::string& str, size_t offset)
{
    try
    {
        ::std::string res;
        size_t braces = 0ull;
        ptrdiff_t _start_off = -1ll;
        while((offset = str.find_first_of("()", offset)) != ::std::string::npos)
        {
            if (_start_off == -1ll) _start_off = offset;
            if (str[offset] == '(') ++braces;
            else                    --braces;
            ++offset;
            if (!braces) break;
        }
        return ::std::make_pair(str.substr(_start_off, offset - _start_off), offset);
    }
    catch(...)
    {
        throw ::std::runtime_error("not found!");
    }
}

template<typename T, typename I = ::std::enable_if_t<::std::is_fundamental_v<T>, void>>
::std::string nts(const T& example)
{
	::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);
	ss << example;
	::std::string s = ss.str();
	return s;
}