#pragma once
#include <iostream>
#include <vector>
#include <cassert>
#include <stack>
#include <queue>
#include <algorithm>
using namespace std;

class SparseGraph
{
private:
    //图信息
    int n, m;        // n为节点个数，m为边的条数
    bool directed;   //是否为有向图
    bool dfsCC(int v,const char* file);
    
public:
    //节点信息
    vector<bool> vised;
    vector<int> degree;//度
    vector<bool> keep;//保留
    vector<int> deposit;//优化时的deposit，因为一次只用一个，所有实际上不用开二维数组，不然浪费空间
    vector<int> pru;
    // vector<int> weight;//求最大流时要求有权重INF
    //图
    vector<vector<int>> g;
    SparseGraph(int n, bool directed);
    ~SparseGraph();
    int V();
    int E();
    void addEdge(int v, int w);
    bool deleteEdge(int v, int w, int bi);
    bool hasEdge(int v, int w);
    void dfs(int v);
    void bfs(int v);
    void print();
    void turn2k_core(int k);
    vector<SparseGraph> overlap_partition();//其实可以放到外面当一个公共函数
    int getCC();
    bool dfsPART(int v,const char *file,vector<int> cut);
    int intersect(int u,int v);//判断两个点的邻域交集个数
    bool is_sidevertex(int u,int k);//判断u是不是strong side vertex，k是kvcc的k
    void sweep(int u,int k);
    stack<int> getorder(int u);//获得non-ascending的顺序
};