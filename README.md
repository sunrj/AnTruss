# AnchorEdge

## Running Format

The source code of our paper are in the folder "src".

The source code of each algorithm has two parameters: graph $G$ and budget $b$.

* Running example for Algorithms GAS

  Take **GAS** as an example, we first replace the datasets which is need to be tested, and set budget in main.cpp.
  > g++ -std=c++11 main.cpp -o program -I.
  >
  > ./GAS
  > 
## Dependencies 

Software: Ubuntu 22.04.4 LTS, kernel version 5.15.0-72-generic, GCC 8.5.0, Python 3.

Hardware: Intel(R)Xeon(R)5218R 2.10GHz CPU and 256GB memory.

## Datasets

To save space, we only provide four datasets in the folder "dataset" for testing, other datasets can be found on https://snap.stanford.edu/data/index.html

Folder "datasets/subgraphs" contains 20 subgraphs that are extracted from Facebook and Brightkites.


| Datasets | \|V\| | \|E\| |   Link |
|----------|----|----|----|
| College | 1,899 | 13,838 | https://snap.stanford.edu/data/CollegeMsg.html |
| Facebook | 4,039 | 88,234 | https://snap.stanford.edu/data/ego-Facebook.html  |
| Brightkite| 58,228 | 214,078 | https://snap.stanford.edu/data/loc-Brightkite.html   |
| Gowalla | 196,591 | 950,327 |  https://snap.stanford.edu/data/loc-Gowalla.html    |
| Youtube | 1,134,890 | 2,987,624 | https://snap.stanford.edu/data/com-Youtube.html    |
| Google |  875,713 | 4,322,051 | https://snap.stanford.edu/data/web-Google.html  |
| Patents | 3,774,768 | 16,518,947 | https://snap.stanford.edu/data/cit-Patents.html  |
| Pokec | 1,632,803 | 22,301,964 | https://snap.stanford.edu/data/soc-Pokec.html |
