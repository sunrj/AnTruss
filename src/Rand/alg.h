#pragma once
#include <random>

vector<int> compute_anchor_followers(Graph& G, vector<int> anchoreid, vector<int>& trussness)  //计算anchorset的所有followers.
{
    vector<int> res;
    for (int i = 0; i < trussness.size(); i++)
    {
        if (trussness[i] != G.trussness[i] && std::find(anchoreid.begin(), anchoreid.end(), i) == anchoreid.end())
        {
            for (int j = 0; j < trussness[i] - G.trussness[i]; j++)  //若有边trussness为提升为2以上，则存对应边的条数,res的size就是trussness gain
            {
                res.push_back(i);
                // cout << "follower " << i << "trussness " << G.trussness[i] << endl;

            }

        }
    }
    return res;
}


int Radom(Graph& G, int b)  //随机选择b条边作为anchor边  利用一次trussdecomposition计算trussness gain  返回trussness gain
{
    Graph g = G;
    vector<int> anchor_set;
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎

    // 定义一个整数分布，范围是边的id
    std::uniform_int_distribution<> dis(0, G.m);

    // 生成随机数


    while (b > 0)  //随机选择b个anchor边
    {
        int random_number = dis(gen);
        anchor_set.push_back(random_number);
        b--;
    }

    vector<int> tru = G.computetruss(anchor_set);//计算 anchor随机边后的  trussness

    int res = compute_anchor_followers(G, anchor_set, tru).size();
    return res;

}