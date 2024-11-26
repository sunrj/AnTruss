#pragma once

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

vector<int> greedy_anchor(Graph& G, int b) //base�㷨��̰�Ŀ�� ����truss decomposition����trussness gain
{
    auto start_SLA = std::chrono::high_resolution_clock::now();
    int sum_trussness = 0;
    vector<int> anchorset;

    while (anchorset.size() < b)  //ÿ��ѡ��һ���������ı�
    {
        cout << "round " << 100-b << endl;
        int eid = -1;
        int max = 0;
        vector<int> trussnessbuffer;  //�ݴ����б�anchore���trussness
        trussnessbuffer = G.computetruss(anchorset);//���¼���anchoredһ���ߺ�� trussness
        vector<int> followers_before;
        followers_before = compute_anchor_followers(G, anchorset, trussnessbuffer);


        for (int i = 0; i <= G.m; i++)   //������ÿһ������Ϊanchoredset������������
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