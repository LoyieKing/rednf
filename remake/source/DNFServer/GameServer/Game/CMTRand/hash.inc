unsigned long CMTRand::hash(long a, long b)
{
    static unsigned long differ = 0;

    unsigned long hashA = 0;
    const unsigned char* p = (const unsigned char*)&a;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        // [b28d-L14 实证] ORIG 0x080da1d6 分两句（先 `(hashA<<8)+hashA` 落槽，
        // 再 `add p[i],hashA`），循环变量 unsigned（setbe）。
        hashA = hashA + (hashA << 8);
        hashA += p[i];
    }

    unsigned long hashB = 0;
    p = (const unsigned char*)&b;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        hashB = hashB + (hashB << 8);
        hashB += p[i];
    }

    return (differ++ + hashA) ^ hashB;
