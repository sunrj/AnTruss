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
using namespace std;

struct TreeNode
{
    int id;        //树的id
    int K;         //树节点内边的trusssness值
    int P;         //树的父节点
    vector<int> C;         //树的叶子节点
    vector<int> E; //树包含的边
    vector<pair<int, int>> edges;  //存储具体的边
};

vector<int> findCommonNeighbors(const std::vector<std::unordered_set<int>*>& neighbor, int node1, int node2) {
    vector<int> commonNeighbors;

    // 检查节点1和节点2是否有效
    if (node1 >= 0 && node1 < neighbor.size() && node2 >= 0 && node2 < neighbor.size()) {
        const std::unordered_set<int>* neighbors1 = neighbor[node1];
        const std::unordered_set<int>* neighbors2 = neighbor[node2];

        // 找到两个节点的公共邻居
        for (int neighbor : *neighbors1) {
            if ((*neighbors2).find(neighbor) != (*neighbors2).end()) {
                commonNeighbors.push_back(neighbor);
            }
        }
    }

    return commonNeighbors;
}   //找哈希表中两个点的公告邻居

vector<int> findCommonNeighbors(const std::vector<std::unordered_set<int>>& neighbor, int node1, int node2) {
    vector<int> commonNeighbors;

    // 检查节点1和节点2是否有效
    if (node1 >= 0 && node1 < neighbor.size() && node2 >= 0 && node2 < neighbor.size()) {
        const std::unordered_set<int> neighbors1 = neighbor[node1];
        const std::unordered_set<int> neighbors2 = neighbor[node2];

        // 找到两个节点的公共邻居
        for (int neighbor : neighbors1) {
            if ((neighbors2).find(neighbor) != (neighbors2).end()) {
                commonNeighbors.push_back(neighbor);
            }
        }
    }

    return commonNeighbors;
}   //找哈希表中两个点的公告邻居  // 重载

