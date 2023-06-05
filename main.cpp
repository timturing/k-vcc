#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include "SparseGraph.hpp"
#include "maxflow.hpp"
using namespace std;
// #define DEBUG
#define DEBUGLINE cout << "# -------------------------" << endl;

int GetCC(const char *filename, int k);
void OVERLAP_PARTITION(const char *filename,vector<int> s);
int kvccenum(const char *filename, int k);
vector<int> GLOBAL_CUT(const char *filename, int k);
vector<int> GLOBAL_CUT_STAR(const char *filename, int k);//优化后的代码
int kvccnum=1;
int partstart=1,partend=1;
const int MAXN=1e6+1;
bool part_caled[MAXN]={false};

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        cout<<"error:The right format is ./main \"filename\" k(integer)"<<endl;
        return 0;
    }
    const char *filename = argv[1];
    kvccenum(filename, atoi(argv[2]));
    //!最终在这里清除所有temppart
    for(int i=1;i<partend;i++)
    {
        string file="./temppart/"+to_string(i)+".txt";
        remove(file.c_str());
    }
    return 0;
}
int kvccenum(const char *filename, int k)
{
    int ccnum;
    ccnum = GetCC(filename, k);
    for (int i = 1; i < ccnum; i++) //!不需要等于
    {
        string file = "./tempcc/cc" + to_string(i) + ".txt";
        vector<int> res;
        res = GLOBAL_CUT_STAR(file.c_str(), k);
        if(res.empty())
        {
            //!直接把数据mov到最终的kvcc里面
            string newkvcc = "./kvcc/"+to_string(kvccnum)+".txt";
            rename(file.c_str(),newkvcc.c_str());
            kvccnum++;
            cout<<"find a kvcc:"<<newkvcc<<endl;
        }
        else
        {
            //TODO 先OVERLAP_PARTITION
            //!然后全部写到temppart里面
            //!全部循环，到时候进一步递归是先检查temppart有没有数据
            OVERLAP_PARTITION(file.c_str(),res);//里面使用partend计数，和partstart搭配控制区间
            
        }
        //!调用完这里记得去掉cc
        remove(file.c_str());
    }
    int tmp2=partend;
    for(int i=1;i<tmp2;i++)
    {
        //从这调用kvcc理论上是一样的
        if(part_caled[i])continue;
        string partname="./temppart/"+to_string(i)+".txt";
        part_caled[i]=true;
        cout<<"kvcc reading: "<<partname<<endl;
        kvccenum(partname.c_str(),k);
    }
    return 0;
}
int GetCC(const char *filename, int k)
{
    //读入
    if (freopen(filename, "r", stdin) == NULL)
    {
        throw ::runtime_error("can't read the file.");
    }
    cout << "Open file successfully,reading" << filename << endl;
    //点数和边数
    int vertex, edge;
    cin >> vertex >> edge;
    //创建无向图
    SparseGraph *myGraphp = new (SparseGraph)(vertex, false);
    for (int i = 0; i < edge; ++i)
    {
        int from, to;
        cin >> from >> to;
#ifdef DEBUG
        if (from >= vertex || to >= vertex)
        {
            cout << from << ' ' << to << endl;
        }
#endif
        assert(from >= 0 && from < vertex);
        assert(from >= 0 && from < vertex);
        myGraphp->addEdge(from, to);
    }
    cout << "Finish reading." << endl;
    cout << "Step 1:turn to k-core..." << endl;
    myGraphp->turn2k_core(k);
    cout << "Finish Step 1." << endl;
    cout << "Step 2:find connected component..." << endl;
    int ccnum = myGraphp->getCC();
    cout << "Finish Step 2." << endl;
    //清理
    delete myGraphp;
    fclose(stdin);
    cin.clear();
    cin.sync();
    return ccnum;
}

