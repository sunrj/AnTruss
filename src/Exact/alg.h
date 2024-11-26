#pragma once
#include <functional>
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

int generateRandomNumber(int max) {
    // ����һ�����������������
    std::random_device rd; // ʹ��Ӳ����Դ��������
    std::mt19937 gen(rd()); // Mersenne Twister α���������������
    std::uniform_int_distribution<int> distribution(0, max); // ������ȷֲ�����Χ��[0, max]

    // ���������
    int randomNum = distribution(gen);
    return randomNum;
}

Graph getsubGraph(Graph& G)//�������ͼ
{
    Graph res;

    //���ѡ��һ���ڵ�
    int nodeid = generateRandomNumber(G.n + 1);

    vector<int> node;
    vector<pair<int, int>> edge;
    queue<int> Q;

    Q.push(nodeid);

    while (node.size() < 25)
    {
        int n = Q.front();
        Q.pop();
        node.push_back(n);



        for (int i = 0; i < G.neighbor[n].size(); i++)
        {
            Q.push(G.neighbor[n][i]);
        }
    }



    for (int i = 0; i < node.size(); i++)  //����Щ��֮��ı�
    {
        for (int j = 0; j < node.size(); j++)
        {
            if (i == j) continue;

            if (G.edge.find(pair<int, int>(node[i], node[j])) != G.edge.end())
            {
                edge.push_back(pair<int, int>(node[i], node[j]));
            }
        }
    }

    res.Init_Graph(edge);

    return res;
}  

void writeSubGraphsToFile(const std::vector<std::pair<int, int>>& data, const std::string& filename)//�����ͼд�� 
{
    std::ofstream outFile(filename);

    if (!outFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    for (const auto& pair : data) {
        outFile << pair.first << " " << pair.second << std::endl;
    }

    outFile.close();
}  


std::vector<std::vector<int>> enumerateCombinations(std::vector<int>& nums, int b) {
    std::vector<std::vector<int>> result;
    std::vector<int> currentCombo;

    // ���������������������
    std::function<void(int, int)> generateCombinations = [&](int index, int remaining) {
        if (remaining == 0) {
            result.push_back(currentCombo);
            return;
        }

        for (int i = index; i < nums.size(); ++i) {
            currentCombo.push_back(nums[i]);
            generateCombinations(i + 1, remaining - 1);
            currentCombo.pop_back();
        }
        };

    generateCombinations(0, b);
    return result;
}

vector<int> compute_anchor_followers(Graph& G, vector<int> anchoreid, vector<int>& trussness)  //����anchorset������followers.
{
    vector<int> res;
    for (int i = 0; i < trussness.size(); i++)
    {
        if (trussness[i] != G.trussness[i] && std::find(anchoreid.begin(), anchoreid.end(), i) == anchoreid.end())
        {
            for (int j = 0; j < trussness[i] - G.trussness[i]; j++)  //���б�trussnessΪ����Ϊ2���ϣ�����Ӧ�ĸ���
            {
                res.push_back(i);
                // cout << "follower " << i << "trussness " << G.trussness[i] << endl;

            }

        }
    }
    return res;
}

vector<int> exact(Graph& G, int b)
{
    auto s = std::chrono::high_resolution_clock::now();
    //vector<int> followers;
    vector<int> edgeid(G.m);

    for (int i = 0; i < G.m; i++)
        edgeid[i] = i;

    vector<std::vector<int>> combined = enumerateCombinations(edgeid, b);

    vector<int> trussnessbuffer;  //�ݴ����б�anchore���trussness
    //trussnessbuffer = G.computetruss(anchorset);//���¼���anchoredһ���ߺ�� trussness
    vector<int> followers;

    int max = 0;
    long long sel = -1;
    for (long long i = 0; i < combined.size(); i++)
    {

        trussnessbuffer = G.computetruss(combined[i]);
        followers = compute_anchor_followers(G, combined[i], trussnessbuffer);
        if (followers.size() > max)
        {
            max = followers.size();
            sel = i;
        }

    }
    cout << "trussness gain=" << max << endl;
    cout << "b" << b << endl;
    auto e = std::chrono::high_resolution_clock::now();
    cout << std::chrono::duration_cast<std::chrono::microseconds>(e - s).count() << endl;
    return combined[sel];
}