void tree(Graph& G, vector<pair<int, int>>& edge, int rootid, std::unordered_map<int, TreeNode>* Tree) //计算三角形联通的子图
{
    //cout << a << endl;
    //a++;
    //vector<TreeNode> res;
    if (edge.size() == 0)//若图为空，则直接返回空树
    {
        return;
    }

    //p = ((*Tree).back());
    //auto aaaa = std::chrono::high_resolution_clock::now();
    //vector<int> del;
    int K_min = G.trussness[G.edge.find(edge[0])->second];  //初始化最小值
    for (int i = 0; i < edge.size(); i++)  //计算trussness的最小值
    {
        //int eid = G.edge.find(edge[i])->second;
        int trussness = G.trussness[G.edge.find(edge[i])->second];


        if (trussness < K_min)
        {
            K_min = G.trussness[G.edge.find(edge[i])->second];
        }

    }
    if (K_min == INF)  //anchor edge不加入任何树节点
    {
        return;
    }



    vector<std::unordered_set<int>*>  neighbor(G.n + 1);


    for (int i = 0; i < edge.size(); i++)
    {
        if (neighbor[edge[i].first] == nullptr)  //若指针指向空，则表示该处邻居节点没有初始化，则初始化后再添加
        {
            neighbor[edge[i].first] = new std::unordered_set<int>();
            neighbor[edge[i].first]->insert(edge[i].second);
        }
        else
        {
            neighbor[edge[i].first]->insert(edge[i].second);
        }

        if (neighbor[edge[i].second] == nullptr)  //同理
        {
            neighbor[edge[i].second] = new std::unordered_set<int>();
            neighbor[edge[i].second]->insert(edge[i].first);
        }
        else
        {
            neighbor[edge[i].second]->insert(edge[i].first);
        }
    }



    vector<int> visited(G.m + 1);
    queue<pair<int, int>> Q;   //广度优先遍历队列
    for (int i = 0; i < edge.size(); i++)  //对子图进行广度优先搜索
    {


        int eid = G.edge.find(edge[i])->second;
        if (visited[eid] == 0)//若这条边未访问过   则从该边开始是个新的三角形联通的子图
        {
            TreeNode node;
            node.K = K_min;
            neighbor[edge[i].first]->erase(edge[i].second);   //访问过就删除邻居信息，避免大量的无效搜索
            neighbor[edge[i].second]->erase(edge[i].first);
            if (rootid == -1)
            {
                node.P = -1;
            }
            else
            {
                node.P = rootid;
            }


            int id = -1;
            vector<pair<int, int>> edge_next_iteration;   //存储下一轮递归的边
            visited[eid] = 1;  //设置成已访问
            if (G.trussness[eid] == K_min)
            {
                node.E.push_back(eid);
                node.edges.push_back(edge[i]);
                if (id == -1)
                {
                    id = eid;
                }
            }
            else
            {
                edge_next_iteration.push_back(edge[i]);
            }


            Q.push(edge[i]);  //入队，从该元素进行访问
            while (Q.size() != 0)
            {
                pair<int, int> e = Q.front();
                Q.pop();

                int x = e.first;
                int y = e.second;


                neighbor[y]->erase(x);   //访问过就删除邻居信息，避免大量的无效搜索
                neighbor[x]->erase(y);


                vector<int> commonneighbor = findCommonNeighbors(neighbor, x, y);


                for (int j = 0; j < commonneighbor.size(); j++)  //逐个处理三角形
                {
                    int w = commonneighbor[j];
                    int e_1 = G.edge[pair<int, int>(x, w)];
                    int e_2 = G.edge[pair<int, int>(y, w)];
                    if (visited[e_1] == 0)//若该边未被访问
                    {

                        Q.push(pair<int, int>(x, w));
                        if (G.trussness[e_1] == K_min)
                        {
                            node.E.push_back(e_1);
                            node.edges.push_back(pair<int, int>(x, w));
                            if (id == -1)
                            {
                                id = e_1;
                            }
                            if (e_1 < id)
                            {
                                id = e_1;
                            }
                        }
                        else
                        {
                            //cout << x << " " << w << endl;
                            edge_next_iteration.push_back(pair<int, int>(x, w));
                        }


                        visited[e_1] = 1;  //设置成已访问
                    }

                    if (visited[e_2] == 0)//若该边未被访问
                    {
                        Q.push(pair<int, int>(y, w));
                        if (G.trussness[e_2] == K_min)
                        {
                            node.E.push_back(e_2);
                            node.edges.push_back(pair<int, int>(y, w));
                            if (id == -1)  //id=-1表示没有被赋值过
                            {
                                id = e_2;
                            }
                            if (e_2 < id)
                            {
                                id = e_2;

                            }
                        }
                        else
                        {
                            //cout << x << " " << w << endl;
                            edge_next_iteration.push_back(pair<int, int>(y, w));
                        }


                        visited[e_2] = 1;  //设置成已访问
                    }
                }

            }     //广度搜索完一轮

            //针对这个三角形联通的component进行下一轮递归
            if (node.edges.size() > 0)
            {
                node.id = id;
                if (rootid != -1)  (*Tree).find(rootid)->second.C.push_back(node.id);  //该节点不是根节点
                (*Tree).insert(make_pair(node.id, node));
                if (edge_next_iteration.size() > 0)
                    tree(G, edge_next_iteration, node.id, Tree);
            }
            else
            {
                if (edge_next_iteration.size() > 0)
                    tree(G, edge_next_iteration, rootid, Tree);
            }
        }
    }


    //释放内存
    for (int i = 0; i < neighbor.size(); ++i) {
        delete neighbor[i];
    }
    //return res;
}

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

struct edge_with_upperbound   //带有upper bound信息的边,用于排序
{
    int eid;
    int upper_bound;
};

