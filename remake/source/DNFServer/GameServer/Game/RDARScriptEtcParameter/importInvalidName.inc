// ============================================================================
// importInvalidName —— ORIG 0x08908338
// 报告 docs/class_func_reports/_lt_global_gt_/importInvalidName.md
// 调用点（init.md +0x2279）：CDataManager::init → this+0x86c4
//   （CDataManager::m_invalidChar，InvalidCharData）。
// 布局：+0x00 vector<unsigned char> m_charList
//       +0x0c vector<stRangeElement<unsigned char>> m_charRange
//       +0x18 vector<unsigned short> m_shortList
//       +0x24 vector<stRangeElement<unsigned short>> m_shortRange
// 验收：P167 差集 ORIG-only=0；OURS-only 4×~stRangeElement = 4.1 对 user
// 空析构类 temp 消解 dtor 调用、4.4 判 non-trivial 必发 D1+EH cleanup
// （P71/P67 跨编译器结构性差异；ORIG D1/D2 W 0x8948ff0/0x89491a6 须保留，
// 不删用户析构）⇒ 标 N 封顶。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] IDENTICAL/AE：ORIG 0x08908338；本波两处钥匙：
// ①6 处二次守卫 `if (!ok) break;` 改「正极性 if + else break」⇒ 4.4 产 ORIG 的
// `movzbl;test;je`（P194/P168a④ 反例复活；`==0`/`!=true` 两写法仍产 xor）；
// ②6 个 bool 按 ORIG 槽浅→深声明（okA..okF，P309）⇒ 槽 -0x21..-0x26 全部归位。
// 173=173 归一化逐条全等（ndiff 158→0）。删 stRangeElement 用户空析构后 4.4 不发
// 4×D1+EH（P71/P67），D1/D2 仍由 vector 使用点发 W（同 ORIG 0x8948ff0/0x89491a6）。
// p167 CLEAR（22/22），语义闭合。
// [SplitC batch10 · 判据] 返回类型权威 = **bool**：ORIG 唯一外部调用点
//   `_ZN12CDataManager4initEv`：`call 8908338` → `xor $0x1,%eax` → `test %al,%al`
//   （运算符 ! 的 8 位语义；若为 int 会走 test %eax,%eax; sete %al）
//   ⇒ 与 DataManagerInitDepends.h:52 的 bool 声明一致；本处原 int 为历史错修。
bool importInvalidName(char* path, InvalidCharData* out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    // [b28d-solo-RDARScriptEtcParameter] ORIG 0x08908338 真形（Ghidra C 复核）：
    // 外层 while(ScanType) + 四个**独立 if**（非 else-if 链）+ 内层 while 条件循环
    // （逗号表达式内联 ScanInt）；ok 变量 6 个独立（okA..okF），lo/hi 跨块共享。
    // [b28d-solo] 4.4 头部声明「先声明浅」⇒ 按 ORIG 槽浅→深排列（okA=-0x21..okF=-0x26，P309）。
    std::string token;                      // ORIG -0x20
    bool okA;                               // ORIG -0x21
    bool okB;                               // ORIG -0x22
    bool okC;                               // ORIG -0x23
    bool okD;                               // ORIG -0x24
    bool okE;                               // ORIG -0x25
    bool okF;                               // ORIG -0x26
    int lo;                                 // ORIG -0x14
    int hi;                                 // ORIG -0x10
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[one byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okA);
                if (!okA)
                    break;
                hi = ScanInt(&okB);
                if (okA)
                {
                    if (okB)
                    {
                        out->m_charRange.push_back(
                            InvalidCharData::stRangeElement<unsigned char>(
                                (unsigned char)lo, (unsigned char)hi));
                    }
                    else
                        break;
                }
                else
                    break;
            }
        }
        if (token == "[one byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okC);
                if (okC)
                    out->m_charList.push_back((unsigned char)lo);
                else
                    break;
            }
        }
        if (token == "[two byte range]")
        {
            for (;;)
            {
                lo = ScanInt(&okD);
                if (!okD)
                    break;
                hi = ScanInt(&okE);
                if (okD)
                {
                    if (okE)
                    {
                        out->m_shortRange.push_back(
                            InvalidCharData::stRangeElement<unsigned short>(
                                (unsigned short)lo, (unsigned short)hi));
                    }
                    else
                        break;
                }
                else
                    break;
            }
        }
        if (token == "[two byte]")
        {
            for (;;)
            {
                lo = ScanInt(&okF);
                if (okF)
                    out->m_shortList.push_back((unsigned short)lo);
                else
                    break;
            }
        }
    }
    return 1;
}