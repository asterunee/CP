#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int A, B; 
    read(A, B); 
    bool ok = false; 
    ok |= A + B == 9; 
    ok |= A - B == 9; 
    ok |= A * B == 9; 
    ok |= A == B * 9; 
    print(ok ? "Nine" : "Nein"); 
}