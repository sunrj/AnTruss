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
    int id;        //����id
    int K;         //���ڵ��ڱߵ�trusssnessֵ
    int P;         //���ĸ��ڵ�
    vector<int> C;         //����Ҷ�ӽڵ�
    vector<int> E; //�������ı�
    vector<pair<int, int>> edges;  //�洢����ı�
};

vector<int> findCommonNeighbors(const std::vector<std::unordered_set<int>*>& neighbor, int node1, int node2) {
    vector<int> commonNeighbors;

    // ���ڵ�1�ͽڵ�2�Ƿ���Ч
    if (node1 >= 0 && node1 < neighbor.size() && node2 >= 0 && node2 < neighbor.size()) {
        const std::unordered_set<int>* neighbors1 = neighbor[node1];
        const std::unordered_set<int>* neighbors2 = neighbor[node2];

        // �ҵ������ڵ�Ĺ����ھ�
        for (int neighbor : *neighbors1) {
            if ((*neighbors2).find(neighbor) != (*neighbors2).end()) {
                commonNeighbors.push_back(neighbor);
            }
        }
    }

    return commonNeighbors;
}   //�ҹ�ϣ����������Ĺ����ھ�

vector<int> findCommonNeighbors(const std::vector<std::unordered_set<int>>& neighbor, int node1, int node2) {
    vector<int> commonNeighbors;

    // ���ڵ�1�ͽڵ�2�Ƿ���Ч
    if (node1 >= 0 && node1 < neighbor.size() && node2 >= 0 && node2 < neighbor.size()) {
        const std::unordered_set<int> neighbors1 = neighbor[node1];
        const std::unordered_set<int> neighbors2 = neighbor[node2];

        // �ҵ������ڵ�Ĺ����ھ�
        for (int neighbor : neighbors1) {
            if ((neighbors2).find(neighbor) != (neighbors2).end()) {
                commonNeighbors.push_back(neighbor);
            }
        }
    }

    return commonNeighbors;
}   //�ҹ�ϣ����������Ĺ����ھ�  // ����

