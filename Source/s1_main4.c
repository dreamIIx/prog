#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#ifndef ER_IF
#define ER_IF(x, Exc)   if ( (x) ) { Exc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, Exc)  if ( !(x) ) { Exc }
#endif

#define szSTR_BUF   (256 * sizeof(char))
#define kBTREE      (3)
#define LOW_BOUND_kBTREE (3)
#if (kBTREE < LOW_BOUND_kBTREE)
#error "the lower limit of <kBTREE> is violated"
#endif

typedef struct TreeNode_
{
    char*               value;
    struct TreeNode_*   right;
    struct TreeNode_*   left;
} TreeNode, *pTreeNode;

typedef struct ListNode_
{
    void*               data;
    struct ListNode_*   next;
} ListNode, *pListNode;

typedef struct BTreeNode_
{
    size_t      size;
    pListNode    vals;
    pListNode    elem;
} BTreeNode, *pBTreeNode;

void __fscanf_str(FILE*, char**, size_t*);
pTreeNode insertTreeNode(pTreeNode, char*, size_t);
pTreeNode seekMostLeftAndRemove(pTreeNode);
pTreeNode findTreeNode(pTreeNode, char*);
pTreeNode removeTreeNode(pTreeNode, char*);
pTreeNode eraseTree(pTreeNode);
int sizeTree(pTreeNode);
void printTree(pTreeNode);
void printTreeLevel(FILE*, pTreeNode, int);
pBTreeNode insertBTreeNode(pBTreeNode, void*, size_t);
pBTreeNode removeBTreeElem(pBTreeNode, void*, size_t);
pListNode findBTreeStrElem(pBTreeNode, char*);
pBTreeNode eraseBTree(pBTreeNode);
void printStrBTree(pBTreeNode);
pListNode copyData2ListNode(pListNode, void*, size_t);
pListNode insertData2ListNode(pListNode, void*);
pListNode insertListNode(pListNode, pListNode);
pListNode copyData2ListNodeTo(pListNode, size_t, void*, size_t);
pListNode insertData2ListNodeTo(pListNode, size_t, void*);
pListNode insertListNodeTo(pListNode, size_t, pListNode);
pListNode findListNode(pListNode, size_t);
pListNode excludeListNode(pListNode, size_t);
pListNode removeListNodeByN(pListNode, size_t);
pListNode removeListNodeByVal(pListNode, void*);
pListNode eraseList(pListNode);
void printStrList(pListNode);

int main()
{
    FILE* filetag;
    size_t szStr = szSTR_BUF;
    char* str = (char*) malloc(szStr);
    ER_IFN(str, return 1; )
    pTreeNode mainroot = NULL;
    pBTreeNode btreeroot = NULL;

    filetag = fopen("input.txt", "r");
    __fscanf_str(filetag, &str, &szStr);
    __fscanf_str(filetag, &str, &szStr);
    while(strncmp(str, "DELETE:", 7))
    {
        strtok(str, ".");
        mainroot = insertTreeNode(mainroot, str, szStr);
        btreeroot = insertBTreeNode(btreeroot, str, szStr);
        printStrBTree(btreeroot);
        printf("\n");
        szStr = szSTR_BUF;
        __fscanf_str(filetag, &str, &szStr);
    }
    szStr = szSTR_BUF;
    __fscanf_str(filetag, &str, &szStr);
    while(strncmp(str, "LEVEL:", 6))
    {
        strtok(str, ".");
        mainroot = removeTreeNode(mainroot, str);
        szStr = szSTR_BUF;
        __fscanf_str(filetag, &str, &szStr);
    }
    int k_level = -1;
    fscanf(filetag, "%d", &k_level);
    fclose(filetag);

    filetag = fopen("output.txt", "w");
    fprintf(filetag, "%d\n", sizeTree(mainroot));
    printTreeLevel(filetag, mainroot, k_level);
    fclose(filetag);

    pListNode tempFoundStr = findBTreeStrElem(btreeroot, "dlkfwe1pk");
    if (tempFoundStr) printf("\nfind1: %s\n", (char*)tempFoundStr->data);

    btreeroot = removeBTreeElem(btreeroot, (char*)"qwerty", szSTR_BUF);
    printf("remove:\n");
    printStrBTree(btreeroot);
    printf("\n");

    btreeroot = removeBTreeElem(btreeroot, (char*)"hello", szSTR_BUF);
    printf("remove:\n");
    printStrBTree(btreeroot);
    printf("\n");

    free(str);
    eraseTree(mainroot);
    printf("---------------------\n");
    eraseBTree(btreeroot);

    return 0;
}

