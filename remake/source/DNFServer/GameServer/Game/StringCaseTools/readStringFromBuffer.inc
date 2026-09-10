void readStringFromBuffer(char* dst, int maxLen, char*& buf)
{
    // [b28e-L5] 改用 inline readWordFromBuffer（ORIG 在此点内联展开并写 static
    // wordBuffer）⇒ 复现 `movzwl (%eax),%edi; mov %di,0x9501a1a; add $0x2,%eax;
    // mov %eax,(%ebx)` 序列。
    const unsigned short n = readWordFromBuffer(buf);
    const int len = (int)n <= maxLen - 1 ? (int)n : maxLen - 1;
    memcpy(dst, buf, len);
    dst[len] = 0;
    buf += n;
