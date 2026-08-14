class Solution {
public:
    int maximumLengthSubstring(string s) {
        int cnt[26] = {}; 
        int ans = 0, l = 0; 
        for (int r = 0; r < s.size(); ++r) {
            ++cnt[s[r] - 'a']; 
            while (cnt[s[r] - 'a'] > 2) {
                --cnt[s[l] - 'a']; 
                ++l; 
            }
            ans = max(ans, r - l + 1); 
        }
        return ans; 
    }
};