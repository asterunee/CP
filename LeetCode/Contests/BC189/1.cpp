class Solution {
public:
    int elevatorRequests(int n, vector<int>& requests) {
        int ans = 0, cur = 0; 
        for (int x : requests) {
            ans += abs(x - cur); 
            cur = x; 
        }
        return ans; 
    }
};