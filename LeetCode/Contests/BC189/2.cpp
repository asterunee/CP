class Solution {
public:
    int minOperations(string s) {
        int n = s.size(); 
        int ans = INT_MAX; 
        for (int r = 0; r < n; r++) {
            int cur = r; 
            for (int i = 0; i < n / 2; i++) {
                int a = s[(i + r) % n] - 'a'; 
                int b = s[(n - 1 - i + r) % n] - 'a'; 
                int d = abs(a - b); 
                cur += min(d, 26 - d); 
            }
            ans = min(ans, cur); 
        }
        return ans; 
    }
};