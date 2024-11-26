#pragma once
#pragma once
#pragma once
//
// Created by 裘鸿波 on 2023/3/8.
//
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
#include <map>
#include <queue>
#include<iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <climits>
using namespace std;


#ifndef TEST_GRAPH_H
#define TEST_GRAPH_H

struct PairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        // 使用标准库的哈希函数
        const std::size_t prime1 = 2654435761u;
        const std::size_t prime2 = 2246822519u;
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 * prime1 ^ h2 * prime2;
    }
};

struct PairEqual {
    template <class T1, class T2>
    bool operator () (const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

class Graph {
public:
    vector<vector<int>> neighbor;//指向vector的指针g 被指向的存储对应节点的邻居标号。存储的是节点编号     //节点标号从0开始，边标号从0开始
    unordered_map<pair<int, int>, int, PairHash, PairEqual> edge;//存储边的信息,pair<int int>为一条边的两个端点的标号，map中的另一个参数为边的标号,表示第几条边
    int n, m;  //最大顶点的标号为n，边的数量为m
    int maxsup;
    vector<int> sup, trussness;   //边的支持度和truss
    int maxTrussness;
    vector<vector<int>> Truss_Table;
    vector<pair<int, int>> edge2;//存储边  可以通过edge2[eid]的方式访问，每条边存储一次
    vector<std::unordered_set<int>> neighbor_hash;   //使用哈希表存储邻居，时间复杂度为o（1）
    unordered_map<pair<int, int>, std::unordered_set<int>, PairHash, PairEqual> common_neighbor;
    int vertex_number;


public:
    Graph() {    //构造函数，创建一个图
        //cout << "empty graph constructed" << endl;
        maxsup = 0;
        maxTrussness = 0;
        m = 0;
        n = 0;
    }

    void Init_Graph(char* data_set) // 初始化图中的所有数据  运行该函数后，public中的所有数据将被计算。用数据集初始化图
    {
        BuildGraph(data_set);   //计算neighbor   edge  edge2  n m;

        sup_calculation();   //计算  maxsup  sup数组。

        caculation_Trussness();  //计算出 truss和 truss_table

        //cout << "trussness caculation time:" << std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count() << endl;
        common();


    }

    void Init_Graph(vector<pair<int, int>> edges) // 初始化图中的所有数据  运行该函数后，public中的所有数据将被计算。用数据集初始化图
    {
        BuildGraph(edges);   //计算neighbor   edge  edge2  n m;
        sup_calculation();   //计算  maxsup  sup数组。
        //auto start_SLA = std::chrono::high_resolution_clock::now();
        caculation_Trussness();  //计算出 truss和 truss_table
        //auto end_SLA = std::chrono::high_resolution_clock::now();
        common();
        //cout << "trussness caculation time:" << std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count() << endl;


    }

    void BuildGraph(vector<pair<int, int>> edges)   //用边初始化图
    {
        cout << "reading graph....." << endl;
        int f = 0;

        //计算最大顶点编号为n
        for (f = 0; f < edges.size(); f++)
        {
            int a = edges[f].first;
            int b = edges[f].second;
            if (a > n) n = a;
            if (b > n) n = b;   //n保存节点编号最大值
            ++m;
        }
        f = 0;
        neighbor.resize(n + 1);
        edge2.resize(m);
        neighbor_hash.resize(n + 1);
        m = 0;
        vertex_number = 0;
        //int eid = 0;
        vector<bool> exist(n + 1, false);


        for (f = 0; f < edges.size(); f++)
        {
            int a = edges[f].first;
            int b = edges[f].second;
            if (a == b) continue;  //忽略指向自己的边
            if (a < b)  //令a>b
            {
                int t = a;
                a = b;
                b = t;
            }
            if (edge.find(pair<int, int>(a, b)) == edge.end())  //这条边目前不存在，避免重边
            {
                if (exist[a] == false)
                {
                    vertex_number++;
                    exist[a] = true;
                }
                if (exist[b] == false)
                {
                    vertex_number++;
                    exist[b] = true;
                }

                neighbor_hash[a].insert(b);
                neighbor_hash[b].insert(a);

                neighbor[a].push_back(b);  //将b作为a的邻居存储到a的邻居队列中
                neighbor[b].push_back(a);  //将a作为b的邻居存储到b的邻居队列中
                edge2[m] = pair<int, int>(a, b);
                edge[pair<int, int>(a, b)] = m;   //正反存两遍
                edge[pair<int, int>(b, a)] = m++;
            }
        }

        if (edge2.size() > m)//有些数据集的边会存储两次，导致edge2的size为m的两倍，会出现bug，这行将多余的部分删掉
        {
            edge2.erase(edge2.begin() + m, edge2.end());
        }


        trussness.resize(m);   //初始化truss集合
        sup.resize(m);     //初始化sup集合

        cout << "finishing reading!" << endl;
    }

    void BuildGraph(char* data_set)   //初始化图
    {
        cout << "reading graph....." << endl;
        FILE* f = fopen(data_set, "r");
        int a, b;
        vertex_number = 0;
        //计算最大顶点编号为n
        while (fscanf(f, "%d %d", &a, &b) != EOF) {
            if (a > n) n = a;
            if (b > n) n = b;   //n保存节点编号最大值
            ++m;
        }
        rewind(f);
        neighbor.resize(n + 1);

        neighbor_hash.resize(n + 1);
        edge2.resize(m);
        m = 0;
        //int eid = 0;

        vector<bool> exist(n + 1, false);

        while (fscanf(f, "%d %d", &a, &b) != EOF) {
            if (a == b) continue;  //忽略指向自己的边
            if (a < b)  //令a>b
            {
                int t = a;
                a = b;
                b = t;
            }
            if (edge.find(pair<int, int>(a, b)) == edge.end())  //这条边目前不存在，避免重边
            {
                if (exist[a] == false)
                {
                    vertex_number++;
                    exist[a] = true;
                }

                if (exist[b] == false)
                {
                    vertex_number++;
                    exist[b] = true;
                }

                neighbor_hash[a].insert(b);
                neighbor_hash[b].insert(a);

                neighbor[a].push_back(b);  //将b作为a的邻居存储到a的邻居队列中
                neighbor[b].push_back(a);  //将a作为b的邻居存储到b的邻居队列中
                edge2[m] = pair<int, int>(a, b);
                edge[pair<int, int>(a, b)] = m;   //正反存两遍
                edge[pair<int, int>(b, a)] = m++;
            }
        }
        if (edge2.size() > m)//有些数据集的边会存储两次，导致edge2的size为m的两倍，会出现bug，这行将多余的部分删掉
        {
            edge2.erase(edge2.begin() + m, edge2.end());
        }


        trussness.resize(m);   //初始化truss集合
        sup.resize(m);     //初始化sup集合
        fclose(f);
        cout << "finishing reading!" << endl;
    }

    void sup_calculation()//计算图的支持度，并初始化support最大值  存于maxsup中
    {
        cout << "support is caculating!" << endl;
        for (int i = 0; i <= n; ++i)   //两层循环访问存储有邻居信息的数组  i 表示第几个节点 neighbor[i][j]表示第i个节点的邻居
        {

            for (int j = 0; j < neighbor[i].size(); ++j) {
                int w = neighbor[i][j];  //w表示i的一个邻居
                if (i > w)  //减少重复边遍历
                {
                    int eid = edge[pair<int, int>(i, w)];  //边(i,w)  eid保存边的标号
                    sup[eid] = 0; //将对应边的sup初始化为0
                    //degree(u)>degree(v)
                    int u = neighbor[i].size() > neighbor[w].size() ? i : w;  //比较g[i]与g[w]的规模，将size大的那个值赋给u
                    int v = neighbor[i].size() > neighbor[w].size() ? w : i;  //size小的赋值给  v  此步骤为了减少遍历时间，每次只遍历那个长度小的数组

                    for (int k = 0; k < neighbor[v].size(); ++k)  //寻找公共顶点
                    {
                        if (edge.find(pair<int, int>(u, neighbor[v][k])) != edge.end())
                            //找到公共顶点  g[v][k]表示点v的邻居，那么edge（v,g[v][k]）一定存在，若再找到edge(u,g[v][k])存在，那么g[v][k]就表示是公共点，找到一个三角形。
                            sup[eid]++;//增加对应的support值
                    }
                    if (sup[eid] > maxsup)
                        maxsup = sup[eid];
                }
            }
        }
        cout << "caculating finished!" << endl;
        //return maxsup;
    }

    vector<int> k_truss(int K)   //计算指定k的k-truss  存储边标号
    {
        queue<int> q;  //队列
        vector<int> res;  //结果集，存储结果编号。
        vector<bool> del(m, false);   //标记边是否被删掉，逻辑上删除
        vector<int> supbuffer;
        supbuffer = sup;
        for (int i = 0; i < m; ++i)    //初始化队列，将sup<k-2的边先插入队列
        {
            if (supbuffer[i] < K - 2)
                q.push(i);
        }
        while (q.empty() == false)  //队列不为空，迭代删除支持度不满足的点
        {
            int eid = q.front();  //记录队头元素id
            q.pop();     //出队
            if (del[eid] == true) continue;   //若该边已经被删除，则跳过本轮循环
            del[eid] = true;      //没被删除，则标记为被删除
            int x = edge2[eid].first;    //记录被删除的端点
            int y = edge2[eid].second;
            if (neighbor[x].size() < neighbor[y].size())   //比较两者度数，把邻居数量多的赋给x
            {
                int t = x;
                x = y;
                y = t;
            }
            for (int k = 0; k < neighbor[y].size(); ++k)   //遍历邻居节点寻找受影响的边，只有与被删边构成三角形的边支持度减1
            {
                int w = neighbor[y][k];  //w为y的一个邻居
                int e = edge[pair<int, int>(y, w)];  //e为edge（y,w）的编号，后面用于更新support
                if (del[e] == true) continue;
                if (edge.find(pair<int, int>(x, w)) != edge.end())  //若找到这样的xyw构成三角形 所以其他两边支持度减1
                {
                    e = edge[pair<int, int>(x, w)];
                    if (del[e] == true) continue;
                    supbuffer[e]--;
                    if (supbuffer[e] < K - 2)    //更新后发现support<k-2，则加入队列供下一轮删除
                        q.push(e);

                    e = edge[pair<int, int>(y, w)]; //更新另一条边的支持度
                    supbuffer[e]--;
                    if (supbuffer[e] < K - 2)
                        q.push(e);
                }
            }
        }
        for (int i = 0; i < m; ++i)
            if (del[i] == false)
                res.push_back(i);
        return res;
    }

    vector<int> computetruss(vector<int> anchored_edges)   //计算所有边的trussness，返回一个二维数组，存储有trussness信息 返回值： 如a[i][j]表示trussness为i的边的第j条边     用不断剥离不符合条件的边的方法
        //该函数计算trussness不会改变全局变量的值
    {
        vector<vector<int>> Trussness_Table(maxsup + 3);

        vector<int> supbuffer;

        vector<int> truss(m);

        supbuffer = sup;

        vector<int>* bin = new vector<int>[maxsup + 1];   //bin为指向vector的指针，指针bin[i]指向的是支持度为i的边集合

        vector<std::unordered_set<int>> neig = neighbor_hash;

        for (int i = 0; i < m; ++i)
        {
            if (find(anchored_edges.begin(), anchored_edges.end(), i) == anchored_edges.end())
                bin[supbuffer[i]].push_back(i);
        }


        vector<bool> del(m, false);
        for (int i = 0; i <= maxsup; ++i) {
            for (int j = 0; j < bin[i].size(); ++j) {
                int eid = bin[i][j];      //从支持度最小的边开始访问
                if (del[eid] == true) continue;
                else {
                    truss[eid] = i + 2;


                }


                del[eid] = true;   //删除该条边
                int x = edge2[eid].first;
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size()) {
                    int t = x;
                    x = y;
                    y = t;
                }
                for (auto iter = neig[y].begin(); iter != neig[y].end(); ++iter)   //寻找与(x,y)构成三角形的边，将其支持度减1
                {
                    int w = *iter;
                    int e = edge[pair<int, int>(y, w)];
                    if (del[e] == true) continue;
                    if (neig[x].find(w) != neig[x].end())  //xyw构成三角形
                    {
                        e = edge[pair<int, int>(x, w)];
                        if (del[e] == true) continue;
                        if (supbuffer[e] > i) {
                            supbuffer[e]--;
                            if (find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end()) //是anchor边就不push到桶中，永远不删除
                                bin[supbuffer[e]].push_back(e);
                        }
                        e = edge[pair<int, int>(y, w)];
                        if (supbuffer[e] > i) {
                            supbuffer[e]--;
                            if (find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end())
                                bin[supbuffer[e]].push_back(e);
                        }
                    }
                }
                neig[x].erase(y);
                neig[y].erase(x);
            }

        }
        //cout << "caculating finished!" << endl;
        delete[] bin;
        //Truss_Table = Trussness_Table;
        return truss;
    }

    vector<vector<int>> computetrussness(vector<int> anchored_edges)   //计算所有边的trussness，返回一个二维数组，存储有trussness信息 返回值： 如a[i][j]表示trussness为i的边的第j条边     用不断剥离不符合条件的边的方法
        //该函数计算trussness不会改变全局变量的值
    {
        auto time = 0;
        queue<int> q;  //队列
        vector<int> res;  //结果集，存储结果编号。
        vector<bool> del(m, false);   //标记边是否被删掉，逻辑上删除
        vector<int> supbuffer;   //存储复制的sup的数据，而不是对原图的sup数据直接操作。


        int K = 2;
        supbuffer = sup;
        vector<int> truss(m + 1);
        vector<vector<int>> alltrussness(maxsup + 2);   //存储最终结果 trussness不会超过maxsup+2
        for (int l = K; l <= maxTrussness + 2; l++) {

            for (int i = 0; i < m; ++i)    //初始化队列，将sup<k-2的边先插入队列
            {
                if (supbuffer[i] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), i) == anchored_edges.end()) //不删除anchored edge.
                    q.push(i);
            }
            //auto start_SLA = std::chrono::high_resolution_clock::now();
            while (q.empty() == false)  //队列不为空，迭代删除支持度不满足的点
            {

                int eid = q.front();  //记录队头元素id
                q.pop();     //出队
                if (del[eid] == true) continue;//若该边已经被删除，则跳过本轮循环
                else {
                    //truss[eid]=l-1;
                    alltrussness[l - 1].push_back(eid);   // 若在该轮删除中被删，则该边的trussness为l-1
                    //cout<<l-1<<"   "<<eid<<endl;
                    del[eid] = true;      //没被删除，则标记为被删除
                }

                int x = edge2[eid].first;    //记录被删除的端点
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size())   //比较两者度数，把邻居数量多的赋给x
                {
                    int t = x;
                    x = y;
                    y = t;
                }

                for (int k = 0; k < neighbor[y].size(); ++k)   //遍历邻居节点寻找受影响的边，只有与被删边构成三角形的边支持度减1  该变量中的k为小写
                {
                    int w = neighbor[y][k];  //w为y的一个邻居
                    int e = edge[pair<int, int>(y, w)];  //e为edge（y,w）的编号，后面用于更新support
                    if (del[e] == true) continue;
                    if (edge.find(pair<int, int>(x, w)) != edge.end())  //若找到这样的xyw构成三角形 所以其他两边支持度减1
                    {
                        e = edge[pair<int, int>(x, w)];
                        if (del[e] == true) continue;
                        supbuffer[e]--;
                        if (supbuffer[e] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end())    //更新后发现support<k-2，则加入队列供下一轮删除
                            q.push(e);

                        e = edge[pair<int, int>(y, w)]; //更新另一条边的支持度
                        supbuffer[e]--;
                        if (supbuffer[e] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end())
                            q.push(e);
                    }
                }


            }    //一轮删除结束



        }
        //  cout << time << endl;
        return alltrussness;
    }

    void caculation_Trussness()  //利用truss_decomposition计算所有边的trussness，对应边的trussness存储于trussness数组里   方法是每次处理sup最小的边   同时返回一个二维数组，a[i][j]表示trussness为i的第j个边的id;
        //同时初始化全局变量tussness的信息
    {
        cout << "trussness is caculating!" << endl;
        vector<vector<int>> Trussness_Table(maxsup + 3);

        vector<int> supbuffer;

        supbuffer = sup;

        vector<int>* bin = new vector<int>[maxsup + 1];   //bin为指向vector的指针，指针bin[i]指向的是支持度为i的边集合

        vector<std::unordered_set<int>> neig = neighbor_hash;

        for (int i = 0; i < m; ++i)
            bin[supbuffer[i]].push_back(i);

        vector<bool> del(m, false);
        for (int i = 0; i <= maxsup; ++i) {
            for (int j = 0; j < bin[i].size(); ++j) {
                int eid = bin[i][j];      //从支持度最小的边开始访问
                if (del[eid] == true) continue;
                else {
                    trussness[eid] = i + 2;
                    Trussness_Table[i + 2].push_back(eid);
                    if (trussness[eid] > maxTrussness) {
                        maxTrussness = trussness[eid];

                    }
                }


                del[eid] = true;   //删除该条边
                int x = edge2[eid].first;
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size()) {
                    int t = x;
                    x = y;
                    y = t;
                }
                //vector<int> comm;
                for (auto iter = neig[y].begin(); iter != neig[y].end(); ++iter)   //寻找与(x,y)构成三角形的边，将其支持度减1
                {
                    int w = *iter;
                    int e = edge[pair<int, int>(y, w)];
                    if (del[e] == true) continue;
                    if (neig[x].find(w) != neig[x].end())  //xyw构成三角形
                    {

                        //comm.push_back(w);
                        e = edge[pair<int, int>(x, w)];
                        if (del[e] == true) continue;
                        if (supbuffer[e] > i) {
                            supbuffer[e]--;
                            bin[supbuffer[e]].push_back(e);
                        }
                        e = edge[pair<int, int>(y, w)];
                        if (supbuffer[e] > i) {
                            supbuffer[e]--;
                            bin[supbuffer[e]].push_back(e);
                        }
                    }
                }
                //common_neighbor[pair<int, int>(x, y)] = comm;
                //common_neighbor[pair<int, int>(y, x)] = comm;
                //neig[x].erase(y);
                //neig[y].erase(x);
            }

        }
        cout << "caculating finished!" << endl;
        delete[] bin;
        Truss_Table = Trussness_Table;
    }

    void common()
    {

        for (int i = 0; i < m; i++)
        {
            int x = edge2[i].first;
            int y = edge2[i].second;
            for (auto iter = neighbor_hash[x].begin(); iter != neighbor_hash[x].end(); iter++)
            {
                int w = *iter;
                if (neighbor_hash[y].find(w) != neighbor_hash[y].end())
                {
                    if (common_neighbor[pair<int, int>(x, y)].find(w) == common_neighbor[pair<int, int>(x, y)].end())
                    {
                        common_neighbor[pair<int, int>(x, y)].insert(w);
                        common_neighbor[pair<int, int>(y, x)].insert(w);
                    }

                }
            }
        }

    }

};

#endif //TEST_GRAPH_H