bool cmp(edge_with_upperbound x, edge_with_upperbound y) {
    if (x.upper_bound == y.upper_bound)
        return x.eid > y.eid;
    else
        return x.upper_bound > y.upper_bound;


}

vector<int> Get_treenode_containing_e(std::unordered_map<int, TreeNode>& Tree, Graph& G)  // 获取每条边所在的treenode id                              //获得数据结构:包含边e的树id
{
    //auto aaaa = std::chrono::high_resolution_clock::now();
    
    vector<int> res(G.m + 1);   //初始化一个大小为图的边数量的res集合
    for (const auto& pair : Tree)
    {
        for (int j = 0; j < pair.second.E.size(); j++)
        {
            res[pair.second.E[j]] = pair.first;

        }
    }

    //auto aaa = std::chrono::high_resolution_clock::now();
    //cout << "containint e:" << std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count()<< endl;
    return res;

}

vector<pair<vector<int>, vector<int>>> GET_SLA_PLA(vector<int> node_containing_e, Graph& G)
{
    //auto aaaa = std::chrono::high_resolution_clock::now();
    vector<pair<vector<int>, vector<int>>> res(G.m);    //存储结果的vector

    //vector<std::unordered_set<int>> neig = G.neighbor_hash;
    for (int i = 0; i < G.m; i++)
    {
        if (G.trussness[i] == INF) continue;

        int x = G.edge2[i].first;
        int y = G.edge2[i].second;

        //if (G.neighbor[x].size() > G.neighbor[y].size())   //比较两者度数，把邻居数量多的赋给x
       // {
          //  int t = x;
          //  x = y;
          //  y = t;
       // }
        for (auto iter = G.common_neighbor[G.edge2[i]].begin(); iter != G.common_neighbor[G.edge2[i]].end(); iter++)
        {
            int w = *iter;
            //if (G.neighbor_hash[y].find(w) != G.neighbor_hash[y].end())  //找到三角形
            {
                int eid_1 = G.edge[pair<int, int>(x, w)];
                int eid_2 = G.edge[pair<int, int>(y, w)];

                if (G.trussness[eid_1] >= G.trussness[i])
                {
                    if (G.trussness[eid_2] >= G.trussness[i])
                    {
                        if (G.trussness[eid_1] != INF && find(res[i].first.begin(), res[i].first.end(), node_containing_e[eid_1]) == res[i].first.end())
                            res[i].first.push_back(node_containing_e[eid_1]);
                        if (G.trussness[eid_2] != INF && find(res[i].first.begin(), res[i].first.end(), node_containing_e[eid_2]) == res[i].first.end())
                            res[i].first.push_back(node_containing_e[eid_2]);
                    }

                }
                if (G.trussness[eid_1] < G.trussness[i])
                {
                    if (G.trussness[eid_2] < G.trussness[i])
                    {
                        if (min(G.trussness[eid_1], G.trussness[eid_2]) == G.trussness[eid_1] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_1]) == res[i].second.end())
                        {
                            res[i].second.push_back(node_containing_e[eid_1]);
                        }
                        if (min(G.trussness[eid_1], G.trussness[eid_2]) == G.trussness[eid_2] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_2]) == res[i].second.end())
                        {
                            res[i].second.push_back(node_containing_e[eid_2]);
                        }


                    }

                }
                if (G.trussness[eid_1] >= G.trussness[i] && G.trussness[eid_2] < G.trussness[i] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_2]) == res[i].second.end())  //trussness一大一小
                {
                    res[i].second.push_back(node_containing_e[eid_2]);
                }
                if (G.trussness[eid_2] >= G.trussness[i] && G.trussness[eid_1] < G.trussness[i] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_1]) == res[i].second.end())  //trussness一大一小
                {
                    res[i].second.push_back(node_containing_e[eid_1]);
                }

            }
        }
    }
    //auto aaa = std::chrono::high_resolution_clock::now();
   // cout << "get SLA_PLA:" << std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count() << endl;
    return res;
}

