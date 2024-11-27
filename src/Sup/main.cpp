#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <cstdio>
#include <vector>
#include <map>
#include <queue>
#include<iostream>
#include <fstream>
#include "Graph.h"
#include "alg.h"
#include <algorithm>
#include <string.h>

using namespace std;

int main()
{

    Graph p1;
    char dataset[100];
    const char* a = "src/datasets/graphname+.txt";     //select_dataset(stoi(argv[1]));
    strcpy(dataset, a);
    //cout<<strlen(dataset)<<endl;
    p1.Init_Graph(dataset);


    
    for (int j = 20; j <= 100; j = j + 20)
    {
        int max = 0;
        for (int i = 0; i < 2000; i++)
        {
            int f = max_support(p1, j);
            cout << f << endl;
            if (f > max) max = f;
        }
        cout << "budget= " << j << endl;
        cout << "max=: " << max << endl;
    }
    return 0;
}
