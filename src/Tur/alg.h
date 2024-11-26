#pragma once

#include "Graph.h"
#include <random>
#include<algorithm>
#include <chrono>


#define TEST_ALGORITHMS_H
#include<iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include<omp.h>
#include <future>
#define INF 1000000
#include <sys/mman.h>
#include <unistd.h>

struct routesize
{
    int size;
    int edge_id;
};

struct Edge {
    int id;
    int layer;
    bool operator<(const Edge& other) const {
        return layer > other.layer;
    }
};

bool routecmp(const routesize& a, const routesize& b) {
    return a.size > b.size; // 按照 size 升序排序
}

int Get_route_size(Graph& G, int& anchor_edge, vector<pair<int, int>>& hull_pair, int& max_layer)
{

    int res = 0;
    int current_size = 0;

    const int size = G.m + 1;
    //vector<std::unordered_set<int>> neighbor_hash = G.neighbor_hash;

    //std::unordered_map<int, vector<int>> res;     //存储生存的边，也就是跟随者,对follower按照树结构进行分类


    map<int, priority_queue<Edge>> classified_edges;   //第一个值为k 第二个为对应layer层 的边集合

    bool* ifexist = (bool*)mmap(nullptr, size * sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    //vector<bool> ifexist(size, false);
    //bool* ifexist = new bool[size]();
    //bool ifexist[G.m];  //标记该边是已经在待处理的集合中  哈希存储加速计算
    //memset(ifexist, false, sizeof(ifexist));

    int x = G.edge2[anchor_edge].first;
    int y = G.edge2[anchor_edge].second;

    for (int i = 0; i < G.neighbor[x].size(); i++)   //将包含anchor的三角形的边按树结构分类
    {
        int w = G.neighbor[x][i];
        if (G.edge.find(pair<int, int>(y, w)) != G.edge.end())   //找到一个三角形
        {
            int e_1 = G.edge[pair<int, int>(x, w)];
            int e_2 = G.edge[pair<int, int>(y, w)];


            if (hull_pair[e_1].first > hull_pair[e_2].first)  //e_1是trussness小的那条边
            {
                int temp = e_1;
                e_1 = e_2;
                e_2 = temp;       //交换值
            }


            if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_1].first == hull_pair[anchor_edge].first)      //若三角形的其他两条边trussness和anchor边一样大  
            {
                if (hull_pair[e_1].second > hull_pair[anchor_edge].second && hull_pair[e_2].second > hull_pair[anchor_edge].second)  //并且这两条边在anchor_edge的内层
                {

                    ifexist[e_1] = true;
                    ifexist[e_2] = true;
                    Edge e1, e2;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    e2.id = e_2;
                    e2.layer = hull_pair[e_2].second;
                    if (classified_edges.find(hull_pair[anchor_edge].first) == classified_edges.end())   //若map中没有
                    {

                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        buffer.push(e2);
                        classified_edges[hull_pair[anchor_edge].first] = buffer;
                    }
                    else
                    {
                        classified_edges[hull_pair[anchor_edge].first].push(e1);  //将该边存入对应队列
                        classified_edges[hull_pair[anchor_edge].first].push(e2);  //将该边存入对应队列
                    }



                }
            }


            if (hull_pair[anchor_edge].first == hull_pair[e_1].first && hull_pair[anchor_edge].first < hull_pair[e_2].first)                // 三条边trussness大小是anchor_edge=e_1  e_2>e_1  e_2>anchor_edge
            {
                if (hull_pair[anchor_edge].second < hull_pair[e_1].second)    //并且在anchoredge的内层
                {
                    ifexist[e_1] = true;
                    Edge e1;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    if (classified_edges.find(hull_pair[anchor_edge].first) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);

                        classified_edges[hull_pair[anchor_edge].first] = buffer;
                    }
                    else
                    {
                        classified_edges[hull_pair[anchor_edge].first].push(e1);  //将该边存入对应队列
                    }
                }
            }

            if (hull_pair[anchor_edge].first < hull_pair[e_1].first && hull_pair[anchor_edge].first < hull_pair[e_2].first)    //另外两条边的trussness都比anchoredge大
            {
                if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_2].first != INF)  //另外两条边的trussness一样大
                {
                    ifexist[e_1] = true;
                    ifexist[e_2] = true;
                    Edge e1, e2;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    e2.id = e_2;
                    e2.layer = hull_pair[e_2].second;
                    if (classified_edges.find(hull_pair[e_1].first) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[hull_pair[e_1].first] = buffer;
                    }
                    else
                    {
                        classified_edges[hull_pair[e_1].first].push(e1);  //将该边存入对应队列
                    }

                    if (classified_edges.find(hull_pair[e_2].first) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e2);
                        classified_edges[hull_pair[e_2].first] = buffer;
                    }
                    else
                    {
                        classified_edges[hull_pair[e_2].first].push(e2);  //将该边存入对应队列
                    }

                }

                if (hull_pair[e_1].first < hull_pair[e_2].first)   //另外两条边的trussness一大一小
                {
                    ifexist[e_1] = true;
                    Edge e1;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    if (classified_edges.find(hull_pair[e_1].first) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[hull_pair[e_1].first] = buffer;
                    }
                    else
                    {
                        classified_edges[hull_pair[e_1].first].push(e1);  //将该边存入对应队列
                    }
                }
            }

        }
    }
    //广度搜索每个树节点的path




    //bool discard[G.m];
    //bool visited[G.m];

    //memset(discard, false, sizeof(discard));
    //memset(visited, false, sizeof(visited));
    //memset(s, 0, sizeof(s));


    int a = 0;
    std::unordered_set<int> suv;            //以哈希表的形式存储存活的边
    //unordered_set<int> discard;
    //unordered_set<int> visited;

    for (auto iter = classified_edges.begin(); iter != classified_edges.end(); iter++)   //一次处理一个treenode
    {


        //int heapid = iter->first;   //得到treeid
       // if (reuseable_node.find(treeid) != reuseable_node.end()) continue;


        //TreeNode node;

           //node= TCT.find(treeid)->second;
          // a++;


        //if (!classified_edges[treeid].empty())

           // auto start_SLA = std::chrono::high_resolution_clock::now();

            //vector<bool> discard(size, false);
            //vector<bool> visited(size, false);
        int* s = (int*)mmap(nullptr, size * sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        bool* visited = (bool*)mmap(nullptr, size * sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        bool* discard = (bool*)mmap(nullptr, size * sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        //bool* discard = new bool[size]();
        //bool* visited = new bool[size]();      //标记该边是否处理过
        //int* s = new int[size]();
       // std::fill(discard, discard + G.m+1, false);
       // std::fill(visited, visited + G.m+1, false);
       // std::fill(s, s + 100, 0);
       // auto end_SLA = std::chrono::high_resolution_clock::now();
        //fuck = fuck + std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count();

        //fuck++;
        visited[anchor_edge] = true;
        suv.insert(anchor_edge);   //设置anchor edge为存活.
        //vector<int> s(G.m + 1, 0);                    //存储边的support upper bound, 初始化为0


        //进行layer by layer search


        int K = iter->first;

        //priority_queue<Edge> Q;
        //K = G.trussness[classified_edges[treeid].top().id];

        while (classified_edges[K].size() != 0)
        {

            int e = classified_edges[K].top().id;
            classified_edges[K].pop();
            res++;

            current_size++;

            // cout << e << endl;
            //auto start_SLA = std::chrono::high_resolution_clock::now();
            //int sup = get_support_upper_bound(G, e, hull_pair, suv, discard);
            //auto end_SLA = std::chrono::high_resolution_clock::now();
            //fuck = fuck + std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count();



            //if (sup >= K - 1)                       //若support upper最大的边满足要求，则处理该边
            {

                // auto start_SLA = std::chrono::high_resolution_clock::now();
                if (visited[e] == false)             //防止重复计算
                {
                    suv.insert(e);                     //加入suv集合
                    //计算strong triangle
                    //s[e] = sup;
                    int u = G.edge2[e].first;
                    int v = G.edge2[e].second;

                    //if (G.neighbor_hash[v].size() < G.neighbor_hash[u].size())   //比较两者度数，把邻居数量多的赋给x
                    //{
                       // int t = v;
                      //  v = u;
                       // u = t;
                    //}
                    //vector<pair<int, int>> del;

                    for (auto ite = G.common_neighbor[pair<int, int>(u, v)].begin(); ite != G.common_neighbor[pair<int, int>(u, v)].end(); ite++)   //将包含anchor的三角形的边按树结构分类
                    {
                        int w = *ite;
                        //if (G.neighbor_hash[v].find(w) != G.neighbor_hash[v].end())   //找到一个三角形
                        {
                            int e_1 = G.edge[pair<int, int>(u, w)];
                            int e_2 = G.edge[pair<int, int>(v, w)];




                            if (hull_pair[e_1].first > hull_pair[e_2].first)  //e_1是trussness小的那条边
                            {
                                int temp = e_1;
                                e_1 = e_2;
                                e_2 = temp;       //交换值
                            }
                            Edge e1, e2;
                            e1.id = e_1;
                            e1.layer = hull_pair[e_1].second;
                            e2.id = e_2;
                            e2.layer = hull_pair[e_2].second;

                            if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_1].first == hull_pair[e].first)      //若三角形的其他两条边trussness和e边一样大  
                            {
                                if (hull_pair[e_1].second > hull_pair[e].second && hull_pair[e_2].second > hull_pair[e].second)  //并且这两条边在anchor_edge的内层
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);  //将该边存入对应队列
                                    }

                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[K].push(e2);  //将该边存入对应队列
                                    }
                                }

                                if (hull_pair[e_1].second == hull_pair[e].second && hull_pair[e_2].second > hull_pair[e].second && discard[e_1] == false)   //weak triangle 的情况
                                {
                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[K].push(e2);  //将该边存入对应队列
                                    }

                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);
                                    }

                                }

                                if (hull_pair[e_2].second == hull_pair[e].second && hull_pair[e_1].second > hull_pair[e].second && discard[e_2] == false)   //weak triangle 的情况
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);  //将该边存入对应队列
                                    }
                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[K].push(e2);
                                    }

                                }

                                if (hull_pair[e_2].second == hull_pair[e].second && hull_pair[e_1].second == hull_pair[e].second && discard[e_1] == false)
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);
                                    }

                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[K].push(e2);
                                    }

                                }
                            }

                            if (hull_pair[e].first == hull_pair[e_1].first && hull_pair[e].first < hull_pair[e_2].first)                // 三条边trussness大小是anchor_edge=e_1  e_2>e_1  e_2>e
                            {
                                if (hull_pair[e].second < hull_pair[e_1].second)    //并且在anchoredge的内层
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);  //将该边存入对应队列
                                    }

                                }

                                if (hull_pair[e].second == hull_pair[e_1].second)    //并且在anchoredge的本层
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[K].push(e1);  //将该边存入对应队列
                                    }

                                }
                            }


                        }       //找三角形

                    }


                    visited[e] = true;
                    // Q.erase(std::remove(Q.begin(), Q.end(), eid_maxsup), Q.end());    //处理完就从Q集合中删除

                }
                //auto end_SLA = std::chrono::high_resolution_clock::now();





            }

            //else   //若support upper最大的边都不满足要求，则将Q集合中其他的边设置为丢弃
            //{

            //    discard[e] = true;   //不满足supportcheck的边将被丢弃
            //    s[e] = -1;  //设置为丢弃
            //    // cout << e << endl;
            //     //vector<bool> proccessed(G.m, false);

            //    vector<bool> used_to_suv(G.m + 1, false);
            //    vector<bool> proccessed(G.m + 1, false);
            //    //memset(used_to_suv, false, sizeof(used_to_suv));
            //    //memset(proccessed, false, sizeof(proccessed));

            //    // cout << "root discard:" << e << "  " << hull_pair[e].first << "  " << hull_pair[e].second << endl;
            //    shrink(G, e, &suv, discard, s, hull_pair, K, anchor_edge, &used_to_suv, &proccessed);



            //    //更新因为discard边而受到影响的support.

            //}





        }





        //for (auto it = suv.begin(); it != suv.end(); it++)
        //{
        //    if ((*it) != anchor_edge)
        //        res[K].push_back(*it);
        //    // cout << "follower:" << k << endl;
        //}



       // suv.clear();
        munmap(discard, size * sizeof(bool));
        munmap(visited, size * sizeof(bool));
        munmap(s, size * sizeof(int));
        //delete[] discard;
        //delete[] visited;
        //delete[] s;
    }

    munmap(ifexist, size * sizeof(bool));
    ////delete[] ifexist;

    //if (current_size < minmal_route_size) //统计路径的长度
    //    minmal_route_size = current_size;

    //if (current_size > maximal_route_size)
    //    maximal_route_size = current_size;

    //sum_size = sum_size + current_size;

    return res;
}

