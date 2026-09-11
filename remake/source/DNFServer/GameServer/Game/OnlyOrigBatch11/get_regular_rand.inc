// ORIG T 0x086b1c7c：12 次 get_rand_int(100) 求和 → /100（截断）→
// 距中值 (n/2) 的偏移 * 0.95f 截断后 + n（0x3f733333=0.95f；原码久错 0.45f 已改）。
int get_regular_rand(int n)
{
    int rounds = 0xc;
    int sum = 0;
    int centered = 0;
    float factor = 0.95f;
    int i = 0;
    for (; i < rounds; ++i)
        sum += get_rand_int(100);
    sum /= 100;
    centered = sum - (rounds / 2);
    centered = static_cast<int>(static_cast<float>(centered) * factor);
    return centered + n;
}