vector<int> GLOBAL_CUT(const char *filename, int k)
{
    //读入
    if (freopen(filename, "r", stdin) == NULL)
    {
        throw ::runtime_error("can't read the file.");
    }
    cout << "Open file successfully,reading" << filename << endl;
    //点数和边数
    int vertex, edge;
    cin >> vertex >> edge;
    //创建无向图
    SparseGraph *myGraphp = new (SparseGraph)(vertex, false);
    for (int i = 0; i < edge; ++i)
    {
        int from, to;
        cin >> from >> to;
#ifdef DEBUG
        if (from >= vertex || to >= vertex)
        {
            cout << from << ' ' << to << endl;
        }
#endif
        assert(from >= 0 && from < vertex);
        assert(from >= 0 && from < vertex);
        myGraphp->addEdge(from, to);
    }
    //清除
    fclose(stdin);
    cin.clear();
    cin.sync();

    //找最小度点
    int mind = 0x3f3f3f3f;
    int u = 0;
    for (int i = 0; i < vertex; i++)
    {
        if (myGraphp->keep[i])
        {
            // cout<<i<<endl;
            if (myGraphp->degree[i] < mind)
            {
                mind = myGraphp->degree[i];
                u = i;
            }
        }
    }
#ifdef DEBUG
    DEBUGLINE
    cout << "minnum degree node:" << u << endl;
    DEBUGLINE
#endif
    vector<int> res;
    //循环判断
    for (int v = 0; v < vertex; v++)
    {
        if (!myGraphp->keep[v])
            continue;
        if (u == v)
            continue;
        //邻域
        bool adj = false;
        for (int i = 0; i < myGraphp->g[u].size(); i++)
        {
            if (myGraphp->g[u][i] == v)
            {
                adj = true;
                break;
            }
        }
        if (adj)
            continue;

        if (freopen(filename, "r", stdin) == NULL)
        {
            throw ::runtime_error("can't read the file.");
        }

#ifdef DEBUG
                DEBUGLINE
                cout << "enter loc-cut" << endl;
                DEBUGLINE
#endif
            res = LOC_CUT(u, v);
#ifdef DEBUG
                DEBUGLINE
                cout << "finish loc-cut" << endl;
                DEBUGLINE
#endif
        fclose(stdin);
        cin.clear();
        cin.sync();
        if (res.size() > 0 && res.size() < k)
        {
#ifdef DEBUG
            DEBUGLINE
            cout << "find a global cut:" << endl;
            for (int i = 0; i < res.size(); i++)
            {
                cout << res[i] << ' ';
            }
            cout << endl;
            DEBUGLINE
#endif
            return res;
        }
    }
#ifdef DEBUG
    DEBUGLINE
    cout<<"First try finished."<<endl;
    DEBUGLINE
#endif


    // 第二个循环，是判断邻域的
    for (int a = 0; a < vertex; a++)
    {
        if (!myGraphp->keep[a])
                continue;
        for (int b = a+1; b < vertex; b++)
        {
            if (!myGraphp->keep[b])
                continue;
            if (a == b)
                continue;
            //邻域
            bool adj = false;
            for (int i = 0; i < myGraphp->g[a].size(); i++)
            {
                if (myGraphp->g[a][i] == b)
                {
                    adj = true;
                    break;
                }
            }
            if (adj)
                continue;

            if (freopen(filename, "r", stdin) == NULL)
            {
                throw ::runtime_error("can't read the file.");
            }
#ifdef DEBUG
                DEBUGLINE
                cout << "enter loc-cut" << endl;
                DEBUGLINE
#endif
            res = LOC_CUT(a, b);
#ifdef DEBUG
                DEBUGLINE
                cout << "finish loc-cut" << endl;
                DEBUGLINE
#endif
            fclose(stdin);
            cin.clear();
            cin.sync();
            if (res.size() > 0 && res.size() < k)
            {
#ifdef DEBUG
                DEBUGLINE
                cout << "find a global cut:" << endl;
                for (int i = 0; i < res.size(); i++)
                {
                    cout << res[i] << ' ';
                }
                cout << endl;
                DEBUGLINE
#endif
                return res;
            }
        }
    }
#ifdef DEBUG
    DEBUGLINE
    cout<<"Second try finished."<<endl;
    DEBUGLINE
#endif

    delete myGraphp;
    return vector<int>();
}