vector<edge_with_upperbound> get_follower_upper_bound(Graph& G, std::unordered_map<int, TreeNode>* Tree, vector<pair<vector<int>, vector<int>>>& SLA_PLA, vector<std::unordered_map<int, vector<int>>>& collected_followers, vector<std::unordered_set<int>>& reuseable_node, vector<bool>& ifcomputed)   //获取一条边的followers数量的上界值
{
    vector<edge_with_upperbound> edges(G.m);

    for (int i = 0; i < SLA_PLA.size(); i++)
    {
        int sum = 0;


        for (int j = 0; j < SLA_PLA[i].first.size(); j++)
        {
            //auto aaaa = std::chrono::high_resolution_clock::now();
            if (reuseable_node[i].find(SLA_PLA[i].first[j]) != reuseable_node[i].end() && ifcomputed[i] == true)   //若发现某个节点的follower可以reuse，则将具体的follower数量代替upper bound，更精确,并且该边的followers被计算过了
            {
                if (collected_followers[i].find(SLA_PLA[i].first[j]) != collected_followers[i].end())
                    sum = sum + collected_followers[i].find(SLA_PLA[i].first[j])->second.size();
            }
            else
            {
                sum = sum + (*Tree).find(SLA_PLA[i].first[j])->second.edges.size();
            }
            

        }

        edges[i].eid = i;
        edges[i].upper_bound = sum;

    }



    //auto aaaa = std::chrono::high_resolution_clock::now();
    sort(edges.begin(), edges.end(), cmp);

    return edges;
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

inline std::unordered_map<int, vector<int>> Get_Followers_With_Triangle_Path(Graph& G, int& anchor_edge, std::unordered_map<int, TreeNode>& TCT, vector<int>& containing_e, vector<pair<int, int>>& hull_pair, int& max_layer, std::unordered_set<int>& reuseable_node)    //找某一条anchor edge的followers
{


    const int size = G.m + 1;
    //vector<std::unordered_set<int>> neighbor_hash = G.neighbor_hash;

    std::unordered_map<int, vector<int>> res;     //存储生存的边，也就是跟随者,对follower按照树结构进行分类


    map<int, priority_queue<Edge>> classified_edges;   //第一个值为tree的id  第二个为对应layer层 的边集合

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
                    if (classified_edges.find(containing_e[anchor_edge]) == classified_edges.end())   //若map中没有
                    {

                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        buffer.push(e2);
                        classified_edges[containing_e[anchor_edge]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[anchor_edge]].push(e1);  //将该边存入对应队列
                        classified_edges[containing_e[anchor_edge]].push(e2);  //将该边存入对应队列
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
                    if (classified_edges.find(containing_e[anchor_edge]) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);

                        classified_edges[containing_e[anchor_edge]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[anchor_edge]].push(e1);  //将该边存入对应队列
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
                    if (classified_edges.find(containing_e[e_1]) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[containing_e[e_1]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_1]].push(e1);  //将该边存入对应队列
                    }

                    if (classified_edges.find(containing_e[e_2]) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e2);
                        classified_edges[containing_e[e_2]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_2]].push(e2);  //将该边存入对应队列
                    }

                }

                if (hull_pair[e_1].first < hull_pair[e_2].first)   //另外两条边的trussness一大一小
                {
                    ifexist[e_1] = true;
                    Edge e1;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    if (classified_edges.find(containing_e[e_1]) == classified_edges.end())   //若map中没有
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[containing_e[e_1]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_1]].push(e1);  //将该边存入对应队列
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


        int treeid = iter->first;   //得到treeid
        if (reuseable_node.find(treeid) != reuseable_node.end()) continue;


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



        visited[anchor_edge] = true;
        suv.insert(anchor_edge);   //设置anchor edge为存活.
        //vector<int> s(G.m + 1, 0);                    //存储边的support upper bound, 初始化为0


        //进行layer by layer search


        int K;

        //priority_queue<Edge> Q;
        K = G.trussness[classified_edges[treeid].top().id];

        while (classified_edges[treeid].size() != 0)
        {

            int e = classified_edges[treeid].top().id;
            classified_edges[treeid].pop();



            // cout << e << endl;
   
            int sup = get_support_upper_bound(G, e, hull_pair, suv, discard);
 



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
                                        classified_edges[treeid].push(e1);  //将该边存入对应队列
                                    }

                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);  //将该边存入对应队列
                                    }
                                }

                                if (hull_pair[e_1].second == hull_pair[e].second && hull_pair[e_2].second > hull_pair[e].second && discard[e_1] == false)   //weak triangle 的情况
                                {
                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);  //将该边存入对应队列
                                    }

                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);
                                    }

                                }

                                if (hull_pair[e_2].second == hull_pair[e].second && hull_pair[e_1].second > hull_pair[e].second && discard[e_2] == false)   //weak triangle 的情况
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //将该边存入对应队列
                                    }
                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);
                                    }

                                }

                                if (hull_pair[e_2].second == hull_pair[e].second && hull_pair[e_1].second == hull_pair[e].second && discard[e_1] == false)
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);
                                    }

                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);
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
                                        classified_edges[treeid].push(e1);  //将该边存入对应队列
                                    }

                                }

                                if (hull_pair[e].second == hull_pair[e_1].second)    //并且在anchoredge的本层
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //将该边存入对应队列
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
                res[treeid].push_back(*it);
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

