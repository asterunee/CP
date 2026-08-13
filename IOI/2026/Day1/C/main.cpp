#include "library/template.hpp"
#include "tiling.h"
using namespace std; 
using namespace suisen; 

namespace {
    int up, down; 
    vector<int> lft, rgt; 
    void shrink() {
        while (up <= down && lft[up] > rgt[up]) {
            ++up; 
        }
        while (up <= down && lft[down] > rgt[down]) {
            --down; 
        }
    }
}
void init(int N, int M) {
    up = 0; 
    down = N - 1; 
    lft.assign(N, 0); 
    rgt.assign(N, M - 1); 
}
pair<int, int> receive_block(int TL, int TR, int BL, int BR) {
    int c[4] = {TL, TR, BL, BR}; 
    int k = 0; 
    while (c[k]) {
        ++k; 
    }
    bool bottom = k >= 2; 
    bool right = k & 1; 
    int i = bottom ? up : down; 
    int j; 
    if (right) {
        j = lft[i]++; 
    } else {
        j = rgt[i]--; 
    }
    shrink(); 
    return {2 * i, 2 * j}; 
}