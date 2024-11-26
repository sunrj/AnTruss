#pragma once
#define _CRT_SECURE_NO_WARNINGS
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

//int maximal_route_size = 0;
//
//int minmal_route_size = INF;
//
//int sum_size = 0;

vector<pair<int, int>> get_hull_pair(Graph& G, int(*max))    //返回图中所有边的hull_pair
{
    auto time = 0;

    vector<pair<int, int>> res(G.m);                 //存储结果集

    vector<pair<int, int>> edges = G.edge2;     //复制边

    vector<int> sup = G.sup;

    vector<bool> del(G.m, false);    //标明该边是否被删除

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

void Update_trussness(Graph* G, int anchor_edge, std::unordered_map<int, vector<int>> followers)//更新anchor一条边后的followers
{
    (*G).Truss_Table[(*G).trussness[anchor_edge]].erase(remove((*G).Truss_Table[(*G).trussness[anchor_edge]].begin(), (*G).Truss_Table[(*G).trussness[anchor_edge]].end(), anchor_edge), (*G).Truss_Table[(*G).trussness[anchor_edge]].end());
    (*G).trussness[anchor_edge] = INF;   //将anchor edge的trussness值设为无穷大
    (*G).sup[anchor_edge] = INF;   //将anchor边的支持度设置为无穷大


    for (auto iter = followers.begin(); iter != followers.end(); iter++)  //更新trussness，trussness table
    {
        for (int i = 0; i < iter->second.size(); i++)
        {
            int trussness = (*G).trussness[iter->second[i]];
            //更新trussTable
            (*G).Truss_Table[trussness].erase(remove((*G).Truss_Table[trussness].begin(), (*G).Truss_Table[trussness].end(), iter->second[i]), (*G).Truss_Table[trussness].end());
            (*G).Truss_Table[trussness + 1].push_back(iter->second[i]);


            (*G).trussness[iter->second[i]]++;     //更新followers的trussness
            if ((*G).trussness[iter->second[i]] > (*G).maxTrussness)
            {
                (*G).maxTrussness = (*G).trussness[iter->second[i]];
            }
        }
    }

}

inline int get_support_upper_bound(Graph& G, int eid, vector<pair<int, int>>& hull_pair, std::unordered_set<int>& suv, bool* discard)     //计算支持度上界
{


    pair<int, int> edge = G.edge2[eid];   //获取边

    int trussness_eid = hull_pair[eid].first;
    int layer_eid = hull_pair[eid].second;

    int x = edge.first;
    int y = edge.second;
    //cout << eid << endl;
   // if (G.neighbor[x].size() > G.neighbor[y].size()) {
     //   int t = x;
    //    x = y;
   //     y = t;
   // }
    int sup_upper = 0;



    std::unordered_set<int> comm = G.common_neighbor[edge];


    for (auto iter = comm.begin(); iter != comm.end(); iter++)    // 遍历该边的所有公共邻居
    {
        //cout << comm.size() << endl;
        int w = *iter;    //w为点x的一个邻居
        //if (G.neighbor_hash[y].find(w) != G.neighbor_hash[y].end())    //找到一个三角形
        {

            int eid1 = G.edge[pair<int, int>(x, w)];

            int eid2 = G.edge[pair<int, int>(y, w)];



            bool inner_1 = false;    //标注其他两条边是否是内层边
            bool suv_1 = false;      //是否是生存的边
            int trussness_eid1 = hull_pair[eid1].first;
            //if (trussness_eid1 < trussness_eid) continue;
            int layer_eid1 = hull_pair[eid1].second;
            bool inner_2 = false;
            bool suv_2 = false;
            int trussness_eid2 = hull_pair[eid2].first;
            //if (trussness_eid2 < trussness_eid) continue;
            int layer_eid2 = hull_pair[eid2].second;


            if ((trussness_eid1 > trussness_eid) || (((trussness_eid1 == trussness_eid) && layer_eid1 >= layer_eid) && discard[eid1] == false)) inner_1 = true;
            if (suv.find(eid1) != suv.end()) suv_1 = true;









            if ((trussness_eid2 > trussness_eid) || (((trussness_eid2 == trussness_eid) && layer_eid2 >= layer_eid) && discard[eid2] == false)) inner_2 = true;
            if (suv.find(eid2) != suv.end()) suv_2 = true;




            if ((inner_1 || suv_1) && (inner_2 || suv_2))             //两条边都满足，则support upperbound++
            {
                sup_upper++;
            }

        }
    }

    return sup_upper;
}

struct Edge {
    int id;
    int layer;
    bool operator<(const Edge& other) const {
        return layer > other.layer;
    }
};

void shrink(Graph& G, int discard_edge, std::unordered_set<int>* suv, bool* discard, int* sup, vector<pair<int, int>>& hull_pair, int K, int anchor_edge, vector<bool>(*used_to_suv), vector<bool>(*proccessed))  //递归方式调整存活的边的support。
{

    pair<int, int> edge = G.edge2[discard_edge];   //获取边
    int x = edge.first;
    int y = edge.second;
    //if (G.neighbor[x].size() > G.neighbor[y].size()) {
        //int t = x;
        //x = y;
        //y = t;
   // }
   // std::unordered_set<int> comm = G.common_neighbor[edge];


    for (auto iter = G.common_neighbor[edge].begin(); iter != G.common_neighbor[edge].end(); iter++)  // 遍历该边的所有公共邻居
    {
        int w = *iter;    //w为点x的一个邻居

        // if (discard[e1] == true&&(*proccessed)[e1]==false) continue;   //若边已经被discrd，则该三角形不用处理
       // if (G.edge.find(pair<int, int>(y, w)) != G.edge.end())    //找到一个三角形
        {



            int e1 = G.edge[pair<int, int>(x, w)];
            int e2 = G.edge[pair<int, int>(y, w)];

            // if (G.trussness[e1] < G.trussness[anchor_edge] || G.trussness[e2] < G.trussness[anchor_edge]) continue;

            if (!((discard[e2] == true && (*proccessed)[e2] == false) || (discard[e1] == true && (*proccessed)[e1] == false)))
            {
                //if(discard[e1]==false&& (*proccessed)[e1] == false&& discard[e2] == true && (*proccessed)[e2] == false)

                if (hull_pair[e1].first > hull_pair[e2].first)  //e1 是trussness小的那条边
                {
                    int temp = e1;
                    e1 = e2;
                    e2 = temp;       //交换值
                }



                if (hull_pair[e1].first == hull_pair[e2].first && hull_pair[discard_edge].first == hull_pair[e1].first && e1 != anchor_edge && e2 != anchor_edge)   //三条边trussness都相同的情况
                {




                    if (hull_pair[e1].second <= hull_pair[discard_edge].second && hull_pair[e2].second <= hull_pair[discard_edge].second)//其他两条边层数相同并且在discard edge 外层 或本层
                    {
                        if (hull_pair[e1].second < hull_pair[e2].second)    //若两条边layer有大有小，且e1是小的那个  //则只处理e1
                        {
                            if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge && discard[e2] == false)  //两条边都suv的情况下，都处理
                            {
                                sup[e1]--;
                                if (sup[e1] < K - 1)
                                {

                                    (*used_to_suv)[e1] = true;
                                    discard[e1] = true;
                                    (*suv).erase(e1);

                                    shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);
                                }
                                if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge)
                                {
                                    sup[e2]--;
                                    if (sup[e2] < K - 1)
                                    {
                                        (*used_to_suv)[e2] = true;
                                        discard[e2] = true;
                                        (*suv).erase(e2);

                                        shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                    }
                                }

                            }


                        }

                        if (hull_pair[e1].second > hull_pair[e2].second)    //若两条边layer有大有小，且e2是小的那个  //则只处理e2
                        {
                            if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge && discard[e1] == false)
                            {
                                sup[e2]--;
                                if (sup[e2] < K - 1)
                                {

                                    (*used_to_suv)[e2] = true;
                                    discard[e2] = true;
                                    (*suv).erase(e2);

                                    shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);
                                }
                                if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge)
                                {
                                    sup[e1]--;
                                    if (sup[e1] < K - 1)
                                    {

                                        (*used_to_suv)[e1] = true;
                                        discard[e1] = true;
                                        (*suv).erase(e1);

                                        shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                    }
                                }

                            }


                        }

                        if (hull_pair[e1].second == hull_pair[e2].second && discard[e1] == false && discard[e2] == false)//如果两条边层数相同
                        {
                            if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge)
                            {
                                sup[e1]--;
                                if (sup[e1] < K - 1)
                                {

                                    (*used_to_suv)[e1] = true;
                                    discard[e1] = true;
                                    (*suv).erase(e1);

                                    shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                }

                            }
                            if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge)
                            {
                                sup[e2]--;
                                if (sup[e2] < K - 1)
                                {

                                    (*used_to_suv)[e2] = true;
                                    discard[e2] = true;
                                    (*suv).erase(e2);

                                    shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                }

                            }


                        }
                    }



                    if (hull_pair[e1].second <= hull_pair[discard_edge].second && hull_pair[e2].second > hull_pair[discard_edge].second)//其他两条边层数一大一小，e1在discard或更外层，e2在更内层
                    {



                        if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge && discard[e2] == false)
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }

                        if ((*suv).find(e2) != (*suv).end() && (*used_to_suv)[discard_edge] == true && ((*suv).find(e1) != (*suv).end() || (*used_to_suv)[e1] == true))
                        {
                            if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge)
                            {
                                sup[e2]--;
                                if (sup[e2] < K - 1)
                                {

                                    (*used_to_suv)[e2] = true;
                                    discard[e2] = true;
                                    (*suv).erase(e2);

                                    shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                }

                            }
                        }


                    }

                    if (hull_pair[e2].second <= hull_pair[discard_edge].second && hull_pair[e1].second > hull_pair[discard_edge].second)//其他两条边层数一大一小，一个在discard或更外层，一个在更内层
                    {


                        if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge && discard[e1] == false)
                        {
                            sup[e2]--;
                            if (sup[e2] < K - 1)
                            {

                                (*used_to_suv)[e2] = true;
                                discard[e2] = true;
                                (*suv).erase(e2);

                                shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }

                        if ((*suv).find(e1) != (*suv).end() && (*used_to_suv)[discard_edge] == true && ((*suv).find(e2) != (*suv).end() || (*used_to_suv)[e2] == true))
                        {
                            if ((*suv).find(e1) != (*suv).end())
                            {
                                sup[e1]--;
                                if (sup[e1] < K - 1)
                                {

                                    (*used_to_suv)[e1] = true;
                                    discard[e1] = true;
                                    (*suv).erase(e1);

                                    shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                                }

                            }
                        }


                    }

                    if (hull_pair[e1].second > hull_pair[discard_edge].second && hull_pair[e2].second > hull_pair[discard_edge].second && (*used_to_suv)[discard_edge] == true && discard[e1] == false && discard[e2] == false)   //都在内层  这部分感觉有点问题，但运行起来没出bug
                    {
                        if ((*suv).find(e1) != (*suv).end())
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                        if ((*suv).find(e2) != (*suv).end())
                        {
                            sup[e2]--;
                            if (sup[e2] < K - 1)
                            {

                                (*used_to_suv)[e2] = true;
                                discard[e2] = true;
                                (*suv).erase(e2);

                                shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }



                    }



                }

                if (hull_pair[e1].first < hull_pair[e2].first && hull_pair[discard_edge].first == hull_pair[e1].first && e1 != anchor_edge && e2 != anchor_edge)  //有一条边trussness更大
                {
                    if (hull_pair[e1].second <= hull_pair[discard_edge].second)
                    {
                        if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge)
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }
                    if (hull_pair[e1].second > hull_pair[discard_edge].second)
                    {
                        if ((*suv).find(e1) != (*suv).end() && (*used_to_suv)[discard_edge] == true && e1 != anchor_edge)
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }
                }

                if (anchor_edge == e1 && hull_pair[e2].first == hull_pair[discard_edge].first)  //如果有两条的trussness相等  有一条为anchor edge
                {
                    if (hull_pair[e2].second <= hull_pair[discard_edge].second)
                    {
                        if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge)
                        {
                            sup[e2]--;
                            if (sup[e2] < K - 1)
                            {

                                (*used_to_suv)[e2] = true;
                                discard[e2] = true;
                                (*suv).erase(e2);

                                shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }

                    if (hull_pair[e2].second > hull_pair[discard_edge].second && (*used_to_suv)[discard_edge] == true)
                    {
                        if ((*suv).find(e2) != (*suv).end() && e2 != anchor_edge)
                        {
                            sup[e2]--;
                            if (sup[e2] < K - 1)
                            {

                                (*used_to_suv)[e2] = true;
                                discard[e2] = true;
                                (*suv).erase(e2);

                                shrink(G, e2, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }
                }

                if (anchor_edge == e2 && hull_pair[e1].first == hull_pair[discard_edge].first)  //如果有两条的trussness相等  有一条为anchor edge
                {
                    if (hull_pair[e1].second <= hull_pair[discard_edge].second)
                    {
                        if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge)
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }

                    if (hull_pair[e1].second > hull_pair[discard_edge].second && (*used_to_suv)[discard_edge] == true)
                    {
                        if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge)
                        {
                            sup[e1]--;
                            if (sup[e1] < K - 1)
                            {

                                (*used_to_suv)[e1] = true;
                                discard[e1] = true;
                                (*suv).erase(e1);

                                shrink(G, e1, suv, discard, sup, hull_pair, K, anchor_edge, used_to_suv, proccessed);

                            }

                        }
                    }
                }
            }







        }

    }

    (*proccessed)[discard_edge] = true;

}

