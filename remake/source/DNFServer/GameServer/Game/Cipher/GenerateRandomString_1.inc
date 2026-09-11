// ORIG @0x0809e58c（wchar_t 版；ORIG 全库无调用点，但符号必须存在）：
// 结构与 char 版逐条同形，仅元素写宽度 4 字节（out[i] 走 shl $2 缩放），
// 尾写 `movl $0,(%eax)` = out[len] = L'\0'。
void GenerateRandomString(wchar_t* out, int len)
{
    static const unsigned int nAlphabetCount = 26;
    --len;
    for (int i = 0; i < len; ++i)
    {
        int r = std::rand();
        if (r % 3 == 0)
        {
            r >>= 2;
            out[i] = (wchar_t)('A' + r % nAlphabetCount);
        }
        else if (r % 3 == 1)
        {
            r >>= 2;
            out[i] = (wchar_t)('a' + r % nAlphabetCount);
        }
        else
        {
            r >>= 2;
            out[i] = (wchar_t)('0' + r % 10);
        }
    }
    out[len] = L'\0';
}