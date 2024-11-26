#include <cstdio>
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
    char* a = "src/datasets/graphname+.txt";     //select_dataset(stoi(argv[1]));

    strcpy(dataset, a);
    //cout<<strlen(dataset)<<endl;
    p1.Init_Graph(dataset);

  
    tup(p1);
    
    return 0;
}