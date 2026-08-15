class Solution {
public:
    long long elevatorRequests(int n, int start, vector<int>& requests) {
        bool has = false;
        for (int x : requests)
            if (x == start) has = true;

        vector<int> a = requests;
        if (!has) a.push_back(start);
        sort(a.begin(), a.end());

        int m = requests.size();
        int s = lower_bound(a.begin(), a.end(), start) - a.begin();
        int sz = a.size();

        const long long INF = 4e18;

        vector<vector<array<long long, 2>>> dp(
            sz, vector<array<long long, 2>>(sz, {INF, INF})
        );

        dp[s][s][0] = dp[s][s][1] = 0;

        for (int len = 1; len <= sz; len++) {
            for (int l = 0; l + len - 1 < sz; l++) {
                int r = l + len - 1;
                if (l > s || r < s) continue;

                int got = r - l + 1 - (!has);
                int rem = m - got;

                if (l > 0) {
                    dp[l - 1][r][0] = min(
                        dp[l - 1][r][0],
                        dp[l][r][0] + 1LL * (a[l] - a[l - 1]) * rem
                    );

                    dp[l - 1][r][0] = min(
                        dp[l - 1][r][0],
                        dp[l][r][1] + 1LL * (a[r] - a[l - 1]) * rem
                    );
                }

                if (r + 1 < sz) {
                    dp[l][r + 1][1] = min(
                        dp[l][r + 1][1],
                        dp[l][r][0] + 1LL * (a[r + 1] - a[l]) * rem
                    );

                    dp[l][r + 1][1] = min(
                        dp[l][r + 1][1],
                        dp[l][r][1] + 1LL * (a[r + 1] - a[r]) * rem
                    );
                }
            }
        }

        return min(dp[0][sz - 1][0], dp[0][sz - 1][1]);
    }
};