vector<pair<int, int>> get_hull_pair(Graph& G, int(*max))    //计算（trussness，layer）pair
{
    auto time = 0;

    vector<pair<int, int>> res(G.m);                 

    vector<pair<int, int>> edges = G.edge2;     

    vector<int> sup = G.sup;

    vector<bool> del(G.m, false);   

    //for (int k = 3; k <= G.maxTrussness + 1; k++)
    for (int k = 3; k <= G.Truss_Table.size(); k++)
    {



        std::unordered_set<int> K_hull;
        K_hull.insert(G.Truss_Table[k - 1].begin(), G.Truss_Table[k - 1].end());   //得到k-1 hull 的所有边的边标号

        int layer = 1;      //记录层数

        while (K_hull.size() != 0)
        {



            queue<int> Q;


            for (auto iter = K_hull.begin(); iter != K_hull.end(); iter++)
            {

                if (sup[*iter] < k - 2)
                {
                    res[*iter].first = k - 1;
                    res[*iter].second = layer;
                    Q.push(*iter);
                }
            }

            //调整受影响的边支持度
            while (Q.size() > 0)
            {
                int a = Q.front();
                K_hull.erase(a);
                del[a] = true;    //该边标记为删除

                Q.pop();



                int x = G.edge2[a].first;
                int y = G.edge2[a].second;


                // if (G.neighbor[x].size() > G.neighbor[y].size()) {
                   //  int t = x;
                    // x = y;
                   //  y = t;
                 //}

                for (auto iter = G.common_neighbor[pair<int, int>(x, y)].begin(); iter != G.common_neighbor[pair<int, int>(x, y)].end(); iter++)   //计算受到影响的sup
                {
                    int w = *iter;
                    int e1 = G.edge[pair<int, int>(x, w)];
                    if (del[e1] == true) continue;         //若一条边已被删除，则什么都不做

                    //if (neighbor_hash[y].find(*iter) != neighbor_hash[y].end())    //找到公共点，一个三角形
                    {
                        int e2 = G.edge[pair<int, int>(y, w)];
                        if (del[e2] == true) continue;      //边被删除，则表示该三角形已经不存在
                        sup[e2]--;

                        //e = G.edge[pair<int, int>(x, w)];
                        sup[e1]--;

                    }


                }

                // neighbor_hash[x].erase(y);
                // neighbor_hash[y].erase(x);

            }


            layer++;

        }

        if (layer > (*max))
        {
            (*max) = layer;
        }

    }
    //cout << time << endl;
    return res;
}