inline std::unordered_map<int, vector<int>> Get_Followers_With_Triangle_Path(Graph& G, int& anchor_edge, vector<pair<int, int>>& hull_pair, int& max_layer)    //找某一条anchor edge的followers
{

    int current_size = 0;

    const int size = G.m + 1;
    //vector<std::unordered_set<int>> neighbor_hash = G.neighbor_hash;

    std::unordered_map<int, vector<int>> res;     //存储生存的边，也就是跟随者,对follower按照树结构进行分类


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

            current_size++;

            // cout << e << endl;
            //auto start_SLA = std::chrono::high_resolution_clock::now();
            int sup = get_support_upper_bound(G, e, hull_pair, suv, discard);
            //auto end_SLA = std::chrono::high_resolution_clock::now();
            //fuck = fuck + std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count();



            if (sup >= K - 1)                       //若support upper最大的边满足要求，则处理该边
            {

                // auto start_SLA = std::chrono::high_resolution_clock::now();
                if (visited[e] == false)             //防止重复计算
                {
                    suv.insert(e);                     //加入suv集合
                    //计算strong triangle
                    s[e] = sup;
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

            else   //若support upper最大的边都不满足要求，则将Q集合中其他的边设置为丢弃
            {

                discard[e] = true;   //不满足supportcheck的边将被丢弃
                s[e] = -1;  //设置为丢弃
                // cout << e << endl;
                 //vector<bool> proccessed(G.m, false);

                vector<bool> used_to_suv(G.m + 1, false);
                vector<bool> proccessed(G.m + 1, false);
                //memset(used_to_suv, false, sizeof(used_to_suv));
                //memset(proccessed, false, sizeof(proccessed));

                // cout << "root discard:" << e << "  " << hull_pair[e].first << "  " << hull_pair[e].second << endl;
                shrink(G, e, &suv, discard, s, hull_pair, K, anchor_edge, &used_to_suv, &proccessed);



                //更新因为discard边而受到影响的support.

            }





        }





        for (auto it = suv.begin(); it != suv.end(); it++)
        {
            if ((*it) != anchor_edge)
                res[K].push_back(*it);
            // cout << "follower:" << k << endl;
        }



        suv.clear();
        munmap(discard, size * sizeof(bool));
        munmap(visited, size * sizeof(bool));
        munmap(s, size * sizeof(int));
        //delete[] discard;
        //delete[] visited;
        //delete[] s;



    }

    munmap(ifexist, size * sizeof(bool));
    //delete[] ifexist;

    //if (current_size < minmal_route_size) //统计路径的长度
    //    minmal_route_size = current_size;

    //if (current_size > maximal_route_size)
    //    maximal_route_size = current_size;

    //sum_size = sum_size + current_size;

    return res;
}



