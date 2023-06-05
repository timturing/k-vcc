#include "SparseGraph.hpp"

// bool keep[MAXN]={0}; //还留着的
// Node Info[MAXN]; //用来存节点信息，方便索引
// 只能当全局变量，不然栈炸了
SparseGraph::SparseGraph(int n, bool directed)
{
    //完成所有初始化
    this->n = n;
    this->m = 0;
    this->directed = directed;
    degree.resize(n,0);
    vised.resize(n,0);
    keep.resize(n,0);
    for (int i = 0; i < n; ++i)
        g.push_back(vector<int>());
}
SparseGraph::~SparseGraph()
{
    degree.clear();
    vised.clear();
    keep.clear();
    for(int i=0;i<n;++i)
    {
        g.clear();
    }
}
int SparseGraph::V()
{
    return n;
}
int SparseGraph::E()
{
    return m;
}
void SparseGraph::addEdge(int v, int w)
{
    if(hasEdge(v,w))return;
    assert(v >= 0 && v < n);
    assert(w >= 0 && w < n);
    g[v].push_back(w);
    degree[v]++;
    keep[v]=true;
    ++m;
    if (!directed)
    {
        g[w].push_back(v);
        degree[w]++;
        keep[w]=true;
        ++m;
    }
}
bool SparseGraph::hasEdge(int v, int w)
{
    // assert(v >= 0 && v < n);
    // assert(w >= 0 && w < n);
    if(v<0||v>=n)return 0;
    if(w<0||w>=n)return 0;
    for (int i = 0; i < g[v].size(); ++i)
    {
        if (g[v][i] == w)
            return 1;
    }
    return 0;
}
//完成边的删除，返回删除边的条数，bi表示删除两边还是一边，仅无向图可用
bool SparseGraph::deleteEdge(int v, int w, int bi = 1)
{
    //由于每次只删除一条边，所以不会出现索引错乱的现象
    //!删除多边时得小心!!!
    assert(v >= 0 && v < n);
    assert(w >= 0 && w < n);
    int del = false;
    for (int i = 0; i < g[v].size(); ++i)
    {
        if (g[v][i] == w)
        {
            g[v].erase(g[v].begin() + i);
            del = true;
            m--;
            break;
        }
    }
    if (!directed && bi)
    {
        for (int i = 0; i < g[w].size(); ++i)
        {
            if (g[w][i] == v)
            {
                g[w].erase(g[w].begin() + i);
                del = true;
                break;
            }
        }
    }
    return del;
}

