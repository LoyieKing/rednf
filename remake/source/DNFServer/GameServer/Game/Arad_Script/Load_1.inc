// [b28d-solo-Arad_Script] Load(PKc) verdict=N 封顶（fresh .o 408/408 gap=0,
// p167 CLEAR 39/39）。残余同双参版：块内 allocator/string 临时槽整体差 0x10
// （N82/N93 槽方向）、4.1 空块 nop;jmp 岛与块序（N104/N112/N121）、
// `mov %eax,%ecx; lea (%ecx,%eax,1)` vs `add`（N18a/P30；rowIdx 为 int 且需与 -1
// 比较，无法用 P211 同型化翻 add-vs-lea）。语义完成、byte 层 N 封顶。
bool Arad_Script::Load(const char* path)
{
    // ORIG _ZN11Arad_Script4LoadEPKc @ 0x0817bea4（报告缺失：docs 同名 Load.md
    // 仅含双参重载；本函数按 objdump [0x0817bea4,0x0817c44d] 全量转写）。
    // 与双参版同构，差异：字符来源为 fgetc(fp)；节匹配循环每轮先读
    // pathC_[scanIdx++] 入死槽（ORIG +0x82..+0x9d 真实调用，原码遗留行为，
    // 按指令恢复）；失败 return 0 均不 fclose（ORIG 泄漏语义），仅成功路径
    // 末尾 fclose(fp) 后返 1。
    // 【残差豁免·同双参版】语义/调用序逐段对齐；残余为 gcc4.1↔4.4 CFG 块排布、
    // 回边极性与临时栈槽偏移差异（负例表「跨版本」族：空 else nop;jmp 岛 vs
    // jump-threading、outgoing 槽序、EH cleanup epilogue 合并），证据同
    // 双参重载注释（ORIG 0x0817bea4..0x0817c44d 全区间 objdump 转写）。
    std::FILE* fp = std::fopen(pathC_.c_str(), "rt");
    if (fp == 0)
        return 0;
    if (!Create())
        return 0;
    char* pStart = 0;
    char tag[0x400] = { 0 };
    std::sprintf(tag, "[%s]", path);
    unsigned int i = 0;
    unsigned int tokenLen = 0;
    int pos = 0;
    int tagLen = (int)std::strlen(tag);
    int c = 0;
    int rowIdx = 0;
    int colLen = 0;
    int scanIdx = 0;
    int probe;

    while (true)
    {
        probe = pathC_[scanIdx++];
        c = std::fgetc(fp);
        if (c == -1)
            return 0;
        if (c <= 0x20)
        {
        }
        else if (c > 0x7e)
        {
        }
        else if ((int)(signed char)tag[pos] == c)
        {
            ++pos;
            if (pos == tagLen)
                break;
        }
        else
            pos = 0;
    }

    char colBuf[0x200] = { 0 };
    colLen = 0;
    while (true)
    {
        c = std::fgetc(fp);
        if (c == -1)
            return 0;
        if (c == 0x20 || c == 9 || c == 10 || c == 13)
        {
            if (colLen > 0)
            {
                colBuf[colLen] = 0;
                colLen = 0;
                pStart = colBuf;
                tokenLen = (unsigned int)std::strlen(colBuf);
                for (i = 0; i < tokenLen; ++i)
                {
                    if (colBuf[i] == '/')
                    {
                        colBuf[i] = 0;
                        {
                            std::allocator<char> alloc;
                            std::string key(pStart, alloc);
                            colIndex_[key] = usedCols_;
                        }
                        pStart = colBuf + i + 1;
                    }
                }
                {
                    std::allocator<char> alloc;
                    std::string key(pStart, alloc);
                    colIndex_[key] = usedCols_;
                    usedCols_ = usedCols_ + 1;
                }
                if (usedCols_ + 1 >= cols_)
                {
                    if (!ExpandCol())
                        return 0;
                }
            }
        }
        else
        {
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (c == 10)
            break;
    }

    while (true)
    {
        rowIdx = -1;
        colLen = 0;
        while (true)
        {
            c = std::fgetc(fp);
            if (c == 0x20 || c == 9 || c == 10 || c == 13 || c == -1)
            {
                if (colLen > 0)
                {
                    colBuf[colLen] = 0;
                    colLen = 0;
                    if (rowIdx == -1)
                    {
                        {
                            std::allocator<char> alloc;
                            std::string key(colBuf, alloc);
                            rowIndex_[key] = usedRows_;
                        }
                    }
                    else
                    {
                        std::strcpy(cells_ + ((rowIdx + (usedRows_ * cols_)) << 7), colBuf);
                    }
                    ++rowIdx;
                    if (rowIdx == usedCols_)
                    {
                        while (c != -1 && c != 10)
                            c = std::fgetc(fp);
                    }
                }
                else
                {
                    // [DNF-FIX 2026-10-08 c40] ORIG 0x0817c2a7 `movl $-1,-0x2c; jmp 0x0817c428` -> outer loop
                    //   test, c == -1 -> fclose; return 1 (0x0817c433). An empty token (blank
                    //   line, trailing blank, EOF after the last '\n') ends the block successfully;
                    //   the old fall-through hit `usedCols_ != rowIdx` with rowIdx == -1 and
                    //   returned 0 (same fix as the 2-arg Load, b24FIX). Non-empty token + '\n'/EOF
                    //   still takes the row-end check (0x0817c3af-0x0817c3b9).
                    c = -1;
                    goto rows_done;
                }
                if (c == 10 || c == -1)
                    break;
                continue;
            }
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (usedCols_ != rowIdx)
            return 0;
        ++usedRows_;
        if (usedRows_ + 1 >= rows_)
        {
            if (!ExpandRow())
                return 0;
        }
        if (c == -1)
            break;
    }
rows_done:
    std::fclose(fp);
    return 1;
}