void __fscanf_str(FILE* file, char** buf, size_t* sz)
{
    ER_IFN(buf, return; )
    ER_IFN(*buf, return; )
    ER_IF(*sz <= 0, return; )
    char c;
    size_t i = 0ull;
    c = (char) fgetc(file);
    while(c && c != ' ' && c != '\n')
    {
        (*buf)[i++] = c;
        if (i >= *sz)
        {
            *sz += szSTR_BUF;
            char* tmp = (char*) realloc(*buf, *sz);
            ER_IFN(tmp, *sz -= szSTR_BUF; return; )
            *buf = tmp;
            tmp = NULL;
        }
        c = (char) fgetc(file);
    }
    (*buf)[i] = 0;
}

pTreeNode insertTreeNode(pTreeNode root, char* str, size_t size)
{
    if (root == NULL)
    {
        root = (pTreeNode) malloc(sizeof(TreeNode));
        ER_IFN(root, return NULL; )
        root->value = (char*) malloc(size);
        strcpy(root->value, str);
        root->right = NULL;
        root->left = NULL;
        return root;
    }
    if (strcmp(root->value, str) > 0)
    {
        root->left = insertTreeNode(root->left, str, size);
        return root;
    }
    if (strcmp(root->value, str) < 0)
    {
        root->right = insertTreeNode(root->right, str, size);
        return root;
    }
    return root;
}

pTreeNode seekMostLeftAndRemove(pTreeNode node)
{
    if (node == NULL || node->left == NULL) return node;
    pTreeNode res = seekMostLeftAndRemove(node->left);
    assert(res);
    if (node->left == res)   node->left = res->right;
    return res;
}

pTreeNode findTreeNode(pTreeNode root, char* str)
{
    int res = 0;
    if (root == NULL || !(res = strcmp(root->value, str))) return root;
    if (res > 0) return findTreeNode(root->left, str);
    return findTreeNode(root->right, str);
}

pTreeNode removeTreeNode(pTreeNode root, char* str)
{
    if (root == NULL) return root;
    if (strcmp(root->value, str) > 0)
    {
        root->left = removeTreeNode(root->left, str);
    }
    else if (strcmp(root->value, str) < 0)
    {
        root->right = removeTreeNode(root->right, str);
    }
    else
    {
        if (root->left == NULL && root->right == NULL)
        {
            free(root->value);
            free(root);
            return NULL;
        }
        else if (root->left == NULL)
        {
            pTreeNode res = root->right;
            free(root->value);
            free(root);
            return res;
        }
        else if (root->right == NULL)
        {
            pTreeNode res = root->left;
            free(root->value);
            free(root);
            return res;
        }
        else
        {
            pTreeNode mostLeft = seekMostLeftAndRemove(root->right);
            if (mostLeft == root->right)
            {
                root->right = mostLeft->right;
            }
            strcpy(root->value, mostLeft->value);
            free(mostLeft->value);
            free(mostLeft);
            return root;
            /*mostLeft->left = root->left;
            free(root->value);
            free(root);
            return mostLeft;*/
        }
    }
    return root;
}

pTreeNode eraseTree(pTreeNode root)
{
    if (root == NULL) return root;
    if (root->right)
    {
        root->right = eraseTree(root->right);
    }
    if (root->left)
    {
        root->left = eraseTree(root->left);
    }
    free(root->value);
    free(root);
    return root;
}

int sizeTree(pTreeNode root)
{
    if (root == NULL) return 0;
    return 1 + ((root->left) ? (sizeTree(root->left)) : 0) + ((root->right) ? (sizeTree(root->right)) : 0);
}

void printTree(pTreeNode root)
{
    if (!root)
    {
        printf("x");
        return;
    }
    printf("(%s", root->value);
    printTree(root->left);
    printTree(root->right);
    printf(")");
}

void printTreeLevel(FILE* stream, pTreeNode root, int level)
{
    if (root == NULL) return;
    if (!level) fprintf(stream, "%s ", root->value);
    else
    {
        if (root->left)     printTreeLevel(stream, root->left, level - 1);
        if (root->right)    printTreeLevel(stream, root->right, level - 1);
    }
}

