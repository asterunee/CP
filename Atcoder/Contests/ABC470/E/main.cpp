#include "library/template.hpp"

using namespace std;
using namespace suisen;
double prv[205][205]; 
double dp[205][205]; 
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N, L; 
    read(N, L); 
    vector<long long> A(N); 
    read(A); 
    REP(life, 1, L + 1) {
        REP(a, N + 1) {
            REP(b, N - a + 1) {
                dp[a][b] = 0; 
            }
        }
        REP(a, N + 1) {
            REP(b, N - a + 1) {
                if (a == 0) {
                    dp[a][b] = b; 
                    continue; 
                }
                double tot = 2 * a + b; 
                double res = 0; 
                if (b) {
                    res += b / tot * (1 + dp[a][b - 1]); 
                }
                double p = 2.0 * a / tot; 
                double rem =tot - 1; 
                double nxt = 0; 
                nxt += 1.0 / rem * (1 + dp[a - 1][b]); 
                if (b && life >= 2) {
                    nxt += b / rem * (1 + prv[a - 1][b]); 
                }
                if (a >= 2 && life >= 2) {
                    nxt += (2.0 * a - 2) / rem * prv[a - 2][b + 2]; 
                }
                dp[a][b] = res + p * nxt; 
            }
        }
        REP(a, N + 1) {
            REP(b, N + 2) {
                prv[a][b] = dp[a][b]; 
            }
        }
    }
    double sum = 0; 
    for (long long x : A) {
        sum += x; 
    }
    double ans = sum * prv[N][0] / N; 
    cout << fixed << setprecision(10) << ans << '\n'; 
}