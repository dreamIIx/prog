#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cctype>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <type_traits>
#include <chrono>

#define CURL_STATICLIB

#include "curl/curl.h"

#ifdef _DEBUG
#pragma comment(lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment(lib, "curl/libcurl_a.lib")
#endif

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

#define _dx_COMMENTS
//#undef _dx_COMMENTS

#define _MAX_BUF_SIZE 8096000000
#define _PATH_TO_WRITE_TO "./testpath/output/"
#define _PATH_TO_READ_FROM "./testpath/input/"

::std::mutex mt_filesys;

::std::string parse_tag(const ::std::string& source, const ::std::string& tag, const ::std::string& prop, size_t& start_pos);
static size_t writer(char*, size_t, size_t, ::std::string*);
static size_t fwriter(char*, size_t, size_t, ::std::ofstream*);
bool send_req(const ::std::string& addr, void* response, bool isFileWriter, bool secure_access);

template<typename T>
class Counter
{
public:
    static ::std::atomic_size_t countOf;
    static ::std::atomic_ptrdiff_t _current_active;
	::std::chrono::time_point<::std::chrono::system_clock> startTime = ::std::chrono::system_clock::now();

    Counter() noexcept(true)
    {
        ++countOf;
        ++_current_active;
    }

    Counter(const Counter&) noexcept(true)
    {
        ++countOf;
        ++_current_active;
    }

    Counter(Counter&&) noexcept(true)
    {
        ++countOf;
        ++_current_active;
    }

    virtual ~Counter() noexcept(false)
    {
        --_current_active;
    }
};

template<typename T> ::std::atomic_size_t Counter<T>::countOf(0ull);
template<typename T> ::std::atomic_ptrdiff_t Counter<T>::_current_active(0ull);

class Parser : public Counter<Parser>
{
public:
    enum class filetype : unsigned
    {
        none = 0,
        localfile,
        remotefile
    };
    filetype type;
    ::std::string ptcl;
    ::std::string path;
    ::std::string filename;
    ::std::string str;

    Parser(filetype getDepend = Parser::filetype::none) noexcept(true) : Counter<Parser>(), type(getDepend) {}

    Parser(::std::string filepath, filetype getDepend = Parser::filetype::none) noexcept(false)
        : Counter<Parser>(), type(getDepend)
    {
        ::std::unique_lock<::std::mutex> lock(mt_filesys, ::std::defer_lock);
        lock.lock();
        if (filepath.find("://") != ::std::string::npos && !std::filesystem::exists(_PATH_TO_WRITE_TO + filepath.substr(filepath.find("://") + 3)))
        {
            ::std::ifstream read;
            ::std::ofstream file;
            if (type == filetype::none)
            {
                size_t pos = 0ull;
                ptcl = filepath.substr(0ull, (pos = filepath.find("://") + 3));
                filename = filepath.substr(pos);
                if (filename.back() == '/') filename = filename.substr(0ull, filename.size() - 1);
                path = filename.substr(0ull, filename.find_last_of("/") != ::std::string::npos ? (pos = filename.find_last_of("/") + 1) : (pos = 0ull));
                filename.erase(0ull, pos);
                ::std::string temp;
                if (ptcl == "file://")
                {
                    type = filetype::localfile;
                    temp = filename;
                    if (temp.find(".htm") == ::std::string::npos) temp += ".html";
                    read.open(_PATH_TO_READ_FROM + path + temp);
                    ER_IFN(read.is_open(),
                        ::std::cerr << "source_name = " << _PATH_TO_READ_FROM + path + temp << ::std::endl;,
                        throw ::std::runtime_error("cannot open the file to read from"); )
                    file.open(_PATH_TO_WRITE_TO + path + temp);
                    ER_IFN(file.is_open(),
                        ::std::cerr << "source_name = " << _PATH_TO_WRITE_TO + path + temp << ::std::endl;
                        read.close();,
                        throw ::std::runtime_error("cannot open the file to write to"); )
#ifdef _dx_COMMENTS
                    ::std::cout << _PATH_TO_READ_FROM + path + temp << ::std::endl;
                    ::std::cout << "to " << _PATH_TO_WRITE_TO + path + temp << ::std::endl;
#endif
                }
                else if (ptcl == "http://" || ptcl == "https://")
                {
                    type = filetype::remotefile;
                    path += filename + '/';
                    temp = path;
                    ::std::filesystem::create_directories(_PATH_TO_WRITE_TO + temp);
                    temp += filename;
                    file.open(_PATH_TO_WRITE_TO + temp);
                    ER_IFN(file.is_open(),
                        ::std::cerr << "source_name = " << _PATH_TO_WRITE_TO + temp << ::std::endl;,
                        throw ::std::runtime_error("cannot open the file to write to"); )
#ifdef _dx_COMMENTS
                    ::std::cout << "h to " << _PATH_TO_WRITE_TO + temp << ::std::endl;
#endif
                }
                lock.unlock();
            }
            try
            {
                if (type == filetype::localfile)
                {
                    char* buf = new char[_MAX_BUF_SIZE];
                    while(!read.eof()) { read.getline(buf, _MAX_BUF_SIZE); str.append(buf); str.append("\n"); }
                    delete[] buf;
                    read.close();
                    file << str;
                    file.close();
                }
                else if (type == filetype::remotefile)
                {
                    ER_IFN(send_req(filepath.c_str(), &str, false, (ptcl == "https://")),
                        ::std::cerr << "[!ERROR] oops link = " << filepath << ::std::endl;,
                        /*throw ::std::runtime_error("cannot get response!");*/ )
                    file << str;
                    file.close();
                }
                else throw::std::runtime_error("unsupported format!");
            }
            catch(...)
            {
                if (read.is_open()) read.close();
                if (file.is_open()) file.close();
            }
        }
        else
        {
            --countOf;
            lock.unlock();
        }
    }
    