pBTreeNode insertBTreeNode(pBTreeNode root, void* str, size_t size)
{
    if (root == NULL)
    {
        root = (pBTreeNode) malloc(sizeof(BTreeNode));
        ER_IFN(root, return NULL; )
        root->vals = NULL;
        root->vals = copyData2ListNode(root->vals, str, size);
        root->elem = NULL;
        root->size = 1ul;
        return root;
    }
    if (root->size == 2 * kBTREE - 1)
    {
        pBTreeNode new_root = (pBTreeNode) malloc(sizeof(BTreeNode));
        ER_IFN(new_root, return root; )
        pBTreeNode right_branch = (pBTreeNode) malloc(sizeof(BTreeNode));
        ER_IFN(right_branch, free(new_root); return root; )

        // root will be as left_branch
        right_branch->vals = NULL;
        right_branch->elem = NULL;
        right_branch->vals = findListNode(root->vals, kBTREE);
        findListNode(root->vals, kBTREE - 1)->next = NULL;
        if (root->elem)
        {
            right_branch->elem = findListNode(root->elem, kBTREE);
            findListNode(root->elem, kBTREE - 1)->next = NULL;
        }
        right_branch->size = kBTREE - 1;

        new_root->vals = NULL;
        new_root->vals = findListNode(root->vals, kBTREE - 1);
        new_root->size = 1ul;
        findListNode(root->vals, kBTREE - 2)->next = NULL;
        root->size = kBTREE - 1;

        new_root->elem = NULL;
        new_root->elem = insertData2ListNode(new_root->elem, root);
        new_root->elem = insertData2ListNode(new_root->elem, right_branch); 
        root = new_root;
    }
    pListNode curnode = NULL;
    size_t i = 0ul;
    int res = 0;
    while((curnode = findListNode(root->vals, i)))
    {
        if (!(res = strcmp((char*)curnode->data, (char*)str))) return root;
        if (res > 0)  break;
        ++i;
    }
    pBTreeNode seek = NULL;
    if ((curnode = findListNode(root->elem, i)))
    {
        seek = insertBTreeNode((pBTreeNode)curnode->data, str, size);
        if (seek->size == 1ul)
        {
            root->elem = excludeListNode(root->elem, i);
            root->vals = insertData2ListNodeTo(root->vals, i, seek->vals->data);
            root->elem = insertListNodeTo(root->elem, i, seek->elem->next);
            root->elem = insertListNodeTo(root->elem, i, seek->elem);
            ++root->size;
        }
        return root;
    }
    root->vals = copyData2ListNodeTo(root->vals, i, str, size);
    ++root->size;
    return root;
}



