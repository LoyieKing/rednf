// ---------------------------------------------------------------------------
// Arad_Script::Load(dir, file) —— ORIG _ZN11Arad_Script4LoadEPKcS1_ @ 0x0817b950
// 报告 docs/class_func_reports/Arad_Script/Load.md（汇编 + Ghidra C 全文转写）。
// 语义：内存流定位 "[%s]"(file) 节标记 → 列名行解析（'/' 分段写入 colIndex_，
// 末尾统一 ++usedCols_ 并按需 ExpandCol）→ 数据块行解析（首 token 写 rowIndex_，
// 其余 strcpy 进 cells_，行满 usedCols_ 后跳行尾，按需 ExpandRow）。
// EOF(-1) 在节/列行段直接失败；数据段空 token 视作 EOF。失败路径不 fclose
// （本重载无文件句柄）。
// 【残差豁免·跨版本 N 类】ORIG 397 / ours 400（gap 24→3）。语义与调用序逐段对齐；
// 剩余差异均为 gcc4.1↔4.4 结构性产物：①do-while/while 回边 `je exit;jmp top` vs
// `jmp top;nop;jne top`（负例表"空 else nop;jmp 岛 vs jump-threading"，ORIG
// 0x0817bc7d/0x0817ba70）；②key2 写回 `mov %eax,%edx` 缓存 ref（4.1 regmove 方向，
// N18a，ORIG 0x0817bbf4）；③cells_ 寻址 `lea (%ecx,%eax,1)` vs `add mem,%eax`
// （N4 折叠，ORIG 0x0817bdbc）；④string/allocator 临时栈槽偏移 -0x54/-0x4d… vs
// -0x44/-0x3d…（NEAR 级槽序）；⑤EH landing-pad 块序（"EH 双份 dtor epilogue vs
// merge"）。源码层无合法写法翻转以上各向。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_Script] Load(PKc,PKc) verdict=N 封顶（fresh .o 387/387 gap=0,
// p167 CLEAR 31/31，ndiff 196 中 139 条为块内临时对象槽号整体深 0x10：4.1 把循环内
// allocator/string 对放在 -0x4d/-0x54 贴近 colBuf，4.4 放在 -0x3d/-0x44 贴近 ebp
// —— 4.1 声明序 vs 4.4 首次使用序的槽方向差，N82/N93 族（P212 全头部声明法不适用
// 于依赖运行期实参构造的块内对象）；其余为 4.1 空块 nop;jmp 岛/块序（N104/N112/N121）
// 与 `mov %eax,%ecx; lea` vs `add`（N18a/P30）。语义完成、byte 层 N 封顶。
// [L3Free25to36.L3AUserHistoryAradScript 2026-09-11 · 再补一形] 列名行循环 `for(;;)`
//   改 `while (true)`（语义等价）复编后：ndiff 196 不变、ORIG-only/OURS-only 助记符差
//   不变（仍 {mov,je,lea} vs {add,jne,jmp}）⇒ 回滚。4.4 对本处两种书写发同一 CFG。
bool Arad_Script::Load(const char* dir, const char* file)
{
    if (!Create())
        return 0;
    char* pStart = 0;
    char tag[0x400] = { 0 };
    std::sprintf(tag, "[%s]", file);
    unsigned int i = 0;
    unsigned int tokenLen = 0;
    int pos = 0;
    int tagLen = (int)std::strlen(tag);
    int c = 0;
    int rowIdx = 0;
    int colLen = 0;
    int unused = 0;
    (void)unused;

    // 节标记滑动匹配（ORIG +0xad..+0x101：空白/可打印外字符忽略）
    while (true)
    {
        c = (int)(signed char)*dir++;
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

    // 列名行（ORIG +0x103..+0x34d）：'/' 分段键各写 colIndex_[seg]=usedCols_，
    // 末段写完后 usedCols_ 自增一次并按需 ExpandCol。
    char colBuf[0x200] = { 0 };
    colLen = 0;
    for (;;)
    {
        c = (int)(signed char)*dir++;
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

    // 数据块行（ORIG +0x34f..+0x544）：rowIdx=-1 起；首 token 建行名，
    // 之后逐格 strcpy；行满后跳至行尾/EOF；行结束但 rowIdx!=usedCols_ → 失败。
    while (true)
    {
        rowIdx = -1;
        colLen = 0;
        while (true)
        {
            c = (int)(signed char)*dir++;
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
                            c = (int)(signed char)*dir++;
                    }
                }
                else
                {
                    c = -1;
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
    return 1;
}