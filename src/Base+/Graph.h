#pragma once
#pragma once
#pragma once
//
// Created by �ú貨 on 2023/3/8.
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
        // ʹ�ñ�׼��Ĺ�ϣ����
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
    vector<vector<int>> neighbor;//ָ��vector��ָ��g ��ָ��Ĵ洢��Ӧ�ڵ���ھӱ�š��洢���ǽڵ���     //�ڵ��Ŵ�0��ʼ���߱�Ŵ�0��ʼ
    unordered_map<pair<int, int>, int, PairHash, PairEqual> edge;//�洢�ߵ���Ϣ,pair<int int>Ϊһ���ߵ������˵�ı�ţ�map�е���һ������Ϊ�ߵı��,��ʾ�ڼ�����
    int n, m;  //��󶥵�ı��Ϊn���ߵ�����Ϊm
    int maxsup;
    vector<int> sup, trussness;   //�ߵ�֧�ֶȺ�truss
    int maxTrussness;
    vector<vector<int>> Truss_Table;
    vector<pair<int, int>> edge2;//�洢��  ����ͨ��edge2[eid]�ķ�ʽ���ʣ�ÿ���ߴ洢һ��
    vector<std::unordered_set<int>> neighbor_hash;   //ʹ�ù�ϣ��洢�ھӣ�ʱ�临�Ӷ�Ϊo��1��
    unordered_map<pair<int, int>, std::unordered_set<int>, PairHash, PairEqual> common_neighbor;
    int vertex_number;


