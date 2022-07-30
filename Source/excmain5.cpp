///
// DigitalLab
// Compile with:
// g++ -c -Wall -std=c++20 -ftree-vectorize -mavx -lboost_thread
//
// Author:  Shibanov Evgeny
// Email:   shiba.zheka65@gmail.com
///

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <boost/thread.hpp>     // just for ::boost::thread::physical_concurrency()

#if !defined(defDX_S)
#define defDX_S(x)			#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)			defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__		defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef NOTATION_
#define NOTATION_			::std::cout << "![NTTN]" << (const char*)defDX__FILELINE << ::std::endl;
#define NTTN_ NOTATION_
#endif

#ifndef ERROR_

#if defined(_WIN32)
#define ERROR_				{ ::std::cout << "![EXPT]" << (const char*)defDX__FILELINE << ::std::endl;      \
							throw ::std::exception(); }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				try {																			\
								throw ::std::exception();													\
							} catch (::std::exception& x) {													\
								::std::cout << "![EXPT]" << (const char*)defDX__FILELINE << ::std::endl;	\
							}
#else
#error This UNIX operating system is not supported
#endif
#else
#error This operating system is not supported
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc)		if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc)		if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

#define _CORE_LOAD_VALUE_ 4u
#define _COUNT_OF_THREADS_ (::boost::thread::physical_concurrency() * _CORE_LOAD_VALUE_)

::std::atomic_bool is_finished;
::std::mutex mt_main;
::std::vector<::std::thread> vThreads;
::std::queue<::std::pair<size_t, size_t>> main_queue;
::std::condition_variable cv;

template<typename T>
class Counter
{
public:
    static ::std::atomic_ptrdiff_t _current_active;

    Counter() noexcept(true)
    {
        ++_current_active;
    }

    Counter(const Counter&) noexcept(true)
    {
        ++_current_active;
    }

    Counter(Counter&&) noexcept(true)
    {
        ++_current_active;
    }

    virtual ~Counter() noexcept(false)
    {
        --_current_active;
    }
};

template<typename T> ::std::atomic_ptrdiff_t Counter<T>::_current_active(0ull);

class Map;

class Recognizer : public Counter<Recognizer>
{
public:
    bool recognizeCell(const Map&, size_t, size_t) const noexcept(false);

};

class Map
{
    typedef char elem_t;

public:
    ::std::vector<::std::vector<elem_t>> vvMap;
    ::std::vector<::std::vector<elem_t>> vvKernel;
    ::std::vector<::std::vector<elem_t>> vvResMap;

    Map() noexcept(true) {}

    template< template<typename, typename> typename C, typename _C, typename _T >
    friend ::std::enable_if_t<::std::conjunction_v<
        ::std::is_base_of<::std::basic_istream<_C, _T>, C<_C, _T>>,
        ::std::is_same<char, _C>>, C<_C, _T>&>
    operator>>(C<_C, _T>& _stream, Map& instance) noexcept(false)
    {
        instance.fillVectorMap(_stream, instance.vvKernel);
        instance.fillVectorMap(_stream, instance.vvMap);
        size_t temp_y = instance.vvMap.size() - instance.vvKernel.size() + 1;
        size_t temp_x = instance.vvMap.back().size() - instance.vvKernel.back().size() + 1;
        instance.vvResMap.reserve(temp_y);
        for(size_t i {0}; i < temp_y; ++i)
        {
            instance.vvResMap.emplace_back(temp_x, 0);
        }
        return _stream;
    }

    ~Map() {}

    // recognizing patterns multithreaded
    void recognize() noexcept(false)
    {
        ER_IF(vvMap.empty() || vvKernel.empty(), ::std::cerr << "[EXC] Enter initial data before!" << ::std::endl;, return; )
        startThreads();
        for(size_t y {0}; y < vvResMap.size(); ++y)
        {
            for(size_t x {0}; x < vvResMap.back().size(); ++x)
            {
                {
                    ::std::unique_lock<::std::mutex> lock(mt_main);
                    main_queue.push({x, y});
                }
                cv.notify_one();
            }
        }
        Recognizer temp;
        while(temp._current_active > 1 || !main_queue.empty())
        {
            ::std::this_thread::sleep_for(::std::chrono::microseconds(1));
        }
        finishThreads();
    }

