﻿#define _CRT_SECURE_NO_WARNINGS
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


int main()
{
    Graph G;
    char dataset[100];
    char* a = "/home/qhb/dataset/sub6_facebook.txt";

    strcpy(dataset, a);
    //cout<<strlen(dataset)<<endl;
    G.Init_Graph(dataset);

    exact(G, 2); //input G and budget
    return 0;
}