void OVERLAP_PARTITION(const char *filename,vector<int> s)
{
    //读入
    if (freopen(filename, "r", stdin) == NULL)
    {
        throw ::runtime_error("can't read the file.");
    }
    cout << "OVERLAP_PARTITION,reading" << filename << endl;
    //点数和边数
    int vertex, edge;
    cin >> vertex >> edge;
    //创建无向图
    SparseGraph *myGraphp = new (SparseGraph)(vertex, false);
    for (int i = 0; i < edge; ++i)
    {
        int from, to;
        cin >> from >> to;
#ifdef DEBUG
        if (from >= vertex || to >= vertex)
        {
            cout << from << ' ' << to << endl;
        }
#endif
        assert(from >= 0 && from < vertex);
        assert(from >= 0 && from < vertex);
        myGraphp->addEdge(from, to);
    }
    
    //进行dfs以分区，在partend基础上加数
    for (int i = 0; i < vertex; i++)
    {
        myGraphp->vised[i]=0;
    }
    for(int i=0;i<s.size();i++)
    {
        myGraphp->vised[s[i]]=1;
    }
    for (int i = 0; i < vertex; i++)
    {
        if (!myGraphp->vised[i]&&myGraphp->keep[i])
        {
            string newfile="./temppart/"+to_string(partend)+".txt";
            cout<<"Writing to "<<newfile<<endl;
            partend++;
            //如果写入失败则重试
            //TODO 可以在dfsPART函数里面利用hasEdge完成添加
            while(!myGraphp->dfsPART(i,newfile.c_str(),s))
            {
                i++;
                if(i>=vertex)break;
            }
        }
    }

    //清理
    delete myGraphp;
    fclose(stdin);
    cin.clear();
    cin.sync();
}
vector<int> GLOBAL_CUT_STAR(const char *filename, int k)
{
    //读入
    if (freopen(filename, "r", stdin) == NULL)
    {
        throw ::runtime_error("can't read the file.");
    }
    cout << "Open file successfully,reading" << filename << endl;
    //点数和边数
    int vertex, edge;
    cin >> vertex >> edge;
    //创建无向图
    SparseGraph *myGraphp = new (SparseGraph)(vertex, false);
    for (int i = 0; i < edge; ++i)
    {
        int from, to;
        cin >> from >> to;
#ifdef DEBUG
        if (from >= vertex || to >= vertex)
        {
            cout << from << ' ' << to << endl;
        }
#endif
        assert(from >= 0 && from < vertex);
        assert(from >= 0 && from < vertex);
        myGraphp->addEdge(from, to);
    }
    //清除
    fclose(stdin);
    cin.clear();
    cin.sync();

    //找最小度点
    int mind = 0x3f3f3f3f;
    int u = -1;
    for (int i = 0; i < vertex; i++)
    {
        if (myGraphp->keep[i])
        {
            // cout<<i<<endl;
            if (myGraphp->degree[i] < mind)
            {
                mind = myGraphp->degree[i];
                u = i;
            }
        }
    }
#ifdef DEBUG
    DEBUGLINE
    cout << "minnum degree node:" << u << endl;
    DEBUGLINE
#endif
    vector<int> res;
    //执行sweep操作
    myGraphp->deposit.resize(vertex,0);
    myGraphp->pru.resize(vertex,0);
    myGraphp->sweep(u,k);

    

    //循环判断
    stack<int> order=myGraphp->getorder(u);//用一次bfs来获得non-ascending顺序，当然这里也是局部变量开在栈上，要是不够再malloc

    while(!order.empty())
    {
        int v=order.top();
        order.pop();
        if (!myGraphp->keep[v])
            continue;
        if (u == v)
            continue;
        if(myGraphp->pru[v])
            continue;

        //邻域
        bool adj = false;
        for (int i = 0; i < myGraphp->g[u].size(); i++)
        {
            if (myGraphp->g[u][i] == v)
            {
                adj = true;
                break;
            }
        }
        if (adj)
            continue;

        if (freopen(filename, "r", stdin) == NULL)
        {
            throw ::runtime_error("can't read the file.");
        }

#ifdef DEBUG
                DEBUGLINE
                cout << "enter loc-cut" << endl;
                DEBUGLINE
#endif
            res = LOC_CUT(u, v);
#ifdef DEBUG
                DEBUGLINE
                cout << "finish loc-cut" << endl;
                DEBUGLINE
#endif
        fclose(stdin);
        cin.clear();
        cin.sync();
        if (res.size() > 0 && res.size() < k)
        {
#ifdef DEBUG
            DEBUGLINE
            cout << "find a global cut:" << endl;
            for (int i = 0; i < res.size(); i++)
            {
                cout << res[i] << ' ';
            }
            cout << endl;
            DEBUGLINE
#endif
            return res;
        }
        myGraphp->sweep(v,k);
    }
#ifdef DEBUG
    DEBUGLINE
    cout<<"First try finished."<<endl;
    DEBUGLINE
#endif


    // 第二个循环，是判断邻域的
    if(!myGraphp->is_sidevertex(u,k))
    for (int a = 0; a < vertex; a++)
    {
        if (!myGraphp->keep[a])
                continue;
        for (int b = a+1; b < vertex; b++)
        {
            if (!myGraphp->keep[b])
                continue;
            if (a == b)
                continue;
            //邻域
            bool adj = false;
            for (int i = 0; i < myGraphp->g[a].size(); i++)
            {
                if (myGraphp->g[a][i] == b)
                {
                    adj = true;
                    break;
                }
            }
            if (adj)
                continue;

            if (freopen(filename, "r", stdin) == NULL)
            {
                throw ::runtime_error("can't read the file.");
            }
#ifdef DEBUG
                DEBUGLINE
                cout << "enter loc-cut" << endl;
                DEBUGLINE
#endif
            res = LOC_CUT(a, b);
#ifdef DEBUG
                DEBUGLINE
                cout << "finish loc-cut" << endl;
                DEBUGLINE
#endif
            fclose(stdin);
            cin.clear();
            cin.sync();
            if (res.size() > 0 && res.size() < k)
            {
#ifdef DEBUG
                DEBUGLINE
                cout << "find a global cut:" << endl;
                for (int i = 0; i < res.size(); i++)
                {
                    cout << res[i] << ' ';
                }
                cout << endl;
                DEBUGLINE
#endif
                return res;
            }
        }
    }
#ifdef DEBUG
    DEBUGLINE
    cout<<"Second try finished."<<endl;
    DEBUGLINE
#endif

    delete myGraphp;
    return vector<int>();
}