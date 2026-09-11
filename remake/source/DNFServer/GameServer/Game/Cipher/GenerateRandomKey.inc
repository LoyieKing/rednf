void GenerateRandomKey(unsigned char* out, int len)
{
    for (int i = 0; i < len; ++i)
        // ORIG 对 rand() 结果做 `sarl$31;shrl$24;addl;andl$255;subl` 非负
        // mod256 归一（P176 族）——显式 `% 256` 触发该 codegen；rand 非负，
        // 语义与 (unsigned char) 相同。
        out[i] = (unsigned char)(std::rand() % 256);
}