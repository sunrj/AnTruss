#pragma once
#include <random>

vector<int> compute_anchor_followers(Graph& G, vector<int> anchoreid, vector<int>& trussness)  //����anchorset������followers.
{
    vector<int> res;
    for (int i = 0; i < trussness.size(); i++)
    {
        if (trussness[i] != G.trussness[i] && std::find(anchoreid.begin(), anchoreid.end(), i) == anchoreid.end())
        {
            for (int j = 0; j < trussness[i] - G.trussness[i]; j++)  //���б�trussnessΪ����Ϊ2���ϣ�����Ӧ�ߵ�����,res��size����trussness gain
            {
                res.push_back(i);
                // cout << "follower " << i << "trussness " << G.trussness[i] << endl;

            }

        }
    }
    return res;
}


int Radom(Graph& G, int b)  //���ѡ��b������Ϊanchor��  ����һ��trussdecomposition����trussness gain  ����trussness gain
{
    Graph g = G;
    vector<int> anchor_set;
    std::random_device rd;
    std::mt19937 gen(rd()); // ʹ�� Mersenne Twister ����

    // ����һ�������ֲ�����Χ�Ǳߵ�id
    std::uniform_int_distribution<> dis(0, G.m);

    // ���������


    while (b > 0)  //���ѡ��b��anchor��
    {
        int random_number = dis(gen);
        anchor_set.push_back(random_number);
        b--;
    }

    vector<int> tru = G.computetruss(anchor_set);//���� anchor����ߺ��  trussness

    int res = compute_anchor_followers(G, anchor_set, tru).size();
    return res;

}