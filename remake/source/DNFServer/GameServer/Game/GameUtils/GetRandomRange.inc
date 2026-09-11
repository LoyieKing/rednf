// ---- GetRandomRange（ORIG W 0x08539d90 _Z14GetRandomRangeii）----
// a<b: rand()%(b-a+1)+a；b<a: rand()%(a-b+1)+b（C 有符号取模语义）。
int GetRandomRange(int a, int b)
{
    if (b < a)
        return (int)((rand() % (a - b + 1)) + b);
    return (int)((rand() % (b - a + 1)) + a);
}