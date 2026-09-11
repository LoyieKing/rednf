// _Z20readStringFromBufferPciRS_ 0x08ad0130：
// 读 ushort 长度 n（buf 前移 2），拷贝 min(n, maxLen-1) 字节并以 NUL 收尾，
// buf 前进 n 字节。
// [b28d-28] DIFF | ORIG 0x08ad0130 | N82/P75：ORIG 以 ebx/esi/edi 缓存
// dst/maxLen/buf 与 n（`mov %di,_ZZ18readWordFromBufferRPcE10wordBuffer`——
// 4.1 inline 展开遗留的 only-ORIG weak 符号）；4.4 重读形参并把 n 槽化
// （-0xe）、min() 折 cmovg。源形不可造，语义等价。
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
}