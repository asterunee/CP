class Solution {
public:
    int kthDigit(long long k) {
        if (k <= 9) {
            return k;
        }
        k -= 9; 
        long long p = 10; 
        for (int d = 2; ; d++,  p *= 10) {
            long long cnt = 9 * p * d; 
            if (k > cnt) {
                k -= cnt; 
                continue; 
            }
            long long len = 10LL * d, block = (k - 1) / len, pos = (k - 1) % len, base = p + block * 10, b = base / 10; 
            int idx = pos / d, digit = pos % d; 
            long long x; 
            if (b % 2 == 0) {
                x = base + idx; 
            } else {
                x = base + 9 - idx; 
            }
            return to_string(x)[digit] - '0'; 
        }
    }
};