pBTreeNode removeBTreeElem(pBTreeNode root, void* str, size_t size)
{
    if (root == NULL) return root;
    pListNode curnode = NULL;
    ptrdiff_t i = 0ul;
    int res = 0;
    while((curnode = findListNode(root->vals, i)))
    {
        if (!(res = strcmp((char*)curnode->data, (char*)str)))
        {
            root->vals = removeListNodeByN(root->vals, i);
            --root->size;
            break;
        }
        if (res > 0)  break;
        ++i;
    }
    if ((curnode = findListNode(root->elem, i)))
    {
        if (!res)
        {
            pListNode temp = NULL;
            temp = findListNode(((pBTreeNode)curnode->data)->vals, ((pBTreeNode)curnode->data)->size - 1);
            root->vals = copyData2ListNodeTo(root->vals, i, temp->data, size);
            ++root->size;
            str = (char*)temp->data;
        }
        printf("\n%zu\n", ((pBTreeNode)curnode->data)->size);
        printStrBTree(root);
        if (((pBTreeNode)curnode->data)->size <= kBTREE - 1)
        {
            ptrdiff_t neighbour = 0ul;
            if (i > 0)  neighbour = i - 1;
            else        neighbour = i + 1;
            if (((pBTreeNode)curnode->data)->size + ((pBTreeNode)findListNode(root->elem, neighbour)->data)->size > 2 * kBTREE - 1)
            {
                if (i == 0)  neighbour = i;
                pListNode temp2 = findListNode(root->vals, neighbour);
                if (neighbour)      findListNode(root->vals, neighbour - 1)->next = temp2->next;
                else                root->vals = temp2->next;
                if (i)  ((pBTreeNode)curnode->data)->vals = insertListNodeTo(((pBTreeNode)curnode->data)->vals, 0, temp2);
                else    ((pBTreeNode)curnode->data)->vals = insertListNodeTo(((pBTreeNode)curnode->data)->vals, ((pBTreeNode)curnode->data)->size, temp2);
                ++((pBTreeNode)curnode->data)->size;

                if (i == 0)  neighbour = i + 1;
                pListNode curnodeTemp = findListNode(root->elem, neighbour);
                if (((pBTreeNode)curnodeTemp->data)->elem)
                {
                    if (i)  ((pBTreeNode)curnodeTemp->data)->elem = insertListNodeTo(((pBTreeNode)curnodeTemp->data)->elem, 0, findListNode(((pBTreeNode)curnodeTemp->data)->elem, ((pBTreeNode)curnodeTemp->data)->size));
                    else    ((pBTreeNode)curnodeTemp->data)->elem = insertListNodeTo(((pBTreeNode)curnodeTemp->data)->elem, ((pBTreeNode)curnodeTemp->data)->size, findListNode(((pBTreeNode)curnodeTemp->data)->elem, ((pBTreeNode)curnodeTemp->data)->size));
                    findListNode(((pBTreeNode)curnodeTemp->data)->elem, ((pBTreeNode)curnodeTemp->data)->size - 1)->next = NULL;
                }

                if (i == 0)  neighbour = i;
                pListNode temp = NULL;
                if (i)
                {
                    temp = findListNode(((pBTreeNode)curnodeTemp->data)->vals, ((pBTreeNode)curnodeTemp->data)->size - 1);
                    findListNode(((pBTreeNode)curnodeTemp->data)->vals, ((pBTreeNode)curnodeTemp->data)->size - 2)->next = NULL;
                }
                else
                {
                    temp = ((pBTreeNode)curnodeTemp->data)->vals;
                    ((pBTreeNode)curnodeTemp->data)->vals = ((pBTreeNode)curnodeTemp->data)->vals->next;
                }
                root->vals = insertListNodeTo(root->vals, neighbour, temp);
                --((pBTreeNode)curnodeTemp->data)->size;
            }
            else
            {
                if (i == 0)  neighbour = i;
                pListNode temp2 = findListNode(root->vals, neighbour);
                if (neighbour)      findListNode(root->vals, neighbour - 1)->next = temp2->next;
                else                root->vals = temp2->next;

                if (i == 0)  neighbour = i + 1;
                pListNode temp = findListNode(root->elem, neighbour);
                if (i)  ((pBTreeNode)temp->data)->vals = insertListNodeTo(((pBTreeNode)temp->data)->vals, ((pBTreeNode)temp->data)->size, temp2);
                else    ((pBTreeNode)temp->data)->vals = insertListNodeTo(((pBTreeNode)temp->data)->vals, 0, temp2);
                if (((pBTreeNode)temp->data)->elem)
                {
                    findListNode(((pBTreeNode)temp->data)->elem, ((pBTreeNode)temp->data)->size)->next = ((pBTreeNode)curnode->data)->elem;
                }
                if (i)  findListNode(((pBTreeNode)temp->data)->vals, ((pBTreeNode)temp->data)->size)->next = ((pBTreeNode)curnode->data)->vals;
                else    findListNode(((pBTreeNode)curnode->data)->vals, ((pBTreeNode)curnode->data)->size - 1)->next = ((pBTreeNode)temp->data)->vals;
                --root->size;
                ((pBTreeNode)temp->data)->size += ((pBTreeNode)curnode->data)->size + 1;
                if (i)  root->elem = excludeListNode(root->elem, i);
                else    root->elem = excludeListNode(root->elem, neighbour);
                curnode = findListNode(root->elem, i ? i - 1 : 0);
            }
        }
        curnode->data = removeBTreeElem((pBTreeNode)curnode->data, str, szSTR_BUF);
        return root;
    }
    return root;
}

pListNode findBTreeStrElem(pBTreeNode root, char* str)
{
    if (root == NULL) return NULL;
    pListNode curnode = NULL;
    size_t i = 0ul;
    int res = 0;
    while((curnode = findListNode(root->vals, i)))
    {
        if (!(res = strcmp((char*)curnode->data, str))) return curnode;
        if (res > 0) break;
        ++i;
    }
    if ((curnode = findListNode(root->elem, i)))    return findBTreeStrElem((pBTreeNode)curnode->data, str);
    return NULL;
}

