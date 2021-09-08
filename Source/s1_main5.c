#include <stdio.h>
#include <malloc.h>

#ifndef ER_IF
#define ER_IF(x, Exc)   if ( (x) ) { Exc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, Exc)  if ( !(x) ) { Exc }
#endif

typedef struct _pairEdge
{
    int vertex;
    int weight;
} PairEdge, *pPairEdge;

typedef struct ListNode_
{
    void*               data;
    struct ListNode_*   next;
} ListNode, *pListNode;

void searchDijkstra(int, pListNode, size_t, pPairEdge);
void heapify(pPairEdge*, int, int, int (*)(int, int));
pListNode insertData2ListNode(pListNode, void*);
pListNode findListNode(pListNode, size_t);
pListNode eraseList(pListNode);
void _swap(pPairEdge*, pPairEdge*);
int _lesser(int, int);
int _greater(int, int);

int main()
{
    FILE* file = NULL;
    int k;
    size_t n, m;

    file = fopen("input.txt", "r");
    ER_IFN(file, return 1; )
#if defined(__unix__)
#if defined(__linux__)
    fscanf(file, "%d %zu %zu", &k, &n, &m);
#endif
#elif defined(_WIN32)
    fscanf(file, "%d %I64d %I64d", &k, &n, &m);
#else
    #error This operating system is not supported
#endif

    int* vtxVals = (int*) malloc(n * sizeof(int));
    ER_IFN(vtxVals, return 1; )
    pPairEdge res = (pPairEdge) malloc(n * sizeof(PairEdge));
    ER_IFN(res, free(vtxVals); return 1; )
    pListNode edges = NULL;

    res[0].vertex = 0;
    res[0].weight = 0;
    edges = insertData2ListNode(edges, NULL);
    for(size_t i = 1ull; i < n; ++i)
    {
        res[i].vertex = i;
        res[i].weight = __INT_MAX__;
        edges = insertData2ListNode(edges, NULL);
    }
    for(size_t i = 0ull; i < m; ++i)
    {
        int tmp;
        size_t from, to;
#if defined(__unix__)
#if defined(__linux__)
        fscanf(file, "%zu %zu %d", &from, &to, &tmp);
#endif
#elif defined(_WIN32)
        fscanf(file, "%I64d %I64d %d", &from, &to, &tmp);
#else
    #error This operating system is not supported
#endif
        pPairEdge tmp_data = (pPairEdge) malloc(sizeof(PairEdge));
        ER_IFN(tmp_data,
            free(vtxVals);
            free(res);
            for(size_t i = 0ull; i < n; ++i)
            {
                findListNode(edges, i)->data = eraseList((pListNode)findListNode(edges, i)->data);
            }
            edges = eraseList(edges);
            return 1; )
        tmp_data->vertex = to;
        tmp_data->weight = tmp;
        findListNode(edges, from)->data = insertData2ListNode((pListNode)findListNode(edges, from)->data, tmp_data);
    }
    for(size_t i = 0ull; i < n; ++i)
    {
        int tmp;
        fscanf(file, "%d", &tmp);
        vtxVals[i] = tmp;
    }

    int result = 0;
    searchDijkstra(0, edges, n, res);
    for(size_t i = 0ull; i < n; ++i)
    {
        if (res[i].weight <= k)
        {
            if (vtxVals[res[i].vertex] > result)    result = vtxVals[res[i].vertex];
        }
    }
    fclose(file);
    file = fopen("output.txt", "w");
    ER_IFN(file,
        free(vtxVals);
        free(res);
        for(size_t i = 0ull; i < n; ++i)
        {
            findListNode(edges, i)->data = eraseList((pListNode)findListNode(edges, i)->data);
        }
        edges = eraseList(edges);
        return 1; )
    fprintf(file, "%d", result);
    fclose(file);

    free(vtxVals);
    free(res);
    for(size_t i = 0ull; i < n; ++i)
    {
        findListNode(edges, i)->data = eraseList((pListNode)findListNode(edges, i)->data);
    }
    edges = eraseList(edges);

    return 0;
}

void searchDijkstra(int from, pListNode edges, size_t n, pPairEdge ares)
{
    ER_IFN(edges, return; )
    ER_IFN(n > 0, return; )

    pPairEdge* pares = (pPairEdge*) malloc(n * sizeof(pPairEdge));
    ER_IFN(pares, return; )
    for(int i = 0; i < n; ++i)
    {
        pares[i] = &ares[i];
    }
    int shift = 0;
    for (int i = n / 2 - 1; i >= 0; --i)
    {
        heapify(pares, n, i, &_lesser);
    }
    while(shift < n - 1)
    {
        size_t i = 0ull;
        pListNode cur = findListNode((pListNode)findListNode(edges, from)->data, i++);
        while(cur && (pPairEdge)cur->data)
        {
            int tmp = ((pPairEdge)cur->data)->vertex;
            int tmp_sum = ares[from].weight + ((pPairEdge)cur->data)->weight;
            if (ares[tmp].weight > tmp_sum) ares[tmp].weight = tmp_sum;
            cur = findListNode((pListNode)findListNode(edges, from)->data, i++);
        }

        _swap(pares, pares + n - shift - 1);
        ++shift;
        heapify(pares, (n - shift), 0, &_lesser);
        from = (*pares)->vertex;
    }
    free(pares);
}

void heapify(pPairEdge* arr, int n, int i, int (*cmp)(int, int))
{
    if (n <= 1) return;
    int largest;
    int left;
    int right;
    
    largest = i;

    do
    {
        _swap(&arr[i], &arr[largest]);

        i = largest;
        left = 2 * i + 1;
        right = 2 * i + 2;

        if (left < n)
        {
            if (cmp(arr[left]->weight, arr[largest]->weight)) largest = left;
        }
        if (right < n)
        {
            if (cmp(arr[right]->weight, arr[largest]->weight)) largest = right;
        }
    } while(largest != i);
}

pListNode insertData2ListNode(pListNode root, void* data)
{
    if (root == NULL)
    {
        root = (pListNode) malloc(sizeof(ListNode));
        ER_IFN(root, return NULL; )
        root->data = data;
        root->next = NULL;
        return root;
    }
    root->next = insertData2ListNode(root->next, data);
    return root;
}

pListNode findListNode(pListNode root, size_t n)
{
    if (root == NULL) return NULL;
    if (n > 0) return findListNode(root->next, n - 1);
    return root;
}

pListNode eraseList(pListNode root)
{
    if (root == NULL) return root;
    if (root->next) root->next = eraseList(root->next);
    free(root->data);
    free(root);
    root = NULL;
    return root;
}

void _swap(pPairEdge* a, pPairEdge* b)
{
    pPairEdge c = *a;
    *a = *b;
    *b = c;
}

inline int _lesser(int a, int b)
{
    return a < b;
}

inline int _greater(int a, int b)
{
    return a > b;
}
