# AnchorEdge

## Running Format

The source code of our paper are in the folder "src".

The source code of each algorithm has five parameters, corresponding to the algorithm name (e.g., **VFree**), the dataset name (e.g., **D5**), the two size constraints $\tau_U$ and $\tau_V$ (e.g., **5 4**), and the frequency constraint &lambda; (e.g., **3**).

* Running example for Algorithms **VFree** and **VFree-**

  Take **VFree** as an example, on dataset **D5** with parameters $\tau_U$ = **5**, $\tau_V$ = **4** and &lambda; = **3**

  > g++ -std=c++11 RecurrenceGroup.cpp -O3 -o VFree
  >
  > ./VFree VFree D5 5 4 3


* Running example for Algorithms **FilterV**, **FilterV-**, **FilterV-FR**, **FilterV-VM** and **BK-ALG+**

  Take **FilterV** as an example, on dataset **D5** with parameters $\tau_U$ = **5**, $\tau_V$ = **4** and &lambda; = **3**

  > g++ -std=c++11 main.cpp graph.cpp -O3 -o FilterV
  >
  > ./FilterV FilterV D5 5 4 3


## Datasets

We provide five datasets in the folder "sample_dataset" for testing, D2 to D6.
