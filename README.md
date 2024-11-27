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

To save space, we only provide four datasets in the folder "dataset" for testing, other datasets can be found on https://snap.stanford.edu/data/index.html

Folder "datasets/subgraphs" contains 20 subgraphs that extracted from facebook and brightkites.

| Datasets | \|V\| | \|E\| |   Link             |
|----------|----|----|----|
| College | 1,899 | 13,838 | https://snap.stanford.edu/data/CollegeMsg.html |
| Facebook | 4,039 | 88,234 | https://snap.stanford.edu/data/ego-Facebook.html  |
| Brightkite| 58,228 | 214,078 | https://snap.stanford.edu/data/loc-Brightkite.html   |
| Gowalla | 196,591 | 950,327 |  https://snap.stanford.edu/data/loc-Gowalla.html    |
| Youtube | 1,134,890 | 2,987,624 | https://snap.stanford.edu/data/com-Youtube.html    |
| Google |  875,713 | 4,322,051 | https://snap.stanford.edu/data/web-Google.html  |
| Patents | 3,774,768 | 16,518,947 | https://snap.stanford.edu/data/cit-Patents.html  |
| Pokec | 1,632,803 | 22,301,964 | https://snap.stanford.edu/data/soc-Pokec.html |