public:
    Graph() {    //���캯��������һ��ͼ
        //cout << "empty graph constructed" << endl;
        maxsup = 0;
        maxTrussness = 0;
        m = 0;
        n = 0;
    }

    void Init_Graph(char* data_set) // ��ʼ��ͼ�е���������  ���иú�����public�е��������ݽ������㡣�����ݼ���ʼ��ͼ
    {
        BuildGraph(data_set);   //����neighbor   edge  edge2  n m;

        sup_calculation();   //����  maxsup  sup���顣

        caculation_Trussness();  //����� truss�� truss_table

        //cout << "trussness caculation time:" << std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count() << endl;
        common();


    }

    void Init_Graph(vector<pair<int, int>> edges) // ��ʼ��ͼ�е���������  ���иú�����public�е��������ݽ������㡣�����ݼ���ʼ��ͼ
    {
        BuildGraph(edges);   //����neighbor   edge  edge2  n m;
        sup_calculation();   //����  maxsup  sup���顣
        //auto start_SLA = std::chrono::high_resolution_clock::now();
        caculation_Trussness();  //����� truss�� truss_table
        //auto end_SLA = std::chrono::high_resolution_clock::now();
        common();
        //cout << "trussness caculation time:" << std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count() << endl;


    }

    void BuildGraph(vector<pair<int, int>> edges)   //�ñ߳�ʼ��ͼ
    {
        cout << "reading graph....." << endl;
        int f = 0;

        //������󶥵���Ϊn
        for (f = 0; f < edges.size(); f++)
        {
            int a = edges[f].first;
            int b = edges[f].second;
            if (a > n) n = a;
            if (b > n) n = b;   //n����ڵ������ֵ
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
            if (a == b) continue;  //����ָ���Լ��ı�
            if (a < b)  //��a>b
            {
                int t = a;
                a = b;
                b = t;
            }
            if (edge.find(pair<int, int>(a, b)) == edge.end())  //������Ŀǰ�����ڣ������ر�
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

                neighbor[a].push_back(b);  //��b��Ϊa���ھӴ洢��a���ھӶ�����
                neighbor[b].push_back(a);  //��a��Ϊb���ھӴ洢��b���ھӶ�����
                edge2[m] = pair<int, int>(a, b);
                edge[pair<int, int>(a, b)] = m;   //����������
                edge[pair<int, int>(b, a)] = m++;
            }
        }

        if (edge2.size() > m)//��Щ���ݼ��ı߻�洢���Σ�����edge2��sizeΪm�������������bug�����н�����Ĳ���ɾ��
        {
            edge2.erase(edge2.begin() + m, edge2.end());
        }


        trussness.resize(m);   //��ʼ��truss����
        sup.resize(m);     //��ʼ��sup����

        cout << "finishing reading!" << endl;
    }

    void BuildGraph(char* data_set)   //��ʼ��ͼ
    {
        cout << "reading graph....." << endl;
        FILE* f = fopen(data_set, "r");
        int a, b;
        vertex_number = 0;
        //������󶥵���Ϊn
        while (fscanf(f, "%d %d", &a, &b) != EOF) {
            if (a > n) n = a;
            if (b > n) n = b;   //n����ڵ������ֵ
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
            if (a == b) continue;  //����ָ���Լ��ı�
            if (a < b)  //��a>b
            {
                int t = a;
                a = b;
                b = t;
            }
            if (edge.find(pair<int, int>(a, b)) == edge.end())  //������Ŀǰ�����ڣ������ر�
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

                neighbor[a].push_back(b);  //��b��Ϊa���ھӴ洢��a���ھӶ�����
                neighbor[b].push_back(a);  //��a��Ϊb���ھӴ洢��b���ھӶ�����
                edge2[m] = pair<int, int>(a, b);
                edge[pair<int, int>(a, b)] = m;   //����������
                edge[pair<int, int>(b, a)] = m++;
            }
        }
        if (edge2.size() > m)//��Щ���ݼ��ı߻�洢���Σ�����edge2��sizeΪm�������������bug�����н�����Ĳ���ɾ��
        {
            edge2.erase(edge2.begin() + m, edge2.end());
        }


        trussness.resize(m);   //��ʼ��truss����
        sup.resize(m);     //��ʼ��sup����
        fclose(f);
        cout << "finishing reading!" << endl;
    }

    void sup_calculation()//����ͼ��֧�ֶȣ�����ʼ��support���ֵ  ����maxsup��
    {
        cout << "support is caculating!" << endl;
        for (int i = 0; i <= n; ++i)   //����ѭ�����ʴ洢���ھ���Ϣ������  i ��ʾ�ڼ����ڵ� neighbor[i][j]��ʾ��i���ڵ���ھ�
        {

            for (int j = 0; j < neighbor[i].size(); ++j) {
                int w = neighbor[i][j];  //w��ʾi��һ���ھ�
                if (i > w)  //�����ظ��߱���
                {
                    int eid = edge[pair<int, int>(i, w)];  //��(i,w)  eid����ߵı��
                    sup[eid] = 0; //����Ӧ�ߵ�sup��ʼ��Ϊ0
                    //degree(u)>degree(v)
                    int u = neighbor[i].size() > neighbor[w].size() ? i : w;  //�Ƚ�g[i]��g[w]�Ĺ�ģ����size����Ǹ�ֵ����u
                    int v = neighbor[i].size() > neighbor[w].size() ? w : i;  //sizeС�ĸ�ֵ��  v  �˲���Ϊ�˼��ٱ���ʱ�䣬ÿ��ֻ�����Ǹ�����С������

                    for (int k = 0; k < neighbor[v].size(); ++k)  //Ѱ�ҹ�������
                    {
                        if (edge.find(pair<int, int>(u, neighbor[v][k])) != edge.end())
                            //�ҵ���������  g[v][k]��ʾ��v���ھӣ���ôedge��v,g[v][k]��һ�����ڣ������ҵ�edge(u,g[v][k])���ڣ���ôg[v][k]�ͱ�ʾ�ǹ����㣬�ҵ�һ�������Ρ�
                            sup[eid]++;//���Ӷ�Ӧ��supportֵ
                    }
                    if (sup[eid] > maxsup)
                        maxsup = sup[eid];
                }
            }
        }
        cout << "caculating finished!" << endl;
        //return maxsup;
    }

    vector<int> k_truss(int K)   //����ָ��k��k-truss  �洢�߱��
    {
        queue<int> q;  //����
        vector<int> res;  //��������洢�����š�
        vector<bool> del(m, false);   //��Ǳ��Ƿ�ɾ�����߼���ɾ��
        vector<int> supbuffer;
        supbuffer = sup;
        for (int i = 0; i < m; ++i)    //��ʼ�����У���sup<k-2�ı��Ȳ������
        {
            if (supbuffer[i] < K - 2)
                q.push(i);
        }
        while (q.empty() == false)  //���в�Ϊ�գ�����ɾ��֧�ֶȲ�����ĵ�
        {
            int eid = q.front();  //��¼��ͷԪ��id
            q.pop();     //����
            if (del[eid] == true) continue;   //���ñ��Ѿ���ɾ��������������ѭ��
            del[eid] = true;      //û��ɾ��������Ϊ��ɾ��
            int x = edge2[eid].first;    //��¼��ɾ���Ķ˵�
            int y = edge2[eid].second;
            if (neighbor[x].size() < neighbor[y].size())   //�Ƚ����߶��������ھ�������ĸ���x
            {
                int t = x;
                x = y;
                y = t;
            }
            for (int k = 0; k < neighbor[y].size(); ++k)   //�����ھӽڵ�Ѱ����Ӱ��ıߣ�ֻ���뱻ɾ�߹��������εı�֧�ֶȼ�1
            {
                int w = neighbor[y][k];  //wΪy��һ���ھ�
                int e = edge[pair<int, int>(y, w)];  //eΪedge��y,w���ı�ţ��������ڸ���support
                if (del[e] == true) continue;
                if (edge.find(pair<int, int>(x, w)) != edge.end())  //���ҵ�������xyw���������� ������������֧�ֶȼ�1
                {
                    e = edge[pair<int, int>(x, w)];
                    if (del[e] == true) continue;
                    supbuffer[e]--;
                    if (supbuffer[e] < K - 2)    //���º���support<k-2���������й���һ��ɾ��
                        q.push(e);

                    e = edge[pair<int, int>(y, w)]; //������һ���ߵ�֧�ֶ�
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

    vector<int> computetruss(vector<int> anchored_edges)   //�������бߵ�trussness������һ����ά���飬�洢��trussness��Ϣ ����ֵ�� ��a[i][j]��ʾtrussnessΪi�ıߵĵ�j����     �ò��ϰ��벻���������ıߵķ���
        //�ú�������trussness����ı�ȫ�ֱ�����ֵ
    {
        vector<vector<int>> Trussness_Table(maxsup + 3);

        vector<int> supbuffer;

        vector<int> truss(m);

        supbuffer = sup;

        vector<int>* bin = new vector<int>[maxsup + 1];   //binΪָ��vector��ָ�룬ָ��bin[i]ָ�����֧�ֶ�Ϊi�ı߼���

        vector<std::unordered_set<int>> neig = neighbor_hash;

        for (int i = 0; i < m; ++i)
        {
            if (find(anchored_edges.begin(), anchored_edges.end(), i) == anchored_edges.end())
                bin[supbuffer[i]].push_back(i);
        }


        vector<bool> del(m, false);
        for (int i = 0; i <= maxsup; ++i) {
            for (int j = 0; j < bin[i].size(); ++j) {
                int eid = bin[i][j];      //��֧�ֶ���С�ı߿�ʼ����
                if (del[eid] == true) continue;
                else {
                    truss[eid] = i + 2;


                }


                del[eid] = true;   //ɾ��������
                int x = edge2[eid].first;
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size()) {
                    int t = x;
                    x = y;
                    y = t;
                }
                for (auto iter = neig[y].begin(); iter != neig[y].end(); ++iter)   //Ѱ����(x,y)���������εıߣ�����֧�ֶȼ�1
                {
                    int w = *iter;
                    int e = edge[pair<int, int>(y, w)];
                    if (del[e] == true) continue;
                    if (neig[x].find(w) != neig[x].end())  //xyw����������
                    {
                        e = edge[pair<int, int>(x, w)];
                        if (del[e] == true) continue;
                        if (supbuffer[e] > i) {
                            supbuffer[e]--;
                            if (find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end()) //��anchor�߾Ͳ�push��Ͱ�У���Զ��ɾ��
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

    vector<vector<int>> computetrussness(vector<int> anchored_edges)   //�������бߵ�trussness������һ����ά���飬�洢��trussness��Ϣ ����ֵ�� ��a[i][j]��ʾtrussnessΪi�ıߵĵ�j����     �ò��ϰ��벻���������ıߵķ���
        //�ú�������trussness����ı�ȫ�ֱ�����ֵ
    {
        auto time = 0;
        queue<int> q;  //����
        vector<int> res;  //��������洢�����š�
        vector<bool> del(m, false);   //��Ǳ��Ƿ�ɾ�����߼���ɾ��
        vector<int> supbuffer;   //�洢���Ƶ�sup�����ݣ������Ƕ�ԭͼ��sup����ֱ�Ӳ�����


        int K = 2;
        supbuffer = sup;
        vector<int> truss(m + 1);
        vector<vector<int>> alltrussness(maxsup + 2);   //�洢���ս�� trussness���ᳬ��maxsup+2
        for (int l = K; l <= maxTrussness + 2; l++) {

            for (int i = 0; i < m; ++i)    //��ʼ�����У���sup<k-2�ı��Ȳ������
            {
                if (supbuffer[i] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), i) == anchored_edges.end()) //��ɾ��anchored edge.
                    q.push(i);
            }
            //auto start_SLA = std::chrono::high_resolution_clock::now();
            while (q.empty() == false)  //���в�Ϊ�գ�����ɾ��֧�ֶȲ�����ĵ�
            {

                int eid = q.front();  //��¼��ͷԪ��id
                q.pop();     //����
                if (del[eid] == true) continue;//���ñ��Ѿ���ɾ��������������ѭ��
                else {
                    //truss[eid]=l-1;
                    alltrussness[l - 1].push_back(eid);   // ���ڸ���ɾ���б�ɾ����ñߵ�trussnessΪl-1
                    //cout<<l-1<<"   "<<eid<<endl;
                    del[eid] = true;      //û��ɾ��������Ϊ��ɾ��
                }

                int x = edge2[eid].first;    //��¼��ɾ���Ķ˵�
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size())   //�Ƚ����߶��������ھ�������ĸ���x
                {
                    int t = x;
                    x = y;
                    y = t;
                }

                for (int k = 0; k < neighbor[y].size(); ++k)   //�����ھӽڵ�Ѱ����Ӱ��ıߣ�ֻ���뱻ɾ�߹��������εı�֧�ֶȼ�1  �ñ����е�kΪСд
                {
                    int w = neighbor[y][k];  //wΪy��һ���ھ�
                    int e = edge[pair<int, int>(y, w)];  //eΪedge��y,w���ı�ţ��������ڸ���support
                    if (del[e] == true) continue;
                    if (edge.find(pair<int, int>(x, w)) != edge.end())  //���ҵ�������xyw���������� ������������֧�ֶȼ�1
                    {
                        e = edge[pair<int, int>(x, w)];
                        if (del[e] == true) continue;
                        supbuffer[e]--;
                        if (supbuffer[e] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end())    //���º���support<k-2���������й���һ��ɾ��
                            q.push(e);

                        e = edge[pair<int, int>(y, w)]; //������һ���ߵ�֧�ֶ�
                        supbuffer[e]--;
                        if (supbuffer[e] < l - 2 && std::find(anchored_edges.begin(), anchored_edges.end(), e) == anchored_edges.end())
                            q.push(e);
                    }
                }


            }    //һ��ɾ������



        }
        //  cout << time << endl;
        return alltrussness;
    }

    void caculation_Trussness()  //����truss_decomposition�������бߵ�trussness����Ӧ�ߵ�trussness�洢��trussness������   ������ÿ�δ���sup��С�ı�   ͬʱ����һ����ά���飬a[i][j]��ʾtrussnessΪi�ĵ�j���ߵ�id;
        //ͬʱ��ʼ��ȫ�ֱ���tussness����Ϣ
    {
        cout << "trussness is caculating!" << endl;
        vector<vector<int>> Trussness_Table(maxsup + 3);

        vector<int> supbuffer;

        supbuffer = sup;

        vector<int>* bin = new vector<int>[maxsup + 1];   //binΪָ��vector��ָ�룬ָ��bin[i]ָ�����֧�ֶ�Ϊi�ı߼���

        vector<std::unordered_set<int>> neig = neighbor_hash;

        for (int i = 0; i < m; ++i)
            bin[supbuffer[i]].push_back(i);

        vector<bool> del(m, false);
        for (int i = 0; i <= maxsup; ++i) {
            for (int j = 0; j < bin[i].size(); ++j) {
                int eid = bin[i][j];      //��֧�ֶ���С�ı߿�ʼ����
                if (del[eid] == true) continue;
                else {
                    trussness[eid] = i + 2;
                    Trussness_Table[i + 2].push_back(eid);
                    if (trussness[eid] > maxTrussness) {
                        maxTrussness = trussness[eid];

                    }
                }


                del[eid] = true;   //ɾ��������
                int x = edge2[eid].first;
                int y = edge2[eid].second;
                if (neighbor[x].size() < neighbor[y].size()) {
                    int t = x;
                    x = y;
                    y = t;
                }
                //vector<int> comm;
                for (auto iter = neig[y].begin(); iter != neig[y].end(); ++iter)   //Ѱ����(x,y)���������εıߣ�����֧�ֶȼ�1
                {
                    int w = *iter;
                    int e = edge[pair<int, int>(y, w)];
                    if (del[e] == true) continue;
                    if (neig[x].find(w) != neig[x].end())  //xyw����������
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
