#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    ll A, B; 
    char op; 
    read(A, op, B); 
    print(op == '+' ? A + B : A - B); 
}