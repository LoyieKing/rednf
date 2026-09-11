// ORIG 0x0809e473：开头即 `subl $0x1,0xc(%ebp)`（--len），循环三分支各自
// `sarl $0x2,-0xc(%ebp)`（同一 r 槽内存直移）且各自重算一次 r % 3；
// %26 走无符号除法（mul 魔术数 0x4ec4ec4f），%10 走有符号除法
// （imul 0x66666667 + sar 修正）⇒ nAlphabetCount 为 unsigned 型常量。
// ORIG 另发射局部静态 `_ZZ20GenerateRandomStringPciE14nAlphabetCount`
// （.rodata 4B，值 0x1a）⇒ 必须保留 static const 声明。
void GenerateRandomString(char* out, int len)
{
    static const unsigned int nAlphabetCount = 26;
    --len;
    for (int i = 0; i < len; ++i)
    {
        int r = std::rand();
        if (r % 3 == 0)
        {
            r >>= 2;
            out[i] = (char)('A' + r % nAlphabetCount);
        }
        else if (r % 3 == 1)
        {
            r >>= 2;
            out[i] = (char)('a' + r % nAlphabetCount);
        }
        else
        {
            r >>= 2;
            out[i] = (char)('0' + r % 10);
        }
    }
    out[len] = '\0';
}