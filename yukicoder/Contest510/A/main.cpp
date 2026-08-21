#include "library/template.hpp"
using namespace std; 
using namespace suisen; 

signed main() {
    vector<int> A(3); 
    read(A); 
    sort(ALL(A)); 
    print(A[0] + A[1] > A[2] ? "Yes" : "No"); 
}