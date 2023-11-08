#include <iostream>
#include <vector>
#include <deque>

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

class Graph
{
public:
	::std::vector<::std::vector<size_t>> vData;

	Graph(size_t n) noexcept(true) : vData(n, ::std::vector<size_t>())
	{

	}

	inline auto operator[](size_t v) noexcept(false) -> decltype(auto)
	{
		return vData[v];
	}

};

void search4blocks(Graph g, size_t start_v);

int main()
{
	size_t n;
	::std::cout << "Enter vertices count: ";
	::std::cin >> n;
	Graph G(n);
	::std::cout << "Enter edges count: ";
	::std::cin >> n;
	::std::cout << "Enter adjacent vertices in pairs\n";
	for (size_t i{ 0 }; i < n; ++i)
	{
		size_t v1, v2;
		::std::cin >> v1 >> v2;
		--v1;
		--v2;
		G[v1].reserve(G[v1].capacity() + 1);
		G[v2].reserve(G[v2].capacity() + 1);
		G[v1].emplace_back(v2);
		G[v2].emplace_back(v1);
	}
	search4blocks(G, 0);

	return 0;
}

void search4blocks(Graph g, size_t start_v)
{
	ER_IF(!g.vData.size() || start_v >= g.vData.size(), , return;)

	::std::deque<size_t> deq;
	::std::vector<unsigned> is_visited(g.vData.size(), 0u);
	::std::vector<unsigned> n_childs(g.vData.size(), 0u);
	// <in, min>, <id_parent>
	::std::vector<::std::pair<::std::pair<unsigned, unsigned>, ptrdiff_t>> info(g.vData.size(), ::std::make_pair(::std::make_pair(0u, 0u), -1ll));
	deq.push_front(start_v);

	unsigned n = 0ull;
	while(!deq.empty())
	{
		size_t cur = deq.front();
		deq.pop_front();
		if (!is_visited[cur])
		{
			is_visited[cur] = 1;
			info[cur].first.first = ++n;
			for (auto& x : g[cur])
			{
				if (!is_visited[x])
				{
					deq.push_front(x);
					if (info[x].second != -1ll)
					{
						--n_childs[info[x].second];
					}
					info[x].second = cur;
					++n_childs[cur];
				}
			}
		}
	}

	while (!deq.empty())
	{
		size_t cur = deq.front();
		deq.pop_front();
		if (!is_visited[cur])
		{
			is_visited[cur] = 1;
			info[cur].first.first = ++n;
			info[cur].first.second = n;
			ptrdiff_t parent = info[cur].second;
			if (parent != -1)	info[parent].first.second = ::std::min(info[parent].first.second, info[cur].first.second);
			for (auto& x : g[cur])
			{
				if (!is_visited[x])
				{
					deq.push_front(x);
					if (info[x].second != -1ll)
					{
						--n_childs[info[x].second];
					}
					info[x].second = cur;
					info[x].first.second = info[cur].first.first;
					++n_childs[cur];
				}
				else if (parent == x) info[cur].first.second = ::std::min(info[cur].first.second, info[x].first.first);
			}
		}
	}

	size_t _idx = 0ull;
	for (auto& x : info)
	{
		::std::cout << _idx << '\n';
		::std::cout << "\ttin = " << x.first.first << "\n\tup = " << x.first.second <<
			"\n\tidx_parent = " << x.second << "\n\tChilds = " << n_childs[_idx] << '\n';
		++_idx;
	}

	n = 1ull;
	::std::deque<size_t> res;
	deq.push_front(start_v);
	while (!deq.empty())
	{
		size_t cur = deq.front();
		deq.pop_front();
		res.push_front(cur);
		if (is_visited[cur])
		{
			is_visited[cur] = 0;
			for (auto& x : g[cur])
			{
				if (is_visited[x])
				{
					if (info[x].first.second == info[cur].first.first && cur == info[x].second &&
						(info[cur].second != -1ll || n_childs[cur] >= 2u))
					{
						::std::cout << "Block #" << n++ << "\n\tVertices: ";
						size_t vert;
						while (!res.empty())
						{
							vert = res.back();
							res.pop_back();
							::std::cout << vert + 1 << ' ';
						}
						::std::cout << '\n';
					}
					deq.push_front(x);
				}
			}
		}
	}
}