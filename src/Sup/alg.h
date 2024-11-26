#pragma once
#include <random>
#include <set>

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

std::vector<int> getRandomNonRepeatingIndicesFromTop20Percent(const std::vector<int>& data, int b) { 
    if (data.empty()) {
        std::cerr << "Vector is empty." << std::endl;
        return {}; // ����һ�����������׳��쳣
    }

    // ������Ҫѡ���Ԫ������
    int numElementsToSelect = static_cast<int>(0.2 * data.size());

    if (numElementsToSelect == 0) {
        std::cerr << "Vector is too small to select from the top 20%." << std::endl;
        return {}; // ����һ�����������׳��쳣
    }

    // �������ݣ��Ա㲻�޸�ԭʼ����
    std::vector<int> sortedData = data;

    // ʹ�ò��������ҵ�ǰ20%����Ԫ��
    std::partial_sort(sortedData.begin(), sortedData.begin() + numElementsToSelect, sortedData.end(), std::greater<int>());

    // ��ʼ�������������
    std::random_device rd;
    std::mt19937 gen(rd());

    // ���ѡ��b�����ظ�Ԫ�ص��±�
    std::vector<int> selectedIndices;
    std::set<int> selectedIndicesSet;
    while (selectedIndices.size() < b) {
        std::uniform_int_distribution<int> dist(0, numElementsToSelect - 1);
        int randomIndex = dist(gen);

        // ȷ�����ظ�ѡ����ͬ��Ԫ��
        if (selectedIndicesSet.insert(randomIndex).second) {
            selectedIndices.push_back(randomIndex);
        }
    }

    return selectedIndices;
}


int max_support(Graph G, int b)//ѡȡ֧�ֶ�����b���߽���anchor
{
    vector<int> anchor_set = getRandomNonRepeatingIndicesFromTop20Percent(G.sup, b);

    vector<int> tru = G.computetruss(anchor_set);
    int res = compute_anchor_followers(G, anchor_set, tru).size();

    return res;
}