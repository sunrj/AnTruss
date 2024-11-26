#pragma once

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

vector<int> greedy_anchor(Graph& G, int b) //base算法，贪心框架 利用truss decomposition计算trussness gain
{
    auto start_SLA = std::chrono::high_resolution_clock::now();
    int sum_trussness = 0;
    vector<int> anchorset;

    while (anchorset.size() < b)  //每次选择一条提升最大的边
    {
        cout << "round " << 100-b << endl;
        int eid = -1;
        int max = 0;
        vector<int> trussnessbuffer;  //暂存所有边anchore后的trussness
        trussnessbuffer = G.computetruss(anchorset);//重新计算anchored一条边后的 trussness
        vector<int> followers_before;
        followers_before = compute_anchor_followers(G, anchorset, trussnessbuffer);


        for (int i = 0; i <= G.m; i++)   //计算以每一条边作为anchoredset所带来的提升
        {

            if (std::find(anchorset.begin(), anchorset.end(), i) == anchorset.end())
            {
                vector<int> anchor_buffer(anchorset);
                vector<int> followers_after;
                anchor_buffer.push_back(i);
                vector<int> trussness_after;
                trussness_after = G.computetruss(anchor_buffer);
                followers_after = compute_anchor_followers(G, anchor_buffer, trussness_after);

                cout << "process edge " << i<<" trussness gain " << followers_after.size() - followers_before.size() << endl;
                if (followers_after.size() > followers_before.size() && followers_after.size() - followers_before.size() > max)
                {
                    max = followers_after.size() - followers_before.size();
                    
                    eid = i;

                }

            }


        }



        //t = t + std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count();
        auto end_SLA = std::chrono::high_resolution_clock::now();
        cout << std::chrono::duration_cast<std::chrono::microseconds>(end_SLA - start_SLA).count() << endl;
        cout << anchorset.size() << endl;
        cout << eid << endl;
        cout << "max" << max << endl;
        b--;
        anchorset.push_back(eid);
    }

    return anchorset;
}