void printStrBTree(pBTreeNode root)
{
    if (root == NULL) return;
    size_t i = 0ul;
    pListNode node = NULL;
    pBTreeNode seek = NULL;
    printf("[ ");
    printStrList(root->vals);
    printf("{ ");
    while ((node = findListNode(root->elem, i++)) && (seek = (pBTreeNode)node->data))
    {
        printStrBTree(seek);
    }
    printf("}]");
}

pBTreeNode eraseBTree(pBTreeNode root)
{
    if (root == NULL) return root;
    size_t i = 0ul;
    pListNode node = NULL;
    pBTreeNode seek = NULL;
    while ((node = findListNode(root->elem, i++)) && (seek = (pBTreeNode)node->data))
    {
        seek = eraseBTree(seek);
    }
    eraseList(root->vals);
    free(root);
    root = NULL;
    return root;
}

pListNode copyData2ListNode(pListNode root, void* data, size_t size)
{
    if (root == NULL)
    {
        root = (pListNode) malloc(sizeof(ListNode));
        ER_IFN(root, return NULL; )
        root->data = malloc(size);
        ER_IFN(root->data, free(root); return NULL; )
        strcpy((char*)root->data, (char*)data);
        root->next = NULL;
        return root;
    }
    root->next = copyData2ListNode(root->next, data, size);
    return root;
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

pListNode insertListNode(pListNode root, pListNode node)
{
    if (root == NULL)
    {
        node->next = NULL;
        return root;
    }
    root->next = insertListNode(root->next, node);
    return root;
}

pListNode copyData2ListNodeTo(pListNode root, size_t n, void* data, size_t size)
{
    if (!n)
    {
        pListNode node = (pListNode) malloc(sizeof(ListNode));
        ER_IFN(node, return NULL; )
        node->data = malloc(size);
        ER_IFN(node->data, free(node); return NULL; )
        strcpy((char*)node->data, (char*)data);
        node->next = root;
        return node;
    }
    if (root == NULL) return root;
    root->next = copyData2ListNodeTo(root->next, n - 1, data, size);
    return root;
}

pListNode insertData2ListNodeTo(pListNode root, size_t n, void* data)
{
    if (!n)
    {
        pListNode node = (pListNode) malloc(sizeof(ListNode));
        ER_IFN(node, return NULL; )
        node->data = data;
        node->next = root;
        return node;
    }
    if (root == NULL) return root;
    root->next = insertData2ListNodeTo(root->next, n - 1, data);
    return root;
}

pListNode insertListNodeTo(pListNode root, size_t n, pListNode node)
{
    if (!n)
    {
        node->next = root;
        return node;
    }
    if (root == NULL) return root;
    root->next = insertListNodeTo(root->next, n - 1, node);
    return root;
}

pListNode findListNode(pListNode root, size_t n)
{
    if (root == NULL) return NULL;
    if (n > 0) return findListNode(root->next, n - 1);
    return root;
}

pListNode excludeListNode(pListNode root, size_t n)
{
    if (root == NULL) return root;
    if (n > 0)
    {
        root->next = excludeListNode(root->next, n - 1);
        return root;
    }
    pListNode temp = root->next;
    root->data = NULL;
    root->next = NULL;
    free(root); // to prevent memory leak
    root = NULL;
    return temp;
}

pListNode removeListNodeByN(pListNode root, size_t n)
{
    if (root == NULL) return root;
    if (n > 0)
    {
        root->next = removeListNodeByN(root->next, n - 1);
        return root;
    }
    pListNode temp = root->next;
    free(root->data);
    free(root);
    root = NULL;
    return temp;
}

pListNode removeListNodeByVal(pListNode root, void* value)
{
    if (root == NULL) return root;
    if (root->data != value)
    {
        root->next = removeListNodeByVal(root->next, value);
        return root;
    }
    pListNode temp = root->next;
    free(root->data);
    free(root);
    root = NULL;
    return temp;
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

void printStrList(pListNode root)
{
    if (root == NULL)   return;
    printf("(%s) ", (char*)root->data);
    printStrList(root->next);
}