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

int main() {

    Graph p1;
    char dataset[100];
    const char* a = "/home/qhb/dataset/college.txt";//select_dataset(stoi(argv[1]));

    strcpy(dataset, a);
    //cout<<strlen(dataset)<<endl;
    auto start = std::chrono::high_resolution_clock::now();
    p1.Init_Graph(dataset);

    cout << "edge size: " << p1.edge.size() / 2 << endl;
    cout << "maximal support: " << p1.maxsup << endl;
    cout << "maximal trussness:" << p1.maxTrussness << endl;
    cout << "edge size:" << p1.m << endl;
    cout << "node size:" << p1.vertex_number << endl;

    GAS(p1, 100);  
    auto end = std::chrono::high_resolution_clock::now();
    cout << "total running time " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << endl;
    return 0;

}