vector<std::unordered_set<int>> follower_reuse_update(int anchor_edge, Graph* G, std::unordered_map<int, TreeNode>* TCT, vector<pair<vector<int>, vector<int>>>* SLA_PLA, vector<int>* containing_e, std::unordered_map<int, vector<int>>& followers)   //根据选择的anchor edge更新树结构，并计算出不需要重新计算的followers
{

   
    vector<std::unordered_set<int>> res((*G).m);   //针对每个edge存放可以reuse的树节点


    for (int i = 0; i < (*G).m; i++)//初始化res
    {
        for (int j = 0; j < (*SLA_PLA)[i].first.size(); j++)
        {
            res[i].insert((*SLA_PLA)[i].first[j]);
        }

    }

    unordered_set<int> un_reauseable_id; //存储所有不能reuse的node id
    //把 sla(anchor_edge) 中的边的PLA 中的边与sla(anchor_edge)交集部分删除
    vector<int> E_ex;

    bool a = false; //标记sla（anchor）是否包含 containinge[anchor]
    for (int i = 0; i < (*SLA_PLA)[anchor_edge].first.size(); i++)   //搜集sla（ex）的所有边
    {
        if ((*SLA_PLA)[anchor_edge].first[i] == (*containing_e)[anchor_edge]) { a = true; }
        TreeNode node = (*TCT).find((*SLA_PLA)[anchor_edge].first[i])->second;
        E_ex.insert(E_ex.end(), node.E.begin(), node.E.end());
        if (un_reauseable_id.find(node.id) == un_reauseable_id.end())
        {
            un_reauseable_id.insert(node.id);
        }
        
    }

    if (a == false)
    {
        TreeNode node = (*TCT).find((*containing_e)[anchor_edge])->second;
        E_ex.insert(E_ex.end(), node.E.begin(), node.E.end());
        un_reauseable_id.insert(node.id);
    }


    for (auto iter = un_reauseable_id.begin(); iter != un_reauseable_id.end(); iter++)
    {
        for (int i = 0; i < (*G).m; i++)
        {
            res[i].erase(*iter);
        }
    }

    //auto aaa = std::chrono::high_resolution_clock::now();
    //t=t+ std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count();
    //for (int i = 0; i < E_ex.size(); i++)         // 把不能reuse的部分删除
    //{
    //    int id = (*containing_e)[E_ex[i]];

    //    res[E_ex[i]].erase(id);

    //    for (auto iter = (*LA)[E_ex[i]].begin(); iter != (*LA)[E_ex[i]].end(); iter++)
    //    {
    //        for (int j = 0; j < iter->second.size(); j++)
    //        {
    //            res[iter->second[j]].erase(id);
    //        }
    //    }
    //}


    Update_trussness(G, anchor_edge, followers);
    //更新trussness 和layer,重构子树

    //auto aaaa = std::chrono::high_resolution_clock::now();
    //搜集以anchor edge所在树节点为根的子树的边
    vector<pair<int, int>> edges;
    int rootid = (*TCT).find((*containing_e)[anchor_edge])->second.P;
    if (rootid != -1)  //若该节点不是根节点
    {
        (*TCT).find(rootid)->second.C.erase(remove((*TCT).find(rootid)->second.C.begin(), (*TCT).find(rootid)->second.C.end(), (*containing_e)[anchor_edge]), (*TCT).find(rootid)->second.C.end());   //删除child信息
    }

    //std::unordered_map<int, TreeNode> tree_buffer;  //存储原先的树节点;
    queue<int> Q;    //广度搜集
    Q.push((*containing_e)[anchor_edge]);


    while (Q.size() != 0)
    {
        int id = Q.front();
        Q.pop();
        TreeNode node = (*TCT).find(id)->second;
        edges.insert(edges.end(), node.edges.begin(), node.edges.end());
        for (int i = 0; i < node.C.size(); i++)
        {
            //cout << node.C[i]<<"  ";
            Q.push(node.C[i]);
        }
        //cout << endl;
       // tree_buffer[id] = node;
        (*TCT).erase(id);
    }
    //树里面follower的合并会导致id发生变化，如果变了，那不用管。

    tree((*G), edges, rootid, TCT);   //重构子树


    //cout << "tree finish!" << endl;
    //auto aaa = std::chrono::high_resolution_clock::now();
    //t=t+ std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count();
    //auto aaaa = std::chrono::high_resolution_clock::now();

    //重新计算SLA_PLA  和LA  containing_e



    (*containing_e) = Get_treenode_containing_e(*TCT, *G);
  


    (*SLA_PLA) = GET_SLA_PLA(*containing_e, *G);



    vector<int> E_ex_buffer;
    vector<bool> if_insert((*G).m, false); //防止数据重复插入

    for (int i = 0; i < E_ex.size(); i++)
    {
        if (E_ex[i] != anchor_edge && if_insert[(*containing_e)[E_ex[i]]] == false)
        {
            TreeNode node = (*TCT).find((*containing_e)[E_ex[i]])->second;
            E_ex_buffer.insert(E_ex_buffer.end(), node.E.begin(), node.E.end());
            if_insert[(*containing_e)[E_ex[i]]] = true;
            if (un_reauseable_id.find(node.id) == un_reauseable_id.end())
            {
                un_reauseable_id.insert(node.id);
            }
        }

    }


    for (auto iter = un_reauseable_id.begin(); iter != un_reauseable_id.end(); iter++)
    {
        for (int i = 0; i < (*G).m; i++)
        {
            res[i].erase(*iter);
        }
    }

    //aaa = std::chrono::high_resolution_clock::now();
    //t = t + std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count();
    //for (int i = 0; i < E_ex_buffer.size(); i++)         // 把不能reuse的部分删除
    //{
    //    if (find(E_ex.begin(), E_ex.end(), E_ex_buffer[i]) != E_ex.end()) continue;

    //    int id = (*containing_e)[E_ex_buffer[i]];

    //    res[E_ex_buffer[i]].erase(id);

    //    for (auto iter = (*LA)[E_ex_buffer[i]].begin(); iter != (*LA)[E_ex_buffer[i]].end(); iter++)
    //    {
    //        for (int j = 0; j < iter->second.size(); j++)
    //        {
    //            res[iter->second[j]].erase(id);
    //        }
    //    }
    //}
    //cout << "reuse finish!" << endl;
    return res;
}

