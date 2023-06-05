#ifndef MAXFLOW_H
#define MAXFLOW_H
#include <bits/stdc++.h>
using namespace std;

#define X first
#define Y second
typedef pair<int, int> ii;
typedef pair<int, ii> i2;

const int maxN = 2e3 + 5;
const long long INF_LL = 1e15;
const int MOD = 1e9 + 7;
const int INF = 1e7 + 100;

struct DinicFlow
{
    int n, NumEdge, m;
    vector<int> points, flow, cap, next, head, dist, work;
    vector<ii> e;
    DinicFlow(int _n, int _m)
    {
        n = _n;
        m = _m;
        NumEdge = 0;
        head.assign(n + 7, -1);
        work.assign(n + 7, -1);
        dist.assign(n + 7, -1);
    }

    void addEdge(int u, int v, int c1, int c2)
    {
        e.push_back({u, v});
        // u -> v
        points.push_back(v);
        flow.push_back(0);
        cap.push_back(c1);
        next.push_back(head[u]);
        head[u] = NumEdge++;
        // v -> u
        points.push_back(u);
        flow.push_back(0);
        cap.push_back(c2);
        next.push_back(head[v]);
        head[v] = NumEdge++;
    }

    bool BFS(int s, int t)
    {
        for (int i = 1; i <= n; i++)
            dist[i] = -1;
        queue<int> Q;
        Q.push(s);
        dist[s] = 0;
        while (!Q.empty())
        {
            int u = Q.front();
            Q.pop();
            for (int i = head[u]; i != -1; i = next[i])
            {
                int v = points[i];
                if (dist[v] == -1 && flow[i] < cap[i])
                {
                    dist[v] = dist[u] + 1;
                    if (v == t)
                        return true;
                    Q.push(v);
                }
            }
        }
        return false;
    }

    int DFS(int s, int t, int f)
    {
        if (s == t)
            return f;
        for (int &i = work[s]; i != -1; i = next[i])
        {
            int v = points[i];
            if (dist[v] == dist[s] + 1 && flow[i] < cap[i])
            {
                int delta = DFS(v, t, min(f, cap[i] - flow[i]));
                if (delta != INF)
                {
                    flow[i] += delta;
                    flow[i ^ 1] -= delta;
                    return delta;
                }
            }
        }
        return INF;
    }

    long long maxFlow(int s, int t)
    {
        long long res = 0;
        while (BFS(s, t))
        {
            for (int i = 1; i <= n; i++)
                work[i] = head[i];
            while (true)
            {
                int delta = DFS(s, t, INF);
                if (delta == INF)
                    break;
                res += delta;
            }
        }
        return res;
    }

    void MincutDFS(int s, vector<int> &p)
    {
        p[s] = 1;
        for (int i = head[s]; i != -1; i = next[i])
        {
            int v = points[i];
            if (p[v] == 0 && flow[i] < cap[i])
            {
                MincutDFS(v, p);
            }
        }
    }

    vector<ii> MincutSet(int s, int t)
    {
        maxFlow(s, t);
        vector<int> p(n, 0);
        MincutDFS(s, p);
        vector<ii> S;
        for (int u = 1; u <= n; u++)
        {
            if (p[u] == 0)
                continue;
            for (int i = head[u]; i != -1; i = next[i])
            {
                int v = points[i];
                if (flow[i] == cap[i] && p[v] == 0)
                    S.push_back({u, v});
            }
        }
        return S;
    }
};

vector<int> LOC_CUT(int u, int v)
{
    int n, m;
    cin >> n >> m;
    DinicFlow df(2 * n, m);
    for (int i = 0; i < m; i++)
    {
        int x, y;
        cin >> x >> y;
        df.addEdge(x + n, y, INF, 0);
        df.addEdge(y + n, x, INF, 0);
    }
    for (int i = 0; i < n; i++)
    {
        if (i == u || i == v)
        {
            df.addEdge(i, i + n, INF, 0);
        }
        else
            df.addEdge(i, i + n, 1, 0);
    }

    vector<ii> S = df.MincutSet(u, v);
    // cout << S.size() << '\n';
    vector<int> res;
    for (int i = 0; i < S.size(); i++)
    {
        int temp1,temp2;
        if (S[i].X > n)
            temp1 = S[i].X - n;
        else
            temp1 = S[i].X;

        if (S[i].Y > n)
            temp2 = S[i].Y - n;
        else
            temp2 = S[i].Y;

        if(temp1==temp2)
        {
            res.push_back(temp1);
        }
    }
    return res;
}

#endif