void SparseGraph::print()
{
    cout<<"[";
    for(int i=0;i<n;i++)
    {
        if(keep[i])
        {
            for(int j=0;j<g[i].size();j++)
            {
                cout<<"("<<i<<','<<g[i][j]<<")"<<',';
            }
        }
    }
    cout<<"]";
}
void SparseGraph::dfs(int v)
{
    vector<bool> visited(n, false);
    stack<int> s;
    s.push(v);
    while (!s.empty())
    {
        int tmp = s.top();
        if (!visited[tmp])
            cout << tmp << " ";
        visited[tmp] = true;
        s.pop();
        int size = g[tmp].size();
        for (int i = 0; i < size; ++i)
        {
            int b = g[tmp][i];
            if (!visited[b])
                s.push(b);
        }
    }
    cout << endl;
}
void SparseGraph::bfs(int v)
{
    vector<bool> visited(n, false);
    queue<int> que;
    que.push(v);
    while (!que.empty())
    {
        int tmp = que.front();
        if (!visited[tmp])
            cout << tmp << " ";
        visited[tmp] = true;
        que.pop();
        int size = g[tmp].size();
        for (int i = 0; i < size; ++i)
        {
            int b = g[tmp][i];
            if (!visited[b])
                que.push(b);
        }
    }
    cout << endl;
}
void SparseGraph::turn2k_core(int k)
{
    bool flag=1;
    while(flag)
    {
        flag=0;
        for (int i = 0; i < n; i++)
        {
            if (degree[i] < k&&keep[i])
            {
                flag=1;
                keep[i]=0;
                for (int j = 0; j < g[i].size(); j++)
                {
                    deleteEdge(g[i][j], i, 0);
                    degree[g[i][j]]--;//!配套操作可以考虑放一起
                    if(degree[g[i][j]]==0)keep[g[i][j]]=0;
                }
                g[i].clear();
                degree[i]=0;
            }
        }
    }
}
//从点v开始勘探，将v所在的连通分支返回出一个子图，由于内存会炸，只能先写入内存
bool SparseGraph::dfsCC(int v,const char *file)
{
    if(vised[v])return false;
    if(!keep[v])return false;
    if(g[v].empty())return false;

    int actn=0,actm=0;
    FILE *fp = NULL;
    fp = fopen(file, "w+");
    fprintf(fp,"%10d %10d\n",n,m);
    
    stack<int> s;
    s.push(v);
    while (!s.empty())
    {
        int tmp = s.top();
        vised[tmp] = 1;
        actn=max(actn,tmp);
        s.pop();
        int size = g[tmp].size();
        for (int i = 0; i < size; ++i)
        {
            int b = g[tmp][i];
            if (!vised[b])
            {
                actm++;
                fprintf(fp,"%10d %10d\n",tmp,b);
                s.push(b);
            }
        }
    }
    fflush(fp);
    fclose(fp);

    //更新num
    FILE *nfp = NULL;
    nfp = fopen(file, "r+");
    fprintf(nfp,"%10d %10d\n",actn+1,actm);
    fflush(nfp);
    return true;
}
//获得连通分支，每个int都是连通分支内的一个点
int SparseGraph::getCC()
{
    //https://blog.csdn.net/qq_43015237/article/details/105174039
    //初始化
    for (int i = 0; i < n; i++)
    {
        vised[i]=0;
    }
    int ccnum=1;
    string filename="./tempcc/cc";
    for (int i = 0; i < n; i++)
    {
        if (!vised[i]&&keep[i])
        {
            string newfile=filename+to_string(ccnum)+".txt";
            ccnum++;
            //如果写入失败则重试
            while(!dfsCC(i,newfile.c_str()))
            {
                i++;
                if(i>=n)break;
            }
        }
    }
    return ccnum;
}
bool invector(int a,vector<int> b)
{
    for(int i=0;i<b.size();i++)
    {
        if(a==b[i])return true;
    }
    return false;
}
bool SparseGraph::dfsPART(int v,const char *file,vector<int> cut)
{
    if(vised[v])return false;
    if(!keep[v])return false;
    if(g[v].empty())return false;

    int actn=0,actm=0;
    FILE *fp = NULL;
    fp = fopen(file, "w+");
    fprintf(fp,"%10d %10d\n",n,m);
    
    for(int i=0;i<cut.size();i++)
    {
        for(int j=i+1;j<cut.size();j++)
        {
            if(hasEdge(cut[i],cut[j]))
            {
                actm++;
                fprintf(fp,"%10d %10d\n",cut[i],cut[j]);
            }
        }
        actn=max(actn,cut[i]);
    }

    stack<int> s;
    s.push(v);
    while (!s.empty())
    {
        int tmp = s.top();
        if(vised[tmp])
        {
            s.pop();
            continue;
        }
        vised[tmp] = 1;
        actn=max(actn,tmp);
        s.pop();
        int size = g[tmp].size();
        for (int i = 0; i < size; ++i)
        {
            int b = g[tmp][i];
            if (!vised[b])
            {
                actm++;
                fprintf(fp,"%10d %10d\n",tmp,b);
                s.push(b);
            }
            if(invector(b,cut))
            {
                actm++;
                fprintf(fp,"%10d %10d\n",tmp,b);
            }
        }
    }
    fflush(fp);
    fclose(fp);

    //更新num
    FILE *nfp = NULL;
    nfp = fopen(file, "r+");
    fprintf(nfp,"%10d %10d\n",actn+1,actm);
    fflush(nfp);
    return true;
}
int SparseGraph::intersect(int u,int v)
{
    int cnt=0;
    for(int i=0;i<g[u].size();i++)
    {
        int nu=g[u][i];
        for(int j=0;j<g[v].size();j++)
        {
            int nv=g[v][j];
            if(nu==nv)
            {
                cnt++;
            }
        }
    }
    return cnt;
}
bool SparseGraph::is_sidevertex(int u,int k)
{
    for(int i=0;i<g[u].size();i++)
    {
        for(int j=i+1;j<g[u].size();j++)
        {
            int v1=g[u][i];
            int v2=g[u][j];
            if(!hasEdge(v1,v2) && (intersect(v1,v2)<k))
            {
                return false;
            }
        }
    }
    return true;
}
void SparseGraph::sweep(int u,int k)
{
    pru[u]=true;
    for(int i=0;i<g[u].size();i++)
    {
        int w=g[u][i];
        if(!pru[w])
        {
            deposit[w]++;
            if(is_sidevertex(u,k)||deposit[w]>=k)
            {
                sweep(w,k);
            }
        }
    }
}

stack<int> SparseGraph::getorder(int u)
{
    stack<int> res;
    vector<bool> visited(n, false);
    queue<int> que;
    que.push(u);

    while (!que.empty())
    {
        int tmp = que.front();
        visited[tmp] = true;
        que.pop();
        int size = g[tmp].size();
        for (int i = 0; i < size; ++i)
        {
            int b = g[tmp][i];
            if (!visited[b])
            {
                res.push(b);
                que.push(b);
            }
        }
    }
    return res;
}