void tree(Graph& G, vector<pair<int, int>>& edge, int rootid, std::unordered_map<int, TreeNode>* Tree) //������������ͨ����ͼ
{
    //cout << a << endl;
    //a++;
    //vector<TreeNode> res;
    if (edge.size() == 0)//��ͼΪ�գ���ֱ�ӷ��ؿ���
    {
        return;
    }

    //p = ((*Tree).back());
    //auto aaaa = std::chrono::high_resolution_clock::now();
    //vector<int> del;
    int K_min = G.trussness[G.edge.find(edge[0])->second];  //��ʼ����Сֵ
    for (int i = 0; i < edge.size(); i++)  //����trussness����Сֵ
    {
        //int eid = G.edge.find(edge[i])->second;
        int trussness = G.trussness[G.edge.find(edge[i])->second];


        if (trussness < K_min)
        {
            K_min = G.trussness[G.edge.find(edge[i])->second];
        }

    }
    if (K_min == INF)  //anchor edge�������κ����ڵ�
    {
        return;
    }



    vector<std::unordered_set<int>*>  neighbor(G.n + 1);


    for (int i = 0; i < edge.size(); i++)
    {
        if (neighbor[edge[i].first] == nullptr)  //��ָ��ָ��գ����ʾ�ô��ھӽڵ�û�г�ʼ�������ʼ���������
        {
            neighbor[edge[i].first] = new std::unordered_set<int>();
            neighbor[edge[i].first]->insert(edge[i].second);
        }
        else
        {
            neighbor[edge[i].first]->insert(edge[i].second);
        }

        if (neighbor[edge[i].second] == nullptr)  //ͬ��
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
    queue<pair<int, int>> Q;   //������ȱ�������
    for (int i = 0; i < edge.size(); i++)  //����ͼ���й����������
    {


        int eid = G.edge.find(edge[i])->second;
        if (visited[eid] == 0)//��������δ���ʹ�   ��Ӹñ߿�ʼ�Ǹ��µ���������ͨ����ͼ
        {
            TreeNode node;
            node.K = K_min;
            neighbor[edge[i].first]->erase(edge[i].second);   //���ʹ���ɾ���ھ���Ϣ�������������Ч����
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
            vector<pair<int, int>> edge_next_iteration;   //�洢��һ�ֵݹ�ı�
            visited[eid] = 1;  //���ó��ѷ���
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


            Q.push(edge[i]);  //��ӣ��Ӹ�Ԫ�ؽ��з���
            while (Q.size() != 0)
            {
                pair<int, int> e = Q.front();
                Q.pop();

                int x = e.first;
                int y = e.second;


                neighbor[y]->erase(x);   //���ʹ���ɾ���ھ���Ϣ�������������Ч����
                neighbor[x]->erase(y);


                vector<int> commonneighbor = findCommonNeighbors(neighbor, x, y);


                for (int j = 0; j < commonneighbor.size(); j++)  //�������������
                {
                    int w = commonneighbor[j];
                    int e_1 = G.edge[pair<int, int>(x, w)];
                    int e_2 = G.edge[pair<int, int>(y, w)];
                    if (visited[e_1] == 0)//���ñ�δ������
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


                        visited[e_1] = 1;  //���ó��ѷ���
                    }

                    if (visited[e_2] == 0)//���ñ�δ������
                    {
                        Q.push(pair<int, int>(y, w));
                        if (G.trussness[e_2] == K_min)
                        {
                            node.E.push_back(e_2);
                            node.edges.push_back(pair<int, int>(y, w));
                            if (id == -1)  //id=-1��ʾû�б���ֵ��
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


                        visited[e_2] = 1;  //���ó��ѷ���
                    }
                }

            }     //���������һ��

            //��������������ͨ��component������һ�ֵݹ�
            if (node.edges.size() > 0)
            {
                node.id = id;
                if (rootid != -1)  (*Tree).find(rootid)->second.C.push_back(node.id);  //�ýڵ㲻�Ǹ��ڵ�
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


    //�ͷ��ڴ�
    for (int i = 0; i < neighbor.size(); ++i) {
        delete neighbor[i];
    }
    //return res;
}

vector<pair<int, int>> get_hull_pair(Graph& G, int(*max))    //����ͼ�����бߵ�hull_pair
{
    auto time = 0;

    vector<pair<int, int>> res(G.m);                 //�洢�����

    vector<pair<int, int>> edges = G.edge2;     //���Ʊ�

    vector<int> sup = G.sup;

    vector<bool> del(G.m, false);    //�����ñ��Ƿ�ɾ��

    //for (int k = 3; k <= G.maxTrussness + 1; k++)
    for (int k = 3; k <= G.Truss_Table.size(); k++)
    {



        std::unordered_set<int> K_hull;
        K_hull.insert(G.Truss_Table[k - 1].begin(), G.Truss_Table[k - 1].end());   //�õ�k-1 hull �����бߵı߱��

        int layer = 1;      //��¼����

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

            //������Ӱ��ı�֧�ֶ�
            while (Q.size() > 0)
            {
                int a = Q.front();
                K_hull.erase(a);
                del[a] = true;    //�ñ߱��Ϊɾ��

                Q.pop();



                int x = G.edge2[a].first;
                int y = G.edge2[a].second;


                // if (G.neighbor[x].size() > G.neighbor[y].size()) {
                   //  int t = x;
                    // x = y;
                   //  y = t;
                 //}

                for (auto iter = G.common_neighbor[pair<int, int>(x, y)].begin(); iter != G.common_neighbor[pair<int, int>(x, y)].end(); iter++)   //�����ܵ�Ӱ���sup
                {
                    int w = *iter;
                    int e1 = G.edge[pair<int, int>(x, w)];
                    if (del[e1] == true) continue;         //��һ�����ѱ�ɾ������ʲô������

                    //if (neighbor_hash[y].find(*iter) != neighbor_hash[y].end())    //�ҵ������㣬һ��������
                    {
                        int e2 = G.edge[pair<int, int>(y, w)];
                        if (del[e2] == true) continue;      //�߱�ɾ�������ʾ���������Ѿ�������
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

struct edge_with_upperbound   //����upper bound��Ϣ�ı�,��������
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

vector<int> Get_treenode_containing_e(std::unordered_map<int, TreeNode>& Tree, Graph& G)  // ��ȡÿ�������ڵ�treenode id                              //������ݽṹ:������e����id
{
    //auto aaaa = std::chrono::high_resolution_clock::now();
    //cout << "fffkcu" << endl;
    vector<int> res(G.m + 1);   //��ʼ��һ����СΪͼ�ı�������res����
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
    vector<pair<vector<int>, vector<int>>> res(G.m);    //�洢�����vector

    //vector<std::unordered_set<int>> neig = G.neighbor_hash;
    for (int i = 0; i < G.m; i++)
    {
        if (G.trussness[i] == INF) continue;

        int x = G.edge2[i].first;
        int y = G.edge2[i].second;

        //if (G.neighbor[x].size() > G.neighbor[y].size())   //�Ƚ����߶��������ھ�������ĸ���x
       // {
          //  int t = x;
          //  x = y;
          //  y = t;
       // }
        for (auto iter = G.common_neighbor[G.edge2[i]].begin(); iter != G.common_neighbor[G.edge2[i]].end(); iter++)
        {
            int w = *iter;
            //if (G.neighbor_hash[y].find(w) != G.neighbor_hash[y].end())  //�ҵ�������
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
                if (G.trussness[eid_1] >= G.trussness[i] && G.trussness[eid_2] < G.trussness[i] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_2]) == res[i].second.end())  //trussnessһ��һС
                {
                    res[i].second.push_back(node_containing_e[eid_2]);
                }
                if (G.trussness[eid_2] >= G.trussness[i] && G.trussness[eid_1] < G.trussness[i] && find(res[i].second.begin(), res[i].second.end(), node_containing_e[eid_1]) == res[i].second.end())  //trussnessһ��һС
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

vector<edge_with_upperbound> get_follower_upper_bound(Graph& G, std::unordered_map<int, TreeNode>* Tree, vector<pair<vector<int>, vector<int>>>& SLA_PLA, vector<std::unordered_map<int, vector<int>>>& collected_followers, vector<std::unordered_set<int>>& reuseable_node, vector<bool>& ifcomputed)   //��ȡһ���ߵ�followers�������Ͻ�ֵ
{
    vector<edge_with_upperbound> edges(G.m);

    for (int i = 0; i < SLA_PLA.size(); i++)
    {
        int sum = 0;


        for (int j = 0; j < SLA_PLA[i].first.size(); j++)
        {
            //auto aaaa = std::chrono::high_resolution_clock::now();
            if (reuseable_node[i].find(SLA_PLA[i].first[j]) != reuseable_node[i].end() && ifcomputed[i] == true)   //������ĳ���ڵ��follower����reuse���򽫾����follower��������upper bound������ȷ,���Ҹñߵ�followers���������
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

inline int get_support_upper_bound(Graph& G, int eid, vector<pair<int, int>>& hull_pair, std::unordered_set<int>& suv, bool* discard)     //����֧�ֶ��Ͻ�
{


    pair<int, int> edge = G.edge2[eid];   //��ȡ��

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


    for (auto iter = comm.begin(); iter != comm.end(); iter++)    // �����ñߵ����й����ھ�
    {
        //cout << comm.size() << endl;
        int w = *iter;    //wΪ��x��һ���ھ�
        //if (G.neighbor_hash[y].find(w) != G.neighbor_hash[y].end())    //�ҵ�һ��������
        {

            int eid1 = G.edge[pair<int, int>(x, w)];

            int eid2 = G.edge[pair<int, int>(y, w)];



            bool inner_1 = false;    //��ע�����������Ƿ����ڲ��
            bool suv_1 = false;      //�Ƿ�������ı�
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




            if ((inner_1 || suv_1) && (inner_2 || suv_2))             //�����߶����㣬��support upperbound++
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

void shrink(Graph& G, int discard_edge, std::unordered_set<int>* suv, bool* discard, int* sup, vector<pair<int, int>>& hull_pair, int K, int anchor_edge, vector<bool>(*used_to_suv), vector<bool>(*proccessed))  //�ݹ鷽ʽ�������ıߵ�support��
{

    pair<int, int> edge = G.edge2[discard_edge];   //��ȡ��
    int x = edge.first;
    int y = edge.second;
    //if (G.neighbor[x].size() > G.neighbor[y].size()) {
        //int t = x;
        //x = y;
        //y = t;
   // }
   // std::unordered_set<int> comm = G.common_neighbor[edge];


    for (auto iter = G.common_neighbor[edge].begin(); iter != G.common_neighbor[edge].end(); iter++)  // �����ñߵ����й����ھ�
    {
        int w = *iter;    //wΪ��x��һ���ھ�

        // if (discard[e1] == true&&(*proccessed)[e1]==false) continue;   //�����Ѿ���discrd����������β��ô���
       // if (G.edge.find(pair<int, int>(y, w)) != G.edge.end())    //�ҵ�һ��������
        {



            int e1 = G.edge[pair<int, int>(x, w)];
            int e2 = G.edge[pair<int, int>(y, w)];

            // if (G.trussness[e1] < G.trussness[anchor_edge] || G.trussness[e2] < G.trussness[anchor_edge]) continue;

            if (!((discard[e2] == true && (*proccessed)[e2] == false) || (discard[e1] == true && (*proccessed)[e1] == false)))
            {
                //if(discard[e1]==false&& (*proccessed)[e1] == false&& discard[e2] == true && (*proccessed)[e2] == false)

                if (hull_pair[e1].first > hull_pair[e2].first)  //e1 ��trussnessС��������
                {
                    int temp = e1;
                    e1 = e2;
                    e2 = temp;       //����ֵ
                }



                if (hull_pair[e1].first == hull_pair[e2].first && hull_pair[discard_edge].first == hull_pair[e1].first && e1 != anchor_edge && e2 != anchor_edge)   //������trussness����ͬ�����
                {




                    if (hull_pair[e1].second <= hull_pair[discard_edge].second && hull_pair[e2].second <= hull_pair[discard_edge].second)//���������߲�����ͬ������discard edge ��� �򱾲�
                    {
                        if (hull_pair[e1].second < hull_pair[e2].second)    //��������layer�д���С����e1��С���Ǹ�  //��ֻ����e1
                        {
                            if ((*suv).find(e1) != (*suv).end() && e1 != anchor_edge && discard[e2] == false)  //�����߶�suv������£�������
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

                        if (hull_pair[e1].second > hull_pair[e2].second)    //��������layer�д���С����e2��С���Ǹ�  //��ֻ����e2
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

                        if (hull_pair[e1].second == hull_pair[e2].second && discard[e1] == false && discard[e2] == false)//��������߲�����ͬ
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



                    if (hull_pair[e1].second <= hull_pair[discard_edge].second && hull_pair[e2].second > hull_pair[discard_edge].second)//���������߲���һ��һС��e1��discard�����㣬e2�ڸ��ڲ�
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

                    if (hull_pair[e2].second <= hull_pair[discard_edge].second && hull_pair[e1].second > hull_pair[discard_edge].second)//���������߲���һ��һС��һ����discard�����㣬һ���ڸ��ڲ�
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

                    if (hull_pair[e1].second > hull_pair[discard_edge].second && hull_pair[e2].second > hull_pair[discard_edge].second && (*used_to_suv)[discard_edge] == true && discard[e1] == false && discard[e2] == false)   //�����ڲ�  �ⲿ�ָо��е����⣬����������û��bug
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

                if (hull_pair[e1].first < hull_pair[e2].first && hull_pair[discard_edge].first == hull_pair[e1].first && e1 != anchor_edge && e2 != anchor_edge)  //��һ����trussness����
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

                if (anchor_edge == e1 && hull_pair[e2].first == hull_pair[discard_edge].first)  //�����������trussness���  ��һ��Ϊanchor edge
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

                if (anchor_edge == e2 && hull_pair[e1].first == hull_pair[discard_edge].first)  //�����������trussness���  ��һ��Ϊanchor edge
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

inline std::unordered_map<int, vector<int>> Get_Followers_With_Triangle_Path(Graph& G, int& anchor_edge, std::unordered_map<int, TreeNode>& TCT, vector<int>& containing_e, vector<pair<int, int>>& hull_pair, int& max_layer, std::unordered_set<int>& reuseable_node)    //��ĳһ��anchor edge��followers
{


    const int size = G.m + 1;
    //vector<std::unordered_set<int>> neighbor_hash = G.neighbor_hash;

    std::unordered_map<int, vector<int>> res;     //�洢����ıߣ�Ҳ���Ǹ�����,��follower�������ṹ���з���


    map<int, priority_queue<Edge>> classified_edges;   //��һ��ֵΪtree��id  �ڶ���Ϊ��Ӧlayer�� �ı߼���

    bool* ifexist = (bool*)mmap(nullptr, size * sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    //vector<bool> ifexist(size, false);
    //bool* ifexist = new bool[size]();
    //bool ifexist[G.m];  //��Ǹñ����Ѿ��ڴ�����ļ�����  ��ϣ�洢���ټ���
    //memset(ifexist, false, sizeof(ifexist));

    int x = G.edge2[anchor_edge].first;
    int y = G.edge2[anchor_edge].second;

    for (int i = 0; i < G.neighbor[x].size(); i++)   //������anchor�������εı߰����ṹ����
    {
        int w = G.neighbor[x][i];
        if (G.edge.find(pair<int, int>(y, w)) != G.edge.end())   //�ҵ�һ��������
        {
            int e_1 = G.edge[pair<int, int>(x, w)];
            int e_2 = G.edge[pair<int, int>(y, w)];


            if (hull_pair[e_1].first > hull_pair[e_2].first)  //e_1��trussnessС��������
            {
                int temp = e_1;
                e_1 = e_2;
                e_2 = temp;       //����ֵ
            }


            if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_1].first == hull_pair[anchor_edge].first)      //�������ε�����������trussness��anchor��һ����  
            {
                if (hull_pair[e_1].second > hull_pair[anchor_edge].second && hull_pair[e_2].second > hull_pair[anchor_edge].second)  //��������������anchor_edge���ڲ�
                {

                    ifexist[e_1] = true;
                    ifexist[e_2] = true;
                    Edge e1, e2;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    e2.id = e_2;
                    e2.layer = hull_pair[e_2].second;
                    if (classified_edges.find(containing_e[anchor_edge]) == classified_edges.end())   //��map��û��
                    {

                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        buffer.push(e2);
                        classified_edges[containing_e[anchor_edge]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[anchor_edge]].push(e1);  //���ñߴ����Ӧ����
                        classified_edges[containing_e[anchor_edge]].push(e2);  //���ñߴ����Ӧ����
                    }



                }
            }


            if (hull_pair[anchor_edge].first == hull_pair[e_1].first && hull_pair[anchor_edge].first < hull_pair[e_2].first)                // ������trussness��С��anchor_edge=e_1  e_2>e_1  e_2>anchor_edge
            {
                if (hull_pair[anchor_edge].second < hull_pair[e_1].second)    //������anchoredge���ڲ�
                {
                    ifexist[e_1] = true;
                    Edge e1;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    if (classified_edges.find(containing_e[anchor_edge]) == classified_edges.end())   //��map��û��
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);

                        classified_edges[containing_e[anchor_edge]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[anchor_edge]].push(e1);  //���ñߴ����Ӧ����
                    }
                }
            }

            if (hull_pair[anchor_edge].first < hull_pair[e_1].first && hull_pair[anchor_edge].first < hull_pair[e_2].first)    //���������ߵ�trussness����anchoredge��
            {
                if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_2].first != INF)  //���������ߵ�trussnessһ����
                {
                    ifexist[e_1] = true;
                    ifexist[e_2] = true;
                    Edge e1, e2;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    e2.id = e_2;
                    e2.layer = hull_pair[e_2].second;
                    if (classified_edges.find(containing_e[e_1]) == classified_edges.end())   //��map��û��
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[containing_e[e_1]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_1]].push(e1);  //���ñߴ����Ӧ����
                    }

                    if (classified_edges.find(containing_e[e_2]) == classified_edges.end())   //��map��û��
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e2);
                        classified_edges[containing_e[e_2]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_2]].push(e2);  //���ñߴ����Ӧ����
                    }

                }

                if (hull_pair[e_1].first < hull_pair[e_2].first)   //���������ߵ�trussnessһ��һС
                {
                    ifexist[e_1] = true;
                    Edge e1;
                    e1.id = e_1;
                    e1.layer = hull_pair[e_1].second;
                    if (classified_edges.find(containing_e[e_1]) == classified_edges.end())   //��map��û��
                    {
                        priority_queue<Edge> buffer;
                        buffer.push(e1);
                        classified_edges[containing_e[e_1]] = buffer;
                    }
                    else
                    {
                        classified_edges[containing_e[e_1]].push(e1);  //���ñߴ����Ӧ����
                    }
                }
            }

        }
    }
    //�������ÿ�����ڵ��path




    //bool discard[G.m];
    //bool visited[G.m];

    //memset(discard, false, sizeof(discard));
    //memset(visited, false, sizeof(visited));
    //memset(s, 0, sizeof(s));


    int a = 0;
    std::unordered_set<int> suv;            //�Թ�ϣ�����ʽ�洢���ı�
    //unordered_set<int> discard;
    //unordered_set<int> visited;

    for (auto iter = classified_edges.begin(); iter != classified_edges.end(); iter++)   //һ�δ���һ��treenode
    {


        int treeid = iter->first;   //�õ�treeid
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
        //bool* visited = new bool[size]();      //��Ǹñ��Ƿ����
        //int* s = new int[size]();
       // std::fill(discard, discard + G.m+1, false);
       // std::fill(visited, visited + G.m+1, false);
       // std::fill(s, s + 100, 0);



        visited[anchor_edge] = true;
        suv.insert(anchor_edge);   //����anchor edgeΪ���.
        //vector<int> s(G.m + 1, 0);                    //�洢�ߵ�support upper bound, ��ʼ��Ϊ0


        //����layer by layer search


        int K;

        //priority_queue<Edge> Q;
        K = G.trussness[classified_edges[treeid].top().id];

        while (classified_edges[treeid].size() != 0)
        {

            int e = classified_edges[treeid].top().id;
            classified_edges[treeid].pop();



            // cout << e << endl;
   
            int sup = get_support_upper_bound(G, e, hull_pair, suv, discard);
 



            if (sup >= K - 1)                       //��support upper���ı�����Ҫ������ñ�
            {

                // auto start_SLA = std::chrono::high_resolution_clock::now();
                if (visited[e] == false)             //��ֹ�ظ�����
                {
                    suv.insert(e);                     //����suv����
                    //����strong triangle
                    s[e] = sup;
                    int u = G.edge2[e].first;
                    int v = G.edge2[e].second;

                    //if (G.neighbor_hash[v].size() < G.neighbor_hash[u].size())   //�Ƚ����߶��������ھ�������ĸ���x
                    //{
                       // int t = v;
                      //  v = u;
                       // u = t;
                    //}
                    //vector<pair<int, int>> del;

                    for (auto ite = G.common_neighbor[pair<int, int>(u, v)].begin(); ite != G.common_neighbor[pair<int, int>(u, v)].end(); ite++)   //������anchor�������εı߰����ṹ����
                    {
                        int w = *ite;
                        //if (G.neighbor_hash[v].find(w) != G.neighbor_hash[v].end())   //�ҵ�һ��������
                        {
                            int e_1 = G.edge[pair<int, int>(u, w)];
                            int e_2 = G.edge[pair<int, int>(v, w)];




                            if (hull_pair[e_1].first > hull_pair[e_2].first)  //e_1��trussnessС��������
                            {
                                int temp = e_1;
                                e_1 = e_2;
                                e_2 = temp;       //����ֵ
                            }
                            Edge e1, e2;
                            e1.id = e_1;
                            e1.layer = hull_pair[e_1].second;
                            e2.id = e_2;
                            e2.layer = hull_pair[e_2].second;

                            if (hull_pair[e_1].first == hull_pair[e_2].first && hull_pair[e_1].first == hull_pair[e].first)      //�������ε�����������trussness��e��һ����  
                            {
                                if (hull_pair[e_1].second > hull_pair[e].second && hull_pair[e_2].second > hull_pair[e].second)  //��������������anchor_edge���ڲ�
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //���ñߴ����Ӧ����
                                    }

                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);  //���ñߴ����Ӧ����
                                    }
                                }

                                if (hull_pair[e_1].second == hull_pair[e].second && hull_pair[e_2].second > hull_pair[e].second && discard[e_1] == false)   //weak triangle �����
                                {
                                    if (visited[e_2] == false && ifexist[e_2] == false)
                                    {
                                        ifexist[e_2] = true;
                                        classified_edges[treeid].push(e2);  //���ñߴ����Ӧ����
                                    }

                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);
                                    }

                                }

                                if (hull_pair[e_2].second == hull_pair[e].second && hull_pair[e_1].second > hull_pair[e].second && discard[e_2] == false)   //weak triangle �����
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //���ñߴ����Ӧ����
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

                            if (hull_pair[e].first == hull_pair[e_1].first && hull_pair[e].first < hull_pair[e_2].first)                // ������trussness��С��anchor_edge=e_1  e_2>e_1  e_2>e
                            {
                                if (hull_pair[e].second < hull_pair[e_1].second)    //������anchoredge���ڲ�
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //���ñߴ����Ӧ����
                                    }

                                }

                                if (hull_pair[e].second == hull_pair[e_1].second)    //������anchoredge�ı���
                                {
                                    if (visited[e_1] == false && ifexist[e_1] == false)
                                    {
                                        ifexist[e_1] = true;
                                        classified_edges[treeid].push(e1);  //���ñߴ����Ӧ����
                                    }

                                }
                            }


                        }       //��������

                    }


                    visited[e] = true;
                    // Q.erase(std::remove(Q.begin(), Q.end(), eid_maxsup), Q.end());    //������ʹ�Q������ɾ��

                }
                //auto end_SLA = std::chrono::high_resolution_clock::now();





            }

            else   //��support upper���ı߶�������Ҫ����Q�����������ı�����Ϊ����
            {

                discard[e] = true;   //������supportcheck�ı߽�������
                s[e] = -1;  //����Ϊ����
                // cout << e << endl;
                 //vector<bool> proccessed(G.m, false);

                vector<bool> used_to_suv(G.m + 1, false);
                vector<bool> proccessed(G.m + 1, false);
                //memset(used_to_suv, false, sizeof(used_to_suv));
                //memset(proccessed, false, sizeof(proccessed));

                // cout << "root discard:" << e << "  " << hull_pair[e].first << "  " << hull_pair[e].second << endl;
                shrink(G, e, &suv, discard, s, hull_pair, K, anchor_edge, &used_to_suv, &proccessed);



                //������Ϊdiscard�߶��ܵ�Ӱ���support.

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

void Update_trussness(Graph* G, int anchor_edge, std::unordered_map<int, vector<int>> followers)//����anchorһ���ߺ��followers
{
    (*G).Truss_Table[(*G).trussness[anchor_edge]].erase(remove((*G).Truss_Table[(*G).trussness[anchor_edge]].begin(), (*G).Truss_Table[(*G).trussness[anchor_edge]].end(), anchor_edge), (*G).Truss_Table[(*G).trussness[anchor_edge]].end());
    (*G).trussness[anchor_edge] = INF;   //��anchor edge��trussnessֵ��Ϊ�����
    (*G).sup[anchor_edge] = INF;   //��anchor�ߵ�֧�ֶ�����Ϊ�����


    for (auto iter = followers.begin(); iter != followers.end(); iter++)  //����trussness��trussness table
    {
        for (int i = 0; i < iter->second.size(); i++)
        {
            int trussness = (*G).trussness[iter->second[i]];
            //����trussTable
            (*G).Truss_Table[trussness].erase(remove((*G).Truss_Table[trussness].begin(), (*G).Truss_Table[trussness].end(), iter->second[i]), (*G).Truss_Table[trussness].end());
            (*G).Truss_Table[trussness + 1].push_back(iter->second[i]);


            (*G).trussness[iter->second[i]]++;     //����followers��trussness
            if ((*G).trussness[iter->second[i]] > (*G).maxTrussness)
            {
                (*G).maxTrussness = (*G).trussness[iter->second[i]];
            }
        }
    }

}

vector<std::unordered_set<int>> follower_reuse_update(int anchor_edge, Graph* G, std::unordered_map<int, TreeNode>* TCT, vector<pair<vector<int>, vector<int>>>* SLA_PLA, vector<int>* containing_e, std::unordered_map<int, vector<int>>& followers)   //����ѡ���anchor edge�������ṹ�������������Ҫ���¼����followers
{

   
    vector<std::unordered_set<int>> res((*G).m);   //���ÿ��edge��ſ���reuse�����ڵ�


    for (int i = 0; i < (*G).m; i++)//��ʼ��res
    {
        for (int j = 0; j < (*SLA_PLA)[i].first.size(); j++)
        {
            res[i].insert((*SLA_PLA)[i].first[j]);
        }

    }

    unordered_set<int> un_reauseable_id; //�洢���в���reuse��node id
    //�� sla(anchor_edge) �еıߵ�PLA �еı���sla(anchor_edge)��������ɾ��
    vector<int> E_ex;

    bool a = false; //���sla��anchor���Ƿ���� containinge[anchor]
    for (int i = 0; i < (*SLA_PLA)[anchor_edge].first.size(); i++)   //�Ѽ�sla��ex�������б�
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
    //for (int i = 0; i < E_ex.size(); i++)         // �Ѳ���reuse�Ĳ���ɾ��
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
    //����trussness ��layer,�ع�����

    //auto aaaa = std::chrono::high_resolution_clock::now();
    //�Ѽ���anchor edge�������ڵ�Ϊ���������ı�
    vector<pair<int, int>> edges;
    int rootid = (*TCT).find((*containing_e)[anchor_edge])->second.P;
    if (rootid != -1)  //���ýڵ㲻�Ǹ��ڵ�
    {
        (*TCT).find(rootid)->second.C.erase(remove((*TCT).find(rootid)->second.C.begin(), (*TCT).find(rootid)->second.C.end(), (*containing_e)[anchor_edge]), (*TCT).find(rootid)->second.C.end());   //ɾ��child��Ϣ
    }

    //std::unordered_map<int, TreeNode> tree_buffer;  //�洢ԭ�ȵ����ڵ�;
    queue<int> Q;    //����Ѽ�
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
    //������follower�ĺϲ��ᵼ��id�����仯��������ˣ��ǲ��ùܡ�

    tree((*G), edges, rootid, TCT);   //�ع�����


    //cout << "tree finish!" << endl;
    //auto aaa = std::chrono::high_resolution_clock::now();
    //t=t+ std::chrono::duration_cast<std::chrono::microseconds>(aaa - aaaa).count();
    //auto aaaa = std::chrono::high_resolution_clock::now();

    //���¼���SLA_PLA  ��LA  containing_e



    (*containing_e) = Get_treenode_containing_e(*TCT, *G);
  


    (*SLA_PLA) = GET_SLA_PLA(*containing_e, *G);



    vector<int> E_ex_buffer;
    vector<bool> if_insert((*G).m, false); //��ֹ�����ظ�����

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
    //for (int i = 0; i < E_ex_buffer.size(); i++)         // �Ѳ���reuse�Ĳ���ɾ��
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

vector<int> GAS(Graph& G, int b)   //������tree��path����candidate��֦��upper bound���㷨
{
    auto s = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();

    vector<int> followersres;
    vector<int> res;
    TreeNode rootnode;
    int max_layer = 0;
    int trussness_gain = 0;
    vector<bool> ifanchored(G.m, false);//��¼�Ѿ�anchor�ıߣ����ټ���
    vector<std::unordered_set<int>> reuseable_node(G.m);  //��ſ���reuse��treenode

    vector<std::unordered_map<int, vector<int>>> collected_followers(G.m);  //���followers
    vector<int> number(G.m, 0);  //��Ŷ�Ӧ��followers����
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


        //����ÿ���ߵ�follower upper bound�������Ӵ�С��˳����


        vector<edge_with_upperbound> follower_upper_bound_ordered_edge = get_follower_upper_bound(G, &Tree, SLA_PLA, collected_followers, reuseable_node, ifcomputed);

        
        for (int i = 0; i < follower_upper_bound_ordered_edge.size(); i++)
        {
            if (ifanchored[follower_upper_bound_ordered_edge[i].eid] == true) continue;
            //cout << i << endl;
            //�����ֿ���reuse�����ټ���
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
            for (auto iter = followers.begin(); iter != followers.end(); ++iter)  //����followers
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

            //������ֵļ������
            //if (i % 200000 == 0) cout << i << " " << std::flush;
            //if (i % 1000000 == 0) cout << endl;
        }
        cout << endl;

        if (select == -1) break;  //ÿ���߶�û��followers��ֱ�ӷ���

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
        int partical = 0;
        int full = 0;
        int unreuse = 0;
        //// ͳ��reuseable�ĵ�
        //for (int i = 0; i < G.m; i++)
        //{
        //    int reuse = 0;  //ͳ����reuse��node

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




        for (int i = 0; i < res.size(); i++)   //���Ѿ�anchor��hull pair ֵ����Ϊinifinity
        {
            hull_pair[res[i]].first = INF;
            hull_pair[res[i]].second = INF;
        }


        // �������ṹ�Լ�������ݽṹ  �Ѳ���reuse��resultɾ��
        for (int i = 0; i < G.m; i++)
        {
            vector<int> del;
            for (auto iter = collected_followers[i].begin(); iter != collected_followers[i].end(); iter++)
            {
                //������id���ˣ����߲���reuse����ӽ������ɾ��
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



        for (int i = 0; i < G.m; i++)//������reuse�Ĳ��� ifproccess����Ϊfalse�����¼���
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