vector<int> compute_anchor_followers(Graph& G, vector<int> anchoreid, vector<int>& trussness)  //计算anchorset的所有followers.
{
    vector<int> res;
    for (int i = 0; i < trussness.size(); i++)
    {
        if (trussness[i] != G.trussness[i] && std::find(anchoreid.begin(), anchoreid.end(), i) == anchoreid.end())
        {
            for (int j = 0; j < trussness[i] - G.trussness[i]; j++)  //若有边trussness为提升为2以上，则存对应的个数
            {
                res.push_back(i);
                // cout << "follower " << i << "trussness " << G.trussness[i] << endl;

            }

        }
    }
    return res;
}

void tup(Graph G)//找b个route size最大的边anchor
{
    int maxlayer = 0;
    vector<pair<int, int>> hp = get_hull_pair(G, &maxlayer);
    vector<routesize> rt;
    for (int i = 0; i < G.m; i++)
    {
        if (i % 10000 == 0) cout << i << endl;
        routesize s;
        int size = Get_route_size(G, i, hp, maxlayer);
        s.edge_id = i; s.size = size;
        rt.push_back(s);
    }

    sort(rt.begin(), rt.end(), routecmp);






    for (int b = 100; b >= 20; b = b - 20)
    {
        int max = 0;
        for (int i = 0; i < 2000; i++)
        {

            vector<int> anchorset;
            while (anchorset.size() < b)  //随机从path最大的边选b条
            {
                std::random_device rd; // 获取随机数种子
                std::mt19937 gen(rd()); // 使用梅森旋转算法
                std::uniform_int_distribution<> distrib(0, G.m * 0.2); // 指定均匀分布范围

                if (find(anchorset.begin(), anchorset.end(), rt[distrib(gen)].edge_id) == anchorset.end())
                    anchorset.push_back(rt[distrib(gen)].edge_id);
            }

            //计算followers
            vector<int> as;
            vector<int>trussness_before = G.computetruss(anchorset);
            vector<int> followers_after = compute_anchor_followers(G, anchorset, trussness_before);
            cout << "followers number:" << followers_after.size() << endl;
            if (max < followers_after.size()) max = followers_after.size();

        }
        cout << "budget=" << b << endl;
        cout << "max=" << max << endl;
    }



    //std::copy(rt.begin(), rt.begin() + b, anchorset.begin());


    //vector<int> trussness_after;




    //return anchorset;
}