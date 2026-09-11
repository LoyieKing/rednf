// [objdump 0x813dd3e]
bool convertStringToZip(char* out, unsigned int outCap, const char* src,
                        unsigned int srcLen, unsigned int& realLen)
{
    if (srcLen == 0)
    {
        realLen = 0;
        return true;
    }
    realLen = outCap;                       // ORIG 0x813dd5d：写入 outCap（容量占位）
    std::memset(out, 0, outCap);
    // [b28d-solo-advancealtar_DB] P0/N96 清绑：原 `register unsigned char c asm("eax")`
    //   为禁用形（-O0 局部恒占栈槽、寄存器仅承载表达式值）。P168a①：callee 返回
    //   bool 时裸负形 `if (!call()) return false;` 直产 ORIG 0x0813dd5d 的
    //   `call; xor $0x1,%eax; test %al,%al; je`（零扩展+取反全在 eax）。
    if (!compress_zip(out, realLen, src, srcLen))
        return false;
    return true;
}