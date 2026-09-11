// [b28d-solo-CUserCharacInfo AE] 160=160：
// ①`out.clear()` 移到 buffer 声明之前 ⇒ ORIG 0x0864607e `call clear` 在 rep stos 前；
// ②删具名 bool stop 改直测条件（P198②）；
// ③P274/P319：offset 更新三处 unsigned cast ⇒ 复现 ORIG 两条 lea
//   （`lea (%eax,%edx,1)` + `lea (%edx,%eax,1)`，探针 /tmp/s156_probe/p10.cpp g3）。
void CUserCharacInfo::getSeparatedTextUnits(std::vector<std::string>& out,
                                            const char* text, const char* sep,
                                            int maxUnits)
{
    // [b28d-L14 真 bug 修正 + P253] ORIG 0x08647874 `mov $0x40,%edx; rep stos`：
    // %edx 是 dword 计数 ⇒ 清零 0x40*4=0x100 字节（全缓冲），旧 memset(0x40)
    // 少清 0xc0 字节。显式零初始化数组复现 4.1 rep stos（P214③）。
    out.clear();
    char buffer[0x100] = {0};
    int offset = 6;
    while (true)
    {
        // [b28d-solo-CUserCharacInfo P198②] ORIG 0x086460d4 无 stop 槽（寄存器 setne
        // 物化直测）⇒ 条件内联，勿用具名 bool 局部。
        if (maxUnits != 0 && (int)out.size() >= maxUnits - 1)
            break;
        const char* found = strstr(text + offset, sep);
        if (found == 0)
            break;
        int length = (int)(found - (text + offset));
        if (length != 0)
        {
            strncpy(buffer, text + offset, length);
            buffer[length] = 0;
            out.push_back(std::string(buffer, std::allocator<char>()));
        }
        // [b28d-solo-CUserCharacInfo P274/P319] ORIG 0x08646196 两条 lea
        // （`lea (%eax,%edx,1)` + `lea (%edx,%eax,1)`）；三处 unsigned 化
        // （异型化旋钮，探针 /tmp/s156_probe/p10.cpp g3）即逐条复现。
        offset = (unsigned int)strlen(sep) + (unsigned int)length +
                 (unsigned int)offset;
    }
    if (text[offset] != 0)
        out.push_back(std::string(text + offset, std::allocator<char>()));
}