    Parser(const Parser& inst) noexcept(true) : Counter<Parser>(inst), type(inst.type), ptcl(inst.ptcl), path(inst.path), str(inst.str) {}
    
    Parser(Parser&& inst) noexcept(true) :
        Counter<Parser>(inst), ptcl(::std::move(inst.ptcl)), path(::std::move(inst.path)), str(::std::move(inst.str)) {}

    void parse()
    {
        auto toUP = [](unsigned char c){ return ::std::toupper(c); };
        size_t pos = 0ull;
        size_t prev_pos = 0ull;
        ::std::vector<::std::thread> vThread;
        ::std::string tag = "a";
        ::std::string property = "href=\"";
        ::std::string temp_str = parse_tag(str, tag, property, pos);
        if (pos == ::std::string::npos)
        {
            std::transform(tag.begin(), tag.end(), tag.begin(), toUP);
            std::transform(property.begin(), property.end(), property.begin(), toUP);
            pos = prev_pos;
            temp_str = parse_tag(str, tag, property, pos);
        }
        while(pos != ::std::string::npos)
        {
            if (temp_str.back() == '/') temp_str = temp_str.substr(0ull, temp_str.size() - 1);
            pos += 9;
            vThread.reserve(vThread.capacity() + 1);
            vThread.emplace_back(::std::thread([this, temp_str]() mutable -> void {
                filetype depend = filetype::none;
                if (temp_str.find("file://") == ::std::string::npos &&
                    temp_str.find("http://") == ::std::string::npos &&
                    temp_str.find("https://") == ::std::string::npos)
                {
                    if (temp_str.front() == '/') temp_str.erase(temp_str.begin());
                    temp_str.insert(0ull, path);
                    temp_str.insert(0ull, ptcl);
                    //depend = type;
                }
                Parser instance(temp_str, depend);
                instance.parse();
            }));
            //vThread.back().detach();
            prev_pos = pos;
            temp_str = parse_tag(str, tag, property, pos);
            if (pos == ::std::string::npos)
            {
                std::transform(tag.begin(), tag.end(), tag.begin(), toUP);
                std::transform(property.begin(), property.end(), property.begin(), toUP);
                pos = prev_pos;
                temp_str = parse_tag(str, tag, property, pos);
            }
        }
        while(vThread.size())
        {
            for(auto it = vThread.begin(); it != vThread.end();)
            {
                if (it->joinable())
                {
                    it->join();
                    it = vThread.erase(it);
                    continue;
                }
                ++it;
            }
        }
    }

    ~Parser() noexcept(false) {}

};

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    Parser test("file://input.html/");
    test.parse();
    //while(test._current_active > 1) {}
    ::std::cout << "time = " << ::std::chrono::duration_cast<::std::chrono::microseconds>(::std::chrono::system_clock::now() - test.startTime).count() << ::std::endl;
    ::std::cout << "count of workers = " << test.countOf << ::std::endl;

	curl_global_cleanup();

    return 0;
}

::std::string parse_tag(const ::std::string& source, const ::std::string& tag, const ::std::string& prop, size_t& start_pos)
{
    if ((start_pos = source.find("<" + tag, start_pos)) != ::std::string::npos)
    {
        size_t border = source.find(">", start_pos);
        if ((start_pos = source.find(prop, start_pos)) != ::std::string::npos && start_pos < border)
        {
            return source.substr(start_pos + prop.size(), source.find("\"", start_pos + prop.size() + 1) - start_pos - prop.size());
        }
    }
    return ::std::string();
}

static size_t writer(char* data, size_t size, size_t nmemb, ::std::string* writerData)
{
	if (writerData == nullptr)	return 0;
	writerData->append(data, size * nmemb);
	return size * nmemb;
}

static size_t fwriter(char* ptr, size_t size, size_t nmemb, ::std::ofstream* stream)
{
    stream->write(ptr, size * nmemb);
    return size * nmemb;
}

bool send_req(const ::std::string& addr, void* response, bool isFileWriter, bool secure_access)
{
    CURLcode res;
    CURL* hcurl = curl_easy_init();

    ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_URL, addr.c_str())) == CURLE_OK,, return false; )
    if (secure_access)
    {
        ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_SSL_VERIFYPEER, 0L)) == CURLE_OK,, return false; )
        ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_SSL_VERIFYHOST, 0L)) == CURLE_OK,, return false; )
    }
    ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_HTTPGET, 1L)) == CURLE_OK,, return false; )
    if (isFileWriter)
    {
        ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, fwriter)) == CURLE_OK,, return false; )
    }
    else
    {
        ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, writer)) == CURLE_OK,, return false; )
    }
    ER_IFN((res = curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, response)) == CURLE_OK,, return false; )
    ER_IFN((res = curl_easy_perform(hcurl)) == CURLE_OK,, return false; )

    curl_easy_cleanup(hcurl);

    return true;
};