vector<int> GAS(Graph& G, int b)   
{
    auto s = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();

    vector<int> followersres;
    vector<int> res;
    TreeNode rootnode;
    int max_layer = 0;
    int trussness_gain = 0;
    vector<bool> ifanchored(G.m, false);//记录已经anchor的边，不再计算
    vector<std::unordered_set<int>> reuseable_node(G.m);  //存放可以reuse的treenode

    vector<std::unordered_map<int, vector<int>>> collected_followers(G.m);  //存放followers
    vector<int> number(G.m, 0);  //存放对应的followers数量
    vector<bool> ifcomputed(G.m, false);


    vector<pair<int, int>> hull_pair;
    hull_pair = get_hull_pair(G, &max_layer);


    std::unordered_map<int, TreeNode> Tree;

 
    tree(G, G.edge2, -1, &Tree);
    
    vector<int> containing_e = Get_treenode_containing_e(Tree, G);
    vector<pair<vector<int>, vector<int>>>  SLA_PLA = GET_SLA_PLA(containing_e, G);
    // vector<unordered_map<int, vector<int>>> LA = Get_LA(G, containing_e);

    //cout << "data structure finished" << endl;
 
    //auto end = std::chrono::high_resolution_clock::now();



    while (b > 0)
    {


        // cout << b << endl;
        int max = 0;
        int select = -1;
        int avoid = 0;
        std::unordered_map<int, vector<int>> selected_followers;




        vector<edge_with_upperbound> follower_upper_bound_ordered_edge = get_follower_upper_bound(G, &Tree, SLA_PLA, collected_followers, reuseable_node, ifcomputed);

        
        for (int i = 0; i < follower_upper_bound_ordered_edge.size(); i++)
        {
            if (ifanchored[follower_upper_bound_ordered_edge[i].eid] == true) continue;
            //cout << i << endl;
            //若发现可以reuse，则不再计算
            if (ifcomputed[follower_upper_bound_ordered_edge[i].eid] == true)
            {
                if (number[follower_upper_bound_ordered_edge[i].eid] > max)
                {
                    selected_followers = collected_followers[follower_upper_bound_ordered_edge[i].eid];
                    max = number[follower_upper_bound_ordered_edge[i].eid];
                    select = follower_upper_bound_ordered_edge[i].eid;
                }
                continue;
            }

            if (follower_upper_bound_ordered_edge[i].upper_bound < max)
            {
                avoid = avoid + G.m - i;
                break;
            }
            //auto b= std::chrono::high_resolution_clock::now();
            std::unordered_map<int, vector<int>> followers;
            followers = Get_Followers_With_Triangle_Path(G, follower_upper_bound_ordered_edge[i].eid, Tree, containing_e, hull_pair, max_layer, reuseable_node[follower_upper_bound_ordered_edge[i].eid]);
            //auto e = std::chrono::high_resolution_clock::now();
            //cout << std::chrono::duration_cast<std::chrono::microseconds>(e - b).count() << endl;


            int follower_number = 0;

            follower_number = number[follower_upper_bound_ordered_edge[i].eid];
            for (auto iter = followers.begin(); iter != followers.end(); ++iter)  //处理followers
            {
                collected_followers[follower_upper_bound_ordered_edge[i].eid][iter->first] = iter->second;
                follower_number = follower_number + iter->second.size();
            }
            number[follower_upper_bound_ordered_edge[i].eid] = follower_number;
            ifcomputed[follower_upper_bound_ordered_edge[i].eid] = true;



            if (follower_number > max)
            {
                selected_followers = collected_followers[follower_upper_bound_ordered_edge[i].eid];
                max = follower_number;
                select = follower_upper_bound_ordered_edge[i].eid;
                //cout << max;
            }

            //输出本轮的计算进度
            //if (i % 200000 == 0) cout << i << " " << std::flush;
            //if (i % 1000000 == 0) cout << endl;
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



        //auto aaaa = std::chrono::high_resolution_clock::now();

        reuseable_node = follower_reuse_update(select, &G, &Tree, &SLA_PLA, &containing_e, selected_followers);
        //reuseable_node = follower_reuse(select, &G, &Tree, &SLA_PLA, &LA, &containing_e, selected_followers);
        //auto aaa = std::chrono::high_resolution_clock::now();
        //t = t + std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count();
        //int partical = 0;
        //int full = 0;
        //int unreuse = 0;
        //// 统计reuseable的点
        //for (int i = 0; i < G.m; i++)
        //{
        //    int reuse = 0;  //统计能reuse的node

        //    for (int j = 0; j < SLA_PLA[i].first.size(); j++)
        //    {

        //        if (reuseable_node[i].find(SLA_PLA[i].first[j]) != reuseable_node[i].end())
        //        {
        //            reuse++;
        //        }
        //    }

        //    if (reuse == SLA_PLA[i].first.size()) full++;
        //    if (reuse < SLA_PLA[i].first.size() && reuse != 0) partical++;
        //    if (reuse == 0 && SLA_PLA[i].first.size() != 0) unreuse++;
        //}

     /*   cout << " full reuseable edges: " << full << endl;
        cout << " partical reuseable edges: " << partical << endl;
        cout << " unreuseable edges: " << unreuse << endl;*/

        hull_pair = get_hull_pair(G, &max_layer);




        for (int i = 0; i < res.size(); i++)   //将已经anchor的hull pair 值设置为inifinity
        {
            hull_pair[res[i]].first = INF;
            hull_pair[res[i]].second = INF;
        }


        // 调整树结构以及相关数据结构  把不能reuse的result删掉
        for (int i = 0; i < G.m; i++)
        {
            vector<int> del;
            for (auto iter = collected_followers[i].begin(); iter != collected_followers[i].end(); iter++)
            {
                //若树的id变了，或者不能reuse，则从结果集中删除
                if (Tree.find(iter->first) == Tree.end() || reuseable_node[i].find(iter->first) == reuseable_node[i].end())
                {
                    del.push_back(iter->first);
                }
            }


            for (int j = 0; j < del.size(); j++)
            {
                number[i] = number[i] - collected_followers[i][del[j]].size();
                collected_followers[i].erase(del[j]);
            }
        }



        for (int i = 0; i < G.m; i++)//将不能reuse的部分 ifproccess设置为false，重新计算
        {
            if (ifcomputed[i] == false)
            {
                reuseable_node[i].clear();
                collected_followers[i].clear();
                number[i] = 0;
            }
            int j;
            bool ifreuse = true;
            for (j = 0; j < SLA_PLA[i].first.size(); j++)
            {

                if (reuseable_node[i].find(SLA_PLA[i].first[j]) == reuseable_node[i].end())
                {
                    number[i] = number[i] - collected_followers[i][SLA_PLA[i].first[j]].size();
                    collected_followers[i][SLA_PLA[i].first[j]].clear();
                    //break;
                    ifreuse = false;
                }
            }

            //for (auto it = reuseable_node[i].begin(); it!= reuseable_node[i].end(); it++)
            //{
                //if (find(SLA_PLA[i].first.begin(), SLA_PLA[i].first.end(), *it) == SLA_PLA[i].first.end())
                //{
                   // number[i] = number[i] - collected_followers[i][SLA_PLA[i].first[j]].size();
                   // collected_followers[i][SLA_PLA[i].first[j]].clear();
                    //break;
                    //ifreuse = false;
               // }
           // }

            if (ifreuse == false)
            {
                ifcomputed[i] = false;
            }
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

    return res;
}
