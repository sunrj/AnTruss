#pragma once
#include <random>
#include <set>

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

std::vector<int> getRandomNonRepeatingIndicesFromTop20Percent(const std::vector<int>& data, int b) { 
    if (data.empty()) {
        std::cerr << "Vector is empty." << std::endl;
        return {}; // 返回一个空向量或抛出异常
    }

    // 计算需要选择的元素数量
    int numElementsToSelect = static_cast<int>(0.2 * data.size());

    if (numElementsToSelect == 0) {
        std::cerr << "Vector is too small to select from the top 20%." << std::endl;
        return {}; // 返回一个空向量或抛出异常
    }

    // 复制数据，以便不修改原始向量
    std::vector<int> sortedData = data;

    // 使用部分排序找到前20%最大的元素
    std::partial_sort(sortedData.begin(), sortedData.begin() + numElementsToSelect, sortedData.end(), std::greater<int>());

    // 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());

    // 随机选择b个不重复元素的下标
    std::vector<int> selectedIndices;
    std::set<int> selectedIndicesSet;
    while (selectedIndices.size() < b) {
        std::uniform_int_distribution<int> dist(0, numElementsToSelect - 1);
        int randomIndex = dist(gen);

        // 确保不重复选择相同的元素
        if (selectedIndicesSet.insert(randomIndex).second) {
            selectedIndices.push_back(randomIndex);
        }
    }

    return selectedIndices;
}


int max_support(Graph G, int b)//选取支持度最大的b条边进行anchor
{
    vector<int> anchor_set = getRandomNonRepeatingIndicesFromTop20Percent(G.sup, b);

    vector<int> tru = G.computetruss(anchor_set);
    int res = compute_anchor_followers(G, anchor_set, tru).size();

    return res;
}