vector<int> Base_with_route(Graph& G, int b)   //BL+T+U+P
{
    auto s = std::chrono::high_resolution_clock::now();
    auto t = 0;
    auto start = std::chrono::high_resolution_clock::now();

    vector<int> followersres;
    vector<int> res;
    //TreeNode rootnode;
    int max_layer = 0;
    int trussness_gain = 0;
    vector<bool> ifanchored(G.m, false);//记录已经anchor的边，不再计算
    vector<std::unordered_set<int>> reuseable_node(G.m);  //存放可以reuse的treenode

    vector<std::unordered_map<int, vector<int>>> collected_followers(G.m);  //存放followers
    vector<int> number(G.m, 0);  //存放对应的followers数量
    vector<bool> ifcomputed(G.m, false);


    vector<pair<int, int>> hull_pair;
    hull_pair = get_hull_pair(G, &max_layer);

    auto end = std::chrono::high_resolution_clock::now();




    while (b > 0)
    {


        // cout << b << endl;
        int avoid = 0;
        int max = 0;
        int select = -1;
        std::unordered_map<int, vector<int>> selected_followers;


        for (int i = 0; i < G.m; i++)
        {
            if (ifanchored[i] == true) continue;


            /*if (follower_upper_bound_ordered_edge[i].upper_bound < max)
            {
                avoid = avoid + G.m - i;
                break;
            }*/

            std::unordered_map<int, vector<int>> followers;
            followers = Get_Followers_With_Triangle_Path(G, i, hull_pair, max_layer);


            int follower_number = 0;

            for (auto iter = followers.begin(); iter != followers.end(); iter++)  //处理followers
            {
                follower_number = follower_number + iter->second.size();
            }

            if (follower_number > max)
            {
                max = follower_number;
                select = i;
                selected_followers = followers;
            }

            if (i % 200000 == 0) cout << i << " " << std::flush;
            if (i % 1000000 == 0) cout << endl;

        }

        cout << endl;
        if (select == -1) break;  //每条边都没有followers，直接返回

        trussness_gain = trussness_gain + max;
        cout << select << endl;
        cout << "max" << max << endl;
        cout << "trussness gain" << trussness_gain << endl;

        b--;

        res.push_back(select);
        for (auto iter = selected_followers.begin(); iter != selected_followers.end(); iter++)
            followersres.insert(followersres.end(), iter->second.begin(), iter->second.end());


        auto end = std::chrono::high_resolution_clock::now();

        cout << 100 - b << "  " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << endl;

        if (b == 0) break;


        auto start2 = std::chrono::high_resolution_clock::now();
        ifanchored[select] = true;



        Update_trussness(&G, select, selected_followers);
        //follower_reuse(select, &G, &Tree, &SLA_PLA, &LA, &containing_e, selected_followers);



        hull_pair = get_hull_pair(G, &max_layer);




        for (int i = 0; i < res.size(); i++)   //将已经anchor的hull pair 值设置为inifinity
        {
            hull_pair[res[i]].first = INF;
            hull_pair[res[i]].second = INF;
        }










    }





    int gain = 0;
    for (int i = 0; i < followersres.size(); i++)
    {
        if (ifanchored[followersres[i]] == false)
        {
            gain++;
        }
    }


    auto e = std::chrono::high_resolution_clock::now();

    cout << std::chrono::duration_cast<std::chrono::microseconds>(e - s).count() << endl;

    cout << gain << endl;




    //cout << "maximal route size: " << maximal_route_size << endl;

    //cout << "minmal_route_size: " << minmal_route_size << endl;

    //cout << "sum of size: " << sum_size << endl;


    //maximal_route_size = 0;

    //minmal_route_size = INF;

    //sum_size = 0;

    return res;
}