    void printResultMap() noexcept(false)
    {
        ::std::vector<::std::pair<ptrdiff_t, ptrdiff_t>> vResList;
        // for optimization
        for(size_t y {0}; y < vvResMap.size(); ++y)
        {
            for(size_t x {0}; x < vvResMap.back().size(); ++x)
            {
                if (static_cast<int>(vvResMap[y][x]))
                {
                    vResList.reserve(vResList.capacity() + 1);
                    vResList.emplace_back(::std::make_pair(y, x));
                }
            }
        }
        // if a pattern is found, then delete the patterns that intersect with this one
        for(size_t i {0}; i < vResList.size(); ++i)
        {
            for(size_t j { i + 1ull }; j < vResList.size(); ++j)
            {
                auto& _s = vResList[i];
                auto& _t = vResList[j];
                ptrdiff_t _y = vvKernel.size();
                ptrdiff_t _x = vvKernel.back().size();
                if (_t.first < _s.first + _y && _t.second < _s.second + _x && _t.second > _s.second - _x)
                {
                    vResList.erase(vResList.begin() + j);
                    --j;
                }
            }
            for(size_t y {0}; y < vvKernel.size(); ++y)
            {
                for(size_t x {0}; x < vvKernel.back().size(); ++x)
                {
                    auto& temp = vvMap[vResList[i].first + y][vResList[i].second + x];
                    if      (temp == '0') temp -= 6;
                    else if (temp == '1') ++temp;
                }
            }
        }
        for(size_t y {0}; y < vvMap.size(); ++y)
        {
            for(size_t x {0}; x < vvMap.back().size(); ++x)
            {
                ::std::cout << vvMap[y][x] << ' ';
            }
            ::std::cout << ::std::endl;
        }
    }

private:
    void startThreads() noexcept(false)
    {
        is_finished.store(false);
        vThreads.reserve(_COUNT_OF_THREADS_);
        for(size_t i {0}; i < _COUNT_OF_THREADS_; ++i)
        {
            vThreads.emplace_back(::std::thread([this]()
            {
                while(!is_finished.load())
                {
                    ::std::pair<size_t, size_t> temp_xy;
                    {
                        ::std::unique_lock<::std::mutex> lock(mt_main);
                        cv.wait(lock, []() -> bool { return !main_queue.empty() || is_finished.load(); });
                        if (is_finished.load()) break;

                        temp_xy = main_queue.front();
                        main_queue.pop();
                    }
                    cv.notify_one();
                    elem_t state;
                    {
                        Recognizer temp;
                        state = temp.recognizeCell(*this, temp_xy.first, temp_xy.second) ? 1 : 0;
                    }
                    vvResMap[temp_xy.second][temp_xy.first] = state;
                }
            }));
        }
    }

    void finishThreads() noexcept(false)
    {
        is_finished.store(true);
        ::std::thread thr([]()
            {
                while (is_finished.load()) cv.notify_all();
            });
        while(vThreads.size())
        {
            for(auto it = vThreads.begin(); it != vThreads.end();)
            {
                if (it->joinable())
                {
                    it->join();
                    it = vThreads.erase(it);
                    continue;
                }
                ++it;
            }
        }
        is_finished.store(false);
        thr.join();
    }

    template< template<typename, typename> typename C, typename _C, typename _T, typename T>
    void fillVectorMap(C<_C, _T>& _stream, ::std::vector<::std::vector<T>>& v) noexcept(false)
    {
        size_t ax, ay;
        _stream >> ax >> ay;
        v.reserve(ay);
        for(size_t y {0}; y < ay; ++y)
        {
            v.emplace_back();
            v.back().reserve(ax);
            for(size_t x {0}; x < ax; ++x)
            {
                T temp;
                _stream >> temp;
                v.back().emplace_back(temp);
            }
        }
    }

};

// thread function.
// it only reads and does not modify the data, which looks "safe" to work with
bool Recognizer::recognizeCell(const Map& instance, size_t ax, size_t ay) const noexcept(false) try
{
    for(size_t ky {0}; ky < instance.vvKernel.size(); ++ky)
    {
        for(size_t kx {0}; kx < instance.vvKernel.back().size(); ++kx)
        {
            if (instance.vvKernel[ky][kx] != instance.vvMap[ay + ky][ax + kx])  return false;
        }
    }
    return true;
}
catch(...)
{
    ::std::cerr << "thread got an error" << ::std::endl;
    return false;
}

int main()
{
    Map test;
    ::std::cin >> test;
    test.recognize();
    ::std::cout << "Output: " << ::std::endl;
    test.printResultMap();

    return 0;
}